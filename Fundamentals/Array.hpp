/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef ARRAY_HPP
#define ARRAY_HPP

#ifdef BUILDING_MEMORY
#include "MemoryLogger.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <numeric>

////////////////////////////////////////////////////////////////////

/** \class Array
An instance of the Array class holds an array of double values, and allows easily
performing mathematical operations on these values. The number of items held by the array can
be adjusted, but only at the cost of losing all previously stored values: the resize operation
sets all values to zero, just as if the array was freshly constructed.

Array offers a subset of Array functionality. However, the assignment operator
adjusts the size of the target to the size of the source if necessary. This small but important
difference requires a private implementation. Once the new C++11 semantics for std::valarray
have been widely adopted, Array could be implemented as a typedef for Array.

The code for this class was copied and adjusted from the standard library implementation
"libc++" which can be found at http://libcxx.llvm.org and is copyrighted by the contributors to
the LLVM project.

Here's a synopis of current Array functionality.

\verbatim
class Array
{
public:
    // construct/destroy:
    Array();
    explicit Array(size_t n);   // sets all values to zero
    Array(const Array& v);
    ~Array();

    // assignment:
    Array& operator=(const Array& v);   // resizes target if needed
    Array& operator=(const double& x);

    // element access:
    const double& operator[](size_t i) const;
    double& operator[](size_t i);

    // unary operators:
    Array operator+() const;
    Array operator-() const;

    // computed assignment:
    Array& operator*= (const double& x);
    Array& operator/= (const double& x);
    Array& operator+= (const double& x);
    Array& operator-= (const double& x);

    Array& operator*= (const Array& v);
    Array& operator/= (const Array& v);
    Array& operator+= (const Array& v);
    Array& operator-= (const Array& v);

    // member functions:
    size_t size() const;
    void resize(size_t n);  // sets all values to zero
    void swap(Array& v);

    double sum() const;     // returns zero for empty array
    double min() const;     // returns zero for empty array
    double max() const;     // returns zero for empty array

    Array apply(double f(double)) const;
    Array apply(double f(const double&)) const;
};

void swap(Array& x, Array& y) noexcept;

Array operator* (const Array& x, const Array& y);
Array operator* (const Array& x, const T& y);
Array operator* (const T& x, const Array& y);

Array operator/ (const Array& x, const Array& y);
Array operator/ (const Array& x, const T& y);
Array operator/ (const T& x, const Array& y);

Array operator+ (const Array& x, const Array& y);
Array operator+ (const Array& x, const T& y);
Array operator+ (const T& x, const Array& y);

Array operator- (const Array& x, const Array& y);
Array operator- (const Array& x, const T& y);
Array operator- (const T& x, const Array& y);

Array abs (const Array& x);
Array acos (const Array& x);
Array asin (const Array& x);
Array atan (const Array& x);

Array atan2(const Array& x, const Array& y);
Array atan2(const Array& x, const T& y);
Array atan2(const T& x, const Array& y);

Array cos (const Array& x);
Array cosh (const Array& x);
Array exp (const Array& x);
Array log (const Array& x);
Array log10(const Array& x);

Array pow(const Array& x, const Array& y);
Array pow(const Array& x, const T& y);
Array pow(const T& x, const Array& y);

Array sin (const Array& x);
Array sinh (const Array& x);
Array sqrt (const Array& x);
Array tan (const Array& x);
Array tanh (const Array& x);

double* begin(Array& v);
const double* begin(const Array& v);
double* end(Array& v);
const double* end(const Array& v);
\endverbatim
*/
class Array;

// hide the rest of the code from Doxygen
/// \cond

// enable_if
template <bool, class _Tp = void> struct _my_enable_if {};
template <class _Tp> struct _my_enable_if<true, _Tp> {typedef _Tp type;};

// remove_reference
template <class _Tp> struct _my_remove_reference        {typedef _Tp type;};
template <class _Tp> struct _my_remove_reference<_Tp&>  {typedef _Tp type;};

