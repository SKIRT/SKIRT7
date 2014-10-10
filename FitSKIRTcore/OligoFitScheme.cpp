/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "OligoFitScheme.hpp"

#include "Array.hpp"
#include "FITSInOut.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "Optimization.hpp"
#include "ParameterRange.hpp"
#include "ParameterRanges.hpp"
#include "ReferenceImage.hpp"
#include "ReferenceImages.hpp"

////////////////////////////////////////////////////////////////////

OligoFitScheme::OligoFitScheme()
    : _simulation(0), _ranges(0), _rimages(0), _optim(0)
{
}

////////////////////////////////////////////////////////////////////

void OligoFitScheme::runSelf()
{
    _optim->initialize();
    while(!_optim->done())
    {
        _optim->step();
    }
}

////////////////////////////////////////////////////////////////////

void OligoFitScheme::setSimulation(AdjustableSkirtSimulation* value)
{
    if (_simulation) delete _simulation;
    _simulation = value;
    if (_simulation) _simulation->setParent(this);
}

////////////////////////////////////////////////////////////////////

AdjustableSkirtSimulation* OligoFitScheme::simulation() const
{
    return _simulation;
}

////////////////////////////////////////////////////////////////////

void OligoFitScheme::setFixedSeed(bool value)
{
    _fixedSeed = value;
}

////////////////////////////////////////////////////////////////////

bool OligoFitScheme::fixedSeed() const
{
    return _fixedSeed;
}

////////////////////////////////////////////////////////////////////

void OligoFitScheme::setParameterRanges(ParameterRanges* value)
{
    if (_ranges) delete _ranges;
    _ranges = value;
    if (_ranges) _ranges->setParent(this);
}

////////////////////////////////////////////////////////////////////

ParameterRanges* OligoFitScheme::parameterRanges() const
{
    return _ranges;
}

////////////////////////////////////////////////////////////////////

void OligoFitScheme::setReferenceImages(ReferenceImages *value)
{
    if (_rimages) delete _rimages;
    _rimages = value;
    if (_rimages) _rimages->setParent(this);
}

////////////////////////////////////////////////////////////////////

ReferenceImages* OligoFitScheme::referenceImages() const
{
    return _rimages;
}

////////////////////////////////////////////////////////////////////

void OligoFitScheme::setOptim(Optimization *value)
{
    if (_optim) delete _optim;
    _optim = value;
    if (_optim) _optim->setParent(this);
}

////////////////////////////////////////////////////////////////////

Optimization* OligoFitScheme::optim() const
{
    return _optim;
}

////////////////////////////////////////////////////////////////////

double OligoFitScheme::objective(AdjustableSkirtSimulation::ReplacementDict replacement,
                                 QList<QList<double> >*luminosities, QList<double> *Chis, int index)
{
    //perform the adjusted simulation
    QString prefix = "tmp/tmp_"+QString::number(index);
    _simulation->performWith(replacement, prefix);
    QString instrname = find<AdjustableSkirtSimulation>()->instrname();

    //read the simulation frames and compare the frame size with the reference image
    int counter=0;
    QList<Array> Simulations;
    QList<QList<Array>> frames;
    FilePaths* path = find<FilePaths>();

    foreach (ReferenceImage* rima, _rimages->images())
    {
        int nx,ny,nz; //dummy values;
        QString filepath;

        for(int i =0; i<_simulation->ncomponents();i++){
            Array CompTotal;
            filepath = path->output(prefix+"_"+instrname+"_stellar_"+
                                    QString::number(i)+"_"+QString::number(counter)+".fits");
            FITSInOut::read(filepath,CompTotal,nx,ny,nz);
            Simulations.append(CompTotal);
        }

        int framesize = (rima->xdim())*(rima->ydim());
        int simsize = Simulations[0].size();
        if (framesize != simsize)
            throw FATALERROR("Simulations and Reference Images have different dimensions");

        frames.append(Simulations);
        Simulations.clear();
        counter++;
    }

    //determine the best fitting luminosities and lowest chi2 value
    double test_chi2functions = _rimages->chi2(&frames,luminosities, Chis);
    return test_chi2functions;
}

////////////////////////////////////////////////////////////////////
