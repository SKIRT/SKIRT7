/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent Un0versity         ////
//////////////////////////////////////////////////////////////////*/

#ifndef TABLE_HPP
#define TABLE_HPP

#include "Array.hpp"

////////////////////////////////////////////////////////////////////

/** \class Table
The Table<N> class template implements simple multi-dimensional tables.
Specifically, an instance of Table<N> holds an N-dimensional table of double values, offering
indexed access for reading and writing individual values. All values are stored in a single
block of memory.

Since C++98 has no type-safe support for a variable number of function call arguments, we
provide a separate template specialization for each value of N (currently 2, 3 and 4).

Here's a synopis of Table functionality.

\verbatim
template <N> class Table
{
    // Constructs an empty table.
    Table();

    // Constructs a table with the specified number of items in each of N dimensions.
    // All values are set to zero.
    Table(size_t n_0, ..., size_t n_Nm1);

    // Resizes the table to the specified number of items in each of N dimensions.
    // All values are set to zero.
    void resize(size_t n_0, ..., size_t n_Nm1);

    // Returns the number of items in the indicated dimension.
    size_t size(size_t dim) const;

    // Returns a (readonly or writable) reference to the value at the specified N indices.
    const double& operator()(size_t i_0, ..., size_t i_Nm1) const;
    double& operator()(size_t i_0, ..., size_t i_Nm1);
};
\endverbatim
*/
template<size_t NDIM> class Table;

// hide the rest of the code from Doxygen
/// \cond

////////////////////////////////////////////////////////////////////

// The Table_Base<N> class template offers basic functionality for the Table<N> class template.
// This functionality is split out in a base class to avoid re-implementing it for each value of N.
template<size_t NDIM> class Table_Base
{
protected:
    Array _v;
    size_t _n[NDIM];

    // Constructs an empty table.
    Table_Base() { std::fill(_n, _n+NDIM, 0); }

    // Resizes the internal data representation so that it can hold the currently
    // specified number of items in each dimension. All values are set to zero.
    void resize_base()
    { _v.resize(std::accumulate(_n, _n+NDIM, 1, std::multiplies<size_t>())); }

public:
    // Returns the number of items in the indicated dimension.
    size_t size(size_t dim) const { return _n[dim]; }
};

////////////////////////////////////////////////////////////////////

// The class template for general NDIM
template<size_t NDIM> class Table : Table_Base<NDIM>
{
    // This disables the default constructor for unsupported dimensions.
    Table() {}
};

// The template specialization for 2 dimensions
template<> class Table<2> : public Table_Base<2>
{
public:
    Table() {}
    Table(size_t n0, size_t n1) { _n[0] = n0; _n[1] = n1; resize_base(); }
    void resize(size_t n0, size_t n1) { _n[0] = n0; _n[1] = n1; resize_base(); }
    const double& operator()(size_t i, size_t j) const { return _v[i*_n[1]+j]; }
    double& operator()(size_t i, size_t j) { return _v[i*_n[1]+j]; }
};

// The template specialization for 3 dimensions
template<> class Table<3> : public Table_Base<3>
{
public:
    Table() {}
    Table(size_t n0, size_t n1, size_t n2) { _n[0] = n0; _n[1] = n1; _n[2] = n2; resize_base(); }
    void resize(size_t n0, size_t n1, size_t n2) { _n[0] = n0; _n[1] = n1; _n[2] = n2; resize_base(); }
    const double& operator()(size_t i, size_t j, size_t k) const { return _v[(i*_n[1]+j)*_n[2]+k]; }
    double& operator()(size_t i, size_t j, size_t k) { return _v[(i*_n[1]+j)*_n[2]+k]; }
};

 // The template specialization for 4 dimensions
template<> class Table<4> : public Table_Base<4>
{
public:
    Table() {}
    Table(size_t n0, size_t n1, size_t n2, size_t n3)
        { _n[0] = n0; _n[1] = n1; _n[2] = n2; _n[3] = n3; resize_base(); }
    void resize(size_t n0, size_t n1, size_t n2, size_t n3)
        { _n[0] = n0; _n[1] = n1; _n[2] = n2; _n[3] = n3; resize_base(); }
    const double& operator()(size_t i, size_t j, size_t k, size_t l) const
        { return _v[((i*_n[1]+j)*_n[2]+k)*_n[3]+l]; }
    double& operator()(size_t i, size_t j, size_t k, size_t l)
        { return _v[((i*_n[1]+j)*_n[2]+k)*_n[3]+l]; }
};

/// \endcond
// end of hiding code from Doxygen

////////////////////////////////////////////////////////////////////

#endif // TABLE_HPP