// true_type & false_type
template <class _Tp, _Tp _v>
struct _my_integral_constant
{
    static const _Tp value = _v;
    typedef _Tp value_type;
    typedef _my_integral_constant type;
    operator value_type() const {return value;}
};
template <class _Tp, _Tp _v>
const _Tp _my_integral_constant<_Tp, _v>::value;
typedef _my_integral_constant<bool, true>  _my_true_type;
typedef _my_integral_constant<bool, false> _my_false_type;

// expressions
template <class _Op, class _A0>
struct _UnaryOp
{
    typedef typename _Op::result_type result_type;
    typedef typename _A0::value_type value_type;
    _Op _op_;
    _A0 _a0_;
    _UnaryOp(const _Op& _op, const _A0& _a0) : _op_(_op), _a0_(_a0) {}
    result_type operator[](size_t _i) const {return _op_(_a0_[_i]);}
    size_t size() const {return _a0_.size();}
};

template <class _Op, class _A0, class _A1>
struct _BinaryOp
{
    typedef typename _Op::result_type result_type;
    typedef typename _A0::value_type value_type;
    _Op _op_;
    _A0 _a0_;
    _A1 _a1_;
    _BinaryOp(const _Op& _op, const _A0& _a0, const _A1& _a1)
        : _op_(_op), _a0_(_a0), _a1_(_a1) {}
    value_type operator[](size_t _i) const {return _op_(_a0_[_i], _a1_[_i]);}
    size_t size() const {return _a0_.size();}
};

template <class _Tp>
class _my_scalar_expr
{
public:
    typedef _Tp        value_type;
    typedef const _Tp& result_type;
private:
    const value_type& _t_;
    size_t _s_;
public:
    explicit _my_scalar_expr(const value_type& _t, size_t _s) : _t_(_t), _s_(_s) {}
    result_type operator[](size_t) const {return _t_;}
    size_t size() const {return _s_;}
};

template <class _Tp>
struct _my_unary_plus : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return +_x;}
};

template <class _Tp, class _Fp>
struct _my_apply_expr : std::unary_function<_Tp, _Tp>
{
private:
    _Fp _f_;
public:
    explicit _my_apply_expr(_Fp _f) : _f_(_f) {}
    _Tp operator()(const _Tp& _x) const
    {return _f_(_x);}
};

template <class _Tp>
struct _my_abs_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return fabs(_x);}
};

template <class _Tp>
struct _my_acos_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return acos(_x);}
};

template <class _Tp>
struct _my_asin_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return asin(_x);}
};

template <class _Tp>
struct _my_atan_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return atan(_x);}
};

template <class _Tp>
struct _my_atan2_expr : std::binary_function<_Tp, _Tp, _Tp>
{
    _Tp operator()(const _Tp& _x, const _Tp& _y) const
    {return atan2(_x, _y);}
};

template <class _Tp>
struct _my_cos_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return cos(_x);}
};

template <class _Tp>
struct _my_cosh_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return cosh(_x);}
};

template <class _Tp>
struct _my_exp_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return exp(_x);}
};

template <class _Tp>
struct _my_log_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return log(_x);}
};

template <class _Tp>
struct _my_log10_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return log10(_x);}
};

template <class _Tp>
struct _my_pow_expr : std::binary_function<_Tp, _Tp, _Tp>
{
    _Tp operator()(const _Tp& _x, const _Tp& _y) const
    {return pow(_x, _y);}
};

template <class _Tp>
struct _my_sin_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return sin(_x);}
};

template <class _Tp>
struct _my_sinh_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return sinh(_x);}
};

template <class _Tp>
struct _my_sqrt_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return sqrt(_x);}
};

template <class _Tp>
struct _my_tan_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return tan(_x);}
};

template <class _Tp>
struct _my_tanh_expr : std::unary_function<_Tp, _Tp>
{
    _Tp operator()(const _Tp& _x) const
    {return tanh(_x);}
};

template<class _ValExpr>
class _my_val_expr;

template<class _ValExpr>
struct _my_is_val_expr : _my_false_type {};

template<class _ValExpr>
struct _my_is_val_expr<_my_val_expr<_ValExpr> > : _my_true_type {};

template<>
struct _my_is_val_expr<Array> : _my_true_type {};

