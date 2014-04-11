/* ----------------------------------------------------------------------------
  allele.h
  mbwall 21mar95
  Copyright (c) 1995 Massachusetts Institute of Technology
                     all rights reserved

 DESCRIPTION:
   Here we define a class of alleles.  An allele is a possible value for a gene
and an allele set is a list of possible values (I use 'set' because it doesn't
imply the specific implementation of the container class).
---------------------------------------------------------------------------- */
#ifndef _ga_allele_h_
#define _ga_allele_h_

#include <gaconfig.h>
#include <garandom.h>
#include <gaerror.h>
#include <std_stream.h>
#include <string.h>


class GAAllele {
public:
  enum Type {ENUMERATED=1, BOUNDED=2, DISCRETIZED};
  enum BoundType {NONE, INCLUSIVE, EXCLUSIVE};
};


/* ----------------------------------------------------------------------------
  This object contains a set of alleles for use with a similarly typed genome.
  This object can be used with any type that has operator= defined for it.  If
you use the remove member then you must have operator== defined for it.
  This should be implemented as a derivative of the Array class?  But I don't
want that overhead at this point.  Also, behaviour is not the same.
  The allele set uses the envelope/message structure.  The core allele object
is a reference-counted structure that contains all of the guts of an allele
set.  The outer shell, the allele set itself, is what users actually see.  It
defines the interface.  With this setup you can create a single allele set then
each genome does not have to make its own copy.  And we don't have to worry
about an allele set going out of scope then leaving genomes hanging (a problem
if we just used a pointer to a single alleleset with no reference counting).
  You can link an allele set to another so that they share the same core.  Use
the 'link' member function (this is typically used within the GAlib to reduce
the number of actual alleleset instances when cloning populations of genomes).
  There is no way to 'resize' an allele set.  You must add to it or remove
elements from it.
  The base template class assumes that the objects in the allele set are
complex, i.e. it is not OK to do a bit-copy of each object.  We should do
specializations for non-complex objects (or perhaps a separate class that does
bit-copies rather than logical-copies?)
  When you clone an allele set, the new one has its own core.
  Why didn't I do this as a couple of objects (enumerated set, bounded set,
discretized set, etc)?  I wanted to be able to have an array that uses sets of
many different types.  I suppose the allele set should be completely
polymorphic like the rest of the GAlib objects, but for now we'll do it as
a single object with multiple personalities (and a state).
  There is no error checking.  You should check the type before you try to
call any of the member functions.  In particular, if you try to get the
bounds on an enumerated set of one element, it will break.

*** should the assignment operator check to be sure that no allele is
    duplicated, or is it OK to have duplicate alleles in a set?  For now we
    allow duplicates (via either the add or assignemnt ops).
---------------------------------------------------------------------------- */
template <class T>
class GAAlleleSetCore {
public:
  GAAlleleSetCore();
  GAAlleleSetCore(unsigned int n, const T array []);
  GAAlleleSetCore(const T& lower, const T& upper,
          GAAllele::BoundType lb=GAAllele::INCLUSIVE,
          GAAllele::BoundType ub=GAAllele::INCLUSIVE);
  GAAlleleSetCore(const T& lower, const T& upper, const T& increment,
          GAAllele::BoundType lb=GAAllele::INCLUSIVE,
          GAAllele::BoundType ub=GAAllele::INCLUSIVE);
  GAAlleleSetCore(const GAAlleleSetCore<T>&);
  virtual ~GAAlleleSetCore();
  GAAlleleSetCore<T>& operator=(const GAAlleleSetCore<T>&);

  GAAllele::Type type;		// is this an ennumerated or bounded set?
  GAAllele::BoundType lowerb, upperb; // what kind of limit is the bound?
  unsigned int cnt;		// how many objects are using us?
  unsigned int csz;		// how big are the chunks to allocate?
  unsigned int sz;		// number we have
  unsigned int SZ;		// how many have we allocated?
  T * a;
};

template <class T> int operator==(const T &, const T &);
template <class T> int operator!=(const T &, const T &);

