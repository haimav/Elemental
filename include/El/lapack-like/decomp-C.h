/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef EL_LAPACK_DECOMP_C_H
#define EL_LAPACK_DECOMP_C_H

#include "El/lapack-like/funcs-C.h"

#ifdef __cplusplus
extern "C" {
#endif

/* HermitianGenDefiniteEigType */
typedef enum {
  EL_AXBX=1,
  EL_ABX=2, 
  EL_BAX=3
} ElHermitianGenDefiniteEigType;

/* HermitianSdcCtrl */
typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  float tol;
  float spreadFactor;
  bool progress;
} ElHermitianSdcCtrl_s;
ElError ElHermitianSdcCtrlDefault_s( ElHermitianSdcCtrl_s* ctrl );

typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  double tol;
  double spreadFactor;
  bool progress;
} ElHermitianSdcCtrl_d;
ElError ElHermitianSdcCtrlDefault_d( ElHermitianSdcCtrl_d* ctrl );

/* HermitianEigSubset */
typedef struct {
  bool indexSubset;
  ElInt lowerIndex, upperIndex;

  bool rangeSubset;
  float lowerBound, upperBound;
} ElHermitianEigSubset_s;
ElError ElHermitianEigSubsetDefault_s( ElHermitianEigSubset_s* subset );

typedef struct {
  bool indexSubset;
  ElInt lowerIndex, upperIndex;

  bool rangeSubset;
  double lowerBound, upperBound;
} ElHermitianEigSubset_d;
ElError ElHermitianEigSubsetDefault_d( ElHermitianEigSubset_d* subset );

/* HermitianEigCtrl */
typedef struct {
  ElHermitianTridiagCtrl tridiagCtrl;
  ElHermitianSdcCtrl_s sdcCtrl;
  bool useSdc;
} ElHermitianEigCtrl_s;
ElError ElHermitianEigCtrlDefault_s( ElHermitianEigCtrl_s* ctrl );

typedef struct {
  ElHermitianTridiagCtrl tridiagCtrl;
  ElHermitianSdcCtrl_d sdcCtrl;
  bool useSdc;
} ElHermitianEigCtrl_d;
ElError ElHermitianEigCtrlDefault_d( ElHermitianEigCtrl_d* ctrl );

/* PolarCtrl */
typedef struct {
  bool qdwh;
  bool colPiv;
  ElInt maxIts;
  ElInt numIts;
} ElPolarCtrl;
ElError ElPolarCtrlDefault( ElPolarCtrl* ctrl );

/* HessQrCtrl */
typedef struct {
  bool aed;
  ElInt blockHeight, blockWidth;
} ElHessQrCtrl;
ElError ElHessQrCtrlDefault( ElHessQrCtrl* ctrl );

/* SdcCtrl */
typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  float tol;
  float spreadFactor;
  bool random;
  bool progress;
  ElSignCtrl_s signCtrl;
} ElSdcCtrl_s;
ElError ElSdcCtrlDefault_s( ElSdcCtrl_s* ctrl );

typedef struct {
  ElInt cutoff;
  ElInt maxInnerIts, maxOuterIts;
  double tol;
  double spreadFactor;
  bool random;
  bool progress;
  ElSignCtrl_d signCtrl;
} ElSdcCtrl_d;
ElError ElSdcCtrlDefault_d( ElSdcCtrl_d* ctrl );

/* SchurCtrl */
typedef struct {
  bool useSdc;
  ElHessQrCtrl qrCtrl;
  ElSdcCtrl_s sdcCtrl;
} ElSchurCtrl_s;
ElError ElSchurCtrlDefault_s( ElSchurCtrl_s* ctrl );

typedef struct {
  bool useSdc;
  ElHessQrCtrl qrCtrl;
  ElSdcCtrl_d sdcCtrl;
} ElSchurCtrl_d;
ElError ElSchurCtrlDefault_d( ElSchurCtrl_d* ctrl );

/* Hermitian eigenvalue solvers
   ============================ */

/* Compute all eigenvalues
   ----------------------- */
ElError ElHermitianEig_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElSortType sort );
ElError ElHermitianEig_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElSortType sort );
ElError ElHermitianEig_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElSortType sort );
ElError ElHermitianEig_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElSortType sort );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, 
  ElSortType sort );
ElError ElHermitianEigDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w,
  ElSortType sort );
ElError ElHermitianEigDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w,
  ElSortType sort );
ElError ElHermitianEigDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w,
  ElSortType sort );

/* TODO: Expert version */

/* Compute the entire eigenvalue decomposition 
   ------------------------------------------- */
ElError ElHermitianEigPair_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElMatrix_s Z,
  ElSortType sort );
ElError ElHermitianEigPair_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElMatrix_d Z,
  ElSortType sort );
