/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef FOAMMATRIX_HPP
#define FOAMMATRIX_HPP

////////////////////////////////////////////////////////////////////

/** A FoamMatrix instance represents a square matrix used by the Foam class. This class is part
    of the foam classes that have been copied from an external library and adjusted to fit the
    SKIRT mold. There is no documentation other than the sparse comments in the source code. */
class FoamMatrix
{
private:
    int Dim;                // Dimension
    int Dim2;               // Dimension
    double* Matrix;         // [dim2] Table of elements
public:
    FoamMatrix(int dim);
    FoamMatrix(const FoamMatrix& From);
    ~FoamMatrix();
    FoamMatrix& operator=(const FoamMatrix& From);
    double& operator()(int i, int j);
    double Determinant();
private:
    FoamMatrix();
};

////////////////////////////////////////////////////////////////////

#endif // FOAMMATRIX_HPP
