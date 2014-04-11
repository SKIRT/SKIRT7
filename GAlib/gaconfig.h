/* ----------------------------------------------------------------------------
  config.h
  mbwall 27jun95
  Copyright (c) 1995-1996 Massachusetts Institute of Technology
                          all rights reserved
  Copyright (c) 1998-2005 Matthew Wall
                          all rights reserved
---------------------------------------------------------------------------- */
#ifndef _ga_config_h_
#define _ga_config_h_


/* ----------------------------------------------------------------------------
PREPROCESSOR DIRECTIVES

  Here are the preprocessor directives that the library understands.

  GALIB_USE_ANSI_HEADERS

                      Some operating systems/compilers have old-style headers
                      (e.g. <iostream.h>) while others have new-school
              headers (e.g. <iostream>).  The default is to use
              ANSI headers.  If you are using an older compiler,
              make sure this variable is not defined.

  GALIB_USE_STD_NAMESPACE

                      Some platforms need to explicitly use the std namespace
              when referring to streams and other std components, but
              others do not.

  GALIB_USE_STREAMS

                      For systems/environments in which streams are not desired
                      and/or required.  If this is not defined, it turns off
                      errors and all of the read/write routines for the
                      classes.

  GALIB_USE_RTTI

                      If your compiler supports RTTI, or if you turn on
                      the RTTI abilities of your compiler, then define this
                      macro.  Without RTTI, if improper casts are made,
              things will die horribly rather than dropping out in an
              RTTI-induced exception.

  GALIB_USE_PID

                     Define this if the system has a getpid function that
                     returns something sane and useful.

  GALIB_USE_RAN1      These specify which random number function to use.  Only
  GALIB_USE_RAN2      one of these may be specified.  You may have to tweak
  GALIB_USE_RAN3      random.h a bit as well (these functions are not defined
  GALIB_USE_RAND      the same way on each platform).  For best results, use
  GALIB_USE_RANDOM    ran2 or ran3 (performance is slightly slower than most
  GALIB_USE_RAND48    system RNGs, but you'll get better results).

                      If you want to use another random number generator you
                      must hack random.h directly (see the comments in that
                      file).

  GALIB_BITBASE       The built-in type to use for bit conversions.  This
                      should be set to the type of the largest integer that
                      your system supports.  If you have long long int then
                      use it.  If you don't plan to use more than 16 or 32
                      bits to represent your binary-to-decimal mappings then
                      you can use something smaller (long int for example).
                      If you do not set this, GAlib will automatically use
                      the size of a long int.  The bitbase determines the
              maximum number of bits you can use to represent a
              decimal number in the binary-to-decimal genomes.

  GALIB_BITS_IN_WORD  How many bits are in a word?  For many systems, a word is
                      a char and is 8 bits long.

---------------------------------------------------------------------------- */

#define GALIB_USE_ANSI_HEADERS
#define GALIB_USE_STD_NAMESPACE
#define GALIB_USE_STREAMS
#define GALIB_USE_RTTI
#define GALIB_USE_PID
#define GALIB_USE_RAN2
#define GALIB_BITBASE long int
#define GALIB_BITS_IN_WORD 8


// If the system/compiler understands C++ casts, then we use them.  Otherwise
// we default to the C-style casts.  The macros make explicit the fact that we
// are doing casts.
#if defined(GALIB_USE_RTTI)
#define DYN_CAST(type,x) (dynamic_cast<type>(x))
#define CON_CAST(type,x) (const_cast<type>(x))
#define STA_CAST(type,x) (static_cast<type>(x))
#define REI_CAST(type,x) (reinterpret_cast<type>(x))
#else
#define DYN_CAST(type,x) ((type)(x))
#define CON_CAST(type,x) ((type)(x))
#define STA_CAST(type,x) ((type)(x))
#define REI_CAST(type,x) ((type)(x))
#endif


/* ----------------------------------------------------------------------------
DEFAULT OPERATORS

  These directives determine which operators will be used by default for each
of the objects in GAlib.
---------------------------------------------------------------------------- */

// scaling schemes
#define USE_LINEAR_SCALING           1
#define USE_SIGMA_TRUNC_SCALING      1
#define USE_POWER_LAW_SCALING        1
#define USE_SHARING                  1

// selection schemes
#define USE_RANK_SELECTOR            1
#define USE_ROULETTE_SELECTOR        1
#define USE_TOURNAMENT_SELECTOR      1
#define USE_DS_SELECTOR              1
#define USE_SRS_SELECTOR             1
#define USE_UNIFORM_SELECTOR         1

