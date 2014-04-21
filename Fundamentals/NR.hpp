/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
//////////////////////////////////////////////////////////////////*/

#ifndef NR_HPP
#define NR_HPP

#include <algorithm>
#include <cmath>
#include <vector>
#include "Array.hpp"

////////////////////////////////////////////////////////////////////

/** This namespace contains a collection of functions that operate on Array objects and on
    std::vector<T> objects where T is some numeric type. All implementations are provided inline in
    the header. */
namespace NR
{
    //======================== Conversion and Assignment =======================

    /** This template function converts the source sequence to an Array object and returns the
        result. This template function works for any source container type with indexed random
        access (including std::vector and QList), and for any item types, as long as the source
        items can be assigned to or converted to double. */
    template<class V> inline Array array(const V& sourcev)
    {
        size_t n = sourcev.size();
        Array resultv(n);
        for (size_t i=0; i<n; i++) resultv[i] = sourcev[i];
        return resultv;
    }

    /** This template function assigns the source sequence to the destination array, resizing the
        destination array if necessary. This template function works for any source container
        type with indexed random access (including std::vector and QList), and for any item
        types, as long as the source items can be assigned to or converted to double. This
        template function serves mostly to convert other container types to Array. */
    template<class V> inline void assign(Array& targetv, const V& sourcev)
    {
        size_t n = sourcev.size();
        if (targetv.size()!=n) targetv.resize(n);
        for (size_t i=0; i<n; i++) targetv[i] = sourcev[i];
    }

    //======================== Sorting =======================

    /** This template function sorts a sequence of items given as a std::vector<T> where T is any
        built-in or user-defined type that implements the less-than operator (including the
        standard numeric types). */
    template<typename T> inline void sort(std::vector<T>& xv)
    {
        std::sort(xv.begin(),xv.end());
    }

    /** This function sorts the values in the specified array. */
    inline void sort(Array& xv)
    {
        std::sort(begin(xv), end(xv));
    }

    //======================== Searching =======================

    /** This template function quickly performs a binary search on an ordered sequence of items
        provided as a std::vector<T>. It works for any item type T that implements the less-than
        operator (including the built-in numeric types).

        Given a sequence \f$\{x_i,\,i=0...N-1\}\f$ and a query value \f$x\f$, the function returns
        the integer number \f$j\f$ such that \f$x_j \leq x < x_{j+1}\f$, as long as \f$x_0 \leq x <
        x_{N-1}\f$. In addition, if \f$x = x_{N-1}\f$ the function returns \f$N-2\f$; in other
        words the rightmost border is considered to be inside the last bin. If \f$x<x_0\f$ the
        function returns \f$-1\f$; if \f$x>x_{N-1}\f$ the function returns \f$N-1\f$.

        The function assumes that the provided sequence contains at least one element, and that the
        elements are sorted in ascending order. If this is not the case, the result is undefined.

        The algorithm is adapted from the Numerical Recipes in C++ handbook.
    */
    template<typename T> inline int locate(const std::vector<T>& xv, const T& x)
    {
        int n = xv.size();
        if (x < xv[0]) return -1;
        if (xv[n-1] < x) return n-1;

        int jl = -1;
        int ju = n;
        while (ju-jl>1)
        {
            int jm = (ju+jl) >> 1;
            if (x < xv[jm])
                ju = jm;
            else
                jl = jm;
        }
        if (jl<=0) return 0;
        if (jl>=n-2) return n-2;
        return jl;
    }

    /** This function is an implementation detail and not intended for public use. See the
        description of the locate() function for more information. */
    inline int locate_basic_impl(const Array& xv, double x, int n)
    {
        int jl = -1;
        int ju = n;
        while (ju-jl>1)
        {
            int jm = (ju+jl) >> 1;
            if (x < xv[jm])
                ju = jm;
            else
                jl = jm;
        }
        return jl;
    }

