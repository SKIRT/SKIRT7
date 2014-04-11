/* ----------------------------------------------------------------------------
  treetmpl.h
  mbwall 25feb95
  Copyright 1995 Massachusetts Institute of Technology

 DESCRIPTION:
  This defines the templatized tree objects.
---------------------------------------------------------------------------- */
#ifndef _ga_treetmpl_h_
#define _ga_treetmpl_h_

#include <gaerror.h>
#include <GATreeBASE.h>

/* ----------------------------------------------------------------------------
 GATree
-------------------------------------------------------------------------------
  This object is a container for nodes that have a tree structure.  The base
tree object is responsible for maintaining the tree heirarchy.  This object is
responsible for doing the memory management (allocating and de-allocating the
nodes in the tree).  We insulate the user entirely from nodes - when you use
a tree, you don't get nodes back, you get the contents of nodes (ie the user
doesn't have to think about the tree parts of a node, they can simply assume
that their data is organized into a tree structure).
  We include an iterator in this object so that you can navigate through the
tree.  You can create another iterator and assign it to your tree so you can
have multiple iterators.
  All of the actions take place relative to the current location of the
embedded iterator.  None of the iterators change the state of the tree.  Be
careful so that you don't end up with an iterator dangling with a pointer to
a part of a tree that no longer exists (I would need some kind of reference
counting and/or message passing to take care of this at a lower level, and I'm
not ready to implement that at this point).
  For now we allocate nodes on the fly.  Eventually I would like to do some
better memory management (arrays perhaps?) so we don't have to do so much
alloc and dealloc and recursion.
  We depend on the template-ized GATreeIter routine, thus the declaration.

copy
  Make a copy of the specified tree.  Iterator goes to root node (should go to
  appropriate node in copy, but we don't do that yet).

clone
  Allocate space and make a copy of the tree and return a pointer to the new
  one.  The iterator of the original is not affected.  The iterator of the
  clone is set to the appropriate place in the clone.  If you specify a node
  index when you call clone then a clone of the subtree is made and the
  iterator in the clone is set to the root node (the top of the subtree).

remove
  Remove the current node (and its subtree) from the tree and stick it into a
  new tree.  Returns a pointer to the new tree.  Leaves the original iterator
  pointing to the eldest child or parent of the node that was removed.  Iter
  of the new tree points to the root node.

destroy
  Destroys the node and subtree where the iterator is currently pointing.
  Moves the iterator to the eldest sibling or parent of the node that was
  deleted from the tree.

swap
  Swap nodes in a tree, leaves the nodes' subtrees in place (subtrees do not
  move with the nodes in the swap).

swaptree - tree
  Swap subtrees referenced by the iterators of this and the second tree.  The
  iterators are reset to point to the new subtrees (same point in the trees,
  but different nodes due to the swap).

swaptree - indices
  Swap the subtrees referenced by the integer values.  Indices must not be
  related (ie one cannot be ancestor of the other).  Iterator is not changed.

insert - tree
  Inserts the contents of tree in to the current tree and removes it from the
  original tree.  Does NOT delete the original tree, but DOES assume
  responsibility for the memory used by original tree contents.

insert - object
  Inserts the object into a new node relative to the location of the iterator

root, current, next, prev, parent, child, warp
  These iterator methods are defined as access to the embedded iterator of the
  tree.  Use these methods to move the insertion point and to traverse the
  tree.  You can also create other iterators for this tree, but they won't
  affect the contents.
---------------------------------------------------------------------------- */
template <class T> class GATreeIter;

