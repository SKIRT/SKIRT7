/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QMultiHash>
#include <QTime>
#include "DistMemTable.hpp"
#include "DustLib.hpp"
#include "DustEmissivity.hpp"
#include "IdenticalAssigner.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PanDustSystem.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "StaggeredAssigner.hpp"
#include "TextOutFile.hpp"
#include "TimeLogger.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustLib::DustLib()
    : _cellAssigner(0), _lambdaAssigner(0)
{
}

////////////////////////////////////////////////////////////////////

void DustLib::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    _lambdaAssigner = lambdagrid->assigner();
    PanDustSystem* dustsystem = find<PanDustSystem>();
    _cellAssigner = dustsystem->assigner();

}

////////////////////////////////////////////////////////////////////

namespace
{
    class EmissionCalculator : public ParallelTarget
    {
    private:
        // data members initialized in constructor
        DistMemTable& _distLvv;     // output luminosities indexed on m or n and ell (writable reference)
        QMultiHash<int,int> _mh;    // hash map <n,m> of cells for each library entry
        Log* _log;
        PanDustSystem* _ds;
        DustEmissivity* _de;
        WavelengthGrid* _lambdagrid;
        int _Nlambda;
        int _Ncomp;
        QTime _timer;           // measures the time elapsed since the most recent log message

        // new stuff
        bool _dataparallel;
        ProcessAssigner* _cellAssigner;

    public:
        // constructor
        EmissionCalculator(DistMemTable& distLvv, vector<int>& nv, int Nlib, SimulationItem* item)
            : _distLvv(distLvv)
        {
            // get basic information about the wavelength grid and the dust system
            _log = item->find<Log>();
            _ds = item->find<PanDustSystem>();
            _de = item->find<DustEmissivity>();
            _lambdagrid = item->find<WavelengthGrid>();
            _Nlambda = _lambdagrid->Nlambda();
            _Ncomp = _ds->Ncomp();

            // new stuff
            _cellAssigner = _ds->assigner();

            // invert mapping vector into a temporary hash map
            int Ncells = nv.size();
            _mh.reserve(Ncells);
            for (int m=0; m<Ncells; m++) if (nv[m] >= 0) _mh.insert(nv[m],_cellAssigner->absoluteIndex(m));

            // log usage statistics
            int Nused = _mh.uniqueKeys().size();
            _log->info("Library entries in use: " + QString::number(Nused) +
                                       " out of " + QString::number(Nlib) + ".");

            // start the logging timer
            _timer.start();
        }

        // the parallized loop body; calculates the emission for a single library entry
        void body(size_t n)
        {
            // get the list of dust cells that map to this library entry (absolute indices)
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
                foreach (int m, mv)
                {
                    Jv += _ds->meanintensityv(m);
                }
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
                        Array& Lv = _distLvv[m];

                        // calculate the emission for this cell
                        double density = _ds->density(m);
                        for (int h=0; h<_Ncomp; h++) Lv += evv[h] * density;

                        // convert to luminosities and normalize the result
                        Lv *= _lambdagrid->dlambdav();
                        double total = Lv.sum();
                        if (total>0) Lv /= total;
                    }
                }
/*
                // single dust component: remember just the libary template, which serves for all mapped cells
                else // not used anymore
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
*/
            }
        }
    };
}

////////////////////////////////////////////////////////////////////

void DustLib::calculate()
{
    if (!_distLvv.initialized()) _distLvv.initialize("Dust Emission",_lambdaAssigner,_cellAssigner,ROW);
    // get mapping from cells to library entries
    int Nlib = entries();
    _nv = mapping();

    // if data is distributed: each process has its own dustlibrary and calculates all entries of it => Identical
    // if data is not distributed: divide calculation over processes => Staggered
    ProcessAssigner* helpAssigner;
    if (_distLvv.distributed()) helpAssigner = new IdenticalAssigner(this);
    else helpAssigner = new StaggeredAssigner(this);
    helpAssigner->assign(Nlib);

    // calculate the emissivity for each library entry
    EmissionCalculator calc(_distLvv, _nv, Nlib, this);
    Parallel* parallel = find<ParallelFactory>()->parallel();
    parallel->call(&calc, helpAssigner);

    // Wait for the other processes to reach this point
    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();
    comm->wait("the emission spectra calculation");

    _distLvv.sync();
}

////////////////////////////////////////////////////////////////////

double DustLib::luminosity(int m, int ell) const
{
    return _distLvv(m,ell);
}

////////////////////////////////////////////////////////////////////

void DustLib::assemble()
{
}

