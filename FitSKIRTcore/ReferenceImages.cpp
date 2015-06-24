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
#include <QList>

using namespace std;

//////////////////////////////////////////////////////////////////////

ReferenceImages::ReferenceImages()
{
}

//////////////////////////////////////////////////////////////////////

void ReferenceImages::insertImage(int index, ReferenceImage* value)
{
    if (!value) throw FATALERROR("Referenceimage pointer shouldn't be null");
    value->setParent(this);
    _rimages.insert(index, value);
}

//////////////////////////////////////////////////////////////////////

void ReferenceImages::removeImage(int index)
{
    delete _rimages.takeAt(index);
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
    if (find<AdjustableSkirtSimulation>()->nframes() != _rimages.size())
        throw FATALERROR("Number of instrument frames does not match the number of reference frames");
}

//////////////////////////////////////////////////////////////////////

QString ReferenceImages::path(int rimi) const
{
    return _rimages[rimi]->filename();
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
        throw FATALERROR("Total number of simulated frames does not match the number of reference frames");
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
    AdjustableSkirtSimulation* adjSS = find<AdjustableSkirtSimulation>();
    QString instrname = adjSS->instrname();
    find<Log>()->info("Found new best fit");

    foreach (ReferenceImage* rima, _rimages)
    {
        int nx, ny, nz;
        QList<Array> Total;
        QString filename;
        for(int i =0; i<adjSS->ncomponents();i++)
        {
            Array CompTotal;
            QString filepath = path->output(prefix+"_"+instrname+"_stellar_"+
                               QString::number(i)+"_"+QString::number(counter)+".fits");
            FITSInOut::read(filepath,CompTotal,nx,ny,nz);
            Total.append(CompTotal);
        }
        rima->returnFrame(&Total);

        // Create an image for the best fitting frame and save it
        filename = "Best_" + QString::number(consec)+"_"+QString::number(counter);
        Image image(this, nx, ny, nz, adjSS->xpress(counter), adjSS->ypress(counter), "surfacebrightness");
        image.saveto(this, Total[0], filename, "best fitting frame");

        // Reuse the image header for the residuals frame and save it
        filename =  "Residual_" + QString::number(consec)+"_"+QString::number(counter);
        image.saveto(this, Total[1], filename, "residuals frame");

        // Increment the counter to keep track of which reference image we're dealing with
        counter++;
    }
}

//////////////////////////////////////////////////////////////////////