template <class T>
class GATree : public GATreeBASE {
public:
  GATree() : GATreeBASE() { iter(*this); }
  GATree(const T & t) : GATreeBASE(new GANode<T>(t)), iter(*this) {}
  GATree(const GATree<T> & orig){iter(*this); copy(orig);}
  GATree<T> & operator=(const GATree<T> & orig)
    {if(&orig != this) copy(orig); return *this;}
  virtual ~GATree();
  GATree<T> * clone(unsigned int i=0) const;

// methods that modify the state of the tree
  void copy(const GATree<T> & orig);
  int destroy();
  int swaptree(GATree<T> * t);
  int swaptree(unsigned int, unsigned int);
  int swap(unsigned int, unsigned int);
  GATree<T> * remove();
  int insert(GATree<T> * t, GATreeBASE::Location where=GATreeBASE::BELOW){
    if(this == t){
      GAErr(GA_LOC, "GATree", "insert", gaErrCannotInsertIntoSelf);
      return GATreeBASE::ERR;
    }
    if(GATreeBASE::insert(t->rt, iter.node, where) == GATreeBASE::ERR){
      return GATreeBASE::ERR;
    }
    iter.node = (t->rt ? t->rt : iter.node);
    t->rt=(GANodeBASE *)0; t->iter.node=(GANodeBASE *)0;
    return GATreeBASE::NO_ERR;
  }
  int insert(const T & t, GATreeBASE::Location where=GATreeBASE::BELOW){
    GANode<T> * c = new GANode<T>(t);
    if(GATreeBASE::insert(c, iter.node, where) == GATreeBASE::ERR){
      delete c;
      return GATreeBASE::ERR;
    }
    iter.node = c;
    return GATreeBASE::NO_ERR;
  }

// typesafes on iteration methods.  These call the built-in iterator then
// return the contents of the now-current node.  They do not affect the state
// of the tree.
  T * root(){return iter.root();}
  T * current(){return iter.current();}
  T * next(){return iter.next();}
  T * prev(){return iter.prev();}
  T * parent(){return iter.parent();}
  T * child(){return iter.child();}
  T * eldest(){return iter.eldest();}
  T * youngest(){return iter.youngest();}
  T * warp(unsigned int i){return iter.warp(i);}
  T * warp(const GATreeIter<T> & i)
    {return((i.tree == this) ? iter.warp(i) : (T *)0);}
  int nchildren(){return iter.nchildren();}
  int nsiblings(){return iter.nsiblings();}

protected:
  int insert(GANode<T> *n, GANode<T> *idx,
         GATreeBASE::Location where=GATreeBASE::BELOW){
    if(GATreeBASE::insert(n, idx, where) == GATreeBASE::ERR)
      return GATreeBASE::ERR;
    iter.node = n;
    return GATreeBASE::NO_ERR;
  }
  GATreeIter<T> iter;
  friend class GATreeIter<T>;
};



/* ----------------------------------------------------------------------------
 GATreeIter
-------------------------------------------------------------------------------
  This is a type-safe derivation of the base TreeIter object.  I copied the
methods from the base class (I know, a no-no) rather than doing calls to the
base class methods.
  We depend on the template-ized GATree, thus the declaration.
  Behaviour for the iterator methods is defined as follows.  If the current
node is null, attempts to access a derived position from the current position
will return NULL.  The only way to reset the current node is to call the root()
locater (you always have to start at the tree root to navigate the tree).  If
the current node is non-null and the derived node is null, the current node is
NOT changed, but NULL is returned.
  When we create a new tree iterator, it defaults to the same node as the one
used to create it.  If it is created with a tree as its argument, it defaults
to the tree's iterator's current position.
---------------------------------------------------------------------------- */
template <class T> class GATree;

