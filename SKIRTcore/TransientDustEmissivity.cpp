/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <limits>
#include "DustDistribution.hpp"
#include "MultiGrainDustMix.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "TransientDustEmissivity.hpp"
#include "Table.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

// container classes that are highly specialized to optimize the operations in this class
namespace
{
    // square matrix
    template<typename T> class Square
    {
    private:
        size_t _n;
        T* _v;

    public:
        // constructor sets maximum size and logical size
        Square(size_t n) : _n(n), _v(new T[n*n]) { }
        ~Square() { delete[] _v; }

        // sets logical size, which must not be larger than maximum size set in constructor (is not checked)
        // does not clear values; does not resize underlying memory
        void resize(size_t n) { _n = n; }

        // access to values  (const version currently not needed)
        T& operator()(size_t i, size_t j) { return _v[i*_n+j]; }
    };

    // square matrix with only items below the diagonal (i>j)
    template<typename T> class Triangle
    {
    private:
        T* _v;
        static size_t offset(size_t i) { return ((i-1)*i)>>1; }

    public:
        // constructor sets size (can't be changed)
        Triangle(size_t n) : _v(new T[offset(n)]) { }
        ~Triangle() { delete[] _v; }

        // access to values; must have i>j (is not checked)
        const T& operator()(size_t i, size_t j) const { return _v[offset(i)+j]; }
        T& operator()(size_t i, size_t j) { return _v[offset(i)+j]; }
    };
}

////////////////////////////////////////////////////////////////////

// helper class to store a particular temperature grid and the corresponding black-body radiation;
// all members are public for ease of use in TDE_Calculator
class TDE_Grid
{
public:
    // physical constants
    double _hc, _hc2, _k;

    // wavelength grid (indexed on ell)
    const WavelengthGrid* _lambdagrid;
    const Array& _lambdav;
    const Array& _dlambdav;
    int _Nlambda;

    // temperature grid (indexed on i)
    Array _Tv;
    int _NT;

    // black-body radiation (indexed on i and ell)
    ArrayTable<2> _Bvv;

    // black-body radiation (given T and ell)
    double B(double T, int ell) const
    {
        double lambda = _lambdav[ell];
        double lambda5 = lambda*lambda*lambda*lambda*lambda;
        double x = _hc / (lambda*_k*T);
        return _hc2 / lambda5 / (exp(x)-1.0);
    }

    // constructor
    TDE_Grid(const WavelengthGrid* lambdagrid, double Tmin, double Tmax, int NT, double ratio)
        : _hc(Units::h()*Units::c()), _hc2(2*_hc*Units::c()), _k(Units::k()),
          _lambdagrid(lambdagrid), _lambdav(lambdagrid->lambdav()), _dlambdav(lambdagrid->dlambdav()),
          _Nlambda(lambdagrid->Nlambda())
    {
        // build temperature grid (linear if ratio==1)
        NR::powgrid(_Tv, Tmin, Tmax, NT-1, ratio);
        _NT = NT;

        // pre-calculate black-body radiation
        _Bvv.resize(NT,_Nlambda);
        for (int i=0; i<NT; i++)
            for (int ell=0; ell<_Nlambda; ell++)
                _Bvv(i,ell) = B(_Tv[i],ell);
    }
};

////////////////////////////////////////////////////////////////////

