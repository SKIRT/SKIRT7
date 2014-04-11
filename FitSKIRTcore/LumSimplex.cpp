/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#include "LumSimplex.hpp"
#include "FatalError.hpp"
#include "Log.hpp"
#include <math.h>

using namespace std;

////////////////////////////////////////////////////////////////////

LumSimplex::LumSimplex()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void LumSimplex::setupSelfBefore()
{
    SimulationItem::setupSelfBefore();
}

////////////////////////////////////////////////////////////////////

void LumSimplex::setMinDlum(double value)
{
    _minDlum = value;
}

////////////////////////////////////////////////////////////////////

double LumSimplex::minDlum() const
{
    return _minDlum;
}

////////////////////////////////////////////////////////////////////

void LumSimplex::setMaxDlum(double value)
{
    _maxDlum = value;
}

////////////////////////////////////////////////////////////////////

double LumSimplex::maxDlum() const
{
    return _maxDlum;
}

////////////////////////////////////////////////////////////////////

void LumSimplex::setMinB2D(double value)
{
    _minB2D = value;
}

////////////////////////////////////////////////////////////////////

double LumSimplex::minB2D() const
{
    return _minB2D;
}

////////////////////////////////////////////////////////////////////

void LumSimplex::setMaxB2D(double value)
{
    _maxB2D = value;
}

////////////////////////////////////////////////////////////////////

double LumSimplex::maxB2D() const
{
    return _maxB2D;
}

////////////////////////////////////////////////////////////////////

bool LumSimplex::inSimplex(double simplex[3][3], double value, int x_y ) const
{
    bool present = false;
    for (int i=0; i<3; i++)
        if(simplex[x_y][i]==value) present = true;

    return present;
}

////////////////////////////////////////////////////////////////////

double LumSimplex::function(Array *disk, Array *bulge, double x, double y)
{
    double chi = 0;
    int arraysize = disk->size();

    for (int m=0; m<arraysize; m++)
    {
        double total_sim = x * ((*disk)[m] + y * (*bulge)[m]);
        double sigma = sqrt( abs((*_ref)[m]) + total_sim);
        if ((*_ref)[m]==0)
        {
            (*disk)[m] = 0;
            (*bulge)[m] = 0;
            total_sim = 0;
            sigma = 0;
        }
        else
        {
            chi += pow( ((*_ref)[m] - total_sim) / sigma,2);
        }
    }
    return chi;
}

////////////////////////////////////////////////////////////////////

void LumSimplex::contract(Array *disk, Array *bulge,
                          double simplex [3][3], double center[], double refl[], double Beta, double Delta)
{
    double point[3];

    if (simplex[2][1] <= refl[2] && refl[2] < simplex[2][2])
    {
        point[0] = center[0] + Beta * (refl[0]-center[0]);
        point[1] = center[1] + Beta * (refl[1]-center[1]);
        point[2] = function( disk, bulge, point[0],point[1]);

        if (point[2] <= refl[2]) place(disk, bulge, simplex, point[0],point[1]);
        else shrink(disk, bulge, simplex, Delta);
    }
    else
    {
        if (refl[2]>=simplex[2][2])
        {
            point[0] = center[0] + Beta * (simplex[0][2] - center[0]);
            point[1] = center[1] + Beta * (simplex[1][2] - center[1]);
            point[2] = function( disk, bulge, point[0],point[1]);

            if (point[2]<simplex[2][2]) place(disk, bulge, simplex, point[0],point[1]);
            else shrink(disk, bulge, simplex, Delta);
        }
        else shrink(disk, bulge, simplex, Delta);
    }
}

////////////////////////////////////////////////////////////////////

void LumSimplex::expand(Array *disk, Array *bulge,
                        double simplex[3][3], double center[], double refl[], int counter, double Gamma)
{
    double point[3];
    point[0] = center[0] + Gamma * (refl[0]-center[0]);
    point[1] = center[1] + Gamma * (refl[1]-center[1]);
    nearEdgeCorrections(simplex, point, counter);
    point[2] = function( disk, bulge, point[0],point[1]);

    if (point[2]<refl[2]) place(disk, bulge, simplex,point[0],point[1]);
    else place(disk, bulge, simplex, refl[0],refl[1]);
}