// array
class Array
{
public:
    typedef double value_type;
    typedef double result_type;

private:
    double* _begin_;
    double* _end_;
    #ifdef BUILDING_MEMORY
    static MemoryLogger* _log;
    #endif

public:
    // construct/destroy:
    Array() : _begin_(0), _end_(0) {}
    explicit Array(size_t _n);
    Array(const Array& _v);
    ~Array();

    #ifdef BUILDING_MEMORY
    // set the memory logger
    static void setLogger(MemoryLogger* logger);
    #endif

    // assignment:
    Array& operator=(const Array& _v);
    Array& operator=(const double& _x);
    template <class _ValExpr>
    Array& operator=(const _my_val_expr<_ValExpr>& _v);

    // element access:
    const double& operator[](size_t _i) const {return _begin_[_i];}
    double&       operator[](size_t _i)       {return _begin_[_i];}

    // unary operators:
    Array operator+() const;
    Array operator-() const;

    // computed assignment:
    Array& operator*= (const double& _x);
    Array& operator/= (const double& _x);
    Array& operator+= (const double& _x);
    Array& operator-= (const double& _x);

    template <class _Expr>
    typename _my_enable_if
    <
    _my_is_val_expr<_Expr>::value,
    Array&
    >::type
    operator*= (const _Expr& _v);

    template <class _Expr>
    typename _my_enable_if
    <
    _my_is_val_expr<_Expr>::value,
    Array&
    >::type
    operator/= (const _Expr& _v);

    template <class _Expr>
    typename _my_enable_if
    <
    _my_is_val_expr<_Expr>::value,
    Array&
    >::type
    operator+= (const _Expr& _v);

    template <class _Expr>
    typename _my_enable_if
    <
    _my_is_val_expr<_Expr>::value,
    Array&
    >::type
    operator-= (const _Expr& _v);

    // member functions:
    void swap(Array& _v);
    size_t size() const {return static_cast<size_t>(_end_ - _begin_);}
    double sum() const;
    double min() const;
    double max() const;
    Array apply(double _f(double)) const;
    Array apply(double _f(const double&)) const;
    void resize(size_t _n);

private:
    void resize_noclear(size_t _n);
    template <class> friend class _my_val_expr;
    friend double* begin(Array& _v);
    friend const double* begin(const Array& _v);
    friend double* end(Array& _v);
    friend const double* end(const Array& _v);
};

// expressions
template <class _Op>
struct _UnaryOp<_Op, Array>
{
    typedef typename _Op::result_type result_type;
    typedef double value_type;
    _Op _op_;
    const Array& _a0_;
    _UnaryOp(const _Op& _op, const Array& _a0) : _op_(_op), _a0_(_a0) {}
    result_type operator[](size_t _i) const {return _op_(_a0_[_i]);}
    size_t size() const {return _a0_.size();}
};

template <class _Op, class _A1>
struct _BinaryOp<_Op, Array, _A1>
{
    typedef typename _Op::result_type result_type;
    typedef double value_type;
    _Op _op_;
    const Array& _a0_;
    _A1 _a1_;
    _BinaryOp(const _Op& _op, const Array& _a0, const _A1& _a1)
        : _op_(_op), _a0_(_a0), _a1_(_a1) {}
    value_type operator[](size_t _i) const {return _op_(_a0_[_i], _a1_[_i]);}
    size_t size() const {return _a0_.size();}
};

template <class _Op, class _A0>
struct _BinaryOp<_Op, _A0, Array>
{
    typedef typename _Op::result_type result_type;
    typedef double value_type;
    _Op _op_;
    _A0 _a0_;
    const Array& _a1_;
    _BinaryOp(const _Op& _op, const _A0& _a0, const Array& _a1)
        : _op_(_op), _a0_(_a0), _a1_(_a1) {}
    value_type operator[](size_t _i) const {return _op_(_a0_[_i], _a1_[_i]);}
    size_t size() const {return _a0_.size();}
};

