/*
   Copyright (c) 2009-2015, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_ELEMENT_DECL_HPP
#define EL_ELEMENT_DECL_HPP

namespace El {

template<typename Real>
using Complex = std::complex<Real>;
// NOTE: It appears that instantiating std::complex for Real=__float128
//       is undefined. This is disappointing; and instantiating std::complex
//       for Real=BigFloat is likely to be even more problematic. There may
//       be a need for using a loose wrapper around std::complex when 
//       Real is not in the approved list of datatypes,
//       {float,double,long double}.

typedef Complex<float>  scomplex;
typedef Complex<double> dcomplex;
#ifdef EL_HAVE_QUAD
typedef Complex<Quad> qcomplex;
#endif
#ifdef EL_HAVE_MPC
// TODO: Decide on how to handle complex BigFloat
#endif

// While Elemental used to make use of typeid(T).name() for analogues of the
// below strings, the 'name' property is not guaranteed to exist for all types,
// such as __float128

template<typename T>
std::string TypeName()
{ return typeid(T).name(); }

template<> std::string TypeName<bool>();
template<> std::string TypeName<char>();
template<> std::string TypeName<char*>();
template<> std::string TypeName<const char*>();
template<> std::string TypeName<std::string>();
template<> std::string TypeName<unsigned>();
template<> std::string TypeName<unsigned long>();
template<> std::string TypeName<unsigned long long>();
template<> std::string TypeName<int>();
template<> std::string TypeName<long int>();
template<> std::string TypeName<long long int>();
template<> std::string TypeName<float>();
template<> std::string TypeName<double>();
template<> std::string TypeName<Complex<float>>();
template<> std::string TypeName<Complex<double>>();
#ifdef EL_HAVE_QUAD
template<> std::string TypeName<Quad>();
template<> std::string TypeName<Complex<Quad>>();
#endif
#ifdef EL_HAVE_MPC
template<> std::string TypeName<BigFloat>();
#endif

// For usage in EnableIf
// =====================

template<typename S,typename T>
using IsSame = std::is_same<S,T>;

template<typename Condition,class T=void>
using EnableIf = typename std::enable_if<Condition::value,T>::type;
template<typename Condition,class T=void>
using DisableIf = typename std::enable_if<!Condition::value,T>::type;

// Types that Matrix, DistMatrix, etc. are instantiatable with
// -----------------------------------------------------------
template<typename T>
using IsIntegral = std::is_integral<T>;

template<typename T> struct IsScalar { static const bool value=false; };
template<> struct IsScalar<Int> { static const bool value=true; };
template<> struct IsScalar<float> { static const bool value=true; };
template<> struct IsScalar<double> { static const bool value=true; };
template<> struct IsScalar<Complex<float>> { static const bool value=true; };
template<> struct IsScalar<Complex<double>> { static const bool value=true; };
#ifdef EL_HAVE_QUAD
template<> struct IsScalar<Quad> { static const bool value=true; };
template<> struct IsScalar<Complex<Quad>> { static const bool value=true; };
#endif
#ifdef EL_HAVE_MPC
template<> struct IsScalar<BigFloat> { static const bool value=true; };
#endif

template<typename T> struct IsBlasScalar
{ static const bool value=false; };
template<> struct IsBlasScalar<float>
{ static const bool value=true; };
template<> struct IsBlasScalar<double>
{ static const bool value=true; };
template<> struct IsBlasScalar<Complex<float>>
{ static const bool value=true; };
template<> struct IsBlasScalar<Complex<double>>
{ static const bool value=true; };

template<typename Real1,typename Real2>
struct PrecisionIsGreater
{ static const bool value=false; };
template<> struct PrecisionIsGreater<double,float>
{ static const bool value=true; };
#ifdef EL_HAVE_QUAD
template<> struct PrecisionIsGreater<Quad,double>
{ static const bool value=true; };
template<> struct PrecisionIsGreater<Quad,float>
{ static const bool value=true; };
#endif
#ifdef EL_HAVE_MPC
// While these aren't necessarily always true, it would be a capitally bad
// idea to use MPFR without using higher than the available fixed precision
#ifdef EL_HAVE_QUAD
template<> struct PrecisionIsGreater<BigFloat,Quad>
{ static const bool value=true; };
#endif
template<> struct PrecisionIsGreater<BigFloat,double>
{ static const bool value=true; };
template<> struct PrecisionIsGreater<BigFloat,float>
{ static const bool value=true; };
#endif

template<typename Real,typename RealNew>
struct ConvertBaseHelper
{ typedef RealNew type; };
template<typename Real,typename RealNew>
struct ConvertBaseHelper<Complex<Real>,RealNew>
{ typedef Complex<RealNew> type; };

template<typename F,typename RealNew>
using ConvertBase = typename ConvertBaseHelper<F,RealNew>::type;

// Increase the precision (if possible)
// ------------------------------------
template<typename F> struct PromoteHelper { typedef F type; };
template<> struct PromoteHelper<float> { typedef double type; };
#ifdef EL_HAVE_QUAD
template<> struct PromoteHelper<double> { typedef Quad type; };
#endif

template<typename Real> struct PromoteHelper<Complex<Real>>
{ typedef Complex<typename PromoteHelper<Real>::type> type; };

template<typename F> using Promote = typename PromoteHelper<F>::type;

// Returning the underlying, or "base", real field
// -----------------------------------------------
// Note: The following is for internal usage only; please use Base
template<typename Real> struct BaseHelper                { typedef Real type; };
template<typename Real> struct BaseHelper<Complex<Real>> { typedef Real type; };

template<typename F> using Base = typename BaseHelper<F>::type;

// For querying whether or not an element's type is complex
// --------------------------------------------------------
// NOTE: This does not guarantee that the type is a field
// NOTE: IsReal is not the negation of IsComplex
template<typename Real> struct IsReal
{ static const bool value=IsScalar<Real>::value; };
template<typename Real> struct IsReal<Complex<Real>>
{ static const bool value=false; };

template<typename Real> struct IsComplex
{ static const bool value=false; };
template<typename Real> struct IsComplex<Complex<Real>>
{ static const bool value=true; };

template<typename S,typename T>
struct CanCast
{   
    static const bool value =
      IsScalar<S>::value &&
      IsScalar<T>::value && 
      (!IsComplex<S>::value || IsComplex<T>::value) &&
      (IsSame<S,Int>::value || !IsSame<T,Int>::value);
};

template<typename S,typename T>
struct CanBidirectionalCast
{   
    static const bool value = CanCast<S,T>::value && CanCast<T,S>::value;
};

// A superset of the above that includes pointers to the above, as well
// as 'int' (which is different than Int if 64-bit integers are enabled)
// ---------------------------------------------------------------------
template<typename T> struct IsData { static const bool value=false; };
template<typename T> struct IsData<T*> { static const bool value=true; };
template<typename T> struct IsData<const T*> { static const bool value=true; };
#ifdef EL_USE_64BIT_INTS
template<> struct IsData<int> { static const bool value=true; };
#endif
template<> struct IsData<Int> { static const bool value=true; };
template<> struct IsData<float> { static const bool value=true; };
template<> struct IsData<double> { static const bool value=true; };
template<> struct IsData<Complex<float>> { static const bool value=true; };
template<> struct IsData<Complex<double>> { static const bool value=true; };
#ifdef EL_HAVE_QUAD
template<> struct IsData<Quad> { static const bool value=true; };
template<> struct IsData<Complex<Quad>> { static const bool value=true; };
#endif
#ifdef EL_HAVE_MPC
template<> struct IsData<BigFloat> { static const bool value=true; };
#endif

// Basic element manipulation and I/O
// ==================================

// Pretty-printing
// ---------------
// TODO: Move into core/imports/quadmath.hpp?
#ifdef EL_HAVE_QUAD
std::ostream& operator<<( std::ostream& os, const Quad& alpha );
std::istream& operator>>( std::istream& is,       Quad& alpha );
#endif

template<typename Real>
std::ostream& operator<<( std::ostream& os, const Complex<Real>& alpha );
template<typename Real>
std::istream& operator>>( std::istream& os,       Complex<Real>& alpha );

// Return the real/imaginary part of an element
// --------------------------------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real RealPart( const Real& alpha ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real RealPart( const Complex<Real>& alpha ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real ImagPart( const Real& alpha ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real ImagPart( const Complex<Real>& alpha ) EL_NO_EXCEPT;

template<typename S,typename T,typename=EnableIf<CanCast<S,T>>>
struct Caster {
    static T Cast( S alpha )
    { return T(alpha); }
};

template<typename S,typename T>
struct Caster<S,Complex<T>,void> {
    static Complex<T> Cast( S alpha )
    { return Complex<T>( T(RealPart(alpha)), T(ImagPart(alpha)) ); }
};

// Set the real/imaginary part of an element
// -----------------------------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
void SetRealPart( Real& alpha, const Real& beta ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
void SetRealPart( Complex<Real>& alpha, const Real& beta ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
void SetImagPart( Real& alpha, const Real& beta );
template<typename Real,typename=EnableIf<IsReal<Real>>>
void SetImagPart( Complex<Real>& alpha, const Real& beta ) EL_NO_EXCEPT;

// Update the real/imaginary part of an element
// --------------------------------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
void UpdateRealPart( Real& alpha, const Real& beta ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
void UpdateRealPart( Complex<Real>& alpha, const Real& beta ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
void UpdateImagPart( Real& alpha, const Real& beta );
template<typename Real,typename=EnableIf<IsReal<Real>>>
void UpdateImagPart( Complex<Real>& alpha, const Real& beta ) EL_NO_EXCEPT;

// Conjugate an element
// --------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Conj( const Real& alpha ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
Complex<Real> Conj( const Complex<Real>& alpha ) EL_NO_EXCEPT;

// Complex argument
// ----------------
template<typename F,typename=EnableIf<IsScalar<F>>>
Base<F> Arg( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Arg( const Complex<Quad>& alpha );
#endif

// Construct a complex number from its polar coordinates
// -----------------------------------------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
Complex<Real> ComplexFromPolar( const Real& r, const Real& theta=0 );
#ifdef EL_HAVE_QUAD
template<> Complex<Quad> ComplexFromPolar( const Quad& r, const Quad& theta );
#endif

// Magnitude and sign
// ==================

// Use the naive algorithm for computing the absolute value
// --------------------------------------------------------
// Note: Unnecessary overflow may occur for complex values, please see SafeAbs
template<typename T,typename=EnableIf<IsScalar<T>>>
Base<T> Abs( const T& alpha ) EL_NO_EXCEPT;
#ifdef EL_HAVE_QUAD
template<> Quad Abs( const Quad& alpha ) EL_NO_EXCEPT;
template<> Quad Abs( const Complex<Quad>& alpha ) EL_NO_EXCEPT;
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Abs( const BigFloat& alpha ) EL_NO_EXCEPT;
#endif

// Carefully avoid unnecessary overflow in an absolute value computation
// ---------------------------------------------------------------------
// Note: The real implementation is equivalent to Abs
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real SafeAbs( const Real& alpha ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real SafeAbs( const Complex<Real>& alpha ) EL_NO_EXCEPT;
#ifdef EL_HAVE_QUAD
template<> Quad SafeAbs( const Complex<Quad>& alpha ) EL_NO_EXCEPT;
#endif

// Return the sum of the absolute values of the real and imaginary components
// --------------------------------------------------------------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real FastAbs( const Real& alpha ) EL_NO_EXCEPT;
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real FastAbs( const Complex<Real>& alpha ) EL_NO_EXCEPT;

// Return the sign of a real element
// ---------------------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Sgn( const Real& alpha, bool symmetric=true ) EL_NO_EXCEPT;
#ifdef EL_HAVE_MPC
BigFloat Sgn( const BigFloat& alpha, bool symmetric=true ) EL_NO_EXCEPT;
#endif

// Exponentiation
// ==============
template<typename F,typename=EnableIf<IsScalar<F>>>
F Exp( const F& alpha ) EL_NO_EXCEPT;
#ifdef EL_HAVE_QUAD
template<> Quad Exp( const Quad& alpha ) EL_NO_EXCEPT;
template<> Complex<Quad> Exp( const Complex<Quad>& alpha ) EL_NO_EXCEPT;
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Exp( const BigFloat& alpha ) EL_NO_EXCEPT;
#endif

template<typename F,typename T,
         typename=EnableIf<IsScalar<F>>,
         typename=EnableIf<IsScalar<T>>>
F Pow( const F& alpha, const T& beta ) EL_NO_EXCEPT;
#ifdef EL_USE_64BIT_INTS
template<typename F,typename=EnableIf<IsScalar<F>>>
F Pow( const F& alpha, const int& beta ) EL_NO_EXCEPT;
#endif
#ifdef EL_HAVE_QUAD
template<> Quad Pow( const Quad& alpha, const Quad& beta ) EL_NO_EXCEPT;
template<> Complex<Quad> Pow
( const Complex<Quad>& alpha, const Complex<Quad>& beta ) EL_NO_EXCEPT;
template<> Complex<Quad> Pow
( const Complex<Quad>& alpha, const Quad& beta ) EL_NO_EXCEPT;
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Pow
( const BigFloat& alpha, const BigFloat& beta ) EL_NO_EXCEPT;
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Log( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Log( const Quad& alpha );
template<> Complex<Quad> Log( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Log( const BigFloat& alpha );
#endif

template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Log2( const Real& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Log2( const Quad& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Log2( const BigFloat& alpha );
#endif
template<typename Integer,typename=EnableIf<IsIntegral<Integer>>,typename=void>
double Log2( const Integer& alpha );

template<typename F,typename=EnableIf<IsScalar<F>>>
F Sqrt( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Sqrt( const Quad& alpha );
template<> Complex<Quad> Sqrt( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Sqrt( const BigFloat& alpha );
#endif

// Trigonometric functions
// =======================
template<typename F,typename=EnableIf<IsScalar<F>>>
F Cos( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Cos( const Quad& alpha );
template<> Complex<Quad> Cos( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Cos( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Sin( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Sin( const Quad& alpha );
template<> Complex<Quad> Sin( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Sin( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Tan( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Tan( const Quad& alpha );
template<> Complex<Quad> Tan( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Tan( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Acos( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Acos( const Quad& alpha );
template<> Complex<Quad> Acos( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Acos( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Asin( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Asin( const Quad& alpha );
template<> Complex<Quad> Asin( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Asin( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Atan( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Atan( const Quad& alpha );
template<> Complex<Quad> Atan( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Atan( const BigFloat& alpha );
#endif

template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Atan2( const Real& y, const Real& x );
#ifdef EL_HAVE_QUAD
template<>
Quad Atan2( const Quad& y, const Quad& x );
#endif
#ifdef EL_HAVE_MPC
template<>
BigFloat Atan2( const BigFloat& y, const BigFloat& x );
#endif

// Hyperbolic functions
// ====================
template<typename F,typename=EnableIf<IsScalar<F>>>
F Cosh( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Cosh( const Quad& alpha );
template<> Complex<Quad> Cosh( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Cosh( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Sinh( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Sinh( const Quad& alpha );
template<> Complex<Quad> Sinh( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Sinh( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Tanh( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Tanh( const Quad& alpha );
template<> Complex<Quad> Tanh( const Complex<Quad>& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Tanh( const BigFloat& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Acosh( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Acosh( const Quad& alpha );
template<> Complex<Quad> Acosh( const Complex<Quad>& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Asinh( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Asinh( const Quad& alpha );
template<> Complex<Quad> Asinh( const Complex<Quad>& alpha );
#endif

template<typename F,typename=EnableIf<IsScalar<F>>>
F Atanh( const F& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Atanh( const Quad& alpha );
template<> Complex<Quad> Atanh( const Complex<Quad>& alpha );
#endif

// Rounding
// ========

// Round to the nearest integer
// ----------------------------
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Round( const Real& alpha );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Complex<Real> Round( const Complex<Real>& alpha );

// Full specializations
// ^^^^^^^^^^^^^^^^^^^^
template<> Int Round( const Int& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Round( const Quad& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Round( const BigFloat& alpha );
#endif

// Ceiling
// -------
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Ceil( const Real& alpha );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Complex<Real> Ceil( const Complex<Real>& alpha );

// Full specializations
// ^^^^^^^^^^^^^^^^^^^^
template<> Int Ceil( const Int& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Ceil( const Quad& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Ceil( const BigFloat& alpha );
#endif

// Floor
// -----
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Floor( const Real& alpha );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Complex<Real> Floor( const Complex<Real>& alpha );

// Full specializations
// ^^^^^^^^^^^^^^^^^^^^
template<> Int Floor( const Int& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Floor( const Quad& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Floor( const BigFloat& alpha );
#endif

// Two-norm formation
// ==================
// TODO: Move this somewhere more fitting; perhaps in blas_like/
template<typename F,typename=EnableIf<IsScalar<F>>>
void UpdateScaledSquare
( const F& alpha, Base<F>& scale, Base<F>& scaledSquare ) EL_NO_EXCEPT;
template<typename F,typename=EnableIf<IsScalar<F>>>
void DowndateScaledSquare
( const F& alpha, Base<F>& scale, Base<F>& scaledSquare ) EL_NO_RELEASE_EXCEPT;

// Pi
// ==
template<typename Real>
Real Pi();
#ifdef EL_HAVE_QUAD
template<> Quad Pi<Quad>();
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Pi<BigFloat>();
BigFloat Pi( mpfr_prec_t prec );
#endif

// Gamma function (and its natural log)
// ====================================
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real Gamma( const Real& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad Gamma( const Quad& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat Gamma( const BigFloat& alpha );
#endif

template<typename Real,typename=EnableIf<IsReal<Real>>>
Real LogGamma( const Real& alpha );
#ifdef EL_HAVE_QUAD
template<> Quad LogGamma( const Quad& alpha );
#endif
#ifdef EL_HAVE_MPC
template<> BigFloat LogGamma( const BigFloat& alpha );
#endif

} // namespace El

#endif // ifndef EL_ELEMENT_DECL_HPP