////////////////////////////////////////////////////////////////////

void LumSimplex::initialize(Array *disk, Array *bulge, double simplex[3][3])
{
    //determine the initial simplex points
    double xle = _maxDlum - _minDlum;
    double yle = _maxB2D - _minB2D;
    double x_1 = _minDlum + 0.8*xle;
    double y_1 = _minB2D + 0.5*yle;
    double x_2 = _minDlum + 0.45*xle;
    double y_2 = _minB2D + 0.05*yle;
    double x_3 = _minDlum + 0.20*xle;
    double y_3 = _minB2D + 0.82*yle;
    double x_max = x_1;
    double y_max = y_1;

    //determine the point with highest chi2
    if (function( disk, bulge, x_1,y_1) > function( disk, bulge, x_2,y_2) && function( disk, bulge, x_1,y_1) > function( disk, bulge, x_3,y_3))
    {
        x_max = x_1;
        y_max = y_1;
    }
    if (function( disk, bulge, x_2,y_2) > function( disk, bulge, x_1,y_1) && function( disk, bulge, x_2,y_2) > function( disk, bulge, x_3,y_3))
    {
        x_max = x_2;
        y_max = y_2;
    }
    if (function( disk, bulge, x_3,y_3) > function( disk, bulge, x_2,y_2) && function( disk, bulge, x_3,y_3) > function( disk, bulge, x_1,y_1))
    {
        x_max = x_3;
        y_max = y_3;
    }

    //fill the simplex with the worst value
    for (int l=0; l<3; l++)
    {
        simplex[0][l] = x_max;
        simplex[1][l] = y_max;
        simplex[2][l] = function( disk, bulge, x_max,y_max);
    }


    //add the other values, ranked from best to worst
    place(disk, bulge, simplex, x_1,y_1);
    place(disk, bulge, simplex, x_2,y_2);
    place(disk, bulge, simplex, x_3,y_3);
}

////////////////////////////////////////////////////////////////////

void LumSimplex::nearEdgeCorrections(double simplex[3][3], double Dpoint[], int counter) const
{
    double xle = _maxDlum - _minDlum;
    double yle = _maxB2D - _minB2D;
    int mod = counter/2;

    if (Dpoint[0]> _maxDlum)
    {
        if (inSimplex(simplex, _maxDlum,0)) Dpoint[0] = _maxDlum - (1+mod) * 0.01 * xle;
        else Dpoint[0] = _maxDlum;
    }
    if (Dpoint[0]< _minDlum)
    {
        if (inSimplex(simplex, _minDlum,0)) Dpoint[0] = _minDlum + (1+mod) * 0.01 * xle;
        else Dpoint[0] = _minDlum;
    }
    if (Dpoint[1]> _maxB2D)
    {
        if (inSimplex(simplex, _maxB2D,1)) Dpoint[1] = _maxB2D - (1+mod) * 0.01 * yle;
        else Dpoint[1] = _maxB2D;
    }
    if (Dpoint[1]<_minB2D)
    {
        if (inSimplex(simplex, _minB2D,1)) Dpoint[1] = _minB2D + (1+mod) * 0.01 * yle;
        else Dpoint[1] = _minB2D;
    }
}

////////////////////////////////////////////////////////////////////

void LumSimplex::place(Array *disk, Array *bulge,
                       double simplex[3][3], double x, double y)
{
    for (int i=0; i<3; i++)
    {
        if (function( disk, bulge, x,y) <= simplex[2][i])
        {
            for (int j=2; j>i; j--)
            {
                simplex[0][j] = simplex[0][j-1];
                simplex[1][j] = simplex[1][j-1];
                simplex[2][j] = simplex[2][j-1];
            }
            simplex[0][i] = x;
            simplex[1][i] = y;
            simplex[2][i] = function( disk, bulge, x,y);
            i = 4;
        }
    }
}

////////////////////////////////////////////////////////////////////

