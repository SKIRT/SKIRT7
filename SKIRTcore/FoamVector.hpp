/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef FOAMVECTOR_HPP
#define FOAMVECTOR_HPP

////////////////////////////////////////////////////////////////////

/** A FoamVector instance represents a vector used by the Foam class. This class is part
    of the foam classes that have been copied from an external library and adjusted to fit the
    SKIRT mold. There is no documentation other than the sparse comments in the source code. */
class FoamVector
{
private:
    int m_Dim;
    double* m_Coords;
public:
    FoamVector(int);
    ~FoamVector();
    double& operator[](int);
    FoamVector& operator=(const FoamVector&);
    FoamVector& operator=(double []);
    FoamVector& operator=(double);
    int GetDim() { return m_Dim; }
    double GetCoord(int i) { return m_Coords[i]; }
private:
    FoamVector(const FoamVector&);
};

/////////////////////////////////////////////////////////////////////

#endif // FOAMVECTOR_HPP
