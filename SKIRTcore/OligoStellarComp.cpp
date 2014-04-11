/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "OligoStellarComp.hpp"
#include "OligoWavelengthGrid.hpp"
#include "PhotonPackage.hpp"
#include "Units.hpp"

using namespace std;

//////////////////////////////////////////////////////////////////////

OligoStellarComp::OligoStellarComp()
{
}

////////////////////////////////////////////////////////////////////

void OligoStellarComp::setupSelfBefore()
{
    StellarComp::setupSelfBefore();

    // Verify that the wavelength grid (and thus the simulation) is of the Oligo type
    OligoWavelengthGrid* lambdagrid = find<OligoWavelengthGrid>();

    // Verify that the number of luminosities equals the number of wavelengths
    int Nlambda = lambdagrid->Nlambda();
    if (_luminosities.size() != Nlambda)
        throw FATALERROR("The number of stellar component luminosities differs from the number of wavelengths");

    // Read the solar SED from a resource file
    QString filename = FilePaths::resource("SED/Sun/SunSED.dat");
    ifstream file(filename.toLocal8Bit().constData());
    if (! file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading solar luminosity data from file " + filename + "...");
    string line;
    getline(file,line);
    int NlambdaSun;
    file >> NlambdaSun;
    Array lambdasunv(NlambdaSun);
    Array Lsunv(NlambdaSun);
    for (int k=0; k<NlambdaSun; k++)
    {
        double lambdasun, Lsun;
        file >> lambdasun >> Lsun;
        lambdasunv[k] = lambdasun/1e6; // conversion from micron to m;
        Lsunv[k] = Lsun*1e6; // conversion from W/micron to W/m;
    }
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // Convert the input luminosities (in solar units) to the program units (W/m)
    _Lv.resize(Nlambda);
    for (int ell=0; ell<Nlambda; ell++)
    {
        double lambda = lambdagrid->lambda(ell);
        int k = NR::locate_fail(lambdasunv,lambda);
        if (k < 0)
        {
            Units* units = find<Units>();
            throw FATALERROR("The sun does not emit at the wavelength of the simulation (lambda = " +
                             QString::number(units->owavelength(lambda)) + " " + units->uwavelength() + ")");
        }
        // solar luminosity in W/m at the wavelength lambda
        double Lsun = NR::interpolate_linlin(lambda, lambdasunv[k], lambdasunv[k+1], Lsunv[k], Lsunv[k+1]);
        // luminosity for the wavelength bin, in W
        _Lv[ell] = _luminosities[ell] * Lsun * lambdagrid->dlambda(ell);
    }
}

//////////////////////////////////////////////////////////////////////

void OligoStellarComp::setLuminosities(QList<double> value)
{
    _luminosities = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> OligoStellarComp::luminosities() const
{
    return _luminosities;
}

//////////////////////////////////////////////////////////////////////