template <class _Op>
struct _BinaryOp<_Op, Array, Array>
{
    typedef typename _Op::result_type result_type;
    typedef double value_type;
    _Op _op_;
    const Array& _a0_;
    const Array& _a1_;
    _BinaryOp(const _Op& _op, const Array& _a0, const Array& _a1)
        : _op_(_op), _a0_(_a0), _a1_(_a1) {}
    value_type operator[](size_t _i) const {return _op_(_a0_[_i], _a1_[_i]);}
    size_t size() const {return _a0_.size();}
};

template<class _ValExpr>
class _my_val_expr
{
    typedef typename _my_remove_reference<_ValExpr>::type  _RmExpr;
    _ValExpr _expr_;
public:
    typedef typename _RmExpr::value_type value_type;
    typedef typename _RmExpr::result_type result_type;
    explicit _my_val_expr(const _RmExpr& _e) : _expr_(_e) {}

    result_type operator[](size_t _i) const
    {return _expr_[_i];}

    _my_val_expr<_UnaryOp<_my_unary_plus<value_type>, _ValExpr> >
    operator+() const
    {
        typedef _UnaryOp<_my_unary_plus<value_type>, _ValExpr> _NewExpr;
        return _my_val_expr<_NewExpr>(_NewExpr(_my_unary_plus<value_type>(), _expr_));
    }

    _my_val_expr<_UnaryOp<std::negate<value_type>, _ValExpr> >
    operator-() const
    {
        typedef _UnaryOp<std::negate<value_type>, _ValExpr> _NewExpr;
        return _my_val_expr<_NewExpr>(_NewExpr(std::negate<value_type>(), _expr_));
    }

    operator Array() const;

    size_t size() const {return _expr_.size();}

    result_type sum() const
    {
        size_t _n = _expr_.size();
        result_type _r = _n ? _expr_[0] : result_type();
        for (size_t _i = 1; _i < _n; ++_i)
            _r += _expr_[_i];
        return _r;
    }

    result_type min() const
    {
        size_t _n = size();
        result_type _r = _n ? (*this)[0] : result_type();
        for (size_t _i = 1; _i < _n; ++_i)
        {
            result_type _x = _expr_[_i];
            if (_x < _r)
                _r = _x;
        }
        return _r;
    }

    result_type max() const
    {
        size_t _n = size();
        result_type _r = _n ? (*this)[0] : result_type();
        for (size_t _i = 1; _i < _n; ++_i)
        {
            result_type _x = _expr_[_i];
            if (_r < _x)
                _r = _x;
        }
        return _r;
    }

    _my_val_expr<_UnaryOp<_my_apply_expr<value_type, value_type(*)(value_type)>, _ValExpr> >
    apply(value_type _f(value_type)) const
    {
        typedef _my_apply_expr<value_type, value_type(*)(value_type)> _Op;
        typedef _UnaryOp<_Op, _ValExpr> _NewExpr;
        return _my_val_expr<_NewExpr>(_NewExpr(_Op(_f), _expr_));
    }

    _my_val_expr<_UnaryOp<_my_apply_expr<value_type, value_type(*)(const value_type&)>, _ValExpr> >
    apply(value_type _f(const value_type&)) const
    {
        typedef _my_apply_expr<value_type, value_type(*)(const value_type&)> _Op;
        typedef _UnaryOp<_Op, _ValExpr> _NewExpr;
        return _my_val_expr<_NewExpr>(_NewExpr(_Op(_f), _expr_));
    }
};

template<class _ValExpr>
inline
_my_val_expr<_ValExpr>::operator Array() const
{
    Array _r;
    size_t _n = _expr_.size();
    if (_n)
    {
        _r._begin_ =
                _r._end_ =
                static_cast<result_type*>(::operator new(_n * sizeof(result_type)));
        for (size_t _i = 0; _i != _n; ++_r._end_, ++_i)
            ::new (_r._end_) result_type(_expr_[_i]);
    }
    return _r;
}

// array
inline
Array::Array(size_t _n)
    : _begin_(0),
      _end_(0)
{
    resize(_n);
}

inline
Array::Array(const Array& _v)
    : _begin_(0),
      _end_(0)
{
    if (_v.size())
    {
        _begin_ = _end_ = static_cast<double*>(::operator new(_v.size() * sizeof(double)));
        for (double* _p = _v._begin_; _p != _v._end_; ++_end_, ++_p)
            ::new (_end_) double(*_p);
    }
}