// helper class to calculate the temperature probability distribution for a dust population in a given radiation field
// on a particular fixed grid, with support for pre-calculating enthalpy-related data on that grid
class TDE_Calculator
{
private:
    const TDE_Grid* _grid;
    const MultiGrainDustMix* _mix;
    const Array& _sigmaabsv;
    Triangle<double> _HRm;  // heating rates (indexed on f,i)
    Triangle<short> _ELLm;  // radiation field wavelength index (indexed on f,i)
    Array _CRv;             // cooling rates (indexed on i)

public:
    TDE_Calculator(const TDE_Grid* grid, const MultiGrainDustMix* mix, int c)
        : _grid(grid), _mix(mix), _sigmaabsv(mix->sigmaabsv(c)), _HRm(grid->_NT), _ELLm(grid->_NT), _CRv(grid->_NT)
    {
        // get info from grid
        double hc = grid->_hc;
        const Array& Tv = grid->_Tv;
        int NT = grid->_NT;
        int Nlambda = grid->_Nlambda;

        // precalculate heating and cooling rates for this population
        Array Hv(NT);
        Array dHv(NT);

        // calculate the enthalpy of a single dust grain in this population across the temperature grid
        for (int i=0; i<NT; i++) Hv[i] = _mix->enthalpy(Tv[i],c);

        // calculate the enthalpy bin widths
        dHv[0] = Hv[1]-Hv[0];
        for (int i=1; i<NT-1; i++)
        {
            double Tmin = (Tv[i-1]+Tv[i])/2.;
            double Tmax = (Tv[i+1]+Tv[i])/2.;
            dHv[i] = _mix->enthalpy(Tmax,c) - _mix->enthalpy(Tmin,c);
        }
        dHv[NT-1] = Hv[NT-1]-Hv[NT-2];

        // calculate the heating rates, barring the dependency on the radiation field
        for (int f=1; f<NT; f++)
        {
            for (int i=0; i<f; i++)
            {
                double Hdiff = Hv[f] - Hv[i];
                double lambda = hc / Hdiff;
                int ell = grid->_lambdagrid->nearest(lambda);
                if (ell>=0) _HRm(f,i) = hc * _sigmaabsv[ell] * dHv[f] / (Hdiff*Hdiff*Hdiff);
                _ELLm(f,i) = ell;
            }
        }

        // calculate the cooling rates
        for (int i=1; i<NT; i++)
        {
            double Hdiff = Hv[i] - Hv[i-1];
            double sum = 0.;
            const Array& Bv = _grid->_Bvv[i];
            for (int ell = 0; ell<Nlambda; ell++)
            {
                sum += _sigmaabsv[ell] * Bv[ell] * grid->_dlambdav[ell];
            }
            _CRv[i] = sum / Hdiff;
        }
    }

    // calculate the probabilities
    // Pv: the calculated probabilities (out)
    // ioff: the index offset in the temperature grid used for this calculation (out)
    // Am: scratch memory for the calculation (internal only)
    // Tmin/Tmax: temperature range in which to perform the calculation (in), and
    //            temperature range where the calculated probabilities are above a certain fraction of maximum (out)
    // Jv: the radiation field (in)
    void calcprobs(Array& Pv, int& ioff, Square<double>& Am, double& Tmin, double& Tmax, const Array& Jv) const
    {
        ioff = NR::locate_clip(_grid->_Tv, Tmin);
        int NT = NR::locate_clip(_grid->_Tv, Tmax) - ioff + 2;

        // copy/calculate the transition matrix coefficients
        Am.resize(NT);
        for (int f=1; f<NT; f++)
        {
            const short* ELLv = &_ELLm(f+ioff,ioff);
            const double* HRv = &_HRm(f+ioff,ioff);
            for (int i=0; i<f; i++)
            {
                int ell = ELLv[i];
                Am(f,i) = ell>=0 ? HRv[i] * Jv[ell] : 0.;
            }
        }
        for (int i=1; i<NT; i++)
        {
            Am(i-1,i) = _CRv[i+ioff];
        }

        // calculate the cumulative matrix coefficients, in place
        for (int f=NT-2; f>0; f--)
        {
            for (int i=0; i<f; i++)
            {
                Am(f,i) += Am(f+1,i);
            }
        }

        // calculate the probabilities
        Pv.resize(NT);
        Pv[0] = 1.;
        for (int i=1; i<NT; i++)
        {
            double sum = 0.;
            for (int j=0; j<i; j++) sum += Am(i,j) * Pv[j];
            Pv[i] = sum / Am(i-1,i);

            // rescale if needed to keep infinities from happening
            if (Pv[i] > 1e10) for (int j=0; j<=i; j++) Pv[j]/=Pv[i];
        }

        // normalize probabilities to unity
        Pv /= Pv.sum();

        // determine the temperature range where the probabability is above a given fraction of its maximum
        double frac = 1e-20 * Pv.max();
        int k;
        for (k=0; k!=NT-2; k++) if (Pv[k]>frac) break;
        Tmin = _grid->_Tv[k+ioff];
        for (k=NT-2; k!=1; k--) if (Pv[k]>frac) break;
        Tmax = _grid->_Tv[k+1+ioff];
    }

