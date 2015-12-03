/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include "Dim1DustLib.hpp"
#include "FatalError.hpp"
#include "ISRF.hpp"
#include "Log.hpp"
#include "PanDustSystem.hpp"
#include "ProcessAssigner.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

Dim1DustLib::Dim1DustLib()
    : _NU(0)
{
}

////////////////////////////////////////////////////////////////////

void Dim1DustLib::setupSelfBefore()
{
    DustLib::setupSelfBefore();

    if (_NU < 10) throw FATALERROR("there must be at least 10 libary entries");
}

////////////////////////////////////////////////////////////////////

void Dim1DustLib::setEntries(int value)
{
    _NU = value;
}

////////////////////////////////////////////////////////////////////

int Dim1DustLib::entries() const
{
    return _NU;
}

////////////////////////////////////////////////////////////////////

std::vector<int> Dim1DustLib::mapping() const
{
    // get basic information about the wavelength grid and the dust system
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    PanDustSystem* ds = find<PanDustSystem>();
    int Ncells = _cellAssigner->nvalues();

    // calculate the properties of the ISRF in all cells of the dust system;
    // remember the minimum and maximum values of the strength of the ISRF
    double JtotMW = ( ISRF::mathis(lambdagrid) * lambdagrid->dlambdav() ).sum();
    double Umin = DBL_MAX;
    double Umax = 0.0;
    vector<double> Ucellv(Ncells);
    for (int m=0; m<Ncells; m++)
    {
        int mAbs = _cellAssigner->absoluteIndex(m);
        double Jtot = ( ds->meanintensityv(mAbs) * lambdagrid->dlambdav() ).sum();
        double U = Jtot/JtotMW;
        // ignore cells with extremely small radiation fields (compared to the average in the Milky Way)
        // to avoid wasting library grid points on fields that won't change simulation results anyway
        if (U > 1e-6)
        {
            Ucellv[m] = U;
            Umin = min(Umin,U);
            Umax = max(Umax,U);
        }
    }
    find<Log>()->info("ISRF strengths vary from U = " + QString::number(Umin)
                                         + " to U = " + QString::number(Umax) + ".");

    // determine for every dust cell m the corresponding library entry n
    vector<int> nv(Ncells);
    for (int m=0; m<Ncells; m++)
    {
        double U = Ucellv[m];
        if (U>0.0)
        {
            double logU = log10(U);
            double logUmin = log10(Umin);
            double logUmax = log10(Umax);
            double dlogU = (logUmax-logUmin)/_NU;
            nv[m] = max(0, min(_NU-1, static_cast<int>((logU-logUmin)/dlogU) ));
        }
        else
        {
            nv[m] = -1;
        }
    }

    return nv;
}