    /** The locate(), locate_clip() and locate_fail() functions perform a binary search on the ordered
        sequence of double values in an array. There are subtle differences between the various "locate"
        functions, as decribed below. All functions assume that the specified array contains at least
        two elements, and that all elements are sorted in ascending order. If this is not the case,
        the result is undefined. The algorithm is adapted from the Numerical Recipes in C++
        handbook.

        The array passed as the first function argument specifies an ordered sequence
        \f$\{x_i,\,i=0...N\}\f$ of \f$N+1\f$ values \f$x_i\f$, interpreted as borders defining a
        grid with \f$N\f$ bins. There must be at least two values (defining a single bin). The second
        function argument specifies a query value \f$x\f$. As long as the query is inside the range
        of the sequence, all functions return the zero-based index of the bin in which the query
        falls. Specifically, if \f$x_0 \leq x < x_N\f$ then all functions return the integer number
        \f$j\f$ such that \f$x_j \leq x < x_{j+1}\f$. In addition, if \f$x = x_N\f$ the function
        returns \f$N-1\f$. In other words the rightmost border is always considered to be inside
        the last bin (and of course the leftmost border is inside the first bin).

        The return value of the various "locate" functions differs for query values outside of the
        range of the sequence, as listed in the table below.

        <TABLE>
        <TR><TD><B>Function</B></TD>    <TD><B>\f$x<x_0\f$</B></TD>   <TD><B>\f$x>x_N\f$</B></TD>
                                        <TD><B>Comments</B> (note that there are \f$N+1\f$ values)</TD></TR>
        <TR><TD>locate()</TD>           <TD>\f$-1\f$</TD>             <TD>\f$N\f$</TD>
                <TD>Out-of-range values are indicated with a corresponding out-of-range index</TD></TR>
        <TR><TD>locate_clip()</TD>      <TD>\f$0\f$</TD>              <TD>\f$N-1\f$</TD>
                <TD>Out-of-range values are considered to be inside the corresponding outermost bin</TD></TR>
        <TR><TD>locate_fail()</TD>      <TD>\f$-1\f$</TD>             <TD>\f$-1\f$</TD>
                <TD>Out-of-range values are indicated with a negative index</TD></TR>
        </TABLE>
    */
    inline int locate(const Array& xv, double x)
    {
        int n = xv.size();
        if (x==xv[n-1]) return n-2;
        return locate_basic_impl(xv, x, n);
    }

    /** This function performs a binary search on the ordered sequence of double values in an
        array. See the description of the locate() function for more information. */
    inline int locate_clip(const Array& xv, double x)
    {
        int n = xv.size();
        if (x<xv[0]) return 0;
        return locate_basic_impl(xv, x, n-1);
    }

    /** This function performs a binary search on the ordered sequence of double values in an
        array. See the description of the locate() function for more information. */
    inline int locate_fail(const Array& xv, double x)
    {
        int n = xv.size();
        if (x>xv[n-1]) return -1;
        return locate_basic_impl(xv, x, n-1);
    }

    //======================== Constructing grids =======================

    /** This function builds a linear grid (with equidistant grid points) for the specified range
        and number of grid points, and stores the resulting values in the provided array. The
        array is resized to the specified number of grid points. The function returns the delta
        between two adjacent grid points. */
    inline double lingrid(Array& xv, double xmin, double xmax, int n)
    {
        xv.resize(n);
        double dx = (xmax-xmin)/(n-1);
        for (int i=0; i<n; i++) xv[i] = xmin + i*dx;
        return dx;
    }

    /** This function builds a power-law grid with the specified range, number of bins, and
        ratio of last over first bin widths, and stores the resulting values in the provided array.
        If the specified ratio is very close to one, the function simply builds a linear grid. The
        array is resized to the specified number of bins plus one. */
    inline void powgrid(Array& xv, double xmin, double xmax, int n, double ratio)
    {
        if (fabs(ratio-1.)<1e-3)
        {
            lingrid(xv, xmin, xmax, n+1);
        }
        else
        {
            xv.resize(n+1);
            double range = xmax-xmin;
            double q = pow(ratio,1./(n-1));
            for (int i=0; i<=n; ++i)
                xv[i] = xmin + (1.-pow(q,i))/(1.-pow(q,n)) * range;
        }
    }

    /** This function builds a logarithmic grid with the specified range and number of grid
        points, and stores the resulting values in the provided array. The array is resized
        to the specified number of grid points. */
    inline void loggrid(Array& xv, double xmin, double xmax, int n)
    {
        xv.resize(n);
        double dlogx = log10(xmax/xmin)/(n-1);
        double logxmin = log10(xmin);
        for (int i=0; i<n; i++) xv[i] = pow(10, logxmin + i*dlogx);
    }

    //=================== Interpolating and resampling ===================

    /** This function computes the interpolated value of a one-dimensional function, given its
        values at the edges of an interval. Both the axes coordinate \f$x\f$ and the function value
        \f$f(x)\f$ are interpolated linearly. */
    inline double interpolate_linlin(double x, double x1, double x2, double f1, double f2)
    {
        return f1 + ((x-x1)/(x2-x1))*(f2-f1);
    }

    /** This function computes the interpolated value of a one-dimensional function, given its
        values at the edges of an interval. The axes coordinate \f$x\f$ is always interpolated
        logarithmically, and thus must have positive values. The function value \f$f(x)\f$ is
        interpolated linearly. */
    inline double interpolate_loglin(double x, double x1, double x2, double f1, double f2)
    {
        // compute logarithm of coordinate values
        x  = log10(x);
        x1 = log10(x1);
        x2 = log10(x2);

        // perform the interpolation
        return f1 + ((x-x1)/(x2-x1))*(f2-f1);
    }

