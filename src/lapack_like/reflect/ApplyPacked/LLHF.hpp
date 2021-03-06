/*
   Copyright (c) 2009-2015, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_APPLYPACKEDREFLECTORS_LLHF_HPP
#define EL_APPLYPACKEDREFLECTORS_LLHF_HPP

namespace El {
namespace apply_packed_reflectors {

//
// Since applying Householder transforms from vectors stored top-to-bottom
// implies that we will be forming a generalization of
//
//  (I - tau_1 v_1^T conj(v_1)) (I - tau_0 v_0^T conj(v_0)) = 
//  I - [ v_0^T, v_1^T ] [  tau_0,                       0     ] [ conj(v_0) ]
//                       [ -tau_0 tau_1 conj(v_1) v_0^T, tau_1 ] [ conj(v_1) ],
//
// which has a lower-triangular center matrix, say S, we will form S as 
// the inverse of a matrix T, which can easily be formed as
// 
//   tril(T) = tril( conj(V V^H) ),  diag(T) = 1/t or 1/conj(t),
//
// where V is the matrix of Householder vectors and t is the vector of scalars.
//

template<typename F> 
inline void
LLHF
( Conjugation conjugation,
  Int offset, 
  const Matrix<F>& H,
  const Matrix<F>& t,
        Matrix<F>& A )
{
    DEBUG_ONLY(
      CSE cse("apply_packed_reflectors::LLHF");
      if( H.Width() != A.Height() )
          LogicError("H's width must match A's height");
    )
    const Int diagLength = H.DiagonalLength(offset);
    DEBUG_ONLY(
      if( t.Height() != diagLength )
          LogicError("t must be the same length as H's offset diag");
    )
    Matrix<F> HPanConj, SInv, Z;

    const Int iOff = ( offset>=0 ? 0      : -offset );
    const Int jOff = ( offset>=0 ? offset : 0       );

    const Int bsize = Blocksize();
    for( Int k=0; k<diagLength; k+=bsize )
    {
        const Int nb = Min(bsize,diagLength-k);
        const Int ki = k+iOff;
        const Int kj = k+jOff;

        auto HPan = H( IR(ki,ki+nb), IR(0,kj+nb) );
        auto ATop = A( IR(0,kj+nb),  ALL         );
        auto t1   = t( IR(k,k+nb),   ALL         );

        Conjugate( HPan, HPanConj );
        MakeTrapezoidal( LOWER, HPanConj, HPanConj.Width()-HPanConj.Height() );
        FillDiagonal( HPanConj, F(1), HPanConj.Width()-HPanConj.Height() );
        Herk( LOWER, NORMAL, Base<F>(1), HPanConj, SInv );
        FixDiagonal( conjugation, t1, SInv );

        Gemm( NORMAL, NORMAL, F(1), HPanConj, ATop, Z );
        Trsm( LEFT, LOWER, NORMAL, NON_UNIT, F(1), SInv, Z );
        Gemm( ADJOINT, NORMAL, F(-1), HPanConj, Z, F(1), ATop );
    }
}

template<typename F> 
inline void
LLHF
( Conjugation conjugation,
  Int offset, 
  const ElementalMatrix<F>& HPre,
  const ElementalMatrix<F>& tPre, 
        ElementalMatrix<F>& APre )
{
    DEBUG_ONLY(
      CSE cse("apply_packed_reflectors::LLHF");
      if( HPre.Width() != APre.Height() )
          LogicError("H's width must match A's height");
      AssertSameGrids( HPre, tPre, APre );
    )

    DistMatrixReadProxy<F,F,MC,MR  > HProx( HPre );
    DistMatrixReadProxy<F,F,MC,STAR> tProx( tPre );
    DistMatrixReadWriteProxy<F,F,MC,MR  > AProx( APre );
    auto& H = HProx.GetLocked();
    auto& t = tProx.GetLocked();
    auto& A = AProx.Get();

    const Int diagLength = H.DiagonalLength(offset);
    DEBUG_ONLY(
      if( t.Height() != diagLength )
          LogicError("t must be the same length as H's offset diag");
    )
    const Grid& g = H.Grid();
    DistMatrix<F> HPanConj(g);
    DistMatrix<F,STAR,VR  > HPan_STAR_VR(g), Z_STAR_VR(g); 
    DistMatrix<F,STAR,MC  > HPan_STAR_MC(g);
    DistMatrix<F,STAR,MR  > Z_STAR_MR(g);
    DistMatrix<F,STAR,STAR> t1_STAR_STAR(g), SInv_STAR_STAR(g);

    const Int iOff = ( offset>=0 ? 0      : -offset );
    const Int jOff = ( offset>=0 ? offset : 0       );

    const Int bsize = Blocksize();
    for( Int k=0; k<diagLength; k+=bsize )
    {
        const Int nb = Min(bsize,diagLength-k);
        const Int ki = k+iOff;
        const Int kj = k+jOff;

        auto HPan = H( IR(ki,ki+nb), IR(0,kj+nb) );
        auto ATop = A( IR(0,kj+nb),  ALL         );
        auto t1   = t( IR(k,k+nb),   ALL         );

        Conjugate( HPan, HPanConj );
        MakeTrapezoidal( LOWER, HPanConj, HPanConj.Width()-HPanConj.Height() );
        FillDiagonal( HPanConj, F(1), HPanConj.Width()-HPanConj.Height() );
        HPan_STAR_VR = HPanConj;
        Zeros( SInv_STAR_STAR, nb, nb );
        Herk
        ( LOWER, NORMAL,
          Base<F>(1), HPan_STAR_VR.LockedMatrix(),
          Base<F>(0), SInv_STAR_STAR.Matrix() );
        El::AllReduce( SInv_STAR_STAR, HPan_STAR_VR.RowComm() );
        t1_STAR_STAR = t1;
        FixDiagonal( conjugation, t1_STAR_STAR, SInv_STAR_STAR );

        HPan_STAR_MC.AlignWith( ATop );
        HPan_STAR_MC = HPan_STAR_VR;
        Z_STAR_MR.AlignWith( ATop );
        LocalGemm( NORMAL, NORMAL, F(1), HPan_STAR_MC, ATop, Z_STAR_MR );
        Z_STAR_VR.AlignWith( ATop );
        Contract( Z_STAR_MR, Z_STAR_VR );

        LocalTrsm
        ( LEFT, LOWER, NORMAL, NON_UNIT, F(1), SInv_STAR_STAR, Z_STAR_VR );

        Z_STAR_MR = Z_STAR_VR;
        LocalGemm
        ( ADJOINT, NORMAL, F(-1), HPan_STAR_MC, Z_STAR_MR, F(1), ATop );
    }
}

} // namespace apply_packed_reflectors
} // namespace El

#endif // ifndef EL_APPLYPACKEDREFLECTORS_LLHF_HPP
