/*
   Copyright (c) 2009-2015, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_OPTIMIZATION_UTIL_POS_ORTH_HPP
#define EL_OPTIMIZATION_UTIL_POS_ORTH_HPP

namespace El {
namespace pos_orth {

// Compute the complementarity ratio
// =================================
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real ComplementRatio
( const Matrix<Real>& s,
  const Matrix<Real>& z );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real ComplementRatio
( const ElementalMatrix<Real>& s,
  const ElementalMatrix<Real>& z );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real ComplementRatio
( const DistMultiVec<Real>& s,
  const DistMultiVec<Real>& z );

// Maximum step
// ============
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real MaxStep
( const Matrix<Real>& s, 
  const Matrix<Real>& ds, 
  Real upperBound=std::numeric_limits<Real>::max() );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real MaxStep
( const ElementalMatrix<Real>& s, 
  const ElementalMatrix<Real>& ds, 
  Real upperBound=std::numeric_limits<Real>::max() );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Real MaxStep
( const DistMultiVec<Real>& s, 
  const DistMultiVec<Real>& ds, 
  Real upperBound=std::numeric_limits<Real>::max() );

// Number of members outside of cone
// =================================
template<typename Real,typename=EnableIf<IsReal<Real>>>
Int NumOutside( const Matrix<Real>& A );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Int NumOutside( const SparseMatrix<Real>& A );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Int NumOutside( const AbstractDistMatrix<Real>& A );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Int NumOutside( const DistSparseMatrix<Real>& A );
template<typename Real,typename=EnableIf<IsReal<Real>>>
Int NumOutside( const DistMultiVec<Real>& A );

// Compute a Nesterov-Todd point
// =============================
// The Nesterov-Todd point, w, is a member of the positive orthant whose 
// quadratic representation maps z to s.
template<typename Real,typename=EnableIf<IsReal<Real>>>
void NesterovTodd
( const Matrix<Real>& s, 
  const Matrix<Real>& z, 
        Matrix<Real>& w );
template<typename Real,typename=EnableIf<IsReal<Real>>>
void NesterovTodd
( const ElementalMatrix<Real>& s, 
  const ElementalMatrix<Real>& z, 
        ElementalMatrix<Real>& w );
template<typename Real,typename=EnableIf<IsReal<Real>>>
void NesterovTodd
( const DistMultiVec<Real>& s, 
  const DistMultiVec<Real>& z, 
        DistMultiVec<Real>& w );

// Push a pair into positive orthant
// =================================
template<typename Real,typename=EnableIf<IsReal<Real>>>
void PushPairInto
(       Matrix<Real>& s,
        Matrix<Real>& z,
  const Matrix<Real>& w,
  Real wMaxNormLimit );
template<typename Real,typename=EnableIf<IsReal<Real>>>
void PushPairInto
(       ElementalMatrix<Real>& s,
        ElementalMatrix<Real>& z,
  const ElementalMatrix<Real>& w,
  Real wMaxNormLimit );
template<typename Real,typename=EnableIf<IsReal<Real>>>
void PushPairInto
(       DistMultiVec<Real>& s,
        DistMultiVec<Real>& z,
  const DistMultiVec<Real>& w,
  Real wMaxNormLimit );

} // namespace pos_orth
} // namespace El

#endif // ifndef EL_OPTIMIZATION_UTIL_POS_ORTH_HPP