    /** This function computes the interpolated value of a one-dimensional function, given its
        values at the edges of an interval. The axes coordinate \f$x\f$ is always interpolated
        logarithmically, and thus must have positive values. The function value \f$f(x)\f$ is
        interpolated logarithmically if the function values are positive; otherwise it is
        interpolated linearly. */
    inline double interpolate_loglog(double x, double x1, double x2, double f1, double f2)
    {
        // compute logarithm of coordinate values
        x  = log10(x);
        x1 = log10(x1);
        x2 = log10(x2);

        // turn off logarithmic interpolation of function value if not all given values are positive
        bool logf = f1>0 && f2>0;

        // compute logarithm of function values if required
        if (logf)
        {
            f1 = log10(f1);
            f2 = log10(f2);
        }

        // perform the interpolation
        double fx = f1 + ((x-x1)/(x2-x1))*(f2-f1);

        // compute the inverse logarithm of the resulting function value if required
        if (logf) fx = pow(10,fx);

        return fx;
    }

    /** This template function resamples the function values \f$y_k\f$ defined on a grid \f$x_k\f$
        (both specified as arrays of the same length) onto an new grid \f$x_l^*\f$. The result is
        returned as an array of function values \f$y_l^*\f$ with the same length as the target
        grid. For new grid points that fall beyond the original grid, the function value is set to
        zero. For new grid points inside the original grid, the function value is interpolated
        using the function specified as template argument. The interpolation functions provided by
        this class can be passed as a template argument for this purpose. */
    template < double interpolate_function(double, double, double, double, double) >
    inline Array resample(const Array& xresv, const Array& xoriv, const Array& yoriv)
    {
        int Nori = xoriv.size();
        double xmin = xoriv[0];
        double xmax = xoriv[Nori-1];
        int Nres = xresv.size();
        Array yresv(Nres);
        for (int l=0; l<Nres; l++)
        {
            double x = xresv[l];
            if (fabs(1.0-x/xmin)<1e-5)
                yresv[l] = yoriv[0];
            else if (fabs(1.0-x/xmax)<1e-5)
                yresv[l] = yoriv[Nori-1];
            else if (x<xmin || x>xmax)
                yresv[l] = 0.0;
            else
            {
                int k = NR::locate(xoriv,x);
                yresv[l] = interpolate_function(x, xoriv[k], xoriv[k+1], yoriv[k], yoriv[k+1]);
            }
        }
        return yresv;
    }

    //=============== Constructing cumulative distribution functions ==================

    /** Given a distribution discretized over \f$N\f$ points \f[p_i \qquad i=0,\dots,N-1\f] this
        function builds the corresponding normalized cumulative distribution
        \f[P_0=0;\quad P_{i+1}=\frac{\sum_{j=0}^i p_j}{\sum_{j=0}^{N-1} p_j} \qquad i=0,\dots,N-1\f]
        with \f$N+1\f$ elements. In this version of the function, the source distribution is
        specified as an array with at least one element; the target array is resized appropriately
        and replaced by the cumulative distribution. */
    inline void cdf(Array& Pv, const Array& pv)
    {
        int n = pv.size();
        Pv.resize(n+1);  // also sets Pv[0] to zero
        for (int i=0; i<n; i++) Pv[i+1] = Pv[i] + pv[i];
        Pv /= Pv[n];
    }

    /** Given a distribution discretized over \f$N\f$ points \f[p_i \qquad i=0,\dots,N-1\f] this
        function builds the corresponding normalized cumulative distribution
        \f[P_0=0;\quad P_{i+1}=\frac{\sum_{j=0}^i p_j}{\sum_{j=0}^{N-1} p_j} \qquad i=0,\dots,N-1\f]
        with \f$N+1\f$ elements. In this version of the function, the source distribution is
        specified by a function object with signature double pv(int i); the number of source points
        \f$N>0\f$ is specified as a separate argument. The source function is called once for each
        index \f$i=0,\dots,N-1\f$. The target array is resized appropriately and replaced by
        the cumulative distribution. */
    template<typename Functor> inline void cdf(Array& Pv, int n, Functor pv)
    {
        Pv.resize(n+1);  // also sets Pv[0] to zero
        for (int i=0; i<n; i++) Pv[i+1] = Pv[i] + pv(i);
        Pv /= Pv[n];
    }
}

////////////////////////////////////////////////////////////////////

#endif // NR_HPP
