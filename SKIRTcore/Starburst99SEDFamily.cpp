/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <fstream>
#include "Starburst99SEDFamily.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "Units.hpp"
#include "WavelengthGrid.hpp"
#include "fitsio.h"

using namespace std;

//////////////////////////////////////////////////////////////////////

namespace
{
    // number of items in the library read by the constructor
    const int Nlambda = 1221;
    const int NZ = 25;
    const int Nt = 308;
}

/////////////////////////////////////////////////////////////////////////

Starburst99SEDFamily::Starburst99SEDFamily()
{
}

///////////////////////////////////////////////////////////////////

Starburst99SEDFamily::Starburst99SEDFamily(SimulationItem* parent)
{
    setParent(parent);
    setup();
}

///////////////////////////////////////////////////////////////////

// The functions in this anonymous namespace were backported from the SKIRT8 FITSInOut class.
// They are implemented here to avoid problems with potential incompatibilities in SKIRT7.
// Known problem: if multiple simulations run in parallel within the same process (the -s option),
// the cfitsio library may be invoked concurrently by the SKIRT7 FITSInOut functions and the functions here,
// which may lead to a crash (because our version of cfitsio is not built for concurrency).
namespace
{
    // mutex to guard the FITS input/output operations
    std::mutex _mutex;

    // function to report cfitsio errors
    void report_error(QString filepath, QString action, int status)
    {
        char message[FLEN_STATUS];
        ffgerr(status, message);
        throw FATALERROR("Error while " + action + " FITS file " + filepath + "\n" + QString(message));
    }

    // function to read 2D or 2D data cube from a FITS file
    void read(QString filepath, Array& data, int& nx, int& ny, int& nz)
    {
        // Acquire a global lock since the cfitsio library is not guaranteed to be reentrant
        std::unique_lock<std::mutex> lock(_mutex);

        // Open the FITS file
        int status = 0;
        fitsfile *fptr;
        ffdopn(&fptr, filepath.toLocal8Bit().constData(), READONLY, &status);
        if (status) report_error(filepath, "opening", status);

        // Get the dimensions of the data
        int naxis;
        long naxes[3];
        ffgidm(fptr, &naxis, &status);
        ffgisz(fptr, 3, naxes, &status);
        if (status) report_error(filepath, "reading", status);
        nx = naxis > 0 ? naxes[0] : 1;
        ny = naxis > 1 ? naxes[1] : 1;
        nz = naxis > 2 ? naxes[2] : 1;

        // Resize the data container
        size_t nelements = static_cast<size_t>(nx)*static_cast<size_t>(ny)*static_cast<size_t>(nz);
        data.resize(nelements);

        // Read the array of pixels from the file
        int dummy;
        ffgpvd(fptr, 0, 1, nelements, 0, &data[0], &dummy, &status);
        if (status) report_error(filepath, "reading", status);

        // Close the file
        ffclos(fptr, &status);
        if (status) report_error(filepath, "reading", status);
    }

    // function to read a single table column from a FITS file
    void readColumn(QString filepath, Array& data, int n)
    {
        // Acquire a global lock since the cfitsio library is not guaranteed to be reentrant
        std::unique_lock<std::mutex> lock(_mutex);

        // Open the FITS file
        int status = 0;
        fitsfile *fptr;
        fftopn(&fptr, filepath.toLocal8Bit().constData(), READONLY, &status);
        if (status) report_error(filepath, "opening", status);

        // Get the dimensions of the table
        int ncols = 0;
        ffgncl(fptr, &ncols, &status);
        if (status) report_error(filepath, "reading", status);
        long nrows = 0;
        ffgnrw(fptr, &nrows, &status);
        if (status) report_error(filepath, "reading", status);
        if (ncols<=0 || nrows<n) throw FATALERROR("Not enough table data in FITS file " + filepath);

        // Resize the data container
        data.resize(n);

        // Read the array of values from the first column in the table
        int dummy;
        ffgcvd(fptr, 1, 1, 1, n, 0., &data[0], &dummy, &status);
        if (status) report_error(filepath, "reading", status);

        // Close the file
        ffclos(fptr, &status);
        if (status) report_error(filepath, "reading", status);
    }
}

///////////////////////////////////////////////////////////////////

