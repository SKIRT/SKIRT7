/* ----------------------------------------------------------------------------
  listtmpl.h
  mbwall 25feb95
  Copyright 1995 Massachusetts Institute of Technology

 DESCRIPTION:
  This defines the template-ized list objects.
---------------------------------------------------------------------------- */
#ifndef _ga_listtmpl_h_
#define _ga_listtmpl_h_

#include <gaerror.h>
#include <GAListBASE.h>

/* ----------------------------------------------------------------------------
 GAList
-------------------------------------------------------------------------------
  This object is a container for nodes that have a list structure.  The base
list object is responsible for maintaining the list heirarchy.  This object is
responsible for doing the memory management (allocating and de-allocating the
nodes in the list).  We insulate the user entirely from nodes - when you use
a list, you don't get nodes back, you get the contents of nodes (ie the user
doesn't have to think about the list parts of a node, they can simply assume
that their data is organized into a list structure).
  We include an iterator in this object so that you can navigate through the
list.  You can create another iterator and assign it to your list so you can
have multiple iterators.
  All of the actions take place relative to the current location of the
embedded iterator.  None of the iterators change the state of the list.  Be
careful so that you don't end up with an iterator dangling with a pointer to
a part of a list that no longer exists (I would need some kind of reference
counting and/or message passing to take care of this at a lower level, and I'm
not ready to implement that at this point).
  For now we allocate nodes on the fly.  Eventually I would like to do some
better memory management (arrays perhaps?) so we don't have to do so much
alloc and dealloc and recursion.
  We depend on the template-ized GAListIter routine, thus the declaration.

copy
  Make a complete copy of the list and return a pointer to the new list.

remove
  Remove the current node from the list, free the memory used by the node, and
  return a pointer to a copy of the object that the node contained.

destroy
  Remove the current node from the list and free the memory it was using.
  Destroys the object that the node contained.

insert - list
  Inserts the contents of list in to the current list and removes it from the
  original list.  Does NOT delete the original list, but DOES assume
  responsibility for the memory used by the contents of the original list.

insert - object
  Insert the object into the list at the specified place relative to the
  current location of the embedded iterator.

current, head, tail, next, prev, warp
  These iterator methods are defined as access to the embedded iterator of the
  list.  Use these methods to move the insertion point and to traverse the
  list.  You can also create other iterators for this list, but they won't
  affect the contents.
---------------------------------------------------------------------------- */
template <class T> class GAListIter;

template <class T>
class GAList : public GAListBASE {
public:
  GAList() : GAListBASE() { iter(*this); }
  GAList(const T & t):GAListBASE(new GANode<T>(t)), iter(*this) {}
  GAList(const GAList<T> & orig){iter(*this); copy(orig);}
  GAList<T> & operator=(const GAList<T> & orig)
    {if(&orig != this) copy(orig); return *this;}
  virtual ~GAList();
  GAList<T> * clone(unsigned int i=0) const;

// methods that modify the state of the list
  void copy(const GAList<T> & orig);
  int destroy();
  int swap(unsigned int, unsigned int);
  T * remove();
  int insert(GAList<T> * t, GAListBASE::Location where=GAListBASE::AFTER){
    if(this == t){
      GAErr(GA_LOC, "GAList", "insert", gaErrCannotInsertIntoSelf);
      return GAListBASE::ERR;
    }
    if(GAListBASE::insert(t->hd, iter.node, where) == GAListBASE::ERR){
      return GAListBASE::ERR;
    }
    iter.node = (t->hd ? t->hd : iter.node);
    t->hd=(GANodeBASE *)0; t->iter.node=(GANodeBASE *)0;
    return GAListBASE::NO_ERR;
  }
  int insert(const T & t, GAListBASE::Location where=GAListBASE::AFTER){
    GANode<T> * c = new GANode<T>(t);
    if(GAListBASE::insert(c, iter.node, where) == GAListBASE::ERR){
      delete c;
      return GAListBASE::ERR;
    }
    iter.node = c;
    return GAListBASE::NO_ERR;
  }

// typesafes on iteration methods.  These call the built-in iterator then
// return the contents of the now-current node.  They do not affect the state
// of the list.
  T * head(){return iter.head();}
  T * tail(){return iter.tail();}
  T * current(){return iter.current();}
  T * next(){return iter.next();}
  T * prev(){return iter.prev();}
  T * warp(unsigned int i){return iter.warp(i);}
  T * warp(const GAListIter<T> & i)
    {return((i.list == this) ? iter.warp(i) : (T *)0);}
  T * operator[](unsigned int i){return iter.warp(i);}

protected:
  int insert(GANode<T> *n, GANode<T> *idx,
         GAListBASE::Location where=GAListBASE::AFTER){
    if(GAListBASE::insert(n, idx, where) == GAListBASE::ERR)
      return GAListBASE::ERR;
    iter.node = n;
    return GAListBASE::NO_ERR;
  }
  GAListIter<T> iter;	// the embedded iterator
  friend class GAListIter<T>;
};


