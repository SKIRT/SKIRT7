/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ReferenceImages.hpp"

#include "AdjustableSkirtSimulation.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "InstrumentFrame.hpp"
#include "Log.hpp"
#include "MultiFrameInstrument.hpp"
#include "Optimization.hpp"
#include "ReferenceImage.hpp"
#include "Units.hpp"
#include <QList>

using namespace std;

//////////////////////////////////////////////////////////////////////

ReferenceImages::ReferenceImages()
{
}

//////////////////////////////////////////////////////////////////////

void ReferenceImages::addImage(ReferenceImage* value)
{
    if (!value) throw FATALERROR("Referenceimage pointer shouldn't be null");
    value->setParent(this);
    _rimages << value;
}

//////////////////////////////////////////////////////////////////////

QList<ReferenceImage*> ReferenceImages::images() const
{
    return _rimages;
}

//////////////////////////////////////////////////////////////////////

void ReferenceImages::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify that there is at least one range
    if (_rimages.isEmpty()) throw FATALERROR("There are no reference images");
}

//////////////////////////////////////////////////////////////////////

QString ReferenceImages::path(int rimi) const
{
    return _rimages[rimi]->path();
}

//////////////////////////////////////////////////////////////////////

int ReferenceImages::size() const
{
    return _rimages.size();
}
//////////////////////////////////////////////////////////////////////

double ReferenceImages::chi2(QList<QList<Array>> *frames,
                             QList<QList<double>> *luminosities, QList<double> *Chis)
{
    if (frames->size() != _rimages.size())
        throw FATALERROR("Number of simulation frames does not match the number of reference frames");
    int counter=0;
    double chi2_sum=0;

    foreach (ReferenceImage* rima, _rimages)
    {
        double chi;
        QList<double> monolum;
        chi=rima->chi2(&((*frames)[counter]), &(monolum));
        luminosities->append(monolum);
        chi2_sum += chi;
        Chis->append(chi);
        counter++;
    }
    return chi2_sum;
}

//////////////////////////////////////////////////////////////////////

void ReferenceImages::writeOutBest(int index, int consec) const
{
    int counter=0;

    QString prefix = "tmp/tmp_"+QString::number(index);
    FilePaths* path = find<FilePaths>();
    Units* units = find<Units>();
    AdjustableSkirtSimulation* adjSS = find<AdjustableSkirtSimulation>();
    QString instrname = adjSS->instrname();
    find<Log>()->info("Found best fit");

    foreach (ReferenceImage* rima, _rimages)
    {
        int nx, ny, nz;
        Array diskTotal, bulgeTotal;
        QString filepath = path->output(prefix+"_"+instrname+"_stellar_0_"+QString::number(counter)+".fits");
        FITSInOut::read(filepath,diskTotal,nx,ny,nz);
        filepath = path->output(prefix+"_"+instrname+"_stellar_1_"+QString::number(counter)+".fits");
        FITSInOut::read(filepath,bulgeTotal,nx,ny,nz);
        rima->returnFrame(&diskTotal, &bulgeTotal);

        filepath = path->output("Best_"+QString::number(consec)+"_"+QString::number(counter)+".fits");
        FITSInOut::write(filepath, diskTotal, nx, ny, nz,
                       units->olength(adjSS->xpress(counter)), units->olength(adjSS->ypress(counter)),
                       units->usurfacebrightness(), units->ulength());
        filepath =  path->output("Residual_"+QString::number(consec)+"_"+QString::number(counter)+".fits");
        FITSInOut::write(filepath, bulgeTotal, nx, ny, nz,
                       units->olength(adjSS->xpress(counter)), units->olength(adjSS->ypress(counter)),
                       units->usurfacebrightness(), units->ulength());

        counter++;
    }
}

//////////////////////////////////////////////////////////////////////
