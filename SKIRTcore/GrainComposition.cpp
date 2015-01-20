/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "GrainComposition.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Units.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

GrainComposition::GrainComposition()
    : _Nlambda(0), _Na(0), _NT(0), _rhobulk(0), _Ntheta(0)
{
}

////////////////////////////////////////////////////////////////////

void GrainComposition::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    // verify the dimensions of the internal grid
    if (_Nlambda < 3) throw FATALERROR("Grid must have at least 3 wavelength points");
    if (_Na < 3) throw FATALERROR("Grid must have at least 3 grain size points");
    if (_NT < 3) throw FATALERROR("Grid must have at least 3 temperature points");

    // verify the bulk density
    if (_rhobulk <= 0) throw FATALERROR("The bulk density must have a positive value");

    // log the extent of the grids
    Log* log = find<Log>();
    Units* units = find<Units>();
    log->info("Grain composition grid (" + name() + "):");
    log->info("   " + QString::number(_Nlambda) + " wavelengths"
              " from " + QString::number(units->owavelength(_lambdav[0])) + " " + units->uwavelength() +
              " to " + QString::number(units->owavelength(_lambdav[_Nlambda-1])) + " " + units->uwavelength() );
    log->info("   " + QString::number(_Na) + " grain sizes"
              " from " + QString::number(units->ograinsize(_av[0])) + " " + units->ugrainsize() +
              " to " + QString::number(units->ograinsize(_av[_Na-1])) + " " + units->ugrainsize() );
    log->info("   " + QString::number(_NT) + " temperatures"
              " from " + QString::number(units->otemperature(_Tv[0])) + " " + units->utemperature() +
            " to " + QString::number(units->otemperature(_Tv[_NT-1])) + " " + units->utemperature() );
}

////////////////////////////////////////////////////////////////////

double GrainComposition::uppertemperature() const
{
    return _NT>0 ? _Tv[_NT-1] : 0.;
}

////////////////////////////////////////////////////////////////////

namespace
{
    // this function computes the interpolated value of a 2D function, given its values at the corners of a rectangle
    // the axes coordinates x and y are always interpolated logarithmically, and thus must have positive values
    // the function value is interpolated logarithmically if 'logf' is true, and if all function values are positive
    double interpolate(double x, double x1, double x2,
                       double y, double y1, double y2,
                       double f11, double f21, double f12, double f22,
                       bool logf)
    {
        // compute logarithm of coordinate values
        x  = log10(x);
        x1 = log10(x1);
        x2 = log10(x2);
        y  = log10(y);
        y1 = log10(y1);
        y2 = log10(y2);

        // turn off logarithmic interpolation of function value if not all given values are positive
        logf = logf && f11>0 && f21>0 && f12>0 && f22>0;

        // compute logarithm of function values if required
        if (logf)
        {
            f11 = log10(f11);
            f21 = log10(f21);
            f12 = log10(f12);
            f22 = log10(f22);
        }

        // perform the interpolation
        double fxy = ( f11*(x2-x)*(y2-y) + f21*(x-x1)*(y2-y) + f12*(x2-x)*(y-y1) + f22*(x-x1)*(y-y1) )
                     / ((x2-x1)*(y2-y1));

        // compute the inverse logarithm of the resulting function value if required
        if (logf) fxy = pow(10,fxy);

        return fxy;
    }
}

////////////////////////////////////////////////////////////////////

double GrainComposition::Qabs(double lambda, double a) const
{
    // determine the wavelength and grain size bins in the internal grid
    int k, i;
    indices(lambda, a, k, i);

    // perform the 2D log-log interpolation
    return interpolate(lambda, _lambdav[k], _lambdav[k+1],  a, _av[i], _av[i+1],
                       _Qabsvv(k,i), _Qabsvv(k+1,i), _Qabsvv(k,i+1), _Qabsvv(k+1,i+1),
                       true);
}

////////////////////////////////////////////////////////////////////