// These are the compiled-in defaults for various genomes and GA objects
#define DEFAULT_SCALING              GALinearScaling
#define DEFAULT_SELECTOR             GARouletteWheelSelector
#define DEFAULT_TERMINATOR           TerminateUponGeneration

#define DEFAULT_1DBINSTR_INITIALIZER UniformInitializer
#define DEFAULT_1DBINSTR_MUTATOR     FlipMutator
#define DEFAULT_1DBINSTR_COMPARATOR  BitComparator
#define DEFAULT_1DBINSTR_CROSSOVER   OnePointCrossover
#define DEFAULT_2DBINSTR_INITIALIZER UniformInitializer
#define DEFAULT_2DBINSTR_MUTATOR     FlipMutator
#define DEFAULT_2DBINSTR_COMPARATOR  BitComparator
#define DEFAULT_2DBINSTR_CROSSOVER   OnePointCrossover
#define DEFAULT_3DBINSTR_INITIALIZER UniformInitializer
#define DEFAULT_3DBINSTR_MUTATOR     FlipMutator
#define DEFAULT_3DBINSTR_COMPARATOR  BitComparator
#define DEFAULT_3DBINSTR_CROSSOVER   OnePointCrossover

#define DEFAULT_BIN2DEC_ENCODER      GABinaryEncode
#define DEFAULT_BIN2DEC_DECODER      GABinaryDecode
#define DEFAULT_BIN2DEC_COMPARATOR   BitComparator

#define DEFAULT_1DARRAY_INITIALIZER  NoInitializer
#define DEFAULT_1DARRAY_MUTATOR      SwapMutator
#define DEFAULT_1DARRAY_COMPARATOR   ElementComparator
#define DEFAULT_1DARRAY_CROSSOVER    OnePointCrossover
#define DEFAULT_2DARRAY_INITIALIZER  NoInitializer
#define DEFAULT_2DARRAY_MUTATOR      SwapMutator
#define DEFAULT_2DARRAY_COMPARATOR   ElementComparator
#define DEFAULT_2DARRAY_CROSSOVER    OnePointCrossover
#define DEFAULT_3DARRAY_INITIALIZER  NoInitializer
#define DEFAULT_3DARRAY_MUTATOR      SwapMutator
#define DEFAULT_3DARRAY_COMPARATOR   ElementComparator
#define DEFAULT_3DARRAY_CROSSOVER    OnePointCrossover

#define DEFAULT_1DARRAY_ALLELE_INITIALIZER  UniformInitializer
#define DEFAULT_1DARRAY_ALLELE_MUTATOR      FlipMutator
#define DEFAULT_1DARRAY_ALLELE_COMPARATOR   ElementComparator
#define DEFAULT_1DARRAY_ALLELE_CROSSOVER    OnePointCrossover
#define DEFAULT_2DARRAY_ALLELE_INITIALIZER  UniformInitializer
#define DEFAULT_2DARRAY_ALLELE_MUTATOR      FlipMutator
#define DEFAULT_2DARRAY_ALLELE_COMPARATOR   ElementComparator
#define DEFAULT_2DARRAY_ALLELE_CROSSOVER    OnePointCrossover
#define DEFAULT_3DARRAY_ALLELE_INITIALIZER  UniformInitializer
#define DEFAULT_3DARRAY_ALLELE_MUTATOR      FlipMutator
#define DEFAULT_3DARRAY_ALLELE_COMPARATOR   ElementComparator
#define DEFAULT_3DARRAY_ALLELE_CROSSOVER    OnePointCrossover

#define DEFAULT_STRING_INITIALIZER   UniformInitializer
#define DEFAULT_STRING_MUTATOR       FlipMutator
#define DEFAULT_STRING_COMPARATOR    ElementComparator
#define DEFAULT_STRING_CROSSOVER     UniformCrossover

#define DEFAULT_REAL_INITIALIZER     UniformInitializer
#define DEFAULT_REAL_MUTATOR         GARealGaussianMutator
#define DEFAULT_REAL_COMPARATOR      ElementComparator
#define DEFAULT_REAL_CROSSOVER       UniformCrossover

#define DEFAULT_TREE_INITIALIZER     NoInitializer
#define DEFAULT_TREE_MUTATOR         SwapSubtreeMutator
#define DEFAULT_TREE_COMPARATOR      TopologyComparator
#define DEFAULT_TREE_CROSSOVER       OnePointCrossover

#define DEFAULT_LIST_INITIALIZER     NoInitializer
#define DEFAULT_LIST_MUTATOR         SwapMutator
#define DEFAULT_LIST_COMPARATOR      NodeComparator
#define DEFAULT_LIST_CROSSOVER       OnePointCrossover

#endif