template <class T>
class GAAlleleSet {
public:
  GAAlleleSet() : core(0) {}
  GAAlleleSet(unsigned int n, const T a[]) :
  core(new GAAlleleSetCore<T>(n,a)) {}
  GAAlleleSet(const T& lower, const T& upper,
          GAAllele::BoundType lb=GAAllele::INCLUSIVE,
          GAAllele::BoundType ub=GAAllele::INCLUSIVE) :
  core(new GAAlleleSetCore<T>(lower, upper, lb, ub)) {}
  GAAlleleSet(const T& lower, const T& upper, const T& increment,
          GAAllele::BoundType lb=GAAllele::INCLUSIVE,
          GAAllele::BoundType ub=GAAllele::INCLUSIVE) :
  core(new GAAlleleSetCore<T>(lower, upper, increment, lb, ub)) {}
  GAAlleleSet(const GAAlleleSet<T>& set) :
  core(new GAAlleleSetCore<T>(*(set.core))) {}
  virtual ~GAAlleleSet(){
    if(core != 0){
      core->cnt -= 1;
      if(core->cnt==0) delete core;
    }
  }
  GAAlleleSet<T> & operator=(const GAAlleleSet<T>& set){
    if(this == &set) return *this;
    if(core) *core = *set.core;
    else core = new GAAlleleSetCore<T>(*(set.core));
    return *this;
  }
  GAAlleleSet<T> * clone() const {return new GAAlleleSet<T>(*this);}
  void link(GAAlleleSet<T>& set);
  void unlink();
  int size() const {return core->sz;}    // only meaninful for enumerated sets
  int add(const T&);		         // only for enumerated sets
  int remove(const T&);			 // only for enumerated sets
  int remove(unsigned int);		 // only for enumerated sets
  T allele() const;		         // ok for any type of set
  T allele(unsigned int i) const;        // only for enumerated sets
  T lower() const {return core->a[0];}   // only for bounded sets
  T upper() const {return core->a[1];}
  T inc() const {return core->a[2];}
  GAAllele::BoundType lowerBoundType() const {return core->lowerb;}
  GAAllele::BoundType upperBoundType() const {return core->upperb;}
  GAAllele::Type type() const {return core->type;}

#ifdef GALIB_USE_STREAMS
  int read(STD_ISTREAM &);
  int write(STD_OSTREAM & os) const;
#endif

  //friend int operator==<>(const GAAlleleSet<T> &, const GAAlleleSet<T> &);
  //friend int operator!=<>(const GAAlleleSet<T> &, const GAAlleleSet<T> &);

  //friend int operator==<T>(const GAAlleleSet<T> &, const GAAlleleSet<T> &);
  //friend int operator!=<T>(const GAAlleleSet<T> &, const GAAlleleSet<T> &);

  friend int operator==(const GAAlleleSet<T> &, const GAAlleleSet<T> &);
  friend int operator!=(const GAAlleleSet<T> &, const GAAlleleSet<T> &);

protected:
  GAAlleleSetCore<T> *core;
};


template <class T>
class GAAlleleSetArray {
public:
  GAAlleleSetArray();
  GAAlleleSetArray(const GAAlleleSet<T>& s);
  GAAlleleSetArray(const GAAlleleSetArray<T>&);
  virtual ~GAAlleleSetArray();
  GAAlleleSetArray<T>& operator=(const GAAlleleSetArray<T>&);

  int size() const {return sz;}
  const GAAlleleSet<T>& set(unsigned int i) const {return *(aset[i]);}
  int add(const GAAlleleSet<T>& s);
  int add(unsigned int n, const T a[]);
  int add(const T& lower, const T& upper,
      GAAllele::BoundType lb=GAAllele::INCLUSIVE,
      GAAllele::BoundType ub=GAAllele::INCLUSIVE);
  int add(const T& lower, const T& upper, const T& increment,
      GAAllele::BoundType lb=GAAllele::INCLUSIVE,
      GAAllele::BoundType ub=GAAllele::INCLUSIVE);
  int remove(unsigned int);

protected:
  unsigned int csz;
  unsigned int sz;
  unsigned int SZ;
  GAAlleleSet<T> **aset;
};


#ifdef GALIB_USE_STREAMS
template <class T> inline STD_OSTREAM &
operator<< (STD_OSTREAM & os, const GAAlleleSet<T> & arg)
{ arg.write(os); return os; }
template <class T> inline STD_ISTREAM &
operator>> (STD_ISTREAM & is, GAAlleleSet<T> & arg)
{ arg.read(is); return is; }
#endif


/* ----------------------------------------------------------------------------
  allele.C
  mbwall 21mar95
  Copyright (c) 1995 Massachusetts Institute of Technology
                     all rights reserved

 DESCRIPTION:
   Definition for the allele set class and its core container.
---------------------------------------------------------------------------- */

#define GA_ALLELE_CHUNK 10