void LumSimplex::optimize(const Array *Rframe, Array *Dframe,
                          Array *Bframe, double &Dlum, double &B2Dratio, double &chi2)
{

    _ref = Rframe;
    Array *disk = Dframe;
    Array *bulge = Bframe;


    double simplex[3][3];

    //Alpha, Beta, Gamma and Delta are simplex optimization parameters
    double Alpha = 1;
    double Beta = 0.5;
    double Gamma = 2;
    double Delta = 0.5;
    initialize(disk, bulge, simplex);

    for (int i=0; i<200; i++)
    {
        //store simplex to check recurrency
        double previous_simpl[3][3];
        for (int j=0; j<3; j++)
        {
            for(int k=0; k<3; k++)
                previous_simpl[j][k] = simplex[j][k];
        }

        //set center and reflected point
        double center[3],refl[3];
        setCenterReflected(disk, bulge, simplex, center, refl, i, Alpha);

        //determine if the simplex needs reflection, expansion or contraction
        if (simplex[2][0] <= refl[2] && refl[2] < simplex[2][1]) place(disk, bulge, simplex, refl[0],refl[1]);
        else
        {
            if (refl[2] < simplex[2][0]) expand(disk, bulge, simplex, center,refl,i,Gamma);
            else if (refl[2] >= simplex[2][1]) contract(disk, bulge, simplex, center,refl,Beta,Delta);
        }

        //recurrency corrections
        if (previous_simpl[0][0] == simplex[0][0] && previous_simpl[0][1] == simplex[1][0]
           && previous_simpl[1][0] == simplex[0][1] && previous_simpl[1][1] == simplex[1][1]
           && previous_simpl[2][0] == simplex[0][2] && previous_simpl[2][1] == simplex[1][2])
        {
            place( disk, bulge, simplex, (simplex[0][2] + simplex[0][1] + simplex[0][0]) / 3,
                    (simplex[1][2] + simplex[1][1] + simplex[1][0]) / 3);
        }

        //end loop if there is hardly any improvement
        double x_diff = abs(simplex[0][0] - simplex[0][1]) + abs(simplex[0][0] - simplex[0][2]);
        double y_diff = abs(simplex[1][0] - simplex[1][1]) + abs(simplex[1][0] - simplex[1][2]);
        if(x_diff <= 1e-6 && y_diff <= 1e-6) i=200;
    }

    Dlum = simplex[0][0];
    B2Dratio = simplex[1][0];
    chi2 = simplex[2][0];

    Dframe = disk;
    Bframe = bulge;

}

////////////////////////////////////////////////////////////////////

void LumSimplex::setCenterReflected(Array *disk, Array *bulge,
                                    double simplex[3][3], double center[], double reflected[], int counter, double Alpha)
{
    double averx = 0;
    double avery = 0;

    for (int i=0; i<2; i++)
    {
        averx += simplex[0][i];
        avery += simplex[1][i];
    }
    center[0] = averx/2;
    center[1] = avery/2;
    center[2] = function( disk, bulge, center[0],center[1]);

    reflected[0] = center[0] + Alpha * (center[0] - simplex[0][2]);
    reflected[1] = center[1] + Alpha * (center[1] - simplex[1][2]);
    nearEdgeCorrections(simplex, reflected,counter);
    reflected[2] = function( disk, bulge, reflected[0],reflected[1]);
}

////////////////////////////////////////////////////////////////////

void LumSimplex::shrink(Array *disk, Array *bulge,
                        double simplex[3][3], double Delta)
{
    double x_1 = simplex[0][0] + Delta * (simplex[0][1] - simplex[0][0]);
    double x_2 = simplex[0][0] + Delta * (simplex[0][2] - simplex[0][0]);
    double y_1 = simplex[1][0] + Delta * (simplex[1][1] - simplex[1][0]);
    double y_2 = simplex[1][0] + Delta * (simplex[1][2] - simplex[1][0]);

    place(disk, bulge, simplex, x_1,y_1);
    place(disk, bulge, simplex, x_2,y_2);
}

////////////////////////////////////////////////////////////////////
