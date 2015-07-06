/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "OligoFitScheme.hpp"
#include "Array.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Image.hpp"
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
                                 QList<QList<double>>& luminosities, QList<double>& chis, int index)
{
    // Perform the adjusted simulation
    QString prefix = "tmp_" + QString::number(index);
    QString outprefix = "tmp/" + prefix;
    QString tmpdir = find<FilePaths>()->output("tmp");
    _simulation->performWith(replacement, outprefix);
    QString instrname = find<AdjustableSkirtSimulation>()->instrname();

    // Read the simulation frames and compare the frame size with the reference image
    int counter=0;
    QList<QList<Image>> frames;
    foreach (ReferenceImage* rima, _rimages->images())
    {
        QList<Image> components;
        for (int i = 0; i < _simulation->ncomponents(); i++)
        {
            QString filename = prefix + "_" + instrname + "_stellar_" + QString::number(i) + "_" + QString::number(counter);
            Image component(this, filename, tmpdir);
            components << component;
        }

        int framesize = (rima->xsize())*(rima->ysize());
        int simsize = components[0].xsize()*components[0].ysize();
        if (framesize != simsize) throw FATALERROR("Simulations and Reference Images have different dimensions");

        frames << components;
        counter++;
    }

    // Determine the best fitting luminosities and lowest chi2 value
    double test_chi2functions = _rimages->chi2(frames, luminosities, chis);
    return test_chi2functions;
}

////////////////////////////////////////////////////////////////////