double GrainComposition::Qsca(double lambda, double a) const
{
    int k, i;
    indices(lambda, a, k, i);

    // perform the 2D log-log interpolation
    return interpolate(lambda, _lambdav[k], _lambdav[k+1],  a, _av[i], _av[i+1],
                       _Qscavv(k,i), _Qscavv(k+1,i), _Qscavv(k,i+1), _Qscavv(k+1,i+1),
                       true);
}

////////////////////////////////////////////////////////////////////

double GrainComposition::asymmpar(double lambda, double a) const
{
    int k, i;
    indices(lambda, a, k, i);

    // perform the 2D log-linear interpolation
    return interpolate(lambda, _lambdav[k], _lambdav[k+1],  a, _av[i], _av[i+1],
                       _asymmparvv(k,i), _asymmparvv(k+1,i), _asymmparvv(k,i+1), _asymmparvv(k+1,i+1),
            false);
}

////////////////////////////////////////////////////////////////////

double GrainComposition::specificenthalpy(double T) const
{
    if (T <= _Tv[0]) return _hv[0];
    if (T >= _Tv[_NT-1]) return _hv[_NT-1];
    int t = NR::locate_clip(_Tv, T);
    return NR::interpolate_loglog(T,  _Tv[t], _Tv[t+1],  _hv[t], _hv[t+1]);
}

////////////////////////////////////////////////////////////////////

double GrainComposition::bulkdensity() const
{
    return _rhobulk;
}

////////////////////////////////////////////////////////////////////

bool GrainComposition::polarization() const
{
    return _Ntheta > 0;
}

////////////////////////////////////////////////////////////////////

namespace
{
    // this helper function returns the appropriate index for the specified value of theta,
    // given the specified number of theta values in the arrays
    int indexForTheta(double theta, int Ntheta)
    {
        double delta = M_PI/(Ntheta-1);
        int d = static_cast<int>(theta/delta+0.5);
        if (d<0) d = 0;
        else if (d>=Ntheta) d = Ntheta-1;
        return d;
    }
}

////////////////////////////////////////////////////////////////////

void GrainComposition::Sxx(double lambda, double a, double theta,
                           double& S11, double& S12, double& S33, double& S34) const
{
    int k, i;
    indices(lambda, a, k, i);
    int d = indexForTheta(theta, _Ntheta);

    // perform the 2D log-linear interpolation
    S11 = interpolate(lambda, _lambdav[k], _lambdav[k+1],  a, _av[i], _av[i+1],
                       _S11vvv(k,i,d), _S11vvv(k+1,i,d), _S11vvv(k,i+1,d), _S11vvv(k+1,i+1,d),  false);
    S12 = interpolate(lambda, _lambdav[k], _lambdav[k+1],  a, _av[i], _av[i+1],
                       _S12vvv(k,i,d), _S12vvv(k+1,i,d), _S12vvv(k,i+1,d), _S12vvv(k+1,i+1,d),  false);
    S33 = interpolate(lambda, _lambdav[k], _lambdav[k+1],  a, _av[i], _av[i+1],
                       _S33vvv(k,i,d), _S33vvv(k+1,i,d), _S33vvv(k,i+1,d), _S33vvv(k+1,i+1,d),  false);
    S34 = interpolate(lambda, _lambdav[k], _lambdav[k+1],  a, _av[i], _av[i+1],
                       _S34vvv(k,i,d), _S34vvv(k+1,i,d), _S34vvv(k,i+1,d), _S34vvv(k+1,i+1,d),  false);
}

////////////////////////////////////////////////////////////////////

