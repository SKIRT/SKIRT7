/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <QMultiHash>
#include <QTime>
#include "ParallelTable.hpp"
#include "DustLib.hpp"
#include "DustEmissivity.hpp"
#include "Log.hpp"
#include "NR.hpp"
#include "PanDustSystem.hpp"
#include "Parallel.hpp"
#include "ParallelFactory.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "StaggeredAssigner.hpp"
#include "StopWatch.hpp"
#include "TimeLogger.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustLib::DustLib()
    : _libAssigner(nullptr), _nIndexed(false)
{
}

////////////////////////////////////////////////////////////////////

void DustLib::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

namespace
{
    class EmissionCalculator : public ParallelTarget
    {
    private:
        // data members initialized in constructor
        ParallelTable& _Lvv;     // output luminosities indexed on m or n and ell (writable reference)
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
        EmissionCalculator(ParallelTable& Lvv, vector<int>& nv, int Nlib, SimulationItem* item)
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
            for (int m=0; m<Ncells; m++)
            {
                int key = nv[m];
                if (key >= 0) _mh.insert(key,m); // key == nv[m] == -1 if cell m has no emission
            }

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

            // if this library entry is not used, skip the calculation
            if (Nmapped <= 0) return;

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
            if (_Ncomp > 1)
            {
                // get emissivity for each dust component (i.e. for the corresponding dust mix)
                ArrayTable<2> evv(_Ncomp,0);
                for (int h=0; h<_Ncomp; h++) evv[h] = _de->emissivity(_ds->mix(h),Jv);

                // combine emissivities into SED for each dust cell, and store the normalized SEDs
                foreach (int m, mv)
                {
                    Array Lv(_Nlambda);

                    // calculate the emission for this cell
                    for (int h=0; h<_Ncomp; h++) Lv += evv[h] * _ds->density(m,h);

                    // convert to luminosities and normalize the result
                    Lv *= _lambdagrid->dlambdav();
                    double total = Lv.sum();
                    if (total>0) Lv /= total;

                    // copy the output array to the corresponding row of the output table
                    for (int ell=0; ell<_Nlambda; ell++) _Lvv(m,ell) = Lv[ell];
                }
            }
            // one dust component: remember just the library template, which serves for all mapped cells
            else
            {
                Array Lv(_Nlambda);

                // get the emissivity of the library entry
                Lv = _de->emissivity(_ds->mix(0),Jv);

                // convert to luminosities and normalize the result
                Lv *= _lambdagrid->dlambdav();
                double total = Lv.sum();
                if (total>0) Lv /= total;

                for (int ell=0; ell<_Nlambda; ell++) _Lvv(n,ell) = Lv[ell];
            }
        }
    };
}

////////////////////////////////////////////////////////////////////

void DustLib::calculate()
{
    PanDustSystem* ds = find<PanDustSystem>();
    WavelengthGrid* lambdagrid = find<WavelengthGrid>();
    PeerToPeerCommunicator* comm = find<PeerToPeerCommunicator>();

    // get mapping linking cells to library entries.
    int Nlib = entries();
    _nv = mapping();

    // prepare the ParallelTable for output
    QString tableName = "Dust Emission Spectra Table";
    int Ncomp = ds->Ncomp();
    bool dataParallel = comm->dataParallel();

    if (_Lvv.initialized())
        _Lvv.reset();
    else if (dataParallel)
        _Lvv.initialize(tableName, ROW, lambdagrid->assigner(), ds->assigner(), comm);
    else
    {
        if (Ncomp > 1)
            _Lvv.initialize(tableName, ROW, lambdagrid->Nlambda(), ds->Ncells(), comm);
        else
            _Lvv.initialize(tableName, ROW, lambdagrid->Nlambda(), Nlib, comm);
            // (When there is only one dust component, the normalized output spectrum will be the same for all cells
            // corresponding to a single library entry. In that case we only need to store the data per library entry.)
    }
    _nIndexed = !dataParallel && Ncomp == 1;

    // calculate the emissivity for each library entry
    EmissionCalculator calc(_Lvv, _nv, Nlib, this);
    Parallel* parallel = find<ParallelFactory>()->parallel();

    if (dataParallel)
    {
        // Each process only has data for a subset of dust cells. The available dust cells are indicated by the
        // cell assigner. The call below calculates the emission for those cells.
        // ONLY WORKS FOR ALLCELLSDUSTLIB AND THIS IS INTENDED
        parallel->call(&calc, ds->assigner());
    }
    else
    {
        // The processes have access to all the cells, and can hence use library subclasses other than AllCellsDustLib.
        // Divide the work over the processes per library entry, using an auxiliary assigner.
        if (!_libAssigner) _libAssigner = new StaggeredAssigner(Nlib, this);
        parallel->call(&calc, _libAssigner);
    }

    // Wait for the other processes to reach this point
    comm->wait("the emission spectra calculation");
    _Lvv.switchScheme();
}

////////////////////////////////////////////////////////////////////

double DustLib::luminosity(int m, int ell) const
{
    if (_nIndexed) // need to convert m to n
    {
        int n = _nv[m];
        return n >= 0 ? _Lvv(n,ell) : 0.;
    }
    else
        return _Lvv(m,ell);
}

////////////////////////////////////////////////////////////////////
