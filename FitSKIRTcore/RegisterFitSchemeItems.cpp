/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "RegisterFitSchemeItems.hpp"
#include "SimulationItemRegistry.hpp"

// add new items in alphabetical order

#include "AdjustableSkirtSimulation.hpp"
#include "Convolution.hpp"
#include "FitScheme.hpp"
#include "LumSimplex.hpp"
#include "MasterSlaveCommunicator.hpp"
#include "OligoFitScheme.hpp"
#include "Optimization.hpp"
#include "ParameterRange.hpp"
#include "ParameterRanges.hpp"
#include "ReferenceImage.hpp"
#include "ReferenceImages.hpp"


using namespace SimulationItemRegistry;

////////////////////////////////////////////////////////////////////

void RegisterFitSchemeItems::registerAll()
{
    // add new items in the order you want them to appear in choice lists for the user
    // !! provide an argument of "false" for abstract classes that shouldn't be instantiated

    add<FitScheme>(false);
    add<OligoFitScheme>();

    add<AdjustableSkirtSimulation>();
    add<Convolution>();
    add<LumSimplex>();
    add<MasterSlaveCommunicator>();
    add<Optimization>();
    add<ParameterRanges>();
    add<ParameterRange>();
    add<ReferenceImages>();
    add<ReferenceImage>();
}

////////////////////////////////////////////////////////////////////