template <class T> inline
GAAlleleSetCore<T>::GAAlleleSetCore() :
type(GAAllele::ENUMERATED), csz(GA_ALLELE_CHUNK), sz(0), SZ(0), a(0) {
  lowerb = GAAllele::NONE;
  upperb = GAAllele::NONE;

  cnt = 1;
}

template <class T> inline
GAAlleleSetCore<T>::GAAlleleSetCore(unsigned int n, const T array []) :
type(GAAllele::ENUMERATED), csz(GA_ALLELE_CHUNK), sz(n), SZ(GA_ALLELE_CHUNK) {
  while(SZ < sz) SZ += csz;
  a = new T [SZ];
//  memcpy(a, array, n*sizeof(T));
  for(unsigned int i=0; i<sz; i++)
    a[i] = *(array+i);
  lowerb = GAAllele::NONE;
  upperb = GAAllele::NONE;

  cnt = 1;
}

template <class T> inline
GAAlleleSetCore<T>::
GAAlleleSetCore(const T& lower, const T& upper,
        GAAllele::BoundType lb, GAAllele::BoundType ub) :
type(GAAllele::BOUNDED), csz(GA_ALLELE_CHUNK), sz(2), SZ(2), a(new T[2]) {
  a[0] = lower;
  a[1] = upper;
  lowerb = lb;
  upperb = ub;

  cnt = 1;
}

template <class T> inline
GAAlleleSetCore<T>::
GAAlleleSetCore(const T& lower, const T& upper, const T& increment,
        GAAllele::BoundType lb, GAAllele::BoundType ub) :
type(GAAllele::DISCRETIZED), csz(GA_ALLELE_CHUNK), sz(3), SZ(3), a(new T[3]) {
  a[0] = lower;
  a[1] = upper;
  a[2] = increment;
  lowerb = lb;
  upperb = ub;

  cnt = 1;
}

// We do not copy the original's reference count!
template <class T> inline
GAAlleleSetCore<T>::GAAlleleSetCore(const GAAlleleSetCore<T> & orig) :
csz(orig.csz), sz(orig.sz), SZ(orig.SZ), a(new T[orig.SZ]) {
//  memcpy(a, orig.a, sz*sizeof(T));
  for(unsigned int i=0; i<sz; i++)
    a[i] = orig.a[i];
  lowerb = orig.lowerb;
  upperb = orig.upperb;
  type = orig.type;

  cnt = 1;
}

// This just trashes everything.  This shouldn't get called before the count
// reaches zero, so if it does, we post an error.
template <class T> inline
GAAlleleSetCore<T>::~GAAlleleSetCore(){
  if(cnt > 0) GAErr(GA_LOC, "GAAlleleSetCore", "destructor", gaErrRefsRemain);
  delete [] a;
}

// Copying the contents of another allele set core does NOT change the current
// count of the allele set core!
template <class T> inline GAAlleleSetCore<T> &
GAAlleleSetCore<T>::operator=(const GAAlleleSetCore<T> & orig){
  if(this == &orig) return *this;

  if(SZ < orig.sz){
    while(SZ < orig.sz) SZ += csz;
    delete [] a;
    a = new T [SZ];
  }

//  memcpy(a, orig.a, orig.sz*sizeof(T));
  for(unsigned int i=0; i<orig.sz; i++)
    a[i] = orig.a[i];

  sz = orig.sz;
  lowerb = orig.lowerb;
  upperb = orig.upperb;
  type = orig.type;

  return *this;
}


// When we link to another allele set, we point our core to that one.  Be sure
// that we have a core.  If not, just point.  If so, trash as needed.
template <class T> inline void
GAAlleleSet<T>::link(GAAlleleSet<T>& set){
  if(&set != this){
    if(core != 0){
      core->cnt -= 1;
      if(core->cnt == 0) delete core;
    }
    core=set.core; core->cnt += 1;
  }
}

template <class T> inline void
GAAlleleSet<T>::unlink(){
  if(core == 0) return;		// nothing to unlink
  if(core->cnt > 1){
    core->cnt -= 1;
    core = new GAAlleleSetCore<T>(*core);
  }
}


// If everthing goes OK, return 0.  If there's an error, we return -1.  I
// really wish there were enough compilers doing exceptions to use them...
template <class T> inline int
GAAlleleSet<T>::add(const T & alle){
  if(core == 0) core = new GAAlleleSetCore<T>;
  if(core->type != GAAllele::ENUMERATED) return 1;
  if(core->sz >= core->SZ){
    core->SZ += core->csz;
    T * tmp = core->a;
    core->a = new T [core->SZ];
    for(unsigned int i=0; i<core->sz; i++)
      core->a[i] = tmp[i];
//      memcpy(core->a, tmp, core->sz*sizeof(T));
    delete [] tmp;
  }
  core->a[core->sz] = alle;
  core->sz += 1;
  return 0;
}