void GrainComposition::loadOpticalGrid(bool resource, QString name, bool reverse, bool skip1, bool skip2, bool skip3)
{
    // open the file
    QString filename = resource ? FilePaths::resource(name) : find<FilePaths>()->input(name);
    ifstream file(filename.toLocal8Bit().constData());
    if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading grain composition from file " + filename + "...");

    // skip header lines and read the grid size
    string line;
    while (file.peek() == '#') getline(file,line);
    file >> _Na;
    getline(file,line); // ignore anything else on this line
    file >> _Nlambda;
    getline(file,line); // ignore anything else on this line

    // resize the vectors
    _lambdav.resize(_Nlambda);
    _av.resize(_Na);
    _Qabsvv.resize(_Nlambda,_Na);
    _Qscavv.resize(_Nlambda,_Na);
    _asymmparvv.resize(_Nlambda,_Na);

    // determine the loop conditions for wavelength lines
    int kbeg = reverse ? _Nlambda-1 : 0;
    int kend = reverse ? -1 : _Nlambda;
    int kinc = reverse ? -1 : 1;

    // read the data blocks
    double dummy;
    for (int i=0; i<_Na; i++)
    {
        file >> _av[i];
        _av[i] *= 1e-6;     // convert from micron to m
        getline(file,line); // ignore anything else on this line

        for (int k=kbeg; k!=kend; k+=kinc)
        {
            if (skip1) file >> dummy;
            file >> _lambdav[k];
            _lambdav[k] *= 1e-6;     // convert from micron to m
            if (skip2) file >> dummy;
            file >> _Qabsvv(k,i);
            file >> _Qscavv(k,i);
            if (skip3) file >> dummy;
            file >> _asymmparvv(k,i);
            getline(file,line); // ignore anything else on this line
        }
    }

    // close the file
    file.close();
    find<Log>()->info("File " + filename + " closed.");
}

////////////////////////////////////////////////////////////////////

void GrainComposition::loadOpticalGrid(QString resourceLambda, QString resourceQ, QString resourceG)
{
    // ------------ wavelengths file ------------
    {
        // open the file
        QString filename = FilePaths::resource(resourceLambda);
        ifstream file(filename.toLocal8Bit().constData());
        if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
        find<Log>()->info("Reading grain composition wavelengths from file " + filename + "...");

        // skip header lines and read the wavelength grid size
        string line;
        while (file.peek() == '#') getline(file,line);
        file >> _Nlambda;
        getline(file,line); // ignore anything else on this line

        // read the wavelengths
        _lambdav.resize(_Nlambda);
        for (int k=0; k<_Nlambda; k++)
        {
            file >> _lambdav[k];
            _lambdav[k] *= 1e-6;   // convert from micron to m
            getline(file,line);    // ignore anything else on this line
        }

        // close the file
        file.close();
        find<Log>()->info("File " + filename + " closed.");
    }

    // ------------ efficiencies file ------------
    {
        // open the file
        QString filename = FilePaths::resource(resourceQ);
        ifstream file(filename.toLocal8Bit().constData());
        if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
        find<Log>()->info("Reading grain composition efficiencies from file " + filename + "...");

        // skip header lines and read the grain size grid size
        string line;
        while (file.peek() == '#') getline(file,line);
        file >> _Na;
        getline(file,line); // ignore anything else on this line

        // read the grain sizes
        _av.resize(_Na);
        for (int i=0; i<_Na; i++)
        {
            file >> _av[i];
            _av[i] *= 1e-6;        // convert from micron to m
        }
        getline(file,line);    // ignore anything else on this line

        // resize the vectors
        _Qabsvv.resize(_Nlambda,_Na);
        _Qscavv.resize(_Nlambda,_Na);
        _asymmparvv.resize(_Nlambda,_Na);

        // skip header lines and read the absorption efficiencies
        while (file.peek() == '#') getline(file,line);
        for (int k=0; k<_Nlambda; k++)
        {
            for (int i=0; i<_Na; i++)
            {
                file >> _Qabsvv(k,i);
            }
            getline(file,line); // ignore anything else on this line
        }

        // skip header lines and read the scattering efficiencies
        while (file.peek() == '#') getline(file,line);
        for (int k=0; k<_Nlambda; k++)
        {
            for (int i=0; i<_Na; i++)
            {
                file >> _Qscavv(k,i);
            }
            getline(file,line); // ignore anything else on this line
        }

        // close the file
        file.close();
        find<Log>()->info("File " + filename + " closed.");
    }

    // ------------ scattering assymmetry parameter file ------------
    {
        // open the file
        QString filename = FilePaths::resource(resourceG);
        ifstream file(filename.toLocal8Bit().constData());
        if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
        find<Log>()->info("Reading grain composition scattering factors from file " + filename + "...");

        // skip header lines and verify the grain size grid size
        string line;
        while (file.peek() == '#') getline(file,line);
        double Na;
        file >> Na;
        getline(file,line); // ignore anything else on this line
        if (Na != _Na) throw FATALERROR("Number of grain sizes differs between resource files");

        // verify the grain sizes
        for (int i=0; i<_Na; i++)
        {
            double a;
            file >> a;
            if (a*1e-6 != _av[i]) throw FATALERROR("Grain sizes differ between resource files");
        }
        getline(file,line);    // ignore anything else on this line

        // skip header lines and read the scattering factors
        while (file.peek() == '#') getline(file,line);
        for (int k=0; k<_Nlambda; k++)
        {
            for (int i=0; i<_Na; i++)
            {
                file >> _asymmparvv(k,i);
            }
            getline(file,line); // ignore anything else on this line
        }

        // close the file
        file.close();
        find<Log>()->info("File " + filename + " closed.");
    }
}

