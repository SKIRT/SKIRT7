/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TRANSIENTDUSTEMISSIVITY_HPP
#define TRANSIENTDUSTEMISSIVITY_HPP

#include <QHash>
#include "DustEmissivity.hpp"
class TDE_Calculator;
class TDE_Grid;

//////////////////////////////////////////////////////////////////////

/** The TransientDustEmissivity class calculates the emissivity of a particular dust mix in a given
    radiation field, including a full treatment of the transient heating of very small grains and
    PAH molecules. Since this requires access to enthalpy data for the various dust grain
    populations, all dust components in the dust system must have a dust mix that inherits from the
    MultiGrainDustMix class.

    Using the discretization of the dust composition and size distribution into a range of dust
    populations held by a MultiGrainDustMix object, the simulation's wavelength grid, and a
    specialized temperature grid constructed in this class, the emissivity in an interstellar
    radiation field \f$J_\lambda\f$ can be calculated as \f[ \varepsilon_\lambda = \frac{1}{\mu}
    \sum_{c=0}^{N_{\text{pop}}-1} \varsigma_{\lambda,c}^{\text{abs}}\,
    \sum_{i=0}^{N_{\text{temp}}-1} P_{c,i}\, B_\lambda(T_i) \f] with \f$\mu\f$ the total dust mass
    of the dust mix, \f$\varsigma_{\lambda,c}^{\text{abs}}\f$ the absorption cross section of the
    \f$c\f$'th dust population, \f$T_i\f$ the \f$i\f$'th temperature grid point, and
    \f$P_{c,i}\f$ the probability of finding a grain of the \f$c\f$'th dust population in the
    \f$i\f$'th temperature bin.

    The probabilities \f$P_{c,i}\f$ are calculated following a scheme based on Guhathakurta &
    Draine (ApJ 1989), Draine & Li (ApJ 2001), Kruegel (book, 2003), and Misselt et al. (arXiv,
    2008). The scheme is also nicely described in Verstappen (PhD thesis, 2013, section 2.5). An
    overview is presented below. To simplify the notation we focus on a single dust population,
    dropping the index \f$c\f$.

    We define a transition matrix \f$A_{f,i}\f$ describing the probability per unit time for a
    grain to transfer from initial temperature bin \f$i\f$ to final temperature bin \f$f\f$.
    The transition matrix elements in the case of heating \f$(f>i)\f$ are given by \f[ A_{f,i} =
    4\pi\, \varsigma_{\lambda_{fi}}^{\text{abs}}\,J_{\lambda_{fi}}\, \frac{hc\,\Delta
    H_f}{(H_f-H_i)^3} \f] where \f$H_f\f$ and \f$H_i\f$ are the enthalpies of the final and initial
    temperature bins, \f$\Delta H_f\f$ is the width of the final temperature bin, and
    \f$\lambda_{fi}\f$ is the transition wavelength which can be obtained from \f[
    \lambda_{fi}=\frac{hc}{H_f-H_i}. \f] We assume that cooling transitions occur only to the next
    lower level, so that \f$A_{f,i}=0\f$ for \f$f<i-1\f$ and \f[ A_{i-1,i} =
    \frac{4\pi}{H_i-H_{i-1}}\, \int_0^\infty \varsigma_{\lambda}^{\text{abs}}\, B_{\lambda}(T_i)\,
    {\text{d}}\lambda. \f] The diagonal matrix elements are defined as \f[ A_{i,i} = -\sum_{f\ne i}
    A_{f,i}\f] however as we will see below there is no need to explicitly calculate these values.

    Assuming a steady state situation, the probabilities \f$P_{i}\f$ can be obtained from the
    transition matrix by solving the set of \f$N\f$ linear equations \f[ \sum_{i=0}^{N-1} A_{f,i}
    \,P_i=0 \qquad f=0,...,N-1 \f] along with the normalization condition \f[ \sum_{i=0}^{N-1} P_i
    = 1, \f] where \f$N\f$ is the number of temperature bins. Because the matrix values for
    \f$f<i-1\f$ are zero these equations can be solved by a recursive procedure of computational
    order \f${\mathcal{O}(N^2)}\f$. To avoid numerical instabilities caused by the negative
    diagonal elements, the procedure employs a well-chosen linear combination of the original
    equations. This leads to the following recursion relations for the adjusted matrix elements
    \f$B_{f,i}\f$, the unnormalized probabilities \f$X_i\f$, and finally the normalized
    probabilities \f$P_{i}\f$: \f{align*} B_{N-1,i} &= A_{N-1,i} & i=0,\ldots,N-2 \\ B_{f,i} &=
    B_{f+1,i}+A_{f,i} & f=N-2,\ldots,1;\,i=0,\ldots,f-1 \\ X_0 &= 1 \\ X_i &=
    \frac{\sum_{j=0}^{i-1}B_{i,j}X_j}{A_{i-1,i}} & i=1,\ldots,N-1 \\ P_i &=
    \frac{X_i}{\sum_{j=0}^{N-1}X_j} & i=0,\ldots,N-1 \f}
*/
class TransientDustEmissivity : public DustEmissivity
{
    Q_OBJECT
    Q_CLASSINFO("Title", "transient heating dust emissivity (with full non-LTE treatment)")

    //============= Construction - Setup - Destruction =============

public:
    /** Default constructor. */
    Q_INVOKABLE TransientDustEmissivity();

    /** Destructor. */
    ~TransientDustEmissivity();

    /** This function verifies that all dust components in the dust system have a dust mix based on
        the MultiGrainDustMix class. */
    void setupSelfBefore();

    //======================== Other Functions =======================

public:
    /** This function returns the dust emissivity \f$\varepsilon_\ell\f$ at all wavelength indices
        \f$\ell\f$ for a dust mix of the specified type residing in the specified mean radiation
        field \f$J_\ell\f$, assuming the simulation's wavelength grid. */

    Array emissivity(const DustMix* mix, const Array& Jv) const;
    /** The return value of this function indicates a meaningful frequency for console-logging when
        repeatly invoking emissivity(). A value of zero means that the calculation is fast and thus
        there should be no logging. A value of one means that the calculation is slow and thus
        every invocation should be logged. Since the calculation in this class is quite slow, the
        function returns one, which means every invocation should be logged. */
    virtual int logfrequency() const;

    //========================= Data members =======================

private:
    int _Nlambda;

    // setupSelfBefore adds all grids to this list so that they stay around (and can be destructed)
    QList<const TDE_Grid*> _grids;

    // setupSelfBefore adds a calculator to these maps for each dust population in the simulation
    QHash< QPair<const DustMix*,int>, const TDE_Calculator* > _calculatorsA;     // coarse grid
    QHash< QPair<const DustMix*,int>, const TDE_Calculator* > _calculatorsB;     // medium grid
    QHash< QPair<const DustMix*,int>, const TDE_Calculator* > _calculatorsC;     // fine grid
};

////////////////////////////////////////////////////////////////////

#endif // TRANSIENTDUSTEMISSIVITY_HPP
