/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ReferenceImage.hpp"

#include "AdjustableSkirtSimulation.hpp"
#include "ConvolutionKernel.hpp"
#include "FatalError.hpp"
#include "GALumfit.hpp"
#include "GoldenSection.hpp"
#include "Log.hpp"
#include "LumSimplex.hpp"
#include "OligoFitScheme.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

ReferenceImage::ReferenceImage()
    :_kernel(0)
{
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // Import the reference image
    import(this, _filename);
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::setFilename(QString value)
{
    _filename = value;
}

////////////////////////////////////////////////////////////////////

QString ReferenceImage::filename() const
{
    return _filename;
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::setKernel(ConvolutionKernel* value)
{
    if (_kernel) delete _kernel;
    _kernel = value;
    if (_kernel) _kernel->setParent(this);
}

////////////////////////////////////////////////////////////////////

ConvolutionKernel* ReferenceImage::kernel() const
{
    return _kernel;
}
//////////////////////////////////////////////////////////////////////

void ReferenceImage::setMinLuminosities(QList<double> value)
{
    _minLum = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> ReferenceImage::minLuminosities() const
{
    return _minLum;
}

//////////////////////////////////////////////////////////////////////

void ReferenceImage::setMaxLuminosities(QList<double> value)
{
    _maxLum = value;
}

//////////////////////////////////////////////////////////////////////

QList<double> ReferenceImage::maxLuminosities() const
{
    return _maxLum;
}

//////////////////////////////////////////////////////////////////////

double ReferenceImage::chi2(QList<Array> *frames, QList<double> *monoluminosities) const
{
    // Here is where I'll have to decide which optimization algorithm for lum fit
    double chi_value = 0;
    for(int i =0; i<frames->size(); i++)
    {
       _kernel->convolve((*frames)[i], xsize(), ysize());
    }

    if (find<AdjustableSkirtSimulation>()->ncomponents() == 1)
    {
        double lum;
        if (_minLum.size() != 1 && _maxLum.size() != 1)
            throw FATALERROR("Number of luminosity boundaries differs from 1!");
        GoldenSection gold;
        gold.setMinlum(_minLum[0]);
        gold.setMaxlum(_maxLum[0]);
        gold.optimize(&_data, &((*frames)[0]), lum, chi_value);
        monoluminosities->append(lum);
    }

    if (find<AdjustableSkirtSimulation>()->ncomponents() == 2)
    {
        if (_minLum.size() != 2 && _maxLum.size() != 2)
            throw FATALERROR("Number of luminosity boundaries differs from 2!");

        double dlum,blum;
        LumSimplex lumsim;
        lumsim.setMinDlum(_minLum[0]);
        lumsim.setMaxDlum(_maxLum[0]);
        lumsim.setMinblum(_minLum[1]);
        lumsim.setMaxblum(_maxLum[1]);
        lumsim.optimize(&_data, &((*frames)[0]), &((*frames)[1]), dlum, blum, chi_value);
        monoluminosities->append(dlum);
        monoluminosities->append(blum);
    }
    if (find<AdjustableSkirtSimulation>()->ncomponents() >= 3)
    {
        GALumfit GAlumi;
        GAlumi.setMinLuminosities(_minLum);
        GAlumi.setMaxLuminosities(_maxLum);
        GAlumi.optimize(&_data, frames, monoluminosities, chi_value);
    }
    return chi_value;
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::returnFrame(QList<Array> *frames) const
{
    double chi_value;
    bool oneDfit=false;
    for(int i =0; i<frames->size(); i++)
    {
       _kernel->convolve((*frames)[i], xsize(), ysize());
    }
    if (frames->size() == 1)
    {
        GoldenSection gold;
        double lum;
        gold.setMinlum(_minLum[0]);
        gold.setMaxlum(_maxLum[0]);
        gold.optimize(&_data, &((*frames)[0]), lum, chi_value);
        (*frames)[0] = lum*((*frames)[0] + 0);
        (*frames).append(abs(_data-(*frames)[0])/abs(_data));
        oneDfit=true;
    }

    if (frames->size() == 2 && !oneDfit)
    {
        double dlum, blum;
        LumSimplex lumsim;
        lumsim.setMinDlum(_minLum[0]);
        lumsim.setMaxDlum(_maxLum[0]);
        lumsim.setMinblum(_minLum[1]);
        lumsim.setMaxblum(_maxLum[1]);
        lumsim.optimize(&_data, &((*frames)[0]), &((*frames)[1]), dlum, blum, chi_value);
        (*frames)[0] = dlum*(*frames)[0] + blum*(*frames)[1];
        (*frames)[1]= abs(_data-(*frames)[0])/abs(_data);
    }
    if (find<AdjustableSkirtSimulation>()->ncomponents() >= 3)
    {
        GALumfit GAlumi;
        QList<double> monoluminosities;
        GAlumi.setFixedSeed(find<OligoFitScheme>()->fixedSeed());
        GAlumi.setMinLuminosities(_minLum);
        GAlumi.setMaxLuminosities(_maxLum);
        GAlumi.optimize(&_data, frames, &(monoluminosities), chi_value);
        (*frames)[0] = monoluminosities[0]*(*frames)[0];
        for(int i =1;i<monoluminosities.size();i++)
        {
            (*frames)[0] = (*frames)[0] + monoluminosities[i]*(*frames)[i];
        }
        (*frames)[1]= abs(_data-(*frames)[0])/abs(_data);
    }
}

////////////////////////////////////////////////////////////////////
