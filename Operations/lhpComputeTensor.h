/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpComputeTensor.h,v $
Language:  C++
Date:      $Date: 2009-01-15 17:17:35 $
Version:   $Revision: 1.1.2.1 $
Authors:   Gregor Klajnsek
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#ifndef __lhpComputeTensor_H__
#define __lhpComputeTensor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include <vtkImageData.h>
#include <vtkRectilinearGrid.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkIntArray.h>
#include <vtkUnsignedShortArray.h>


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
// TODO: remove comments when moved to lhpBuilder directory
//class mafNode;

//----------------------------------------------------------------------------
// lhpOpComputeTensors :
//----------------------------------------------------------------------------
/** TODO: Add your own description here

*/

/** Structure representing a tensor
*/
struct Tensor
{
  double e[6]; // vector form
  double E[3][3]; // matrix form
};

bool ComputeTensor(vtkImageData* volume, vtkRectilinearGrid* displacementVectors, vtkImageData* tensorVolume, bool bUseGaussPoints = false);

template <class T> void TensorCalcCentroidal(T* pVectors, double* pTensors, double J[3][3], int slices, int rows, int cols);
template <class T> void TensorCalcGauss(T* pVectors, double* pTensors, double J[3][3], int slices, int rows, int cols);

void ComputeTensorInVoxel(double xi, double eta, double zeta, const double displacements[24], const double Jprime[3][3], Tensor *tensor);
inline void ComputeJ(double spacing[3], double J[3][3]);
void mutiplyUCwithMInverted(const double UC[24], double UCT[24]);
void multiplyUCTwithDisplacementMatrix1(const double UCT[24], double xi, double eta, double zeta, double *V);  
void multiplyUCTwithDisplacementMatrix2(const double UCT[24], double xi, double eta, double zeta, double *V);  
void multiplyUCTwithDisplacementMatrix3(const double UCT[24], double xi, double eta, double zeta, double *V);  
void multiplyMatrixWithVector(const double m[3][3], const double v[3], double w[3]);
void calculateStrainTensor(const double Q[3][3], Tensor *tensor);

void TransformComponentToScalars(vtkImageData *volume, int component, int min, int max);
void GetArrayLimits(vtkImageData *volume, int& min, int& max);


#endif