void Starburst99SEDFamily::setupSelfBefore()
{
    SEDFamily::setupSelfBefore();

    // get the resource file path
    QString filepath = FilePaths::resource("SED/Starburst99/Patrik-imfKroupa-Zmulti-ml.fits.gz");
    Log* log = find<Log>();
    log->info("Reading Starburst99 data from file " + filepath + "...");

    // read the wavelength, metallicity, and age vectors from the library
    readColumn(filepath+"[AXES][col lambda]", _lambdav, Nlambda);
    readColumn(filepath+"[AXES][col metallicity]", _Zv, NZ);
    readColumn(filepath+"[AXES][col time]", _tv, Nt);

    // read the emissivity data cube from the library
    Array data;
    int nx, ny, nz;
    read(filepath+"[SED]", data, nx, ny, nz);
    if (nx!=Nt || ny!=NZ || nz!=Nlambda) throw FATALERROR("Starburst99 library data size does not match expectations");
    log->info("File " + filepath + " closed.");

    // copy the emissivity data into the table
    _jvv.resize(Nt, NZ, Nlambda);
    size_t i = 0;
    for (size_t k = 0; k!=Nlambda; ++k)
    {
        for (size_t m = 0; m!=NZ; ++m)
        {
            for (size_t p = 0; p!=Nt; ++p)
            {
                _jvv(p, m, k) = pow(10., data[i++]);  // stored in file as log10
            }
        }
    }

    // cache the simulation's wavelength grid
    _lambdagrid = find<WavelengthGrid>();
}

//////////////////////////////////////////////////////////////////////

Array Starburst99SEDFamily::luminosities(double M, double Z, double t, double z) const
{
    // find the appropriate SED from interpolating in the BC library
    int mL, mR;
    double hZ = 0.0;
    if (Z<=_Zv[0])
        mL = mR = 0;
    else if (Z>=_Zv[NZ-1])
        mL = mR = NZ-1;
    else
    {
        mL = NR::locate_clip(_Zv,Z);
        mR = mL+1;
        double ZL = _Zv[mL];
        double ZR = _Zv[mR];
        hZ = (Z-ZL)/(ZR-ZL);
    }
    int pL, pR;
    double ht = 0.0;
    if (t<=_tv[0])
        pL = pR = 0;
    else if (t>=_tv[Nt-1])
        pL = pR = Nt-1;
    else
    {
        pL = NR::locate_clip(_tv,t);
        pR = pL+1;
        double tL = _tv[pL];
        double tR = _tv[pR];
        ht = (t-tL)/(tR-tL);
    }
    const Array& jLLv = _jvv(pL,mL);
    const Array& jLRv = _jvv(pL,mR);
    const Array& jRLv = _jvv(pR,mL);
    const Array& jRRv = _jvv(pR,mR);
    Array jv(Nlambda);
    for (int k=0; k<Nlambda; k++)
        jv[k] = (1.0-ht)*(1.0-hZ)*jLLv[k]
                + (1.0-ht)*hZ*jLRv[k]
                + ht*(1.0-hZ)*jRLv[k]
                + ht*hZ*jRRv[k];

    // resample to the possibly redshifted simulation wavelength grid,
    // convert emissivities to luminosities (i.e. multiply by the wavelength bins),
    // multiply by the mass of the population (in solar masses),
    // and return the result
    return NR::resample<NR::interpolate_loglog>(_lambdagrid->lambdav()*(1-z), _lambdav, jv)
                                    * _lambdagrid->dlambdav() * M;
}

//////////////////////////////////////////////////////////////////////

int Starburst99SEDFamily::nparams() const
{
    return 3;
}

//////////////////////////////////////////////////////////////////////

Array Starburst99SEDFamily::luminosities_generic(const Array& params, int skipvals, double z) const
{
    return luminosities(params[skipvals], params[skipvals+1], params[skipvals+2], z);
}

//////////////////////////////////////////////////////////////////////

double Starburst99SEDFamily::mass_generic(const Array& params, int skipvals) const
{
    return params[skipvals];
}

//////////////////////////////////////////////////////////////////////

QString Starburst99SEDFamily::sourceName() const
{
    return "star";
}

//////////////////////////////////////////////////////////////////////

QString Starburst99SEDFamily::sourceDescription() const
{
    return "star";
}

//////////////////////////////////////////////////////////////////////
