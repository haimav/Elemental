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
( bool print,
  const DistMatrix<F>& A,
  const DistMatrix<F,MD,STAR>& t,
  const DistMatrix<Base<F>,MD,STAR>& d,
        DistMatrix<F>& AOrig )
{
    typedef Base<F> Real;
    const Grid& g = A.Grid();
    const Int m = A.Height();
    const Int n = A.Width();
    const Int minDim = std::min(m,n);

    if( g.Rank() == 0 )
        Output("  Testing orthogonality of Q...");

    // Form Z := Q Q^H as an approximation to identity
    DistMatrix<F> Z(g);
    Identity( Z, m, n );
    lq::ApplyQ( RIGHT, NORMAL, A, t, d, Z );
    lq::ApplyQ( RIGHT, ADJOINT, A, t, d, Z );
    auto ZUpper = View( Z, 0, 0, minDim, minDim );

    // Form X := I - Q Q^H
    DistMatrix<F> X(g);
    Identity( X, minDim, minDim );
    X -= ZUpper;

    Real oneNormError = OneNorm( X );
    Real infNormError = InfinityNorm( X );
    Real frobNormError = FrobeniusNorm( X );
    if( g.Rank() == 0 )
        Output
        ("    ||Q Q^H - I||_1  = ",oneNormError,"\n",
         "    ||Q Q^H - I||_oo = ",infNormError,"\n",
         "    ||Q Q^H - I||_F  = ",frobNormError);

    if( g.Rank() == 0 )
        Output("  Testing if A = LQ...");

    // Form L Q
    auto L( A );
    MakeTrapezoidal( LOWER, L );
    lq::ApplyQ( RIGHT, NORMAL, A, t, d, L );

    // Form L Q - A
    L -= AOrig;
    
    const Real oneNormA = OneNorm( AOrig );
    const Real infNormA = InfinityNorm( AOrig );
    const Real frobNormA = FrobeniusNorm( AOrig );
    oneNormError = OneNorm( L );
    infNormError = InfinityNorm( L );
    frobNormError = FrobeniusNorm( L );
    if( g.Rank() == 0 )
        Output
        ("    ||A||_1       = ",oneNormA,"\n",
         "    ||A||_oo      = ",infNormA,"\n",
         "    ||A||_F       = ",frobNormA,"\n",
         "    ||A - LQ||_1  = ",oneNormError,"\n",
         "    ||A - LQ||_oo = ",infNormError,"\n",
         "    ||A - LQ||_F  = ",frobNormError);
}

template<typename F>
void TestLQ( bool testCorrectness, bool print, Int m, Int n, const Grid& g )
{
    if( g.Rank() == 0 )
        Output("Testing with ",TypeName<F>());
    DistMatrix<F> A(g), AOrig(g);
    Uniform( A, m, n );

    if( testCorrectness )
        AOrig = A;
    if( print )
        Print( A, "A" );
    DistMatrix<F,MD,STAR> t(g);
    DistMatrix<Base<F>,MD,STAR> d(g);

    if( g.Rank() == 0 )
        Output("  Starting LQ factorization...");
    mpi::Barrier( g.Comm() );
    const double startTime = mpi::Time();
    LQ( A, t, d );
    mpi::Barrier( g.Comm() );
    const double runTime = mpi::Time() - startTime;
    const double mD = double(m);
    const double nD = double(n);
    const double realGFlops = (2.*mD*mD*nD - 2./3.*mD*mD*mD)/(1.e9*runTime);
    const double gFlops = ( IsComplex<F>::value ? 4*realGFlops : realGFlops );
    if( g.Rank() == 0 )
        Output("  ",runTime," seconds (",gFlops," GFlop/s)");
    if( print )
    {
        Print( A, "A after factorization" );
        Print( t, "phases" );
        Print( d, "diagonal" );
    }
    if( testCorrectness )
        TestCorrectness( print, A, t, d, AOrig );
}

int 
main( int argc, char* argv[] )
{
    Environment env( argc, argv );
    mpi::Comm comm = mpi::COMM_WORLD;
    const Int commSize = mpi::Size( comm );

    try
    {
        Int r = Input("--gridHeight","height of process grid",0);
        const bool colMajor = Input("--colMajor","column-major ordering?",true);
        const Int m = Input("--height","height of matrix",100);
        const Int n = Input("--width","width of matrix",100);
        const Int nb = Input("--nb","algorithmic blocksize",96);
        const bool testCorrectness = Input
            ("--correctness","test correctness?",true);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        if( r == 0 )
            r = Grid::FindFactor( commSize );
        const GridOrder order = ( colMajor ? COLUMN_MAJOR : ROW_MAJOR );
        const Grid g( comm, r, order );
        SetBlocksize( nb );
        ComplainIfDebug();

        TestLQ<double>( testCorrectness, print, m, n, g );
        TestLQ<Complex<double>>( testCorrectness, print, m, n, g );
    }
    catch( exception& e ) { ReportException(e); }

    return 0;
}