template <class T>
class GATreeIter : public GATreeIterBASE {
public:
  GATreeIter() : GATreeIterBASE(){}
  GATreeIter(const GATree<T> & t) : GATreeIterBASE(t){node=t.iter.node;}
  GATreeIter(const GATreeIter<T> & i) : GATreeIterBASE(i){}
  T * current()
    {return(node ?
        &((GANode<T> *)node)->contents : (T *)0);}
  T * root()
    {return(((node=GATreeIterBASE::root()) != (GANodeBASE *)0) ?
        &((GANode<T> *)GATreeIterBASE::root(node))->contents : (T *)0);}
  T * next()
    {return((node && node->next) ?
        &((GANode<T> *)(node=node->next))->contents : (T *)0);}
  T * prev()
    {return((node && node->prev) ?
        &((GANode<T> *)(node=node->prev))->contents : (T *)0);}
  T * parent()
    {return((node && node->parent) ?
        &((GANode<T> *)(node=node->parent))->contents : (T *)0);}
  T * child()
    {return((node && node->child) ?
        &((GANode<T> *)(node=node->child))->contents : (T *)0);}
  T * eldest()
    {return(node ? &((GANode<T> *)GATreeIterBASE::eldest(node))->contents :
        (T *)0);}
  T * youngest()
    {return(node ? &((GANode<T> *)GATreeIterBASE::youngest(node))->contents :
        (T *)0);}
  T * warp(const GATree<T> & t){
    tree = &t;
    node = t.iter.node;
    return(t.iter.node ? &((GANode<T> *)(node=t.iter.node))->contents :(T *)0);
  }
  T * warp(const GATreeIter<T> & i){
    tree = i.tree;
    node = i.node;
    return(i.node ? &((GANode<T> *)(node=i.node))->contents : (T *)0);
  }
  T * warp(unsigned int i){
    GANodeBASE * n = GATreeIterBASE::warp(i);
    return(n ? &((GANode<T> *)(node=n))->contents : (T *)0);
  }

private:
  friend class GATree<T>;
};


/* ----------------------------------------------------------------------------
  treetmpl.C
  mbwall 25feb95
  Copyright 1995 Massachusetts Institute of Technology

 DESCRIPTION:
  This defines the templatized tree objects.

 TO DO:
  Make insert work better with size and depth so not so many recalcs needed.
  Implement better memory mangement, faster allocation, referencing.
  Use array representation of nodes so we don't have to do so much recursion.
  Figure better way to do traversals so that we speed up the swaps.
---------------------------------------------------------------------------- */

extern GANodeBASE * _GATreeTraverse(unsigned int, unsigned int&, GANodeBASE *);
//template <class T> void _GATreeDestroy(GANode<T> *);
//template <class T> GANode<T> * _GATreeCopy(GANode<T> *, GANode<T> *);


/* ----------------------------------------------------------------------------
Tree
---------------------------------------------------------------------------- */
// The destructor just goes through the tree and deletes every node.  As with
// any method that uses the BASE tree, we have to use its members so it doesn't
// get messed up.
template <class T>
GATree<T>::~GATree()
{
  _GATreeDestroy(DYN_CAST(GANode<T>*, rt));
  iter.node = (GANodeBASE *)0;
}


// Yes, this is really ugly.  We do a complete destruction of the existing tree
// then we copy the new one.  No caching, no nothing.  Oh well.  The iterator
// is set to the root node - it should be set to the corresponding node, but
// I won't do that for now.  THIS IS A BUG!
template <class T> void
GATree<T>::copy(const GATree<T> & orig)
{
  _GATreeDestroy(DYN_CAST(GANode<T>*, rt));
  rt = _GATreeCopy(DYN_CAST(GANode<T>*, orig.rt), (GANode<T> *)0);
  iter.node = rt;
  sz=orig.sz; csz=orig.csz;
  dpth=orig.dpth; cdpth=orig.cdpth;
}


// This remove method returns a tree with the removed node as its root.  The
// node we remove is the current node.  We allocate memory for the tree, but
// we don't allocate any memory for the node or its children.  That is taken
// from the previous (this) tree and it no longer has to worry about it.  It
// is the responsibility of the new tree to delete that memory.
//   The iterator gets set to the elder child of the node that was removed.  If
// there is no elder child, then it gets set to the parent.  If no parent, then
// it gets set to NULL.
//   Thank you to Uli Bubenheimer (uli@aisun1.ai.uga.edu) for the bug fix here.
// I forgot to fix the pointers in the root node of the sub-tree.
template <class T> GATree<T> *
GATree<T>::remove()
{
  GATree<T> * t = new GATree<T>;
  GANode<T> * node = DYN_CAST(GANode<T>*, iter.node);
  if(!node) return t;

  if(node->prev != node) iter.eldest();
  else if(node->parent) iter.parent();
  else iter.node = (GANodeBASE *)0;

  GANode<T> * tmpnode = DYN_CAST(GANode<T>*, GATreeBASE::remove(node));
  tmpnode->prev = tmpnode;
  tmpnode->next = tmpnode;
  tmpnode->parent = (GANodeBASE *)0;

  t->insert(tmpnode, (GANode<T> *)0, GATreeBASE::ROOT);

  return t;
}


