/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent Un0versity         ////
//////////////////////////////////////////////////////////////////*/

#ifndef ARRAYTABLE_HPP
#define ARRAYTABLE_HPP

#include "Array.hpp"
#include <vector>

////////////////////////////////////////////////////////////////////

/** \class ArrayTable
The ArrayTable<N> class template implements multi-dimensional tables with special
support for \em rows in the last table dimension. Specifically, a ArrayTable<N> instance holds an
N-dimensional table of double values. The combination of the first N-1 indices addresses a row
in an (N-1)-dimensional table of rows; the last index addresses a column within a row. The
values are stored as a list of Array objects, allowing read and write access to individual
values as well as to rows of values as a whole.

It is possible and allowed to resize the rows individually through their reference. In that
case the caller should ensure that all rows end up with the same size (the ArrayTable
implementation does not check this requirement and does not rely on it). Note that the
size(N-1) function returns the original row size as it has been specified in the constructor
or in the most recent resize() invocation. The rowsize() function returns the size of the
first row in the table, or zero if the table is empty.

Since C++98 has no type-safe support for a variable number of function call arguments, we
provide a separate template specialization for each value of N (currently 2, 3 and 4).

Here's a synopis of ArrayTable functionality.

\verbatim
template <N> class ArrayTable
{
    // Constructs an empty table.
    ArrayTable();

    // Constructs a table with the specified number of items in each of N dimensions.
    // All values are set to zero.
    ArrayTable(size_t n_0, ..., size_t n_Nm1);

    // Resizes the table to the specified number of items in each of N dimensions.
    // All values are set to zero.
    void resize(size_t n_0, ..., size_t n_Nm1);

    // Returns the number of items in the indicated dimension as it has been specified
    // in the constructor or in the most recent resize() invocation.
    size_t size(size_t dim) const;

    // Returns the number of columns in a row, i.e. the number of items in the last dimension,
    // defined as the size of the first row in the table, or zero if the table is empty.
    size_t rowsize() const;

    // Returns a (readonly or writable) reference to the value at the specified N indices.
    const double& operator()(size_t i_0, ..., size_t i_Nm1) const;
    double& operator()(size_t i_0, ..., size_t i_Nm1);

    // Returns a (readonly or writable) reference to the row at the specified N-1 indices.
    const Array& operator()(size_t i_0, ..., size_t i_Nm2) const;
    Array& operator()(size_t i_0, ..., size_t i_Nm2);

    // Returns a (readonly or writable) reference to the row at the specified index (2D only).
    const Array& operator[](size_t i) const;
    Array& operator[](size_t i);
};
\endverbatim
*/
template<size_t NDIM> class ArrayTable;

// hide the rest of the code from Doxygen
/// \cond

////////////////////////////////////////////////////////////////////

// The ArrayTable_Base<N> class template offers basic functionality for the ArrayTable<N> class template.
// This functionality is split out in a base class to avoid re-implementing it for each value of N.
template<size_t NDIM> class ArrayTable_Base
{
protected:
    std::vector<Array> _vv;
    size_t _n[NDIM];

    // Constructs an empty table.
    ArrayTable_Base() { std::fill(_n, _n+NDIM, 0); }

    // Resizes the internal data representation so that it can hold the currently
    // specified number of items in each dimension. All values are set to zero.
    void resize_base()
    {
        size_t n = std::accumulate(_n, _n+NDIM-1, 1, std::multiplies<size_t>());
        _vv.resize(n);
        for (size_t i=0; i!=n; ++i) _vv[i].resize(_n[NDIM-1]);
    }

public:
    size_t size(size_t dim) const { return _n[dim]; }
    size_t rowsize() const { return _vv.size() ? _vv[0].size() : 0; }
};

////////////////////////////////////////////////////////////////////

// The class template for general NDIM
template<size_t NDIM> class ArrayTable : ArrayTable_Base<NDIM>
{
    // This disables the default constructor for unsupported dimensions.
    ArrayTable() {}
};

// The template specialization for 2 dimensions
template<> class ArrayTable<2> : public ArrayTable_Base<2>
{
public:
    ArrayTable() {}
    ArrayTable(size_t n0, size_t n1) { _n[0] = n0; _n[1] = n1; resize_base(); }
    void resize(size_t n0, size_t n1) { _n[0] = n0; _n[1] = n1; resize_base(); }
    const double& operator()(size_t i, size_t j) const { return _vv[i][j]; }
    double& operator()(size_t i, size_t j) { return _vv[i][j]; }
    const Array& operator()(size_t i) const { return _vv[i]; }
    Array& operator()(size_t i) { return _vv[i]; }
    const Array& operator[](size_t i) const { return _vv[i]; }
    Array& operator[](size_t i) { return _vv[i]; }
};

// The template specialization for 3 dimensions
template<> class ArrayTable<3> : public ArrayTable_Base<3>
{
public:
    ArrayTable() {}
    ArrayTable(size_t n0, size_t n1, size_t n2) { _n[0] = n0; _n[1] = n1; _n[2] = n2; resize_base(); }
    void resize(size_t n0, size_t n1, size_t n2) { _n[0] = n0; _n[1] = n1; _n[2] = n2; resize_base(); }
    const double& operator()(size_t i, size_t j, size_t k) const { return _vv[i*_n[1]+j][k]; }
    double& operator()(size_t i, size_t j, size_t k) { return _vv[i*_n[1]+j][k]; }
    const Array& operator()(size_t i, size_t j) const { return _vv[i*_n[1]+j]; }
    Array& operator()(size_t i, size_t j) { return _vv[i*_n[1]+j]; }
};

// The template specialization for 4 dimensions
template<> class ArrayTable<4> : public ArrayTable_Base<4>
{
public:
    ArrayTable() {}
    ArrayTable(size_t n0, size_t n1, size_t n2, size_t n3)
        { _n[0] = n0; _n[1] = n1; _n[2] = n2; _n[3] = n3; resize_base(); }
    void resize(size_t n0, size_t n1, size_t n2, size_t n3)
        { _n[0] = n0; _n[1] = n1; _n[2] = n2; _n[3] = n3; resize_base(); }
    const double& operator()(size_t i, size_t j, size_t k, size_t l) const
        { return _vv[(i*_n[1]+j)*_n[2]+k][l]; }
    double& operator()(size_t i, size_t j, size_t k, size_t l)
        { return _vv[(i*_n[1]+j)*_n[2]+k][l]; }
    const Array& operator()(size_t i, size_t j, size_t k) const
        { return _vv[(i*_n[1]+j)*_n[2]+k]; }
    Array& operator()(size_t i, size_t j, size_t k)
        { return _vv[(i*_n[1]+j)*_n[2]+k]; }
};

/// \endcond
// end of hiding code from Doxygen

////////////////////////////////////////////////////////////////////

#endif // ARRAYTABLE_HPP