////////////////////////////////////////////////////////////////////

void GrainComposition::loadEnthalpyGrid(bool resource, QString name)
{
    // open the file
    QString filename = resource ? FilePaths::resource(name) : find<FilePaths>()->input(name);
    ifstream file(filename.toLocal8Bit().constData());
    if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading enthalpy data from file " + filename + "...");

    // skip header lines and read the grid size
    string line;
    while (file.peek() == '#') getline(file,line);
    file >> _NT;
    getline(file,line); // ignore anything else on this line

    // resize the vectors
    _Tv.resize(_NT);
    _hv.resize(_NT);

    // read the data
    for (int t=0; t<_NT; t++)
    {
        file >> _Tv[t];
        file >> _hv[t];
        _hv[t] *= 1e-4;     // convert from erg/g to J/kg
        getline(file,line); // ignore anything else on this line
    }

    // close the file
    file.close();
    find<Log>()->info("File " + filename + " closed.");
}

void GrainComposition::loadLogHeatCapacityGrid(QString resourcename)
{
    // verify that the bulk density has been set before calling this function
    if (_rhobulk <= 0) throw FATALERROR("The bulk density must have a positive value");

    // open the file
    QString filename = FilePaths::resource(resourcename);
    ifstream file(filename.toLocal8Bit().constData());
    if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading heat capacity data from file " + filename + "...");

    // skip header lines and read the grid size
    string line;
    while (file.peek() == '#') getline(file,line);
    getline(file,line); // ignore first two non-header lines
    getline(file,line);
    double Nin;
    file >> Nin;
    getline(file,line); // ignore anything else on this line

    // construct the vectors that will hold the input data
    Array logTinv(Nin);
    Array logCinv(Nin);

    // read the data
    for (int t=0; t<Nin; t++)
    {
        file >> logTinv[t];
        file >> logCinv[t];
        getline(file,line); // ignore anything else on this line
    }

    // close the file
    file.close();
    find<Log>()->info("File " + filename + " closed.");

    // interpolate the heat capacity values on a larger grid, to enable accurate integration
    _NT = 5000; // arbitrary value
    Array logTv;
    double dT = NR::lingrid(logTv, logTinv[0], logTinv[Nin-1], _NT-1);
    Array logCv = NR::resample<NR::interpolate_linlin>(logTv, logTinv, logCinv);

    // perform the integration
    _hv.resize(_NT);
    double sum = 0.;
    for (int t=0; t<_NT; t++)
    {
        _hv[t] = sum;
        sum += M_LN10 * pow(10.,logCv[t]+logTv[t]) * dT;
    }

    // convert from erg/cm3 to J/m3 and then to J/kg
    _hv /= 10. * _rhobulk;

    // copy the temperature grid
    _Tv.resize(_NT);
    _Tv = pow(10., logTv);
}