/* ----------------------------------------------------------------------------
 GAListIter
-------------------------------------------------------------------------------
  This is a type-safe derivation of the base ListIter object.  I copied the
methods from the base class (I know, a no-no) rather than doing calls to the
base class methods.
  We depend on the template-ized GAList, thus the declaration.
  Behaviour for the iterator methods is defined as follows.  If the current
node is null, attempts to access a derived position from the current position
will return NULL.  The only way to reset the current node is to call the head()
locater (you always have to start at the list head to navigate the list).  If
the current node is non-null and the derived node is null, the current node is
NOT changed, but NULL is returned.  You can also warp to a new position if you
have another iterator or a list with an embedded iterator.
  When we create a new list iterator, it defaults to the same node as the one
used to create it.  If it is created with a list as its argument, it defaults
to the list's iterator's current position.
---------------------------------------------------------------------------- */

template <class T>
class GAListIter : public GAListIterBASE {
public:
  GAListIter() : GAListIterBASE(){}
  GAListIter(const GAList<T> & t) : GAListIterBASE(t){node=t.iter.node;}
  GAListIter(const GAListIter<T> & i) : GAListIterBASE(i){}
  T * current()
    {return(node ?
        &((GANode<T> *)node)->contents : (T *)0);}
  T * head()
    {return(((node=GAListIterBASE::head()) != (GANodeBASE *)0) ?
        &((GANode<T> *)GAListIterBASE::head())->contents : (T *)0);}
  T * tail()
    {return(((node=GAListIterBASE::tail()) != (GANodeBASE *)0) ?
        &((GANode<T> *)GAListIterBASE::tail())->contents : (T *)0);}
  T * next()
    {return((node && node->next) ?
        &((GANode<T> *)(node=node->next))->contents : (T *)0);}
  T * prev()
    {return((node && node->prev) ?
        &((GANode<T> *)(node=node->prev))->contents : (T *)0);}
  T * warp(const GAList<T> & t){
    list = &t;
    node = t.iter.node;
    return(t.iter.node ? &((GANode<T> *)(node=t.iter.node))->contents :(T *)0);
  }
  T * warp(const GAListIter<T> & i){
    list = i.list;
    node = i.node;
    return(i.node ? &((GANode<T> *)(node=i.node))->contents : (T *)0);
  }
  T * warp(unsigned int i){
    GANodeBASE * n = GAListIterBASE::warp(i);
    return(n ? &((GANode<T> *)(node=n))->contents : (T *)0);
  }

private:
  friend class GAList<T>;	// do I need to do this?
};


/* ----------------------------------------------------------------------------
  listtmpl.C
  mbwall 25feb95
  Copyright 1995 Massachusetts Institute of Technology

 DESCRIPTION:
  This defines the templatized list objects.

 TO DO:
  Make insert work better with size and depth so not so many recalcs needed.
  Implement better memory mangement, faster allocation, referencing.
  Use array representation of nodes so we don't have to do so much recursion.
---------------------------------------------------------------------------- */

extern GANodeBASE * _GAListTraverse(unsigned int index, unsigned int & cur,
                    GANodeBASE * node);
//template <class T> GANode<T> * _GAListCopy(GANode<T> *, GANode<T> *);


/* ----------------------------------------------------------------------------
List
---------------------------------------------------------------------------- */
// The destructor just goes through the list and deletes every node.
template <class T> inline
GAList<T>::~GAList()
{
  while(hd) delete GAListBASE::remove(DYN_CAST(GANode<T>*, hd));
  iter.node = (GANodeBASE *)0;
}


// Yes, this is really ugly.  We do a complete destruction of the existing list
// then we copy the new one.  No caching, no nothing.  Oh well.  We set the
// iterator to the head node - it should be set to the corresponding node, but
// I won't do that right now.  THIS IS A BUG!
template <class T> inline void
GAList<T>::copy(const GAList<T> & orig)
{
  while(hd) delete GAListBASE::remove(DYN_CAST(GANode<T>*, hd));
  hd = _GAListCopy(DYN_CAST(GANode<T>*, orig.hd),
           DYN_CAST(GANode<T>*, orig.hd));
  iter.node = hd;
  sz = orig.sz; csz = orig.csz;
}


