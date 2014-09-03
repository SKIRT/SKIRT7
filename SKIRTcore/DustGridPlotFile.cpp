/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include <iomanip>
#include "DustGridPlotFile.hpp"
#include "Log.hpp"
#include "Units.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////

DustGridPlotFile::DustGridPlotFile(QString filename, Log *log, Units *units)
    : _filename(filename), _log(log), _units(units)
{
    _log->info("Writing data to plot the grid structure to " + _filename + "...");
    _stream.open(_filename.toLocal8Bit().constData());
    _stream << setprecision(8);
}

////////////////////////////////////////////////////////////////////

DustGridPlotFile::~DustGridPlotFile()
{
    _stream.close();
    _log->info("File " + _filename + " created.");
}

////////////////////////////////////////////////////////////////////

void DustGridPlotFile::writeLine(double beg1, double beg2, double end1, double end2)
{
    beg1 = _units->olength(beg1);
    beg2 = _units->olength(beg2);
    end1 = _units->olength(end1);
    end2 = _units->olength(end2);
    _stream << beg1 << '\t' << beg2 << '\n';
    _stream << end1 << '\t' << end2 << '\n' << '\n';
}

////////////////////////////////////////////////////////////////////

void DustGridPlotFile::writeRectangle(double min1, double min2, double max1, double max2)
{
    min1 = _units->olength(min1);
    min2 = _units->olength(min2);
    max1 = _units->olength(max1);
    max2 = _units->olength(max2);
    _stream << min1 << '\t' << min2 << '\n';
    _stream << min1 << '\t' << max2 << '\n';
    _stream << max1 << '\t' << max2 << '\n';
    _stream << max1 << '\t' << min2 << '\n';
    _stream << min1 << '\t' << min2 << '\n' << '\n';
}

////////////////////////////////////////////////////////////////////

void DustGridPlotFile::writeCircle(double radius)
{
    radius = _units->olength(radius);

    for (int l=0; l<=360; l++)
    {
        double phi = l*M_PI/180;
        _stream << radius*cos(phi) << '\t' << radius*sin(phi) << '\n';
    }
    _stream << '\n';
}

////////////////////////////////////////////////////////////////////

void DustGridPlotFile::writeLine(double x1, double y1, double z1, double x2, double y2, double z2)
{
    x1 = _units->olength(x1);
    y1 = _units->olength(y1);
    z1 = _units->olength(z1);
    x2 = _units->olength(x2);
    y2 = _units->olength(y2);
    z2 = _units->olength(z2);
    _stream << x1 << '\t' << y1 << '\t' << z1 << '\n'
            << x2 << '\t' << y2 << '\t' << z2 << '\n' << '\n';
}

////////////////////////////////////////////////////////////////////

void DustGridPlotFile::writeCube(double x1, double y1, double z1, double x2, double y2, double z2)
{
    x1 = _units->olength(x1);
    y1 = _units->olength(y1);
    z1 = _units->olength(z1);
    x2 = _units->olength(x2);
    y2 = _units->olength(y2);
    z2 = _units->olength(z2);
    _stream << x1 << '\t' << y1 << '\t' << z1 << '\n';
    _stream << x2 << '\t' << y1 << '\t' << z1 << '\n';
    _stream << x2 << '\t' << y2 << '\t' << z1 << '\n';
    _stream << x1 << '\t' << y2 << '\t' << z1 << '\n';
    _stream << x1 << '\t' << y1 << '\t' << z1 << '\n' << '\n';

    _stream << x1 << '\t' << y1 << '\t' << z2 << '\n';
    _stream << x2 << '\t' << y1 << '\t' << z2 << '\n';
    _stream << x2 << '\t' << y2 << '\t' << z2 << '\n';
    _stream << x1 << '\t' << y2 << '\t' << z2 << '\n';
    _stream << x1 << '\t' << y1 << '\t' << z2 << '\n' << '\n';

    _stream << x1 << '\t' << y1 << '\t' << z1 << '\n'
            << x1 << '\t' << y1 << '\t' << z2 << '\n' << '\n';
    _stream << x2 << '\t' << y1 << '\t' << z1 << '\n'
            << x2 << '\t' << y1 << '\t' << z2 << '\n' << '\n';
    _stream << x2 << '\t' << y2 << '\t' << z1 << '\n'
            << x2 << '\t' << y2 << '\t' << z2 << '\n' << '\n';
    _stream << x1 << '\t' << y2 << '\t' << z1 << '\n'
            << x1 << '\t' << y2 << '\t' << z2 << '\n' << '\n';
}

////////////////////////////////////////////////////////////////////

void DustGridPlotFile::writePolyhedron(const std::vector<double>& coords, const std::vector<int>& indices)
{
    unsigned int k = 0;
    while (k < indices.size())
    {
        int numvertices = indices[k++];
        int firstindex = indices[k];
        for (int i=0; i<numvertices; i++)
        {
            int currentindex = indices[k++];
            double x = _units->olength(coords[3*currentindex]);
            double y = _units->olength(coords[3*currentindex+1]);
            double z = _units->olength(coords[3*currentindex+2]);
            _stream << x << '\t' << y << '\t' << z << '\n';
        }
        double x = _units->olength(coords[3*firstindex]);
        double y = _units->olength(coords[3*firstindex+1]);
        double z = _units->olength(coords[3*firstindex+2]);
        _stream << x << '\t' << y << '\t' << z << '\n' << '\n';
    }
}

////////////////////////////////////////////////////////////////////