// Recursively duplicate a subtree given a base node.  This uses the _copy
// method (which does a deep and wide copy).  Here we just copy a node, then
// sic the _copy method on the child if it exists.  The parent of the new node
// is set to NULL - this makes it a root node in the new tree object.
//   We do the cloning based on the valued passed to the routine.  0 is the
// root node and makes a clone of the entire tree.  This routine has no effect
// on the iterator in the original tree.
//   The iterator in the clone is left pointing to the root node of the clone.
template <class T> GATree<T> *
GATree<T>::clone(unsigned int i) const
{
  GATree<T> * t = new GATree<T>;
  GANode<T> * node;
  unsigned int w=0;
  if(i == 0) node = DYN_CAST(GANode<T>*, rt);
  else node = DYN_CAST(GANode<T>*, _GATreeTraverse(i, w, rt));
  if(!node) return t;

  GANode<T> * newnode = new GANode<T>(node->contents);
  newnode->child = _GATreeCopy(DYN_CAST(GANode<T>*, node->child), newnode);
  if(newnode->child) newnode->child->parent = newnode;

  t->insert(newnode, (GANode<T> *)0, GATreeBASE::ROOT);

  return t;
}


// Destroy the specified node and all nodes attached to it looking downward.
// This does NOT destroy any nodes above the specified node.  If this node is
// in a tree, it will be removed before the nuking occurs.  This gives the tree
// object a chance to flag for any recalculations it might need.  The destroy
// method effect on the tree as a remove, but it is destructive (it frees up
// the memory as well).
//   We do the nuking recursively, so its not really that efficient.  I'll
// figure out a better way to track these nodes one of these days.
//   We use the _destroy routine to do the recursion.  _destroy kills all of
// the siblings of the node whereas this routine kills only descendents.
//   This uses the current node as the one to destroy, so be sure to use the
// iteration methods to move to the node you want to destroy.  Once the node is
// gone, we set the current node to the eldest child or parent of the node that
// was destroyed.
template <class T> int
GATree<T>::destroy()
{
  GANodeBASE * node = iter.node;
  if(!node) return GATreeBASE::NO_ERR;
  if(node->prev == node || !node->prev)
    if(node->parent) iter.node = node->parent;
    else iter.node = (GANodeBASE *)0;
  else
    iter.eldest();
  _GATreeDestroy(DYN_CAST(GANode<T>*, node->child));
  delete GATreeBASE::remove(node);
  return GATreeBASE::NO_ERR;
}


// Swap two subtrees.  We use the nodes pointed to by the iterators in the
// current tree and the one that was passed to us.  The TreeBASE swaptree
// changes the next, prev, parent, child pointers on the nodes we pass it as
// well as the nodes that those nodes point to.
//   Notice that this routine uses the current location of the iterators to
// do its job, so be sure to set them properly before you call this routine!
//   The iterators are reset to the nodes where the swaps occurred.  Sizes and
// depths are possibly changed - the insert method flags them for a recalc.
//   If an iterator is NULL then we do an insert ONLY if the root node of that
// iterator's tree is NULL.  If the tree's root is non-NULL, we don't do
// anything (most likely the iterator was unset or badly set).
template <class T> int
GATree<T>::swaptree(GATree<T> * t)
{
  GANodeBASE * tmp;
  if(t->iter.node && iter.node){
    if(GATreeBASE::swaptree(t->iter.node, iter.node) == GATreeBASE::ERR)
      return GATreeBASE::ERR;
    if(t->rt == t->iter.node) t->rt = iter.node;
    tmp = t->iter.node;
    t->iter.node = iter.node;
    iter.node = tmp;

    t->csz = 1; t->cdpth = 1;	// remember to flag the changes in t!
    csz = 1; cdpth = 1;
  }
  else if(t->iter.node){	// iter.node is NULL, so we have no root
    if(!rt){			// should always be true at this point
      tmp = t->GATreeBASE::remove(t->iter.node);
//      tmp->next = tmp;
//      tmp->prev = tmp;
      t->iter.node = (GANodeBASE *)0;
      if(insert(DYN_CAST(GANode<T>*, tmp), (GANode<T> *)0, GATreeBASE::ROOT) ==
     GATreeBASE::ERR)
    return GATreeBASE::ERR;
    }
  }
  else if(iter.node){		// t's iter is NULL, so it has no root
    if(!t->rt){			// should always be true
      tmp = GATreeBASE::remove(iter.node);
//      tmp->next = tmp;
//      tmp->prev = tmp;
      iter.node = (GANodeBASE *)0;
      if(t->insert(DYN_CAST(GANode<T>*,tmp),(GANode<T>*)0,GATreeBASE::ROOT) ==
     GATreeBASE::ERR)
    return GATreeBASE::ERR;
    }
  }
// else both t->iter.node and iter.node are NULL, so don't do anything
  return GATreeBASE::NO_ERR;
}


