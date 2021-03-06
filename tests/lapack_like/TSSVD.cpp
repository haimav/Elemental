/*
   Copyright (c) 2009-2015, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#include "El.hpp"
using namespace El;

template<typename F> 
void TestCorrectness
( const DistMatrix<F,VC,  STAR>& U,
  const DistMatrix<Base<F>,STAR,STAR>& s,
  const DistMatrix<F,STAR,STAR>& V,
        DistMatrix<F,VC,  STAR>& A,
  bool print )
{
    typedef Base<F> Real;
    const Grid& g = A.Grid();
    const Int m = A.Height();
    const Int n = A.Width();
    const Int minDim = Min(m,n);

    // Form I - U^H U
    if( g.Rank() == 0 )
        Output("  Testing orthogonality of U...");
    DistMatrix<F> Z(g);
    Identity( Z, minDim, minDim );
    Herk( UPPER, ADJOINT, Real(-1), U, Real(1), Z );
    Real oneNormError = HermitianOneNorm( UPPER, Z );
    Real infNormError = HermitianInfinityNorm( UPPER, Z );
    Real frobNormError = HermitianFrobeniusNorm( UPPER, Z );
    if( g.Rank() == 0 )
        Output
        ("    ||U^H U - I||_1  = ",oneNormError,"\n",
         "    ||U^H U - I||_oo = ",infNormError,"\n",
         "    ||U^H U - I||_F  = ",frobNormError);

    // Form I - V^H V
    if( g.Rank() == 0 )
        Output("  Testing orthogonality of U...");
    Identity( Z, minDim, minDim );
    Herk( UPPER, ADJOINT, Real(-1), V, Real(1), Z );
    oneNormError = HermitianOneNorm( UPPER, Z );
    infNormError = HermitianInfinityNorm( UPPER, Z );
    frobNormError = HermitianFrobeniusNorm( UPPER, Z );
    if( g.Rank() == 0 )
        Output
        ("    ||V^H V - I||_1  = ",oneNormError,"\n",
         "    ||V^H V - I||_oo = ",infNormError,"\n",
         "    ||V^H V - I||_F  = ",frobNormError);

    // Form A - U S V^H
    if( g.Rank() == 0 )
        Output("  Testing if A = U S V^H...");
    const Real oneNormA = OneNorm( A );
    const Real infNormA = InfinityNorm( A );
    const Real frobNormA = FrobeniusNorm( A );
    auto VCopy( V );
    DiagonalScale( RIGHT, NORMAL, s, VCopy );
    LocalGemm( NORMAL, ADJOINT, F(-1), U, VCopy, F(1), A );
    if( print )
        Print( A, "A - U S V^H" );
    oneNormError = OneNorm( A );
    infNormError = InfinityNorm( A );
    frobNormError = FrobeniusNorm( A );
    if( g.Rank() == 0 )
        Output
        ("    ||A||_1            = ",oneNormA,"\n",
         "    ||A||_oo           = ",infNormA,"\n",
         "    ||A||_F            = ",frobNormA,"\n",
         "    ||A - U S V^H||_1  = ",oneNormError,"\n",
         "    ||A - U S V^H||_oo = ",infNormError,"\n",
         "    ||A - U S V^H||_F  = ",frobNormError);
}

template<typename F>
void TestSVD
( bool testCorrectness, bool print,
  Int m, Int n, const Grid& g )
{
    DistMatrix<F,VC,STAR> A(g), U(g);
    DistMatrix<Base<F>,STAR,STAR> s(g);
    DistMatrix<F,STAR,STAR> V(g); 

    Uniform( A, m, n );
    if( print )
        Print( A, "A" );
    U = A;

    if( g.Rank() == 0 )
        Output("  Starting TSQR factorization...");
    mpi::Barrier( g.Comm() );
    const double startTime = mpi::Time();
    svd::TSQR( U, s, V );
    mpi::Barrier( g.Comm() );
    const double runTime = mpi::Time() - startTime;
    if( g.Rank() == 0 )
        Output("  Time = ",runTime," seconds");
    if( print )
    {
        Print( U, "U" );
        Print( s, "s" );
        Print( V, "V" );
    }
    if( testCorrectness )
        TestCorrectness( U, s, V, A, print );
}

int 
main( int argc, char* argv[] )
{
    Environment env( argc, argv );
    mpi::Comm comm = mpi::COMM_WORLD;
    const Int commRank = mpi::Rank( comm );

    try
    {
        const bool colMajor = Input("--colMajor","column-major ordering?",true);
        const Int m = Input("--height","height of matrix",100);
        const Int n = Input("--width","width of matrix",100);
        const Int nb = Input("--nb","algorithmic blocksize",96);
        const bool testCorrectness = Input
            ("--correctness","test correctness?",true);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        const GridOrder order = ( colMajor ? COLUMN_MAJOR : ROW_MAJOR );
        const Grid g( comm, order );
        SetBlocksize( nb );
        ComplainIfDebug();
        if( commRank == 0 )
            Output("Will test TSSVD");

        if( commRank == 0 )
            Output("Testing with doubles:");
        TestSVD<double>( testCorrectness, print, m, n, g );

        if( commRank == 0 )
            Output("Testing with double-precision complex:");
        TestSVD<Complex<double>>( testCorrectness, print, m, n, g );
    }
    catch( exception& e ) { ReportException(e); }

    return 0;
}
