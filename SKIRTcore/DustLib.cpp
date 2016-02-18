/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QMultiHash>
#include <QTime>
#include "DustLib.hpp"
#include "DustEmissivity.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PanDustSystem.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "StaggeredAssigner.hpp"
#include "TimeLogger.hpp"
#include "WavelengthGrid.hpp"
#include "TextOutFile.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustLib::DustLib()
    : _assigner(0)
{
}

////////////////////////////////////////////////////////////////////

void DustLib::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // If no assigner was set, use a StaggeredAssigner as default
    if (!_assigner) setAssigner(new StaggeredAssigner(this));
}

////////////////////////////////////////////////////////////////////

void DustLib::setAssigner(ProcessAssigner* value)
{
    if (_assigner) delete _assigner;
    _assigner = value;
    if (_assigner) _assigner->setParent(this);
}

////////////////////////////////////////////////////////////////////

ProcessAssigner* DustLib::assigner() const
{
    return _assigner;
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
        QTime _timer;           // measures the time elapsed since the most recent log message

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
            // If there are multiple dust components, the _Lvv vector is indexed on m (the dust cells)
            int Nout = _Ncomp>1 ? Ncells : Nlib;
            _Lvv.resize(Nout,_Nlambda);  // also sets all values to zero

            // start the logging timer
            _timer.start();
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
                {
                    // space the messages at least 5 seconds apart; in the interest of speed,
                    // we do this without locking, so once in a while two consecutive messages may slip through
                    if (_timer.elapsed() > 5000)
                    {
                        _timer.restart();
                        _log->info("Calculating emission for library entry " + QString::number(n+1) + "...");
                    }
                }

                // calculate the average ISRF for this library entry from the ISRF of all dust cells that map to it
                Array Jv(_Nlambda);
                foreach (int m, mv) Jv += _ds->meanintensityv(m);
                Jv /= Nmapped;

                // multiple dust components: calculate emission for each dust cell separately
                if (true)//_Ncomp > 1)
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

    // assign each process to a set of library entries
    _assigner->assign(Nlib);

    // calculate the emissivity for each library entry assigned to this process
    EmissionCalculator calc(_Lvv, _nv, Nlib, this);
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(&calc, _assigner);

    // Wait for the other processes to reach this point
    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();
    comm->wait("the emission spectra calculation");

    // assemble _Lvv from the information stored at different processes, if the work is done in parallel processes
    if (_assigner->parallel()) assemble();

}

////////////////////////////////////////////////////////////////////

double DustLib::luminosity(int m, int ell) const
{
    size_t Ncells = _nv.size();
    if (_Lvv.size(0) == Ncells)     // _Lvv is indexed on m, the index of the dust cells
    {
        return _Lvv[m][ell];
    }
    else    // _Lvv is indexed on n, the library entry index
    {
        int n = _nv[m];
        return n>=0 ? _Lvv[n][ell] : 0.;
    }
}

////////////////////////////////////////////////////////////////////

void DustLib::assemble()
{
    // Get a pointer to the PeerToPeerCommunicator of this simulation
    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();

    Log* log = find<Log>();
    TimeLogger logger(log->verbose() && comm->isMultiProc() ? log : 0, "communication of the dust emission spectra");

    size_t Ncells = _nv.size();
    if (_Lvv.size(0) == Ncells)     // _Lvv is indexed on m, the index of the dust cells
    {
        for (size_t m = 0; m < Ncells; m++) // for each dust cell
        {
            size_t n = _nv[m];                      // get the library index for this dust cell ..
            int rank = _assigner->rankForIndex(n);    // .. to determine which process calculated the emission SED

            // finally, broadcast the emission SED from that process to all the other processes
            comm->broadcast(_Lvv[m],rank);
        }

    }
    else    // _Lvv is indexed on n, the library entry index
    {
        for (size_t n = 0; n < _Lvv.size(0); n++)   // for each library entry
        {
            int rank = _assigner->rankForIndex(n);    // determine which process calculated the emission SED of this entry

            // broadcast the emission SED from that process to all the other processes
            comm->broadcast(_Lvv[n],rank);
        }
    }
}

////////////////////////////////////////////////////////////////////
