/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include <vector>
#include "DustGridPath.hpp"
#include "DustGridPlotFile.hpp"
#include "DustGridStructure.hpp"
#include "Direction.hpp"
#include "FatalError.hpp"
#include "FilePaths.hpp"
#include "Log.hpp"
#include "MonteCarloSimulation.hpp"
#include "Position.hpp"
#include "Random.hpp"
#include "Units.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustGridStructure::DustGridStructure()
    : _writeGrid(true), _random(0), _Ncells(0)
{
}

//////////////////////////////////////////////////////////////////////

void DustGridStructure::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();

    // verify that the grid has at least the dimension of the simulation's star and dust geometry
    int dimGeometry = find<MonteCarloSimulation>()->dimension();
    int dimGrid = dimension();
    if (dimGeometry > dimGrid)
        throw FATALERROR("The grid dimension " + QString::number(dimGrid) +
                         " is lower than the geometry dimension " + QString::number(dimGeometry));

    // cache data member used in subclasses
    _random = find<Random>();
}

//////////////////////////////////////////////////////////////////////

void DustGridStructure::setupSelfAfter()
{
    SimulationItem::setupSelfAfter();

    if (_writeGrid)
    {
        int dimension = find<MonteCarloSimulation>()->dimension();
        FilePaths* filepaths = find<FilePaths>();
        Units* units = find<Units>();
        Log* log = find<Log>();

        // For the xy plane (always)
        {
            DustGridPlotFile outfile(filepaths->output("ds_gridxy.dat"), log, units);
            write_xy(&outfile);
        }

        // For the xz plane (only if dimension is at least 2)
        if (dimension >= 2)
        {
            DustGridPlotFile outfile(filepaths->output("ds_gridxz.dat"), log, units);
            write_xz(&outfile);
        }

        // For the yz plane (only if dimension is 3)
        if (dimension == 3)
        {
            DustGridPlotFile outfile(filepaths->output("ds_gridyz.dat"), log, units);
            write_yz(&outfile);
        }

        // Full 3D coordinates (only if dimension is 3)
        if (dimension == 3)
        {
            DustGridPlotFile outfile(filepaths->output("ds_gridxyz.dat"), log, units);
            write_xyz(&outfile);
        }
    }
}

//////////////////////////////////////////////////////////////////////

void DustGridStructure::setWriteGrid(bool value)
{
    _writeGrid = value;
}

//////////////////////////////////////////////////////////////////////

bool DustGridStructure::writeGrid() const
{
    return _writeGrid;
}

////////////////////////////////////////////////////////////////////

int DustGridStructure::Ncells() const
{
    return _Ncells;
}

//////////////////////////////////////////////////////////////////////

double DustGridStructure::weight(int m) const
{
    if (m==-1)
        return 0.0;
    else
        return 1.0;
}

//////////////////////////////////////////////////////////////////////

void DustGridStructure::write_xy(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////

void DustGridStructure::write_xz(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////

void DustGridStructure::write_yz(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////

void DustGridStructure::write_xyz(DustGridPlotFile* /*outfile*/) const
{
}

//////////////////////////////////////////////////////////////////////