inline
Array::~Array()
{
    resize_noclear(0);
}

#ifdef BUILDING_MEMORY
inline
void
Array::setLogger(MemoryLogger *logger)
{
    _log = logger;
}
#endif

inline
Array&
Array::operator=(const Array& _v)
{
    if (this != &_v)
    {
        resize_noclear(_v.size());
        std::copy(_v._begin_, _v._end_, _begin_);
    }
    return *this;
}

inline
Array&
Array::operator=(const double& _x)
{
    std::fill(_begin_, _end_, _x);
    return *this;
}

template <class _ValExpr>
inline
Array&
Array::operator=(const _my_val_expr<_ValExpr>& _v)
{
    size_t _n = _v.size();
    resize_noclear(_n);
    double* _t = _begin_;
    for (size_t _i = 0; _i != _n; ++_t, ++_i)
        *_t = result_type(_v[_i]);
    return *this;
}

inline
Array
Array::operator+() const
{
    Array _r;
    size_t _n = size();
    if (_n)
    {
        _r._begin_ =
                _r._end_ =
                static_cast<double*>(::operator new(_n * sizeof(double)));
        for (const double* _p = _begin_; _n; ++_r._end_, ++_p, --_n)
            ::new (_r._end_) double(+*_p);
    }
    return _r;
}

inline
Array
Array::operator-() const
{
    Array _r;
    size_t _n = size();
    if (_n)
    {
        _r._begin_ =
                _r._end_ =
                static_cast<double*>(::operator new(_n * sizeof(double)));
        for (const double* _p = _begin_; _n; ++_r._end_, ++_p, --_n)
            ::new (_r._end_) double(-*_p);
    }
    return _r;
}

inline
Array&
Array::operator*=(const double& _x)
{
    for (double* _p = _begin_; _p != _end_; ++_p)
        *_p *= _x;
    return *this;
}

inline
Array&
Array::operator/=(const double& _x)
{
    for (double* _p = _begin_; _p != _end_; ++_p)
        *_p /= _x;
    return *this;
}

inline
Array&
Array::operator+=(const double& _x)
{
    for (double* _p = _begin_; _p != _end_; ++_p)
        *_p += _x;
    return *this;
}

inline
Array&
Array::operator-=(const double& _x)
{
    for (double* _p = _begin_; _p != _end_; ++_p)
        *_p -= _x;
    return *this;
}

template <class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
Array&
>::type
Array::operator*=(const _Expr& _v)
{
    size_t _i = 0;
    for (double* _t = _begin_; _t != _end_ ; ++_t, ++_i)
        *_t *= _v[_i];
    return *this;
}

template <class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
Array&
>::type
Array::operator/=(const _Expr& _v)
{
    size_t _i = 0;
    for (double* _t = _begin_; _t != _end_ ; ++_t, ++_i)
        *_t /= _v[_i];
    return *this;
}

template <class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
Array&
>::type
Array::operator+=(const _Expr& _v)
{
    size_t _i = 0;
    for (double* _t = _begin_; _t != _end_ ; ++_t, ++_i)
        *_t += _v[_i];
    return *this;
}

template <class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
Array&
>::type
Array::operator-=(const _Expr& _v)
{
    size_t _i = 0;
    for (double* _t = _begin_; _t != _end_ ; ++_t, ++_i)
        *_t -= _v[_i];
    return *this;
}

inline
void
Array::swap(Array& _v)
{
    std::swap(_begin_, _v._begin_);
    std::swap(_end_, _v._end_);
}

inline
double
Array::sum() const
{
    return std::accumulate(_begin_, _end_, 0.);
}

inline
double
Array::min() const
{
    if (_begin_ == _end_) return 0;
    return *std::min_element(_begin_, _end_);
}

inline
double
Array::max() const
{
    if (_begin_ == _end_) return 0;
    return *std::max_element(_begin_, _end_);
}