////////////////////////////////////////////////////////////////////

void GrainComposition::calculateEnthalpyGrid(EnthalpyFunction efun)
{
    _NT = 3000; // arbitrary value
    NR::loggrid(_Tv, 1., 3000., _NT-1);
    _hv.resize(_NT);
    for (int t=0; t<_NT; t++) _hv[t] = efun(_Tv[t]);
}

////////////////////////////////////////////////////////////////////

void GrainComposition::setBulkDensity(double value)
{
    _rhobulk = value;
}

////////////////////////////////////////////////////////////////////

void GrainComposition::loadPolarizedOpticalGrid(bool resource, QString name)
{
    // open the file
    QString filename = resource ? FilePaths::externalResource(name) : find<FilePaths>()->input(name);
    ifstream file(filename.toLocal8Bit().constData());
    if (!file.is_open()) throw FATALERROR("Could not open the data file " + filename);
    find<Log>()->info("Reading polarized grain composition from file " + filename + "...");

    // skip header lines and read the grid size
    string line;
    int N;
    file >> N;  // N is the number of header lines
    for (int n=0; n<N; n++) getline(file,line);  // skip the header lines
    file >> _Na;
    getline(file,line);
    file >> _Nlambda;
    getline(file,line);
    file >> _Ntheta;
    getline(file,line);
    _Na++; _Nlambda++; _Ntheta++; // these values are given as n-1 in input file
    getline(file,line);
    getline(file,line);
    getline(file,line);
    getline(file,line);

    // resize our arrays
    _lambdav.resize(_Nlambda);
    _av.resize(_Na);
    _Qabsvv.resize(_Nlambda,_Na);
    _Qscavv.resize(_Nlambda,_Na);
    _asymmparvv.resize(_Nlambda,_Na);  // g array is resized but left to zero values
    _S11vvv.resize(_Nlambda,_Na,_Ntheta);
    _S12vvv.resize(_Nlambda,_Na,_Ntheta);
    _S33vvv.resize(_Nlambda,_Na,_Ntheta);
    _S34vvv.resize(_Nlambda,_Na,_Ntheta);

    // read the data
    for (int i=0; i<_Na; i++)
    {
        getline(file,line);
        file >> _av[i];
        _av[i] *= 1e-6;  // conversion from micron to m
        getline(file,line);
        getline(file,line);
        for (int k=_Nlambda-1; k>=0; k--)
        {
            getline(file,line);
            getline(file,line);  // skip the line with the column titles
            file >> _lambdav[k] >> _Qabsvv(k,i) >> _Qscavv(k,i);
            _lambdav[k] *= 1e-6;  // conversion from micron to m
            getline(file,line);
            getline(file,line);
            getline(file,line);  // skip the line with the column titles
            for (int d=0; d<=_Ntheta-1; d++)
            {
                double theta;
                file >> theta >> _S11vvv(k,i,d) >> _S12vvv(k,i,d) >> _S33vvv(k,i,d) >> _S34vvv(k,i,d);
                getline(file,line);
            }
        }
    }

    // close the file
    file.close();
    find<Log>()->info("File " + filename + " closed.");
}

////////////////////////////////////////////////////////////////////

void GrainComposition::indices(double& lambda, double& a, int& k, int& i) const
{
    k = NR::locate(_lambdav, lambda);
    if (k<0)                // beyond lower border: adjust lambda
    {
        lambda = _lambdav[0];
        k = 0;
    }
    else if (k>_Nlambda-2)  // beyond upper border: adjust lambda
    {
        lambda = _lambdav[_Nlambda-1];
        k = _Nlambda-2;
    }

    i = NR::locate(_av, a);
    if (i<0)                // beyond lower border: adjust a
    {
        a = _av[0];
        i = 0;
    }
    else if (i>_Na-2)       // beyond upper border: adjust a
    {
        a = _av[_Na-1];
        i = _Na-2;
    }
}

////////////////////////////////////////////////////////////////////
