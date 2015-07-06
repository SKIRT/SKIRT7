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

double ReferenceImages::chi2(QList<QList<Image>>& frames, QList<QList<double>>& luminosities, QList<double>& chis)
{
    if (frames.size() != _rimages.size())
        throw FATALERROR("Total number of simulated frames does not match the number of reference frames");
    int counter=0;
    double chi2_sum=0;

    foreach (ReferenceImage* rima, _rimages)
    {
        double chi;
        QList<double> monolum;
        chi = rima->chi2(frames[counter], monolum);
        luminosities << monolum;
        chi2_sum += chi;
        chis << chi;
        counter++;
    }
    return chi2_sum;
}

//////////////////////////////////////////////////////////////////////

void ReferenceImages::writeOutBest(int index, int consec) const
{
    int counter=0;

    QString prefix = "tmp_" + QString::number(index);
    QString tmpdir = find<FilePaths>()->output("tmp");
    AdjustableSkirtSimulation* adjSS = find<AdjustableSkirtSimulation>();
    QString instrname = adjSS->instrname();
    find<Log>()->info("Found new best fit");

    foreach (ReferenceImage* rima, _rimages)
    {
        QList<Image> total;
        QString filename;
        for(int i = 0; i < adjSS->ncomponents(); i++)
        {
            QString filename = prefix + "_" + instrname + "_stellar_" + QString::number(i) + "_" + QString::number(counter);
            Image compTotal(this, filename, tmpdir, adjSS->xpress(counter), adjSS->ypress(counter), "surfacebrightness");
            total << compTotal;
        }
        rima->returnFrame(total);

        // Save the best fitting image
        filename = "Best_" + QString::number(consec) + "_" + QString::number(counter);
        total[0].saveto(this, filename, "best fitting frame");

        // Save the residuals frame
        filename =  "Residual_" + QString::number(consec) + "_" + QString::number(counter);
        total[1].saveto(this, filename, "residuals frame");

        // Increment the counter to keep track of which reference image we're dealing with
        counter++;
    }
}

//////////////////////////////////////////////////////////////////////