inline
Array
Array::apply(double _f(double)) const
{
    Array _r;
    size_t _n = size();
    if (_n)
    {
        _r._begin_ =
                _r._end_ =
                static_cast<double*>(::operator new(_n * sizeof(double)));
        for (const double* _p = _begin_; _n; ++_r._end_, ++_p, --_n)
            ::new (_r._end_) double(_f(*_p));
    }
    return _r;
}

inline
Array
Array::apply(double _f(const double&)) const
{
    Array _r;
    size_t _n = size();
    if (_n)
    {
        _r._begin_ =
                _r._end_ =
                static_cast<double*>(::operator new(_n * sizeof(double)));
        for (const double* _p = _begin_; _n; ++_r._end_, ++_p, --_n)
            ::new (_r._end_) double(_f(*_p));
    }
    return _r;
}

inline
void
Array::resize_noclear(size_t _n)
{
    #ifdef BUILDING_MEMORY
    if (_log != nullptr) _log->memory(size(), _n, this);
    #endif

    if (size() != _n)
    {
        if (_begin_ != 0)
        {
            ::operator delete(_begin_);
            _begin_ = _end_ = 0;
        }
        if (_n)
        {
            _begin_ = static_cast<double*>(::operator new(_n * sizeof(double)));
            _end_ = _begin_ + _n;
        }
    }
}

inline
void
Array::resize(size_t _n)
{
    resize_noclear(_n);
    for (double* _p = _begin_; _p != _end_; ++_p)
        ::new (_p) double();
}

inline
void
swap(Array& _x, Array& _y)
{
    _x.swap(_y);
}

