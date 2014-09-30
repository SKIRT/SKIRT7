/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "ReferenceImage.hpp"

#include "Convolution.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "FITSInOut.hpp"
#include "Log.hpp"
#include "LumSimplex.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

ReferenceImage::ReferenceImage()
    :_convolution(0), _lumsimplex(0)
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

////////////////////////////////////////////////////////////////////

void ReferenceImage::setLumSimplex(LumSimplex* value)
{
    if (_lumsimplex) delete _lumsimplex;
    _lumsimplex = value;
    if (_lumsimplex) _lumsimplex->setParent(this);
}

////////////////////////////////////////////////////////////////////

LumSimplex* ReferenceImage::lumSimplex() const
{
    return _lumsimplex;
}

////////////////////////////////////////////////////////////////////

double ReferenceImage::chi2(QList<Array> *frames, QList<double> *luminosities) const
{
    //HERE IS WHERE I'LL HAVE TO DECIDE WHICH OPTIMIZATION ALGORITHM FOR LUM FIT

    double chi_value = 0;
    double dlum,b2d;

    _convolution->convolve(&((*frames)[0]), _xdim, _ydim);
    _convolution->convolve(&((*frames)[1]), _xdim, _ydim);

    _lumsimplex->optimize(&_refim,&((*frames)[0]),&((*frames)[1]),dlum,b2d,chi_value);
    luminosities->append(dlum);
    luminosities->append(b2d);
    return chi_value;
}

////////////////////////////////////////////////////////////////////

void ReferenceImage::returnFrame(Array *Dframe, Array *Bframe) const
{
    double chi_value, dlum, b2d;

    _convolution->convolve(Dframe, _xdim, _ydim);
    _convolution->convolve(Bframe, _xdim, _ydim);

    _lumsimplex->optimize(&_refim,Dframe,Bframe,dlum,b2d,chi_value);

    *Dframe = dlum*((*Dframe) + b2d*((*Bframe)));
    *Bframe = abs(_refim-(*Dframe))/abs(_refim);
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
