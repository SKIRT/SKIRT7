/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cfloat>
#include "Dim2DustLib.hpp"
#include "DustMix.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include "PanDustSystem.hpp"
#include "ProcessAssigner.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

Dim2DustLib::Dim2DustLib()
    : _NT(0), _NW(0)
{
}

////////////////////////////////////////////////////////////////////

void Dim2DustLib::setupSelfBefore()
{
    DustLib::setupSelfBefore();

    if (_NT < 3 || _NW < 3) throw FATALERROR("there must be at least 3 library grid points in each dimension");
}

////////////////////////////////////////////////////////////////////

void Dim2DustLib::setPointsTemperature(int value)
{
    _NT = value;
}

////////////////////////////////////////////////////////////////////

int Dim2DustLib::pointsTemperature() const
{
    return _NT;
}

////////////////////////////////////////////////////////////////////

void Dim2DustLib::setPointsWavelength(int value)
{
    _NW = value;
}

////////////////////////////////////////////////////////////////////

int Dim2DustLib::pointsWavelength() const
{
    return _NW;
}

////////////////////////////////////////////////////////////////////

int Dim2DustLib::entries() const
{
    return _NT*_NW;
}

////////////////////////////////////////////////////////////////////

std::vector<int> Dim2DustLib::mapping() const
{
    // get basic information about the wavelength grid and the dust system
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    PanDustSystem* ds = find<PanDustSystem>();
    int Nlambda = lambdagrid->Nlambda();
    int Ncells = _cellAssigner->assigned();
    int Ncomp = ds->Ncomp();
    Log* log = find<Log>();
    Units* units = find<Units>();

    // calculate the properties of the ISRF in all cells of the dust system;
    // determine the minimum and maximum values of the mean temperature and mean wavelength
    double Tmin = DBL_MAX;
    double Tmax = 0.0;
    double lambdamin = DBL_MAX;
    double lambdamax = 0.0;
    Array Tmeanv(Ncells);
    Array lambdameanv(Ncells);
    for (int m=0; m<Ncells; m++)
    {
        int mAbs = _cellAssigner->absoluteIndex(m);
        if (ds->Labs(mAbs) > 0.0)
        {
            const Array& Jv = ds->meanintensityv(mAbs);
            double sumrho = 0.;
            for (int h=0; h<Ncomp; h++)
            {
                double sum0 = 0.0;
                double sum1 = 0.0;
                for (int ell=0; ell<Nlambda; ell++)
                {
                    double lambda = lambdagrid->lambda(ell);
                    double dlambda = lambdagrid->dlambda(ell);
                    double sigmaJ = ds->mix(h)->sigmaabs(ell) * Jv[ell];
                    sum0 += sigmaJ * dlambda;
                    sum1 += sigmaJ * lambda * dlambda;
                }
                double rho = ds->density(mAbs,h);
                Tmeanv[m] += rho * ds->mix(h)->invplanckabs(sum0);
                lambdameanv[m] += rho * (sum1/sum0);
                sumrho += rho;
            }
            Tmeanv[m] /= sumrho;
            lambdameanv[m] /= sumrho;

            Tmin = min(Tmin,Tmeanv[m]);
            Tmax = max(Tmax,Tmeanv[m]);
            lambdamin = min(lambdamin,lambdameanv[m]);
            lambdamax = max(lambdamax,lambdameanv[m]);
        }
    }
    log->info("Temperatures vary"
              " from T = " + QString::number(units->otemperature(Tmin)) + " " + units->utemperature() +
              " to T = " + QString::number(units->otemperature(Tmax)) + " " + units->utemperature() + ".");
    log->info("Mean wavelengths vary"
              " from λ = " + QString::number(units->owavelength(lambdamin)) + " " + units->uwavelength() +
              " to λ = " + QString::number(units->owavelength(lambdamax)) + " " + units->uwavelength() + ".");

    // determine for every dust cell m the corresponding library entry n
    vector<int> nv(Ncells);
    for (int m=0; m<Ncells; m++)
    {
        if (Tmeanv[m] > 0.0 && lambdameanv[m] > 0.0)
        {
            double T = Tmeanv[m];
            double dT = (Tmax-Tmin)/_NT;
            int i = max(0, min(_NT-1, static_cast<int>((T-Tmin)/dT) ));

            double lambdamean = lambdameanv[m];
            double loglambdamean = log10(lambdamean);
            double loglambdamin = log10(lambdamin);
            double loglambdamax = log10(lambdamax);
            double dloglambdamean = (loglambdamax-loglambdamin)/_NW;
            int j = max(0, min(_NW-1, static_cast<int>((loglambdamean-loglambdamin)/dloglambdamean) ));

            nv[m] = i + _NT*j;
        }
        else
        {
            nv[m] = -1;
        }
    }

    return nv;
}

////////////////////////////////////////////////////////////////////
