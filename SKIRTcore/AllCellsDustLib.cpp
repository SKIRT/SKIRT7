/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "AllCellsDustLib.hpp"
#include "DustSystem.hpp"
#include "PeerToPeerCommunicator.hpp"
#include "WavelengthGrid.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

AllCellsDustLib::AllCellsDustLib()
{
}

////////////////////////////////////////////////////////////////////

int AllCellsDustLib::entries() const
{
    return find<DustSystem>()->Ncells();
}

////////////////////////////////////////////////////////////////////

std::vector<int> AllCellsDustLib::mapping() const
{
    // create a mapping from each cell index to identical library index
    int Ncells = entries();
    vector<int> nv(Ncells);
    for (int m=0; m<Ncells; m++) nv[m] = m;
    return nv;
}
