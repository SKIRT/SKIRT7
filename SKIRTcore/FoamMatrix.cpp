/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include <cmath>
#include "FoamMatrix.hpp"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////

FoamMatrix::FoamMatrix(int dim)
{
    Dim = dim;
    Dim2 = dim*dim;
    Matrix = 0;
    if (dim>0) Matrix = new double[dim*dim];
}

///////////////////////////////////////////////////////////////////////////////////////

FoamMatrix::FoamMatrix(const FoamMatrix& From)
{
    int i, j;
    Dim= From.Dim;
    Matrix = 0;
    if(Dim>0) Matrix = new double[Dim*Dim];
    for(i=0; i<Dim; i++)
        for(j=0; j<Dim; j++)
            *(Matrix + Dim*i + j) = *(From.Matrix + Dim*i + j);
}

///////////////////////////////////////////////////////////////////////////////////////

FoamMatrix::~FoamMatrix()
{
    if(Dim>0) delete[] Matrix;
}

///////////////////////////////////////////////////////////////////////////////////////

FoamMatrix&
FoamMatrix::operator=(const FoamMatrix& From)
{
    int i, j;
    if (&From == this) return *this;
    Dim=From.Dim;
    Matrix = new double[Dim*Dim];
    for(i=0; i<Dim; i++)
        for(j=0; j<Dim; j++)
            *(Matrix + Dim*i + j) = *(From.Matrix + Dim*i + j);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////////////

double&
FoamMatrix::operator()(int i, int j)
{
    return *(Matrix + Dim*i + j);
}

///////////////////////////////////////////////////////////////////////////////////////

double
FoamMatrix::Determinant()
{
    // Determinant using moderately efficient method (Gauss)
    int i, j, k, l;
    double temp, det, elmax;
    FoamMatrix m = *this;
    det=1;
    for(i=0; i<Dim; i++)
    {
        for(j=i+1; j<Dim; j++)    // i-th row will be subtracted from j-th such that M[j,i]=0
        {
            l=i;
            elmax= 0.0;
            for(k=i; k<Dim;k++)   // looking for a row with maximum modulus of i-th element;
                if( fabs(m(k,i))>elmax)
                {
                    elmax=fabs(m(k,i));
                    l=k;
                }
            if(elmax==0.0) return 0.0;
            if (l!=i)             // swapping two rows
            {
                for(k=0; k<Dim; k++)
                {
                    temp = m(i,k);
                    m(i,k)=m(l,k);
                    m(l,k)=temp;
                }
                det *= -1;
            }
            temp = m(j,i)/m(i,i);  // subtract i-th from j-th such that M[j,i]=0
            for(k=0; k<Dim; k++) m(j,k) -= m(i,k) *temp;
        }
    }
    // matrix is now triangle type
    for(i=0; i<Dim; i++)
        det *= m(i,i);
    return det;
}

///////////////////////////////////////////////////////////////////////////////////////
