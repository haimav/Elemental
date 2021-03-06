#
#  Copyright (c) 2009-2015, Jack Poulson
#  All rights reserved.
#
#  This file is part of Elemental and is under the BSD 2-Clause License, 
#  which can be found in the LICENSE file in the root directory, or at 
#  http://opensource.org/licenses/BSD-2-Clause
#
import El

n0 = n1 = 200

display = False
output = False
worldRank = El.mpi.WorldRank()
worldSize = El.mpi.WorldSize()

# A 2D finite-difference matrix with a dense last column
def FD2D(N0,N1):
  A = El.DistSparseMatrix()
  height = N0*N1
  width = N0*N1
  A.Resize(height,width)
  localHeight = A.LocalHeight()
  A.Reserve(6*localHeight)
  for sLoc in xrange(localHeight):
    s = A.GlobalRow(sLoc)
    x0 = s % N0
    x1 = s / N0
    A.QueueLocalUpdate( sLoc, s, 11 )
    if x0 > 0:
      A.QueueLocalUpdate( sLoc, s-1, -1 )
    if x0+1 < N0:
      A.QueueLocalUpdate( sLoc, s+1, 2 )
    if x1 > 0:
      A.QueueLocalUpdate( sLoc, s-N0, -3 )
    if x1+1 < N1:
      A.QueueLocalUpdate( sLoc, s+N0, 4 )

    # The dense last column
    A.QueueLocalUpdate( sLoc, width-1, -10./height );

  A.ProcessQueues()
  return A

A = FD2D(n0,n1)
x = El.DistMultiVec()
y = El.DistMultiVec()
El.Uniform( x, n0*n1, 1 )
El.Copy( x, y )
if display:
  El.Display( A, "A" )
  El.Display( y, "y" )
if output:
  El.Print( A, "A" )
  El.Print( y, "y" )

yNrm = El.Nrm2(y)
if worldRank == 0:
  print "|| y ||_2 =", yNrm

ctrl = El.LeastSquaresCtrl_d()
ctrl.scaleTwoNorm = True
ctrl.basisSize = 15
ctrl.alpha = 1e-5
ctrl.equilibrate = True
ctrl.progress = True
ctrl.solveCtrl.alg = El.REG_REFINE_FGMRES
ctrl.solveCtrl.relTol = 1e-12
ctrl.solveCtrl.relTolRefine = 1e-18
ctrl.solveCtrl.progress = True

solveStart = El.mpi.Time()
El.LinearSolve(A,x,ctrl)
solveStop = El.mpi.Time()
if worldRank == 0:
  print "LinearSolve time:", solveStop-solveStart, "seconds"
if display:
  El.Display( x, "x" )
if output:
  El.Print( x, "x" )
xNrm = El.Nrm2(x)
if worldRank == 0:
  print "|| x ||_2 =", xNrm

El.Multiply(El.NORMAL,-1.,A,x,1.,y)
if display:
  El.Display( y, "A x - y" )
if output:
  El.Print( y, "A x - y" )
eNrm = El.Nrm2(y)
if worldRank == 0:
  print "|| y ||_2 =", yNrm
  print "|| A x - y ||_2 / || y ||_2 =", eNrm/yNrm

# Require the user to press a button before the figures are closed
El.Finalize()
if worldSize == 1:
  raw_input('Press Enter to exit')
