/*
   Copyright (c) 2009-2015, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_RANDOM_IMPL_HPP
#define EL_RANDOM_IMPL_HPP

namespace El {

template<typename Real>
Real Choose( Int n, Int k )
{
    DEBUG_ONLY(CSE cse("Choose"))
    if( k < 0 || k > n )
        LogicError("Choose(",n,",",k,") is not defined");

    // choose(n,k) = (n*(n-1)*...*(n-(k-1)))/(k*(k-1)*...*1)
    //             = (n/k)*(((n-1)/(k-1))*...*((n-(k-1))/1)

    // choose(n,k) = choose(n,n-k), so pick the simpler explicit formula
    if( n-k < k )
        k = n-k;

    // Accumulate the product (TODO: Use higher precision?)
    Real product = 1;
    for( Int j=0; j<k; ++j )
        product *= Real(n-j)/Real(k-j);

    return product;
}

template<typename Real>
Real LogChoose( Int n, Int k )
{
    DEBUG_ONLY(CSE cse("LogChoose"))
    if( k < 0 || k > n )
        LogicError("Choose(",n,",",k,") is not defined");

    // choose(n,k) = (n*(n-1)*...*(n-(k-1)))/(k*(k-1)*...*1)
    //             = (n/k)*(((n-1)/(k-1))*...*((n-(k-1))/1)
    // Thus, 
    //  log(choose(n,k)) = log(n/k) + log((n-1)/(k-1)) + ... + log((n-(k-1))/1).
    //                   = log(n) + log(n-1) + ... log(n-(k-1)) -
    //                     log(k) - log(k-1) - ... log(1)

    // choose(n,k) = choose(n,n-k), so pick the simpler explicit formula
    if( n-k < k )
        k = n-k;

    // Accumulate the log of the product (TODO: Use higher precision?)
    Real logProd = 0;
    for( Int j=0; j<k; ++j )
        logProd += Log(Real(n-j)/Real(k-j));
    // logProd += Log(Real(n-j)) - Log(Real(k-j)).

    return logProd;
}

// Compute log( choose(n,k) ) for k=0,...,n in quadratic time
// TODO: Use the formula from LogChoose to compute the relevant partial 
//       summations in linear time (which should allow for the final solution 
//       to be evaluated in linear time).
// TODO: A parallel prefix version of this algorithm.
template<typename Real>
vector<Real> LogBinomial( Int n )
{
    DEBUG_ONLY(CSE cse("LogBinomial"))
    vector<Real> binom(n+1,0), binomTmp(n+1,0);
    for( Int j=1; j<=n; ++j )
    {
        for( Int k=1; k<j; ++k )
            binomTmp[k] = Log(Exp(binom[k]-binom[k-1])+1) + binom[k-1];
        binom = binomTmp;
    }
    return binom;
}

// This is unfortunately quadratic time
// Compute log( alpha_j ) for j=1,...,n
//
// TODO: Attempt to reduce this to linear time.
template<typename Real>
vector<Real> LogEulerian( Int n )
{
    DEBUG_ONLY(CSE cse("LogEulerian"))
    vector<Real> euler(n,0), eulerTmp(n,0);
    for( Int j=1; j<n; ++j )
    {
        for( Int k=1; k<j; ++k )
            eulerTmp[k] = Log((k+1)*Exp(euler[k]-euler[k-1])+j-k+1) +
                          euler[k-1];
        euler = eulerTmp;
    }
    return euler;
}

template<typename T>
T UnitCell()
{
    typedef Base<T> Real;
    T cell;
    SetRealPart( cell, Real(1) );
    if( IsComplex<T>::value )
        SetImagPart( cell, Real(1) );
    return cell;
}

template<typename T>
T SampleUniform( T a, T b )
{
    typedef Base<T> Real;
    T sample;

#ifdef EL_HAVE_CXX11RANDOM
    std::mt19937& gen = Generator();
    std::uniform_real_distribution<Real> realUni(RealPart(a),RealPart(b));
    SetRealPart( sample, realUni(gen) ); 
    if( IsComplex<T>::value )
    {
        std::uniform_real_distribution<Real> imagUni(ImagPart(a),ImagPart(b));
        SetImagPart( sample, imagUni(gen) );
    }
#else
    Real aReal = RealPart(a);
    Real aImag = ImagPart(a);
    Real bReal = RealPart(b);
    Real bImag = ImagPart(b);
    Real realPart = (Real(rand())/(Real(RAND_MAX)+1))*(bReal-aReal) + aReal;
    SetRealPart( sample, realPart );
    if( IsComplex<T>::value )
    {
        Real imagPart = (Real(rand())/(Real(RAND_MAX)+1))*(bImag-aImag) + aImag;
        SetImagPart( sample, imagPart );
    }
#endif

    return sample;
}

template<typename F>
F SampleNormal( F mean, Base<F> stddev )
{
    typedef Base<F> Real;
    F sample;
    if( IsComplex<F>::value )
        stddev = stddev / Sqrt(Real(2));

#ifdef EL_HAVE_CXX11RANDOM
    std::mt19937& gen = Generator();
    std::normal_distribution<Real> realNormal( RealPart(mean), stddev );
    SetRealPart( sample, realNormal(gen) );
    if( IsComplex<F>::value )
    {
        std::normal_distribution<Real> imagNormal( ImagPart(mean), stddev );
        SetImagPart( sample, imagNormal(gen) );
    }
#else
    // Run Marsiglia's polar method
    // ============================
    // NOTE: Half of the generated samples are thrown away in the case that
    //       F is real.
    while( true )
    {
        const Real U = SampleUniform<Real>(-1,1);
        const Real V = SampleUniform<Real>(-1,1);
        const Real S = Sqrt(U*U+V*V);
        if( S > Real(0) && S < Real(1) )
        {
            const Real W = Sqrt(-2*Log(S)/S);
            SetRealPart( sample, RealPart(mean) + stddev*U*W );
            if( IsComplex<F>::value )
                SetImagPart( sample, ImagPart(mean) + stddev*V*W );
            break;
        }
    }
#endif

    return sample;
}

} // namespace El

#endif // ifndef EL_RANDOM_IMPL_HPP
