/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#include "FoamVector.hpp"
#include "FatalError.hpp"

///////////////////////////////////////////////////////////////////////////////

FoamVector::FoamVector(int n)
{
    // constructor creates n-dimensional vector and its array of components
    m_Dim = n;
    m_Coords = m_Dim>0 ? new double[m_Dim] : 0;
    for (int i=0; i<n; i++) m_Coords[i] = 0.0;
}

///////////////////////////////////////////////////////////////////////////////

FoamVector::~FoamVector()
{
    delete[] m_Coords;
}

///////////////////////////////////////////////////////////////////////////////

double&
FoamVector::operator[](int n)
{
    // [] is for acces to elements as in ordinary matrix like a[j]=b[j]
    // Range protection is built in, consequently for substitution
    // one should use rather use a=b than explicit loop!
    if ((n<0) || (n>=m_Dim)) throw FATALERROR("Index out of range");
    return m_Coords[n];
}

//////////////////////////////////////////////////////////////////////////////

FoamVector&
FoamVector::operator=(const FoamVector& Vect)
{
    if (&Vect == this) return *this;
    if (m_Dim != Vect.m_Dim) throw FATALERROR("Dimensions are different");
    m_Dim=Vect.m_Dim;
    for (int i=0; i<m_Dim; i++)
        m_Coords[i] = Vect.m_Coords[i];
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

FoamVector&
FoamVector::operator=(double Vect[])
{
    // Load in double vector
    if (m_Coords)
        for (int i=0; i<m_Dim; i++)
            m_Coords[i] = Vect[i];
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

FoamVector&
FoamVector::operator=(double x)
{
    // Load in double number
    if (m_Coords)
        for (int i=0; i<m_Dim; i++)
            m_Coords[i] = x;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