// expressions
template<class _Expr1, class _Expr2>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr1>::value && _my_is_val_expr<_Expr2>::value,
_my_val_expr<_BinaryOp<std::multiplies<typename _Expr1::value_type>, _Expr1, _Expr2> >
>::type
operator*(const _Expr1& _x, const _Expr2& _y)
{
    typedef typename _Expr1::value_type value_type;
    typedef _BinaryOp<std::multiplies<value_type>, _Expr1, _Expr2> _Op;
    return _my_val_expr<_Op>(_Op(std::multiplies<value_type>(), _x, _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::multiplies<typename _Expr::value_type>,
_Expr, _my_scalar_expr<typename _Expr::value_type> > >
>::type
operator*(const _Expr& _x, const typename _Expr::value_type& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::multiplies<value_type>, _Expr, _my_scalar_expr<value_type> > _Op;
    return _my_val_expr<_Op>(_Op(std::multiplies<value_type>(),
                               _x, _my_scalar_expr<value_type>(_y, _x.size())));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::multiplies<typename _Expr::value_type>,
_my_scalar_expr<typename _Expr::value_type>, _Expr> >
>::type
operator*(const typename _Expr::value_type& _x, const _Expr& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::multiplies<value_type>, _my_scalar_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(std::multiplies<value_type>(),
                               _my_scalar_expr<value_type>(_x, _y.size()), _y));
}

template<class _Expr1, class _Expr2>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr1>::value && _my_is_val_expr<_Expr2>::value,
_my_val_expr<_BinaryOp<std::divides<typename _Expr1::value_type>, _Expr1, _Expr2> >
>::type
operator/(const _Expr1& _x, const _Expr2& _y)
{
    typedef typename _Expr1::value_type value_type;
    typedef _BinaryOp<std::divides<value_type>, _Expr1, _Expr2> _Op;
    return _my_val_expr<_Op>(_Op(std::divides<value_type>(), _x, _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::divides<typename _Expr::value_type>,
_Expr, _my_scalar_expr<typename _Expr::value_type> > >
>::type
operator/(const _Expr& _x, const typename _Expr::value_type& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::divides<value_type>, _Expr, _my_scalar_expr<value_type> > _Op;
    return _my_val_expr<_Op>(_Op(std::divides<value_type>(),
                               _x, _my_scalar_expr<value_type>(_y, _x.size())));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::divides<typename _Expr::value_type>,
_my_scalar_expr<typename _Expr::value_type>, _Expr> >
>::type
operator/(const typename _Expr::value_type& _x, const _Expr& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::divides<value_type>, _my_scalar_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(std::divides<value_type>(),
                               _my_scalar_expr<value_type>(_x, _y.size()), _y));
}

template<class _Expr1, class _Expr2>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr1>::value && _my_is_val_expr<_Expr2>::value,
_my_val_expr<_BinaryOp<std::plus<typename _Expr1::value_type>, _Expr1, _Expr2> >
>::type
operator+(const _Expr1& _x, const _Expr2& _y)
{
    typedef typename _Expr1::value_type value_type;
    typedef _BinaryOp<std::plus<value_type>, _Expr1, _Expr2> _Op;
    return _my_val_expr<_Op>(_Op(std::plus<value_type>(), _x, _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::plus<typename _Expr::value_type>,
_Expr, _my_scalar_expr<typename _Expr::value_type> > >
>::type
operator+(const _Expr& _x, const typename _Expr::value_type& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::plus<value_type>, _Expr, _my_scalar_expr<value_type> > _Op;
    return _my_val_expr<_Op>(_Op(std::plus<value_type>(),
                               _x, _my_scalar_expr<value_type>(_y, _x.size())));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::plus<typename _Expr::value_type>,
_my_scalar_expr<typename _Expr::value_type>, _Expr> >
>::type
operator+(const typename _Expr::value_type& _x, const _Expr& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::plus<value_type>, _my_scalar_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(std::plus<value_type>(),
                               _my_scalar_expr<value_type>(_x, _y.size()), _y));
}

template<class _Expr1, class _Expr2>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr1>::value && _my_is_val_expr<_Expr2>::value,
_my_val_expr<_BinaryOp<std::minus<typename _Expr1::value_type>, _Expr1, _Expr2> >
>::type
operator-(const _Expr1& _x, const _Expr2& _y)
{
    typedef typename _Expr1::value_type value_type;
    typedef _BinaryOp<std::minus<value_type>, _Expr1, _Expr2> _Op;
    return _my_val_expr<_Op>(_Op(std::minus<value_type>(), _x, _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::minus<typename _Expr::value_type>,
_Expr, _my_scalar_expr<typename _Expr::value_type> > >
>::type
operator-(const _Expr& _x, const typename _Expr::value_type& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::minus<value_type>, _Expr, _my_scalar_expr<value_type> > _Op;
    return _my_val_expr<_Op>(_Op(std::minus<value_type>(),
                               _x, _my_scalar_expr<value_type>(_y, _x.size())));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<std::minus<typename _Expr::value_type>,
_my_scalar_expr<typename _Expr::value_type>, _Expr> >
>::type
operator-(const typename _Expr::value_type& _x, const _Expr& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<std::minus<value_type>, _my_scalar_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(std::minus<value_type>(),
                               _my_scalar_expr<value_type>(_x, _y.size()), _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_abs_expr<typename _Expr::value_type>, _Expr> >
>::type
abs(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_abs_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_abs_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_acos_expr<typename _Expr::value_type>, _Expr> >
>::type
acos(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_acos_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_acos_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_asin_expr<typename _Expr::value_type>, _Expr> >
>::type
asin(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_asin_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_asin_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_atan_expr<typename _Expr::value_type>, _Expr> >
>::type
atan(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_atan_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_atan_expr<value_type>(), _x));
}

template<class _Expr1, class _Expr2>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr1>::value && _my_is_val_expr<_Expr2>::value,
_my_val_expr<_BinaryOp<_my_atan2_expr<typename _Expr1::value_type>, _Expr1, _Expr2> >
>::type
atan2(const _Expr1& _x, const _Expr2& _y)
{
    typedef typename _Expr1::value_type value_type;
    typedef _BinaryOp<_my_atan2_expr<value_type>, _Expr1, _Expr2> _Op;
    return _my_val_expr<_Op>(_Op(_my_atan2_expr<value_type>(), _x, _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<_my_atan2_expr<typename _Expr::value_type>,
_Expr, _my_scalar_expr<typename _Expr::value_type> > >
>::type
atan2(const _Expr& _x, const typename _Expr::value_type& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<_my_atan2_expr<value_type>, _Expr, _my_scalar_expr<value_type> > _Op;
    return _my_val_expr<_Op>(_Op(_my_atan2_expr<value_type>(),
                               _x, _my_scalar_expr<value_type>(_y, _x.size())));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<_my_atan2_expr<typename _Expr::value_type>,
_my_scalar_expr<typename _Expr::value_type>, _Expr> >
>::type
atan2(const typename _Expr::value_type& _x, const _Expr& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<_my_atan2_expr<value_type>, _my_scalar_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_atan2_expr<value_type>(),
                               _my_scalar_expr<value_type>(_x, _y.size()), _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_cos_expr<typename _Expr::value_type>, _Expr> >
>::type
cos(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_cos_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_cos_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_cosh_expr<typename _Expr::value_type>, _Expr> >
>::type
cosh(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_cosh_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_cosh_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_exp_expr<typename _Expr::value_type>, _Expr> >
>::type
exp(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_exp_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_exp_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_log_expr<typename _Expr::value_type>, _Expr> >
>::type
log(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_log_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_log_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_log10_expr<typename _Expr::value_type>, _Expr> >
>::type
log10(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_log10_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_log10_expr<value_type>(), _x));
}

template<class _Expr1, class _Expr2>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr1>::value && _my_is_val_expr<_Expr2>::value,
_my_val_expr<_BinaryOp<_my_pow_expr<typename _Expr1::value_type>, _Expr1, _Expr2> >
>::type
pow(const _Expr1& _x, const _Expr2& _y)
{
    typedef typename _Expr1::value_type value_type;
    typedef _BinaryOp<_my_pow_expr<value_type>, _Expr1, _Expr2> _Op;
    return _my_val_expr<_Op>(_Op(_my_pow_expr<value_type>(), _x, _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<_my_pow_expr<typename _Expr::value_type>,
_Expr, _my_scalar_expr<typename _Expr::value_type> > >
>::type
pow(const _Expr& _x, const typename _Expr::value_type& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<_my_pow_expr<value_type>, _Expr, _my_scalar_expr<value_type> > _Op;
    return _my_val_expr<_Op>(_Op(_my_pow_expr<value_type>(),
                               _x, _my_scalar_expr<value_type>(_y, _x.size())));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_BinaryOp<_my_pow_expr<typename _Expr::value_type>,
_my_scalar_expr<typename _Expr::value_type>, _Expr> >
>::type
pow(const typename _Expr::value_type& _x, const _Expr& _y)
{
    typedef typename _Expr::value_type value_type;
    typedef _BinaryOp<_my_pow_expr<value_type>, _my_scalar_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_pow_expr<value_type>(),
                               _my_scalar_expr<value_type>(_x, _y.size()), _y));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_sin_expr<typename _Expr::value_type>, _Expr> >
>::type
sin(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_sin_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_sin_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_sinh_expr<typename _Expr::value_type>, _Expr> >
>::type
sinh(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_sinh_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_sinh_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_sqrt_expr<typename _Expr::value_type>, _Expr> >
>::type
sqrt(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_sqrt_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_sqrt_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_tan_expr<typename _Expr::value_type>, _Expr> >
>::type
tan(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_tan_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_tan_expr<value_type>(), _x));
}

template<class _Expr>
inline
typename _my_enable_if
<
_my_is_val_expr<_Expr>::value,
_my_val_expr<_UnaryOp<_my_tanh_expr<typename _Expr::value_type>, _Expr> >
>::type
tanh(const _Expr& _x)
{
    typedef typename _Expr::value_type value_type;
    typedef _UnaryOp<_my_tanh_expr<value_type>, _Expr> _Op;
    return _my_val_expr<_Op>(_Op(_my_tanh_expr<value_type>(), _x));
}

// array
inline
double*
begin(Array& _v)
{
    return _v._begin_;
}

inline
const double*
begin(const Array& _v)
{
    return _v._begin_;
}

inline
double*
end(Array& _v)
{
    return _v._end_;
}

inline
const double*
end(const Array& _v)
{
    return _v._end_;
}

/// \endcond
// end of hiding code from Doxygen

////////////////////////////////////////////////////////////////////

#endif // ARRAY_HPP