    // add the transient emissivity of the population
    // ev: the accumulated emissivity (in/out)
    // Tmin/Tmax: temperature range in which to add radiation (in)
    // Pv: the probabilities calculated previously by this calculator (in)
    // ioff: the index offset in the temperature grid used for that previous calculation (in)
    void addtransient(Array& ev, double Tmin, double Tmax, const Array& Pv, int ioff) const
    {
        int imin = NR::locate_clip(_grid->_Tv, Tmin);
        int imax = NR::locate_clip(_grid->_Tv, Tmax);

        for (int i=imin; i<=imax; i++)
        {
            ev += _sigmaabsv * _grid->_Bvv[i] * Pv[i-ioff];
        }
    }

    // add the equilibrium emissivity of the population
    void addequilibrium(Array& ev, double Teq) const
    {
        for (int ell=0; ell<_grid->_Nlambda; ell++)
        {
            ev[ell] += _sigmaabsv[ell] * _grid->B(Teq, ell);
        }
    }
};

////////////////////////////////////////////////////////////////////

// configuration constants
namespace
{
    // we construct three specialized temperature grids:
    // - a coarse grid for quickly determining the appropriate temperature range
    //   (with smaller bins in the lower temperature range, which sees action most often)
    // - a medium and a fine grid for performing the actual calculation (with equally-spaced bins)
    const double Tuppermax = 3000;  // the largest temperature taken into account
    const int NTA = 20;             // the number of grid points in the coarse grid
    const double ratioA = 500.;     // the ratio between the largest and smallest bins in the coarse grid
    const double widthB = 4.;       // the average width of bins in the medium grid, in K
    const double ratioB = 1.;       // the ratio between the largest and smallest bins in the medium grid
    const double widthC = 2.;       // the average width of bins in the fine grid, in K
    const double ratioC = 3.;       // the ratio between the largest and smallest bins in the fine grid
    const double deltaTmedium = 200.;  // the smallest temperature range for which the medium grid is used

    // considering the temperature range over which the probability is nonzero (or larger than a very small fraction);
    // we assume the population to be in equilibrium if one or both of the following conditions is true:
    // - the temperature range is smaller than a given delta-T (i.e. it resembles a delta function)
    // - the equilibrium temperature lies outside of the temperature range
    const double deltaTeq = 10.;    // the cutoff width of the temperature range
}

////////////////////////////////////////////////////////////////////

TransientDustEmissivity::TransientDustEmissivity()
    : _Nlambda(0)
{
}

////////////////////////////////////////////////////////////////////

TransientDustEmissivity::~TransientDustEmissivity()
{
    foreach (const TDE_Calculator* calculator, _calculatorsA.values()) delete calculator;
    foreach (const TDE_Calculator* calculator, _calculatorsB.values()) delete calculator;
    foreach (const TDE_Grid* grid, _grids) delete grid;
}

////////////////////////////////////////////////////////////////////