template <class T> inline int
GAAlleleSet<T>::remove(const T & allele){
  if(core == 0) core = new GAAlleleSetCore<T>;
  if(core->type != GAAllele::ENUMERATED) return 1;
  for(unsigned int i=0; i<core->sz; i++){
    if(core->a[i] == allele){
      for(unsigned int j=i; j<core->sz-1; j++)
    core->a[j] = core->a[j+1];
//	memmove(&(core->a[i]), &(core->a[i+1]), (core->sz-i-1)*sizeof(T));
      core->sz -= 1;
      i = core->sz;		// break out of the loop
    }
  }
  return 0;
}

template <class T> inline int
GAAlleleSet<T>::remove(unsigned int x){
  for(unsigned int j=x; j<core->sz-1; j++)
    core->a[j] = core->a[j+1];
//  memmove(&(core->a[i]), &(core->a[i+1]), (core->sz-i-1)*sizeof(T));
  core->sz -= 1;
  return 0;
}


// When returning an allele from the set, we have to know what type we are.
// The allele that we return depends on the type.  If we're an enumerated set
// then just pick randomly from the list of alleles.  If we're a bounded set
// then pick randomly from the bounds, and respect the bound types.  If we're
// a discretized set then we do much as we would for the bounded set, but we
// respect the discretization.
//   Be sure to specialize this member function (see the real genome for an
// example of how to do this)
template <class T> inline T
GAAlleleSet<T>::allele() const {
  if(core->type == GAAllele::ENUMERATED)
    return core->a[GARandomInt(0, core->sz-1)];
  else if(core->type == GAAllele::DISCRETIZED){
    GAErr(GA_LOC, "GAAlleleSet", "allele(unsigned int)", gaErrOpUndef);
    return core->a[0];
  }
  else{
    GAErr(GA_LOC, "GAAlleleSet", "allele(unsigned int)", gaErrOpUndef);
    return core->a[0];
  }
}


// This works only for enumerated sets.  If someone tries to use this on a
// non-enumerated set then we post an error message.  No bounds checking on
// the value that was passed to us, but we do modulo it so that we'll never
// break.  Also, this means you can wrap an allele set around an array that
// is significantly larger than the allele set that defines its contents.
template <class T> inline T
GAAlleleSet<T>::allele(unsigned int i) const {
  if(core->type == GAAllele::ENUMERATED)
    return core->a[i % core->sz];
  else if(core->type == GAAllele::DISCRETIZED){
    GAErr(GA_LOC, "GAAlleleSet", "allele(unsigned int)", gaErrOpUndef);
    return core->a[0];
  }
  else{
    GAErr(GA_LOC, "GAAlleleSet", "allele(unsigned int)", gaErrNoAlleleIndex);
    return core->a[0];
  }
}


#ifdef GALIB_USE_STREAMS
template <class T> inline int
GAAlleleSet<T>::read(STD_ISTREAM&){
  GAErr(GA_LOC, "GAAlleleSet", "read", gaErrOpUndef);
  return 1;
}
template <class T> inline int
GAAlleleSet<T>::write(STD_OSTREAM &) const {
  GAErr(GA_LOC, "GAAlleleSet", "write", gaErrOpUndef);
  return 1;
}
#endif

// could do these with a memcmp if the type is simple...
template <class T> inline int
operator==(const GAAlleleSet<T> & a, const GAAlleleSet<T> & b) {
  if(a.core == b.core) return 1;
  if(a.core == 0 || b.core == 0) return 0;
  if(a.core->sz != b.core->sz) return 0;
// return((memcmp(a.core, b.core, a.core->sz * sizeof(T)) == 0) ? 1 : 0);
  unsigned int i;
  for(i=0; i<a.core->sz && a.core->a[i] == b.core->a[i]; i++) ;
  return((i == a.core->sz) ? 1 : 0);
}

template <class T> inline int
operator!=(const GAAlleleSet<T> & a, const GAAlleleSet<T> & b) {
  if(a.core == b.core) return 0;
  if(a.core == 0 || b.core == 0) return 1;
  if(a.core->sz != b.core->sz) return 1;
// return((memcmp(a.core, b.core, a.core->sz * sizeof(T)) == 0) ? 0 : 1);
  unsigned int i;
  for(i=0; i<a.core->sz && a.core->a[i] == b.core->a[i]; i++) ;
  return((i == a.core->sz) ? 0 : 1);
}