// Same as the swaptree above, but this routine uses the node indices to do
// the swap.  This can be dangerous:  if one of the nodes is a decendent of the
// other then we could end up with a fragmented tree, so we'll have to check
// for that situation.  Unfortunately this check slows things down quite a bit.
// If one is the ancestor of the other, then we don't do the swap.
//   This routine does not affect the size of the tree, but it could change the
// depth of the tree.  We leave the iterator where it was pointing before the
// swap.
template <class T> int
GATree<T>::swaptree(unsigned int a, unsigned int b)
{
  unsigned int aw=0, bw=0;
  GANodeBASE * anode = _GATreeTraverse(a, aw, rt);
  GANodeBASE * bnode = _GATreeTraverse(b, bw, rt);
  return GATreeBASE::swaptree(anode,bnode);
}


// Swap two nodes in a tree, leave their subtrees intact.  This routine does
// not affect the iterator or the size or depth of the tree.
template <class T> int
GATree<T>::swap(unsigned int a, unsigned int b)
{
  unsigned int aw=0, bw=0;
  GANodeBASE * anode = _GATreeTraverse(a, aw, rt);
  GANodeBASE * bnode = _GATreeTraverse(b, bw, rt);
  return GATreeBASE::swapnode(anode,bnode);
}


/* ----------------------------------------------------------------------------
Recursive routines for the Tree objects
---------------------------------------------------------------------------- */
// Recursively copy a node, including all of its siblings.  This routine copies
// a row, then it calls itself to copy the next generation if it finds a next
// generation in the next node.
template <class T> GANode<T> *
_GATreeCopy(GANode<T> * node, GANode<T> * parent)
{
  if(!node) return (GANode<T> *)0;

  GANode<T> * newnode = new GANode<T>(node->contents);
  newnode->parent = parent;
  newnode->child = _GATreeCopy(DYN_CAST(GANode<T>*, node->child), newnode);

  GANode<T> *lasttmp = newnode, *newtmp = (GANode<T> *)0;
  GANode<T> *tmp = DYN_CAST(GANode<T>*, node->next);
  while(tmp && tmp != node){
    newtmp = new GANode<T>(tmp->contents);
    newtmp->parent = parent;
    newtmp->child = _GATreeCopy(DYN_CAST(GANode<T>*, tmp->child), newtmp);
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


// This routine destroys the specified node, its children, its siblings, and
// all of their children, their childrens' siblings, etc.  Since we kill off
// all of the siblings, we need to set the parent's link to its child to NULL.
template <class T> void
_GATreeDestroy(GANode<T> * node)
{
  if(!node) return;

  if(node->parent)
    node->parent->child = (GANodeBASE *)0;
  _GATreeDestroy(DYN_CAST(GANode<T>*, node->child));

  GANodeBASE * tmp;
  while(node->next && node->next != node){
    tmp = node->next;
    node->next = tmp->next;
    tmp->next->prev = node;
    _GATreeDestroy(DYN_CAST(GANode<T>*, tmp->child));
    delete tmp;
  }
  delete node;
}

#endif