// This remove method returns a pointer to the contents of the node that was
// removed.  The node itself is destroyed.
//   The iterator gets set to the next node toward the head of the list.
//   This routine makes a copy of the node contents using the copy initializer
// of the T object, so the copy initializer MUST be defined and accessible.
//   We return a pointer to the contents rather than the contenst for the same
// reason we return a pointer from all the iter routines - we don't want to
// pass big objects around.
template <class T> inline T *
GAList<T>::remove()
{
  GANode<T> * node = DYN_CAST(GANode<T>*, iter.node);
  if(!node) return (T *)0;

  if(node->prev != node) iter.node = node->prev;
  else iter.node = (GANodeBASE *)0;
  node = DYN_CAST(GANode<T>*, GAListBASE::remove(node));
  T * contents = new T(node->contents);
  delete node;
  return contents;
}


// Make a copy of a list and return the pointer to the new list.  The cloning
// is based on the value passed to this routine.  A value of 0 will clone the
// entire list.  Any other value will clone the list from the index to the end
// of the list.  This routine has no effect on the iterator in the original
// list.
template <class T> inline GAList<T> *
GAList<T>::clone(unsigned int i) const
{
  GAList<T> * t = new GAList<T>;
  GANode<T> * node;
  unsigned int w=0;
  if(i == 0) node = DYN_CAST(GANode<T>*, hd);
  else node = DYN_CAST(GANode<T>*, _GAListTraverse(i, w, hd));
  if(!node) return t;

  GANode<T> * newnode = _GAListCopy(node, DYN_CAST(GANode<T>*, hd));

  t->insert(newnode, (GANode<T> *)0, GAListBASE::HEAD);

// need to set iterator to right spot in the clone!!  for now its at the head

  return t;
}


// Destroy the specified node.  This uses the current node as the one to
// destroy, so be sure to use the iteration methods to move to the node you
// want to destroy.  Once the node is gone, we set the current node to the
// prev node of the one that was destroyed.  If the node that was nuked was the
// head node then we set the current node to the new head.
template <class T> inline int
GAList<T>::destroy()
{
  GANodeBASE * node = iter.node;
  if(!node) return GAListBASE::NO_ERR;
  if(node->prev && node->prev != node)
    if(hd == node) iter.node = node->next;
    else iter.node = node->prev;
  else
    iter.node = (GANodeBASE *)0;
  delete GAListBASE::remove(node);
  return GAListBASE::NO_ERR;
}


// Swap two nodes in the list.  This has no effect on the size or the iterator.
// If either index is out of bounds then we don't do anything.
template <class T> inline int
GAList<T>::swap(unsigned int a, unsigned int b)
{
  if(a == b || a > (unsigned int)size() || b > (unsigned int)size())
    return GAListBASE::NO_ERR;
  GANodeBASE *tmp=hd, *anode=(GANodeBASE *)0, *bnode=(GANodeBASE *)0;
  unsigned int cur=0;
  while(tmp && tmp->next != hd){
    if(a == cur) anode = tmp;
    if(b == cur) bnode = tmp;
    tmp = tmp->next;
    cur++;
  }
  if(a == cur) anode = tmp;
  if(b == cur) bnode = tmp;
  return GAListBASE::swapnode(anode,bnode);
}


/* ----------------------------------------------------------------------------
Utility routines for the List objects
---------------------------------------------------------------------------- */
// Copy a node, including all of its siblings up to the end of the list.  We do
// NOT wrap around the list until we return the first node - we stop at the
// tail of the list.  Return the pointer to the first node in the list.
template <class T> inline GANode<T> *
_GAListCopy(GANode<T> * node, GANode<T> * head)
{
  if(!node) return (GANode<T> *)0;
  GANode<T> *newnode = new GANode<T>(node->contents);
  GANode<T> *lasttmp = newnode, *newtmp = (GANode<T> *)0;
  GANode<T> *tmp = DYN_CAST(GANode<T>*, node->next);
  while(tmp && tmp != head){
    newtmp = new GANode<T>(tmp->contents);
    newtmp->prev = lasttmp;
    lasttmp->next = newtmp;

    lasttmp = newtmp;
    tmp = DYN_CAST(GANode<T>*, tmp->next);
  }
  if(newtmp){
    newtmp->next = newnode;
    newnode->prev = newtmp;
  }
  else{
    newnode->next = newnode;
    newnode->prev = newnode;
  }
  return newnode;
}

#endif
