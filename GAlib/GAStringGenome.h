/* ----------------------------------------------------------------------------
  string.h
  mbwall 25feb95
  Copyright (c) 1995 Massachusetts Institute of Technology

 DESCRIPTION:
  This header defines the interface for the string genome.
---------------------------------------------------------------------------- */
#ifndef _ga_string_h_
#define _ga_string_h_

#include <GAAllele.h>
#include <GA1DArrayGenome.h>

typedef GAAlleleSet<char> GAStringAlleleSet;
typedef GAAlleleSet<char> GACharacterAlleleSet;
typedef GAAlleleSetArray<char> GAStringAlleleSetArray;
typedef GA1DArrayAlleleGenome<char> GAStringGenome;

/* ----------------------------------------------------------------------------
  string.C
  mbwall 21mar95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology
                          all rights reserved

 DESCRIPTION:
   Source file for the string specialization of the array genome.
---------------------------------------------------------------------------- */

template <> inline const char *
GA1DArrayAlleleGenome<char>::className() const {return "GAStringGenome";}
template <> inline int
GA1DArrayAlleleGenome<char>::classID() const {return GAID::StringGenome;}

template <> inline GA1DArrayAlleleGenome<char>::
GA1DArrayAlleleGenome(unsigned int length, const GAAlleleSet<char> & s,
              GAGenome::Evaluator f, void * u) :
GA1DArrayGenome<char>(length, f, u){
  naset = 1;
  aset = new GAAlleleSet<char>[1];
  aset[0] = s;

  initializer(DEFAULT_STRING_INITIALIZER);
  mutator(DEFAULT_STRING_MUTATOR);
  comparator(DEFAULT_STRING_COMPARATOR);
  crossover(DEFAULT_STRING_CROSSOVER);
}

template <> inline GA1DArrayAlleleGenome<char>::
GA1DArrayAlleleGenome(const GAAlleleSetArray<char> & sa,
              GAGenome::Evaluator f, void * u) :
GA1DArrayGenome<char>(sa.size(), f, u){
  naset = sa.size();
  aset = new GAAlleleSet<char>[naset];
  for(int i=0; i<naset; i++)
    aset[i] = sa.set(i);

  initializer(DEFAULT_STRING_INITIALIZER);
  mutator(DEFAULT_STRING_MUTATOR);
  comparator(DEFAULT_STRING_COMPARATOR);
  crossover(DEFAULT_STRING_CROSSOVER);
}

template <> inline
GA1DArrayAlleleGenome<char>::~GA1DArrayAlleleGenome(){
  delete [] aset;
}


#ifdef GALIB_USE_STREAMS
// The read specialization takes in each character whether it is whitespace or
// not and stuffs it into the genome.  This is unlike the default array read.
template <> inline int
GA1DArrayAlleleGenome<char>::read(STD_ISTREAM & is)
{
  unsigned int i=0;
  char c;
  do{
    is.get(c);
    if(!is.fail()) gene(i++, c);
  } while(!is.fail() && !is.eof() && i < nx);

  if(is.eof() && i < nx){
    GAErr(GA_LOC, className(), "read", gaErrUnexpectedEOF);
    is.clear(STD_IOS_BADBIT | is.rdstate());
    return 1;
  }
  return 0;
}

// Unlike the base array genome, here when we write out we don't put any
// whitespace between genes.  No newline at end of it all.
template <> inline int
GA1DArrayAlleleGenome<char>::write(STD_OSTREAM & os) const
{
  for(unsigned int i=0; i<nx; i++)
    os << gene(i);
  return 0;
}
#endif


inline void GAStringUniformInitializer(GAGenome& g){
  GA1DArrayAlleleGenome<char>::UniformInitializer(g);
}
inline void GAStringOrderedInitializer(GAGenome& g){
  GA1DArrayAlleleGenome<char>::OrderedInitializer(g);
}

inline int GAStringFlipMutator(GAGenome& g, double pmut){
  return GA1DArrayAlleleGenome<char>::FlipMutator(g, pmut);
}
inline int GAStringSwapMutator(GAGenome& g, double pmut){
  return GA1DArrayGenome<char>::SwapMutator(g, pmut);
}

inline int GAStringUniformCrossover(const GAGenome& a, const GAGenome& b,
                  GAGenome* c, GAGenome* d) {
  return GA1DArrayGenome<char>::UniformCrossover(a,b,c,d);
}
inline int GAStringEvenOddCrossover(const GAGenome& a, const GAGenome& b,
                  GAGenome* c, GAGenome* d) {
  return GA1DArrayGenome<char>::EvenOddCrossover(a,b,c,d);
}
inline int GAStringOnePointCrossover(const GAGenome& a, const GAGenome& b,
                  GAGenome* c, GAGenome* d) {
  return GA1DArrayGenome<char>::OnePointCrossover(a,b,c,d);
}
inline int GAStringTwoPointCrossover(const GAGenome& a, const GAGenome& b,
                  GAGenome* c, GAGenome* d) {
  return GA1DArrayGenome<char>::TwoPointCrossover(a,b,c,d);
}
inline int GAStringPartialMatchCrossover(const GAGenome& a, const GAGenome& b,
                  GAGenome* c, GAGenome* d) {
  return GA1DArrayGenome<char>::PartialMatchCrossover(a,b,c,d);
}
inline int GAStringOrderCrossover(const GAGenome& a, const GAGenome& b,
                  GAGenome* c, GAGenome* d) {
  return GA1DArrayGenome<char>::OrderCrossover(a,b,c,d);
}
inline int GAStringCycleCrossover(const GAGenome& a, const GAGenome& b,
                  GAGenome* c, GAGenome* d) {
  return GA1DArrayGenome<char>::CycleCrossover(a,b,c,d);
}

#endif
