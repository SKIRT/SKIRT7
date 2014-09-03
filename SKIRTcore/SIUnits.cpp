/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "SIUnits.hpp"

////////////////////////////////////////////////////////////////////

SIUnits::SIUnits()
{
    // select the appropriate units
    _unitForQty["length"] = "m";
    _unitForQty["distance"] = "m";
    _unitForQty["wavelength"] = "m";
    _unitForQty["grainsize"] = "m";
    _unitForQty["section"] = "m2";
    _unitForQty["volume"] = "m3";
    _unitForQty["velocity"] = "m/s";
    _unitForQty["mass"] = "kg";
    _unitForQty["bulkmass"] = "kg";
    _unitForQty["bulkmassdensity"] = "kg/m3";
    _unitForQty["masssurfacedensity"] = "kg/m2";
    _unitForQty["massvolumedensity"] = "kg/m3";
    _unitForQty["opacity"] = "m2/kg";
    _unitForQty["energy"] = "J";
    _unitForQty["bolluminosity"] = "W";
    _unitForQty["monluminosity"] = "W/m";
    _unitForQty["neutralfluxdensity"] = "W/m2";
    _unitForQty["neutralsurfacebrightness"] = "W/m2/sr";
    _unitForQty["wavelengthfluxdensity"] = "W/m3";
    _unitForQty["wavelengthsurfacebrightness"] = "W/m3/sr";
    _unitForQty["frequencyfluxdensity"] = "W/m2/Hz";
    _unitForQty["frequencysurfacebrightness"] = "W/m2/Hz/sr";
    _unitForQty["temperature"] = "K";
    _unitForQty["angle"] = "rad";
    _unitForQty["posangle"] = "rad";
    _unitForQty["solidangle"] = "sr";
    _unitForQty["pressure"] = "Pa";

    // initialize conversion factors in base class
    initCache();
}

////////////////////////////////////////////////////////////////////