template <class T> inline
GAAlleleSetArray<T>::GAAlleleSetArray() :
csz(GA_ALLELE_CHUNK), sz(0), SZ(0), aset(0) {}

template <class T> inline
GAAlleleSetArray<T>::GAAlleleSetArray(const GAAlleleSet<T>& s) :
csz(GA_ALLELE_CHUNK), sz(1), SZ(GA_ALLELE_CHUNK),
aset(new GAAlleleSet<T> * [GA_ALLELE_CHUNK]) {
  aset[0] = new GAAlleleSet<T>(s);
}

template <class T> inline
GAAlleleSetArray<T>::GAAlleleSetArray(const GAAlleleSetArray<T>& orig) :
csz(orig.csz), sz(orig.sz), SZ(orig.SZ),
aset(new GAAlleleSet<T> * [orig.SZ]) {
  for(unsigned int i=0; i<sz; i++)
    aset[i] = new GAAlleleSet<T>(orig.set(i));
}

template <class T> inline
GAAlleleSetArray<T>::~GAAlleleSetArray(){
  for(unsigned int i=0; i<sz; i++)
    delete aset[i];
  delete [] aset;
}

template <class T> inline GAAlleleSetArray<T>&
GAAlleleSetArray<T>::operator=(const GAAlleleSetArray<T>& orig){
  if(this == &orig) return *this;
  unsigned int i;
  for(i=0; i<sz; i++)
    delete aset[i];

  if(SZ < orig.sz){
    while(SZ < orig.sz) SZ += csz;
    delete [] aset;
    aset = new GAAlleleSet<T> * [SZ];
  }
  for(i=0; i<orig.sz; i++)
    aset[i] = new GAAlleleSet<T>(orig.set(i));

  sz = orig.sz;

  return *this;
}

// Return 0 if everything goes OK, otherwise return -1
template <class T> inline int
GAAlleleSetArray<T>::add(const GAAlleleSet<T>& s){
  if(sz+1 > SZ){
    SZ += csz;
    GAAlleleSet<T>** tmp = aset;
    aset = new GAAlleleSet<T> * [SZ];
    memcpy(aset, tmp, sz * sizeof(GAAlleleSet<T>*));
    delete [] tmp;
  }
  aset[sz] = new GAAlleleSet<T>(s);
  sz++;
  return 0;
}

template <class T> inline int
GAAlleleSetArray<T>::add(unsigned int n, const T a[]){
  if(sz+1 > SZ){
    SZ += csz;
    GAAlleleSet<T>** tmp = aset;
    aset = new GAAlleleSet<T> * [SZ];
    memcpy(aset, tmp, sz * sizeof(GAAlleleSet<T>*));
    delete [] tmp;
  }
  aset[sz] = new GAAlleleSet<T>(n, a);
  sz++;
  return 0;
}

template <class T> inline int
GAAlleleSetArray<T>::add(const T& lower, const T& upper,
             GAAllele::BoundType lb, GAAllele::BoundType ub){
  if(sz+1 > SZ){
    SZ += csz;
    GAAlleleSet<T>** tmp = aset;
    aset = new GAAlleleSet<T> * [SZ];
    memcpy(aset, tmp, sz * sizeof(GAAlleleSet<T>*));
    delete [] tmp;
  }
  aset[sz] = new GAAlleleSet<T>(lower,upper,lb,ub);
  sz++;
  return 0;
}

template <class T> inline int
GAAlleleSetArray<T>::add(const T& lower, const T& upper, const T& increment,
             GAAllele::BoundType lb, GAAllele::BoundType ub){
  if(sz+1 > SZ){
    SZ += csz;
    GAAlleleSet<T>** tmp = aset;
    aset = new GAAlleleSet<T> * [SZ];
    memcpy(aset, tmp, sz * sizeof(GAAlleleSet<T>*));
    delete [] tmp;
  }
  aset[sz] = new GAAlleleSet<T>(lower,upper,increment,lb,ub);
  sz++;
  return 0;
}

template <class T> inline int
GAAlleleSetArray<T>::remove(unsigned int n){
  if(n >= sz) return 1;
  delete aset[n];
  for(unsigned int i=n; i<sz-1; i++)
    aset[i] = aset[i+1];
  sz--;
  return 0;
}

#endif
