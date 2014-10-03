/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ReferenceImage.hpp"

#include "AdjustableSkirtSimulation.hpp"
#include "Convolution.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "GALumfit.hpp"
#include "GoldenSection.hpp"
#include "Log.hpp"
#include "LumSimplex.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

ReferenceImage::ReferenceImage()
    :_convolution(0)
{
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // Reads the reference image and stores it.
    QString filepath = find<FilePaths>()->input(_path);
    find<Log>()->info("Read reference image at " + filepath);
    FITSInOut::read(filepath,_refim,_xdim,_ydim,_zdim);
    find<Log>()->info("Frame dimensions: " + QString::number(_xdim) + " x " + QString::number(_ydim));
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::setPath(QString value)
{
    _path = value;
}

////////////////////////////////////////////////////////////////////

QString ReferenceImage::path() const
{
    return _path;
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::setConvolution(Convolution* value)
{
    if (_convolution) delete _convolution;
    _convolution = value;
    if (_convolution) _convolution->setParent(this);
}

////////////////////////////////////////////////////////////////////

Convolution* ReferenceImage::convolution() const
{
    return _convolution;
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
    //HERE IS WHERE I'LL HAVE TO DECIDE WHICH OPTIMIZATION ALGORITHM FOR LUM FIT

    double chi_value = 0;
    for(int i =0;i<frames->size();i++)
    {
       _convolution->convolve(&((*frames)[i]), _xdim, _ydim);
    }

    if (find<AdjustableSkirtSimulation>()->ncomponents() == 1)
    {
        double lum;
        if (_minLum.size() != 1 && _maxLum.size() != 1)
            throw FATALERROR("Number of luminosity boundaries differs from 1!");
        GoldenSection gold;
        gold.setMinlum(_minLum[0]);
        gold.setMaxlum(_maxLum[0]);
        gold.optimize(&_refim,&((*frames)[0]),lum,chi_value);
        monoluminosities->append(lum);
    }

    if (find<AdjustableSkirtSimulation>()->ncomponents() == 2)
    {
        if (_minLum.size() != 2 && _maxLum.size() != 2)
            throw FATALERROR("Number of luminosity boundaries differs from 2!");

        double dlum,b2d;
        LumSimplex lumsim;
        lumsim.setMinDlum(_minLum[0]);
        lumsim.setMaxDlum(_maxLum[0]);
        lumsim.setMinB2D(_minLum[1]/_minLum[0]);
        lumsim.setMaxB2D(_maxLum[1]/_maxLum[0]);
        lumsim.optimize(&_refim,&((*frames)[0]),&((*frames)[1]),dlum,b2d,chi_value);
        monoluminosities->append(dlum);
        monoluminosities->append(dlum*b2d);
    }
    if (find<AdjustableSkirtSimulation>()->ncomponents() >= 3)
    {
        GALumfit GAlumi;
        GAlumi.setMinLuminosities(_minLum);
        GAlumi.setMaxLuminosities(_maxLum);
        GAlumi.optimize(&_refim,frames,monoluminosities,chi_value);
    }
    return chi_value;
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::returnFrame(QList<Array> *frames) const
{
    double chi_value;
    bool oneDfit;
    for(int i =0;i<frames->size();i++)
    {
       _convolution->convolve(&((*frames)[i]), _xdim, _ydim);
    }
    if (frames->size() == 1)
    {
        GoldenSection gold;
        double lum;
        gold.setMinlum(_minLum[0]);
        gold.setMaxlum(_maxLum[0]);
        gold.optimize(&_refim,&((*frames)[0]),lum,chi_value);
        (*frames)[0] = lum*((*frames)[0] + 0);
        (*frames).append(abs(_refim-(*frames)[0])/abs(_refim));
        oneDfit=true;
    }

    if (frames->size() == 2 && !oneDfit)
    {
        double dlum, b2d;
        LumSimplex lumsim;
        lumsim.setMinDlum(_minLum[0]);
        lumsim.setMaxDlum(_maxLum[0]);
        lumsim.setMinB2D(_minLum[1]/_minLum[0]);
        lumsim.setMaxB2D(_maxLum[1]/_maxLum[0]);
        lumsim.optimize(&_refim,&((*frames)[0]),&((*frames)[1]),dlum,b2d,chi_value);
        (*frames)[0] = dlum*((*frames)[0] + b2d*((*frames)[1]));
        (*frames)[1]= abs(_refim-(*frames)[0])/abs(_refim);
    }
    if (find<AdjustableSkirtSimulation>()->ncomponents() >= 3)
    {
        GALumfit GAlumi;
        QList<double> monoluminosities;
        GAlumi.setMinLuminosities(_minLum);
        GAlumi.setMaxLuminosities(_maxLum);
        GAlumi.optimize(&_refim,frames,&(monoluminosities),chi_value);
        (*frames)[0] = monoluminosities[0]*(*frames)[0];
        for(int i =1;i<monoluminosities.size();i++)
        {
            (*frames)[0] = (*frames)[0] + monoluminosities[i]*(*frames)[i];
        }
        (*frames)[1]= abs(_refim-(*frames)[0])/abs(_refim);
    }
}

////////////////////////////////////////////////////////////////////
int ReferenceImage::xdim() const
{
    return _xdim;
}

////////////////////////////////////////////////////////////////////

int ReferenceImage::ydim() const
{
    return _ydim;
}

////////////////////////////////////////////////////////////////////

Array ReferenceImage::refImage() const
{
    return _refim;
}

////////////////////////////////////////////////////////////////////
