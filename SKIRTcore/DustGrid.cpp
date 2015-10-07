/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <vector>
#include "DustGrid.hpp"
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "Direction.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "MonteCarloSimulation.hpp"
#include "Position.hpp"
#include "Units.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustGrid::DustGrid()
    : _writeGrid(true), _Ncells(0), _boundingbox()
{
}

//////////////////////////////////////////////////////////////////////

void DustGrid::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify that the grid has at least the dimension of the simulation's star and dust geometry
    int dimGeometry = find<MonteCarloSimulation>()->dimension();
    int dimGrid = dimension();
    if (dimGeometry > dimGrid)
        throw FATALERROR("The grid dimension " + QString::number(dimGrid) +
                         " is lower than the geometry dimension " + QString::number(dimGeometry));
}

//////////////////////////////////////////////////////////////////////

void DustGrid::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    if (_writeGrid)
    {
        int dimension = find<MonteCarloSimulation>()->dimension();

        // For the xy plane (always)
        {
            DustGridPlotFile outfile(this, "ds_gridxy");
            write_xy(&outfile);
        }

        // For the xz plane (only if dimension is at least 2)
        if (dimension >= 2)
        {
            DustGridPlotFile outfile(this, "ds_gridxz");
            write_xz(&outfile);
        }

        // For the yz plane (only if dimension is 3)
        if (dimension == 3)
        {
            DustGridPlotFile outfile(this, "ds_gridyz");
            write_yz(&outfile);
        }

        // Full 3D coordinates (only if dimension is 3)
        if (dimension == 3)
        {
            DustGridPlotFile outfile(this, "ds_gridxyz");
            write_xyz(&outfile);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void DustGrid::setWriteGrid(bool value)
{
    _writeGrid = value;
}

//////////////////////////////////////////////////////////////////////

bool DustGrid::writeGrid() const
{
    return _writeGrid;
}

////////////////////////////////////////////////////////////////////

void DustGrid::setNumCells(int value)
{
    _Ncells = value;
}

////////////////////////////////////////////////////////////////////

int DustGrid::numCells() const
{
    return _Ncells;
}

////////////////////////////////////////////////////////////////////

void DustGrid::setBoundingbox(Box value)
{
    _boundingbox = value;
}

////////////////////////////////////////////////////////////////////

Box DustGrid::boundingbox() const
{
    return _boundingbox;
}

//////////////////////////////////////////////////////////////////////

double DustGrid::weight(int m) const
{
    return (m==-1) ? 0.0 : 1.0;
}

//////////////////////////////////////////////////////////////////////

void DustGrid::write_xy(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////

void DustGrid::write_xz(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////

void DustGrid::write_yz(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////

void DustGrid::write_xyz(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////