void TransientDustEmissivity::setupSelfBefore()
{
    DustEmissivity::setupSelfBefore();

    // ensure that all dust mixes in the dust system are of type MultiGrainDustMix
    // and construct the appropriate grids and calculators for each of the dust mixes
    find<Log>()->info("Precalculating cached values for transient dust emissivity computations...");
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    _Nlambda = lambdagrid->Nlambda();
    DustDistribution* dd = find<DustDistribution>();
    int Ncomp = dd->Ncomp();
    for (int h=0; h<Ncomp; h++)
    {
        const MultiGrainDustMix* mix = dd->mix(h)->find<MultiGrainDustMix>();

        // create grids
        double Tupper = min(Tuppermax, mix->uppertemperature());
        const TDE_Grid* gridA = new TDE_Grid(lambdagrid, 2.,Tupper,NTA,ratioA);
        const TDE_Grid* gridB = new TDE_Grid(lambdagrid, 2.,Tupper,Tupper/widthB,ratioB);
        const TDE_Grid* gridC = new TDE_Grid(lambdagrid, 2.,Tupper,Tupper/widthC,ratioC);
        _grids << gridA << gridB << gridC;

        // create calculators
        int Npop = mix->Npop();
        for (int c=0; c<Npop; c++)
        {
            _calculatorsA.insert(QPair<const DustMix*,int>(mix,c), new TDE_Calculator(gridA,mix,c));
            _calculatorsB.insert(QPair<const DustMix*,int>(mix,c), new TDE_Calculator(gridB,mix,c));
            _calculatorsC.insert(QPair<const DustMix*,int>(mix,c), new TDE_Calculator(gridC,mix,c));
        }
    }
}

////////////////////////////////////////////////////////////////////

Array TransientDustEmissivity::emissivity(const DustMix* mix, const Array& Jv) const
{
    const MultiGrainDustMix* mgmix = mix->find<MultiGrainDustMix>();

    // This dictionary is updated as the loop over all dust populations in the mix proceeds.
    // For each type of grain composition, it keeps track of the grain mass above which
    // the dust population is most certainly in equilibrium.
    QHash<QString,double> eqMass;

    // provide room for the probabilities calculated over each of the temperature grids
    Array Pv;
    Square<double> Am(ceil(Tuppermax)+1);

    // accumulate the emissivities for all populations in the dust mix
    Array ev(_Nlambda);
    int Npop = mix->Npop();
    for (int c=0; c<Npop; c++)
    {
        // get the coarse calculator for this population
        const TDE_Calculator* calculatorA = _calculatorsA.value(qMakePair(mix,c));

        // determine the equilibrium temperature for this population
        double Teq = mix->equilibrium(Jv,c);

        // consider transient calculation only if the mean mass for this population is below the cutoff mass
        QString gcname = mgmix->gcname(c);
        double meanmass = mgmix->meanmass(c);
        if (meanmass < eqMass.value(gcname,numeric_limits<double>::infinity()))
        {
            // calculate the probabilities over the coarse temperature grid
            double Tmin = 0;
            double Tmax = Tuppermax;
            int ioff = 0;
            calculatorA->calcprobs(Pv, ioff, Am, Tmin, Tmax, Jv);

            // if the population might be transient...
            if (Tmax-Tmin > deltaTeq && Teq < Tmax)
            {
                // select the medium or fine temperature grid depending on the temperature range
                const TDE_Calculator* calculator = (Tmax-Tmin > deltaTmedium)
                                                   ? _calculatorsB.value(qMakePair(mix,c))
                                                   : _calculatorsC.value(qMakePair(mix,c));

                // calculate the probabilities over the chosen grid, in the range determined by the coarse calculation
                calculator->calcprobs(Pv, ioff, Am, Tmin, Tmax, Jv);

                // if the population indeed is transient...
                if (Tmax-Tmin > deltaTeq && Teq < Tmax)
                {
                    // add the transient emissivity of this population to the running total
                    calculator->addtransient(ev, Tmin, Tmax, Pv, ioff);
                    continue;
                }
            }

            // remember that all grains above this mass will be in equilibrium
            eqMass[gcname] = meanmass;
        }

        // otherwise, add the equilibrium emissivity of this population to the running total
        calculatorA->addequilibrium(ev, Teq);
    }

    // convert emissivity from "per hydrogen atom" to "per unit mass"
    ev /= mix->mu();
    return ev;
}

////////////////////////////////////////////////////////////////////

int TransientDustEmissivity::logfrequency() const
{
    return 1;
}

////////////////////////////////////////////////////////////////////