ElError ElHermitianEigPair_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort );
ElError ElHermitianEigPair_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigPairDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElDistMatrix_s Z,
  ElSortType sort );
ElError ElHermitianEigPairDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort );
ElError ElHermitianEigPairDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort );
ElError ElHermitianEigPairDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort );

/* TODO: Expert version */

/* Compute a partial set of eigenvalues
   ------------------------------------ */
ElError ElHermitianEigPartial_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartial_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianEigPartial_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartial_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigPartialDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartialDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianEigPartialDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianEigPartialDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Compute a partial set of eigenpairs
   ----------------------------------- */
ElError ElHermitianEigPairPartial_s
( ElUpperOrLower uplo, ElMatrix_s A, ElMatrix_s w, ElMatrix_s Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartial_d
( ElUpperOrLower uplo, ElMatrix_d A, ElMatrix_d w, ElMatrix_d Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianEigPairPartial_c
( ElUpperOrLower uplo, ElMatrix_c A, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartial_z
( ElUpperOrLower uplo, ElMatrix_z A, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianEigPairPartialDist_s
( ElUpperOrLower uplo, ElDistMatrix_s A, ElDistMatrix_s w, ElDistMatrix_s Z, 
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartialDist_d
( ElUpperOrLower uplo, ElDistMatrix_d A, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianEigPairPartialDist_c
( ElUpperOrLower uplo, ElDistMatrix_c A, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianEigPairPartialDist_z
( ElUpperOrLower uplo, ElDistMatrix_z A, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Hermitian generalized-definite eigensolvers
   =========================================== */

/* Compute all of the eigenvalues
   ------------------------------ */
ElError ElHermitianGenDefiniteEig_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElSortType sort );
ElError ElHermitianGenDefiniteEig_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElSortType sort );
ElError ElHermitianGenDefiniteEig_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElSortType sort );
ElError ElHermitianGenDefiniteEig_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElSortType sort );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefiniteEigDist_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, 
  ElSortType sort );
ElError ElHermitianGenDefiniteEigDist_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigDist_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigDist_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w,
  ElSortType sort );

/* TODO: Expert version */

/* Compute the entire eigenvalue decomposition 
   ------------------------------------------- */
ElError ElHermitianGenDefiniteEigPair_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElMatrix_s Z,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigPair_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElMatrix_d Z,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigPair_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElMatrix_c Z,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigPair_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElMatrix_z Z,
  ElSortType sort );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefiniteEigPairDist_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, ElDistMatrix_s Z,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigPairDist_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigPairDist_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort );
ElError ElHermitianGenDefiniteEigPairDist_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort );

/* TODO: Expert version */

/* Compute a partial set of eigenvalues
   ------------------------------------ */
ElError ElHermitianGenDefiniteEigPartial_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPartial_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefiniteEigPartial_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPartial_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* NOTE: 'A' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefiniteEigPartialDist_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPartialDist_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefiniteEigPartialDist_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPartialDist_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* TODO: Expert version */

/* Compute a partial set of eigenpairs
   ----------------------------------- */
ElError ElHermitianGenDefiniteEigPairPartial_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_s A, ElMatrix_s B, ElMatrix_s w, ElMatrix_s Z, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPairPartial_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_d A, ElMatrix_d B, ElMatrix_d w, ElMatrix_d Z, ElSortType sort,
  ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefiniteEigPairPartial_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_c A, ElMatrix_c B, ElMatrix_s w, ElMatrix_c Z, ElSortType sort,
  ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPairPartial_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElMatrix_z A, ElMatrix_z B, ElMatrix_d w, ElMatrix_z Z, ElSortType sort,
  ElHermitianEigSubset_d subset );

/* NOTE: 'A' and 'Z' must be in a [MC,MR] distribution, while 
         'w' must be in a [VR,STAR] distribution */
ElError ElHermitianGenDefiniteEigPairPartialDist_s
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_s A, ElDistMatrix_s B, ElDistMatrix_s w, ElDistMatrix_s Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPairPartialDist_d
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_d A, ElDistMatrix_d B, ElDistMatrix_d w, ElDistMatrix_d Z,
  ElSortType sort, ElHermitianEigSubset_d subset );
ElError ElHermitianGenDefiniteEigPairPartialDist_c
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_c A, ElDistMatrix_c B, ElDistMatrix_s w, ElDistMatrix_c Z,
  ElSortType sort, ElHermitianEigSubset_s subset );
ElError ElHermitianGenDefiniteEigPairPartialDist_z
( ElHermitianGenDefiniteEigType type, ElUpperOrLower uplo, 
  ElDistMatrix_z A, ElDistMatrix_z B, ElDistMatrix_d w, ElDistMatrix_z Z,
  ElSortType sort, ElHermitianEigSubset_d subset );

/* TODO: Expert version */

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* ifndef EL_LAPACK_DECOMP_C_H */
