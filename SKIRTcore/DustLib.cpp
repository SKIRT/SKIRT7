/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <QMultiHash>
#include "DustLib.hpp"
#include "DustEmissivity.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PanDustSystem.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustLib::DustLib()
{
}

////////////////////////////////////////////////////////////////////

namespace
{
    class EmissionCalculator : public ParallelTarget
    {
    private:
        // data members initialized in constructor
        ArrayTable<2>& _Lvv;        // output luminosities indexed on m or n and ell (writable reference)
        QMultiHash<int,int> _mh;    // hash map <n,m> of cells for each library entry
        Log* _log;
        PanDustSystem* _ds;
        DustEmissivity* _de;
        WavelengthGrid* _lambdagrid;
        int _Nlambda;
        int _Ncomp;

    public:
        // constructor
        EmissionCalculator(ArrayTable<2>& Lvv, vector<int>& nv, int Nlib, SimulationItem* item)
            : _Lvv(Lvv)
        {
            // get basic information about the wavelength grid and the dust system
            _log = item->find<Log>();
            _ds = item->find<PanDustSystem>();
            _de = item->find<DustEmissivity>();
            _lambdagrid = item->find<WavelengthGrid>();
            _Nlambda = _lambdagrid->Nlambda();
            _Ncomp = _ds->Ncomp();

            // invert mapping vector into a temporary hash map
            int Ncells = nv.size();
            _mh.reserve(Ncells);
            for (int m=0; m<Ncells; m++) if (nv[m] >= 0) _mh.insert(nv[m],m);

            // log usage statistics
            int Nused = _mh.uniqueKeys().size();
            _log->info("Library entries in use: " + QString::number(Nused) +
                                       " out of " + QString::number(Nlib) + ".");

            // resize result vectors appropriately (for every cell or for every library entry)
            int Nout = _Ncomp>1 ? Ncells : Nlib;
            _Lvv.resize(Nout,_Nlambda);  // also sets all values to zero

            // if there are multiple components, rewire the mapping so that n==m for luminosity()
            if (_Ncomp > 1) for (int m=0; m<Ncells; m++) nv[m] = m;
        }

        // the parallized loop body; calculates the emission for a single library entry
        void body(size_t n)
        {
            // get the list of dust cells that map to this library entry
            QList<int> mv = _mh.values(n);
            int Nmapped = mv.size();

            // if this library entry is used by at least one cell, calculate the emission for those cells
            if (Nmapped > 0)
            {
                if (_de->logfrequency())
                    _log->info("Calculating emission for library entry " + QString::number(n+1) + "...");

                // calculate the average ISRF for this library entry from the ISRF of all dust cells that map to it
                Array Jv(_Nlambda);
                foreach (int m, mv) Jv += _ds->meanintensityv(m);
                Jv /= Nmapped;

                // multiple dust components: calculate emission for each dust cell separately
                if (_Ncomp > 1)
                {
                    // get emissivity for each dust component (i.e. for the corresponding dust mix)
                    ArrayTable<2> evv(_Ncomp,0);
                    for (int h=0; h<_Ncomp; h++) evv[h] = _de->emissivity(_ds->mix(h),Jv);

                    // combine emissivities into SED for each dust cell, and store the normalized SEDs
                    foreach (int m, mv)
                    {
                        // get a reference to the output array for this dust cell
                        Array& Lv = _Lvv[m];

                        // calculate the emission for this cell
                        for (int h=0; h<_Ncomp; h++) Lv += evv[h] * _ds->density(m,h);

                        // convert to luminosities and normalize the result
                        Lv *= _lambdagrid->dlambdav();
                        double total = Lv.sum();
                        if (total>0) Lv /= total;
                    }
                }

                // single dust component: remember just the libary template, which serves for all mapped cells
                else
                {
                    // get a reference to the output array for this library entry
                    Array& Lv = _Lvv[n];

                    // get the emissivity of the library entry
                    Lv = _de->emissivity(_ds->mix(0),Jv);

                    // convert to luminosities and normalize the result
                    Lv *= _lambdagrid->dlambdav();
                    double total = Lv.sum();
                    if (total>0) Lv /= total;
                }
            }
        }
    };
}

////////////////////////////////////////////////////////////////////

void DustLib::calculate()
{
    // get mapping from cells to library entries
    int Nlib = entries();
    _nv = mapping();

    // calculate the emissivity for each library entry
    EmissionCalculator calc(_Lvv, _nv, Nlib, this);
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(&calc, Nlib);
}

////////////////////////////////////////////////////////////////////

double DustLib::luminosity(int m, int ell) const
{
    int n = _nv[m];
    return n>=0 ? _Lvv[n][ell] : 0.;
}

////////////////////////////////////////////////////////////////////
