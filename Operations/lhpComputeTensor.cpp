/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpComputeTensor.cpp,v $
Language:  C++
Date:      $Date: 2009-01-23 13:48:42 $
Version:   $Revision: 1.1.2.2 $
Authors:   Gregor Klajnsek
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"  
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpComputeTensor.h"


//---------------------------------------------------------------------------------------------------------------------
// T E N S O R   C A L C U L U S
//---------------------------------------------------------------------------------------------------------------------

/**
*/

/** Function computes nodal tensors in the nodes of the volume
@volume vtkImageData dataset that has to include additional array of vectors
*/

//------------------------------------------------------------------------------
bool ComputeTensor(vtkImageData* volume, vtkRectilinearGrid* displacementVectors, vtkImageData* tensorVolume, bool bUseGaussPoints)
//------------------------------------------------------------------------------
{
  double origin[3];
  double spacing[3];
  int dimensions[3];
  int sliceSize;
  volume->GetOrigin(origin);
  volume->GetSpacing(spacing);
  volume->GetDimensions(dimensions);
  int cols = dimensions[0]-1;  // dimension represents number of nodes in a row so we must subtract 1 to get the number of voxels in a row
  int rows = dimensions[1]-1;  // dimension represents number of nodes in a column so we must subtract 1 to get the number of voxels in a column
  int slices = dimensions[2]-1;
  sliceSize = rows * cols;       // number of voxels in one slice
  int nodesPerSlice = dimensions[0] * dimensions[1];


  // From the spacing we can calculate matrix J. As the input is image data all the voxels are of the same size and the cells are axis aligned.
  // The matrix J transforms global coordinates into local coordinates. 
  double J[3][3];
  ComputeJ(spacing, J);


  // Check if the array of displacement vectors is present and store the pointer to it into variable for faster access.
  vtkDataArray* vectors;
  vectors = displacementVectors->GetPointData()->GetVectors();  // get the list of displacement vectors
  if (!vectors)                                    // if there are no vectors present 
    return false;                                        // terminate the process


  // Create a tensor array which we will fill with data.
  int numPoints = volume->GetNumberOfPoints();
  vtkDoubleArray* tensors = vtkDoubleArray::New();
  tensors->SetNumberOfComponents(9);
  tensors->SetNumberOfTuples(numPoints);

  // Calculate tensors. For each voxel we at first calculate the tensor in the centroid. 
  // Nodal tensor is then calculated from strain tensors.
  int offsetOfFirstNode = 0;
  double xi = 0.5, eta = 0.5, zeta = 0.5;
  int counter = 0;
  void* pVectors = vectors->GetVoidPointer(0);
  void* pTensors = tensors->GetVoidPointer(0);


  // call the routine that performs calculation1
  if (bUseGaussPoints)
  {
    if (vectors->GetDataType() == VTK_FLOAT)
      TensorCalcGauss((float*)pVectors, (double*)pTensors, J, slices, rows, cols);
    else if (vectors->GetDataType() == VTK_DOUBLE)
      TensorCalcGauss((double*)pVectors, (double*)pTensors, J, slices, rows, cols);
  }
  else
  {
    if (vectors->GetDataType() == VTK_FLOAT)
      TensorCalcCentroidal((float*)pVectors, (double*)pTensors, J, slices, rows, cols);
    else if (vectors->GetDataType() == VTK_DOUBLE)
      TensorCalcCentroidal((double*)pVectors, (double*)pTensors, J, slices, rows, cols);
  }

  // set the tensors to the Volume dataset
  tensorVolume->Update();
  tensorVolume->GetPointData()->SetTensors(tensors);
  tensors->Delete();
  return true;
}


//------------------------------------------------------------------------------
bool ComputeEigenvalues(vtkImageData* tensorVolume)
//------------------------------------------------------------------------------
{
  vtkDoubleArray* tensorArray =  vtkDoubleArray::SafeDownCast(tensorVolume->GetPointData()->GetTensors());
  if (!tensorArray)
    return false;
  int numTensors = tensorArray->GetNumberOfTuples();

  vtkDoubleArray* eigenvalueArray = vtkDoubleArray::New();
  eigenvalueArray->SetNumberOfComponents(3);
  eigenvalueArray->SetNumberOfTuples(numTensors);

  double Tensor[3][3];  // matrix representing the current tensor
  double Evalues[3];        // array of eigenvalues
  double Evectors[3][3];     // matrix representing the eigenvectors

  for (int i=0; i<numTensors; i++)
    {
    tensorArray->GetTuple(i, (double*)Tensor);
    // calculate eigenvalues
    vtkMath::Diagonalize3x3(Tensor, Evalues, Evectors);
    eigenvalueArray->SetTuple(i, Evalues);
    }
  
  // set the tensors to the Volume dataset
  tensorVolume->GetPointData()->SetVectors(eigenvalueArray);
  tensorVolume->Update();
  eigenvalueArray->Delete();
  return true;
}










/**
*/

//------------------------------------------------------------------------------
template <class T> 
void TensorCalcCentroidal(T* pVectors, double* pTensors, double J[3][3], int slices, int rows, int cols)
//------------------------------------------------------------------------------
{  

  //Tensor currentTensor;
  int nodesPerSlice = (cols + 1) * (rows + 1);
  double xi = 0.5, eta = 0.5, zeta = 0.5;
  int counter = 0;
  // small speedup
  int r3 = 1 * 3;
  int c3 = (cols+1)*3;
  int nps3 = nodesPerSlice * 3;
  const int blockLength = sizeof(double)*9;  


  // reserve memory temporary arrays. This temporary arrays will store the values of centroidal tensors
  // which will be used for calculating nodal tensors
  Tensor *centroidalTensorsCurrentSlice = NULL, *centroidalTensorsPrevSlice = NULL;
  centroidalTensorsCurrentSlice = new Tensor[cols * rows];
  centroidalTensorsPrevSlice = new Tensor[cols * rows];
  memset(centroidalTensorsCurrentSlice,0,sizeof(Tensor) * cols * rows);
  memset(centroidalTensorsPrevSlice,0,sizeof(Tensor) * cols * rows);


  if (centroidalTensorsCurrentSlice == NULL || centroidalTensorsPrevSlice == NULL)  // we could not reserve memory
    return;                                                      // should probably trigger some error here ...

  // loop through all slices
  for (int z=0; z<=slices; z++)   // slices
  {
    // at first we calculate the centroidal tensors for all voxels in a slice
    if (z == slices)
      memset(centroidalTensorsCurrentSlice,0,sizeof(Tensor) * cols * rows);
    else
      for (int y=0; y<rows; y++)     // rows 
        for (int x=0; x<cols; x++)     // columns
        {
          double UC[24];

          int offsetOfFirstNode = (z * nodesPerSlice + y * (cols+1) + x)*3;

          int voxelIndex = offsetOfFirstNode;
          UC[0] = pVectors[voxelIndex++ ];
          UC[1] = pVectors[voxelIndex++ ];
          UC[2] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + r3;
          UC[3] = pVectors[voxelIndex++ ];
          UC[4] = pVectors[voxelIndex++ ];
          UC[5] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + r3 + c3;
          UC[6] = pVectors[voxelIndex++ ];
          UC[7] = pVectors[voxelIndex++ ];
          UC[8] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + c3;
          UC[9] = pVectors[voxelIndex++ ];
          UC[10] = pVectors[voxelIndex++ ];
          UC[11] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3;
          UC[12] = pVectors[voxelIndex++ ];
          UC[13] = pVectors[voxelIndex++ ];
          UC[14] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3 + r3;
          UC[15] = pVectors[voxelIndex++ ];
          UC[16] = pVectors[voxelIndex++ ];
          UC[17] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3 + c3 + r3;
          UC[18] = pVectors[voxelIndex++ ];
          UC[19] = pVectors[voxelIndex++ ];
          UC[20] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3 + c3;
          UC[21] = pVectors[voxelIndex++ ];
          UC[22] = pVectors[voxelIndex++ ];
          UC[23] = pVectors[voxelIndex ];

          ComputeTensorInVoxel(xi, eta, zeta, UC, J, &centroidalTensorsCurrentSlice[y * cols + x]); 
        }


        // and then we calculate the nodal tensors from the gauss tensors
        //int offsetOfFirstNodeInSlice = counter;
        Tensor AverageTensor;

        for (int y=0; y<=rows; y++)     // rows 
          for (int x=0; x<=cols; x++)     // columns
          {
            memset(AverageTensor.E, 0, sizeof(double)*9);
            int numValid = 0;                                // number of tensors we were able to use
            for (int i = 0; i < 3; i++)
              for (int j = 0; j < 3; j++)
              {

                if (x<cols && y<rows)
                { 
                  AverageTensor.E[i][j] += centroidalTensorsCurrentSlice[y*cols + x].E[i][j];  
                  AverageTensor.E[i][j] += centroidalTensorsPrevSlice[y*cols + x].E[i][j];   
                  numValid +=2; 
                }

                if (x>0 && y < rows)            
                { 
                  AverageTensor.E[i][j] += centroidalTensorsCurrentSlice[y*cols + (x - 1)].E[i][j];  
                  AverageTensor.E[i][j] += centroidalTensorsPrevSlice[y*cols + (x-1)].E[i][j]; 
                  numValid += 2; 
                } 

                if (x > 0 && y > 0)
                { 
                  AverageTensor.E[i][j] += centroidalTensorsCurrentSlice[(y-1)*cols + (x-1)].E[i][j]; 
                  AverageTensor.E[i][j] += centroidalTensorsPrevSlice[(y-1)*cols + (x-1)].E[i][j]; 
                  numValid +=2; 
                }    

                if (y>0 && x<cols)
                { 
                  AverageTensor.E[i][j] += centroidalTensorsCurrentSlice[(y-1)*cols + x].E[i][j]; 
                  AverageTensor.E[i][j] += centroidalTensorsPrevSlice[(y-1)*cols + x].E[i][j];  
                  numValid +=2; 
                }   

                AverageTensor.E[i][j] /= numValid; 
              }

              memcpy(&pTensors[counter], AverageTensor.E, blockLength);
              counter += 9;
          }

          // swap the pointers to the current and previous slice tensors to avoid unnecesessary copy
          Tensor* tmpArray = centroidalTensorsPrevSlice;
          centroidalTensorsPrevSlice = centroidalTensorsCurrentSlice;
          centroidalTensorsCurrentSlice = tmpArray;
  }

  // free the memory used for temporary arrays
  delete[] centroidalTensorsCurrentSlice;
  delete[] centroidalTensorsPrevSlice;
}




//------------------------------------------------------------------------------
template <class T> 
void TensorCalcGauss(T* pVectors, double* pTensors, double J[3][3], int slices, int rows, int cols)
//------------------------------------------------------------------------------
{  

  //Tensor currentTensor;
  int nodesPerSlice = (cols + 1) * (rows + 1);
  double xi = 0.5, eta = 0.5, zeta = 0.5;
  int counter = 0;
  // small speedup
  int r3 = 1 * 3;
  int c3 = (cols+1)*3;
  int nps3 = nodesPerSlice * 3;
  const int blockLength = sizeof(double)*9;  


  // reserve memory temporary arrays. This temporary arrays will store the values of gauss tensors
  // which will be used for calculating nodal tensors
  Tensor *gaussTensorsCurrentSlice = NULL, *gaussTensorsPrevSlice = NULL;  // we will need two slices of gauss points
  gaussTensorsCurrentSlice = new Tensor[cols * rows * 8]; // Each voxel has 8 Gauss points
  gaussTensorsPrevSlice = new Tensor[cols * rows * 8];
  memset(gaussTensorsPrevSlice,0,sizeof(Tensor) * cols * rows * 8);

  if (gaussTensorsCurrentSlice == NULL  || gaussTensorsPrevSlice == NULL)  // we could not reserve memory
    return;                                                      // should probably trigger some error here ...


  // TODO: LOOP THROUGH ALL EIGHT GAUSS POINTS AND CALCULATE THE TENSOR IN THAT POINT
  // PERFORM DIFFERENT KIND OF INTERPOLATION
  // WE MIGHT NEED TO USE TWO SLICES INSTEAD OF ONE - SO THE ALGORITHM MIGHT BE A BIT DIFFERENT


  // fill the matrix of Gauss Points
  double GP[8][3];
  for (int GaussIndex = 0; GaussIndex < 8; GaussIndex++) 
    GetGaussPoint(GaussIndex, GP[GaussIndex]);

  // loop through all slices
  for (int z=0; z<=slices; z++)   // slices
  {

    if (z == slices)
      memset(gaussTensorsCurrentSlice,0,sizeof(Tensor) * cols * rows * 8);
    else
      for (int y=0; y<rows; y++)     // rows 
        for (int x=0; x<cols; x++)     // columns
        {
          double UC[24];

          int offsetOfFirstNode = (z * nodesPerSlice + y * (cols+1) + x)*3;

          int voxelIndex = offsetOfFirstNode;
          UC[0] = pVectors[voxelIndex++ ];
          UC[1] = pVectors[voxelIndex++ ];
          UC[2] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + r3;
          UC[3] = pVectors[voxelIndex++ ];
          UC[4] = pVectors[voxelIndex++ ];
          UC[5] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + r3 + c3;
          UC[6] = pVectors[voxelIndex++ ];
          UC[7] = pVectors[voxelIndex++ ];
          UC[8] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + c3;
          UC[9] = pVectors[voxelIndex++ ];
          UC[10] = pVectors[voxelIndex++ ];
          UC[11] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3;
          UC[12] = pVectors[voxelIndex++ ];
          UC[13] = pVectors[voxelIndex++ ];
          UC[14] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3 + r3;
          UC[15] = pVectors[voxelIndex++ ];
          UC[16] = pVectors[voxelIndex++ ];
          UC[17] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3 + c3 + r3;
          UC[18] = pVectors[voxelIndex++ ];
          UC[19] = pVectors[voxelIndex++ ];
          UC[20] = pVectors[voxelIndex ];

          voxelIndex = offsetOfFirstNode + nps3 + c3;
          UC[21] = pVectors[voxelIndex++ ];
          UC[22] = pVectors[voxelIndex++ ];
          UC[23] = pVectors[voxelIndex ];

          for (int GaussIndex = 0; GaussIndex < 8; GaussIndex++) 
            ComputeTensorInVoxel(GP[GaussIndex][0], GP[GaussIndex][1], GP[GaussIndex][2], UC, J, &gaussTensorsCurrentSlice[y * cols + x + GaussIndex]); 
        }

        // and then we calculate the nodal tensors from the gauss tensors
        //int offsetOfFirstNodeInSlice = counter;
        Tensor AverageTensor;

        for (int y=0; y<=rows; y++)     // rows 
          for (int x=0; x<=cols; x++)     // columns
          {
            memset(AverageTensor.E, 0, sizeof(double)*9);
            int numValid = 0;                                // number of tensors we were able to use
            for (int i = 0; i < 3; i++)
              for (int j = 0; j < 3; j++)
              {
                if (x<cols && y<rows)
                { 
                  AverageTensor.E[i][j] +=  gaussTensorsCurrentSlice[y*cols + x + 0].E[i][j];  
                  AverageTensor.E[i][j] +=  gaussTensorsPrevSlice[y*cols + x + 4].E[i][j];   
                  numValid +=2; 
                }

                if (x>0 && y < rows)            
                { 
                  AverageTensor.E[i][j] +=  gaussTensorsCurrentSlice[y*cols + (x - 1) + 1].E[i][j];  
                  AverageTensor.E[i][j] +=  gaussTensorsPrevSlice[y*cols + (x-1) + 5].E[i][j]; 
                  numValid += 2; 
                } 

                if (x > 0 && y > 0)
                { 
                  AverageTensor.E[i][j] +=  gaussTensorsCurrentSlice[(y-1)*cols + (x-1)  + 2].E[i][j]; 
                  AverageTensor.E[i][j] +=  gaussTensorsPrevSlice[(y-1)*cols + (x-1) + 6].E[i][j]; 
                  numValid +=2; 
                }    

                if (y>0 && x<cols)
                { 
                  AverageTensor.E[i][j] +=  gaussTensorsCurrentSlice[(y-1)*cols + x + 3].E[i][j]; 
                  AverageTensor.E[i][j] +=  gaussTensorsPrevSlice[(y-1)*cols + x + 7].E[i][j];  
                  numValid +=2; 
                }   

                AverageTensor.E[i][j] /= numValid; 
              }

              memcpy(&pTensors[counter], AverageTensor.E, blockLength);
              counter += 9;
          }

          // swap the pointers to the current and previous slice tensors to avoid unnecesessary copy
          Tensor* tmpArray = gaussTensorsPrevSlice;
          gaussTensorsPrevSlice = gaussTensorsCurrentSlice;
          gaussTensorsCurrentSlice = tmpArray; 
  }

  // free the memory used for temporary arrays
  delete[] gaussTensorsCurrentSlice;
  delete[] gaussTensorsPrevSlice;
}











/**  Function calculates the matrix J from the size of the voxel.
@param spacing		Spacing of the voxel in three directions. 
@param J			The matrix that represents a transformation from global coordinates to local coordinates inside voxel.
*/

//------------------------------------------------------------------------------
inline void ComputeJ(double spacing[3], double J[3][3])
//------------------------------------------------------------------------------
{
  J[0][0] = spacing[0];  J[1][0] = 0;           J[2][0] = 0;
  J[0][1] = 0;           J[1][1] = spacing[1];  J[2][1] = 0;
  J[0][2] = 0;           J[1][2] = 0;           J[2][2] = spacing[2];
  vtkMath::Invert3x3(J, J);

}


/**  Function calculates the matrix J from the coordinates of the voxel
This function is useful if the size of the voxels changes (ie. if we are working with a rectilinear grid or if voxels are not axis aligned.
@param h	Coordinates of node H of the voxel.
@param i	Coordinates of node I of the voxel.
@param k	Coordinates of node K of the voxel.
@param l	Coordinates of node L of the voxel.
*/
//------------------------------------------------------------------------------
inline void ComputeJ(double h[3], double i[3], double k[3], double l[3], double J[3][3])
//------------------------------------------------------------------------------
{  
  J[0][0] = i[0] - h[0]; J[1][0] = k[0] - h[0]; J[2][0] = l[0] - h[0];
  J[0][1] = i[1] - h[1]; J[1][1] = k[1] - h[1]; J[2][1] = l[1] - h[1];
  J[0][2] = i[2] - h[2]; J[1][2] = k[2] - h[2]; J[2][2] = l[2] - h[2];
  vtkMath::Invert3x3(J, J);
}



/** Computes the strain tensor in a point P that lies inside a voxel. The parameters xi, eta, zeta represent the local 
coordinates of the point P.
@param xi		Local x coordinate of point P.
@param eta		Local y coordinate of point P.
@param zeta		Local z coordinate of Point P.
@param displacements	Vector consisting of 24 displacements values obtained from displacement vectors associated with nodes. 
@param JPrime	Precomputed matrix JPrime. 	
@param tensor	Tensor structure that is filled with data after in the function.
*/
//------------------------------------------------------------------------------
void ComputeTensorInVoxel(double xi, double eta, double zeta, const double displacements[24], const double Jprime[3][3], Tensor *tensor)
//------------------------------------------------------------------------------
{  
  // Step 1 - calculate the matrix Jprime.
  // As all the voxels are of the same size we precompute matrix J and send it into the function as a parameter

  // In the second step we take the displacement vectors of all eight nodes and join them into a single vector consisting of 
  // eight components. The structure of the combined vector is: 
  // {h_u, h_v, h_w, i_u, i_v, i_w, j_u, j_v, j_w, k_u, k_v, k_w, l_u, l_v, l_w, m_u, m_v, m_w, n_u, n_v, n_w, p_u, p_v, p_w }
  // vector is created outside and comes in as a parameter

  // Step 3
  // Multiply the vector UC with the inverted matrix M 
  // The result is vector UCT
  double UCT[24];
  mutiplyUCwithMInverted(displacements, UCT);

  // Step 4
  double V0[3] = {0,0,0}, V1[3] = {0,0,0}, V2[3] = {0,0,0};
  multiplyUCTwithDisplacementMatrix1(UCT, xi, eta, zeta, V0);  
  multiplyUCTwithDisplacementMatrix1(UCT, xi, eta, zeta, V1); 
  multiplyUCTwithDisplacementMatrix1(UCT, xi, eta, zeta, V2);  

  // Step 5 - calculate matrix Q from matrix J and vectors V1, V2, V3;
  double Q[3][3];
  multiplyMatrixWithVector(Jprime, V0, Q[0]);
  multiplyMatrixWithVector(Jprime, V1, Q[1]);
  multiplyMatrixWithVector(Jprime, V2, Q[2]);


  // Step 6 - calculate the strain tensor
  calculateStrainTensor(Q, tensor);
}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void mutiplyUCwithMInverted(const double UC[24], double UCT[24])
//------------------------------------------------------------------------------
{
  UCT[0]  =  UC[0];
  UCT[1]  =  UC[1];
  UCT[2]  =  UC[2];

  UCT[3]  = -UC[0] + UC[3];
  UCT[4]  = -UC[1] + UC[4];
  UCT[5]  = -UC[2] + UC[5];

  UCT[6]  = -UC[0] + UC[9];
  UCT[7]  = -UC[1] + UC[10];
  UCT[8]  = -UC[2] + UC[11];

  UCT[9]  = -UC[0] + UC[12];
  UCT[10] = -UC[1] + UC[13];
  UCT[11] = -UC[2] + UC[14];

  UCT[12] =  UC[0] - UC[3]  + UC[6]  - UC[9];
  UCT[13] =  UC[1] - UC[4]  + UC[7]  - UC[10];
  UCT[14] =  UC[2] - UC[5]  + UC[8]  - UC[11];

  UCT[15] =  UC[0] - UC[3]  - UC[12] + UC[15];
  UCT[16] =  UC[1] - UC[4]  - UC[13] + UC[16];
  UCT[17] =  UC[2] - UC[5]  - UC[14] + UC[17];

  UCT[18] =  UC[0] - UC[9]  - UC[12] + UC[21];
  UCT[19] =  UC[1] - UC[10] - UC[13] + UC[22];
  UCT[20] =  UC[2] - UC[11] - UC[14] + UC[23];

  UCT[21] = -UC[0] + UC[3]  - UC[6]  + UC[9]  + UC[12] - UC[15] + UC[18] - UC[21]; 
  UCT[22] = -UC[1] + UC[4]  - UC[7]  + UC[10] + UC[13] - UC[16] + UC[19] - UC[22];
  UCT[23] = -UC[2] + UC[5]  - UC[8]  + UC[11] + UC[14] - UC[17] + UC[20] - UC[23];
}



/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void multiplyUCTwithDisplacementMatrix1(const double UCT[24], double xi, double eta, double zeta, double *V)  
//------------------------------------------------------------------------------
{
  V[0] = UCT[3] + eta * UCT[12] + zeta * UCT[15] + eta * zeta * UCT[21]; 
  V[1] = UCT[6] + xi  * UCT[12] + zeta * UCT[18] + xi  * zeta * UCT[21];
  V[2] = UCT[9] + xi  * UCT[15] +  eta * UCT[18] + xi  *  eta * UCT[21];
}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void multiplyUCTwithDisplacementMatrix2(const double UCT[24], double xi, double eta, double zeta, double *V)  
//------------------------------------------------------------------------------
{
  V[0] = UCT[4]  + eta * UCT[13] + zeta * UCT[16] + eta * zeta * UCT[22]; 
  V[1] = UCT[7]  + xi  * UCT[13] + zeta * UCT[19] + xi  * zeta * UCT[22];
  V[2] = UCT[10] + xi  * UCT[16] +  eta * UCT[19] + xi  *  eta * UCT[22];
}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void multiplyUCTwithDisplacementMatrix3(const double UCT[24], double xi, double eta, double zeta, double *V)  
//------------------------------------------------------------------------------
{
  V[0] = UCT[5]  + eta * UCT[14] + zeta * UCT[17] + eta * zeta * UCT[23]; 
  V[1] = UCT[8]  + xi  * UCT[14] + zeta * UCT[20] + xi  * zeta * UCT[23];
  V[2] = UCT[11] + xi  * UCT[17] +  eta * UCT[20] + xi  *  eta * UCT[23];
}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void multiplyMatrixWithVector(const double m[3][3], const double v[3], double w[3])
//------------------------------------------------------------------------------
{
  w[0] = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2]; 
  w[1] = m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2]; 
  w[2] = m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2]; 
}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
inline void calculateStrainTensor(const double Q[3][3], Tensor* tensor)
//------------------------------------------------------------------------------
{
  // vector
  tensor->e[0] = Q[0][0];
  tensor->e[1] = Q[1][1];
  tensor->e[2] = Q[2][2];
  tensor->e[3] = Q[0][1] + Q[1][0];
  tensor->e[4] = Q[0][2] + Q[2][0];
  tensor->e[5] = Q[1][2] + Q[2][1];

  // matrix diagonal
  tensor->E[0][0] = tensor->e[0];
  tensor->E[1][1] = tensor->e[1];
  tensor->E[2][2] = tensor->e[2];

  // matrix non diagonal
  tensor->E[0][1] = tensor->E[1][0] = 0.5 * tensor->e[3];
  tensor->E[0][2] = tensor->E[2][0] = 0.5 * tensor->e[4];
  tensor->E[1][2] = tensor->E[2][1] = 0.5 * tensor->e[5];
}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void GetGaussPoint(int i, double coordinates[3])
//------------------------------------------------------------------------------
{
  const double a = 0.57735027;
  const double a1 = (1 - a) / 2.0;
  const double a2 = (1 + a) / 2.0;
  double points[8][3] = { {a1, a1, a1}, {a2, a1, a1}, {a2, a2, a1}, { a1, a2, a1},
  {a1, a1, a2}, {a2, a1, a2}, {a2, a2, a2}, {a1, a2, a2}};

  coordinates[0] = points[i][0]; 
  coordinates[1] = points[i][1]; 
  coordinates[2] = points[i][2]; 
}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void CreateGaussPointMatrix(double matrix[8][8])
//------------------------------------------------------------------------------
{
  for (int i=0; i<8; i++)
  {
    double GP[3];
    GetGaussPoint(i, GP);
    matrix[i][0] = 1;
    matrix[i][1] = GP[0];
    matrix[i][2] = GP[1];
    matrix[i][3] = GP[3];
    matrix[i][4] = GP[0] * GP[1];
    matrix[i][5] = GP[1] * GP[2];
    matrix[i][6] = GP[2] * GP[0];
    matrix[i][7] = GP[0] * GP[1] * GP[2];
  }

}


/** TODO: Add comment
*/
//------------------------------------------------------------------------------
void TransformComponentToScalars(vtkImageData *volume, vtkDataArray* dataArray, int component, int min, int max, int type)
//------------------------------------------------------------------------------
{
  if (!volume)
    return;
  
  if (!dataArray)
    {
    volume->GetPointData()->GetScalars()->Reset();
    return;
    }


  // perform sanity checks
  int numTuples = dataArray->GetNumberOfTuples();
  if (numTuples <= 0)
    return; 
  int numComponents = dataArray->GetNumberOfComponents();
  if (numComponents <= component)     // invalid component index
    return;
  if (min > max) // swap
  { unsigned int tmp = min; min = max; max = tmp; }


  // run through the volume and get min and max values and the range
  double value[9];              // tensors have 9 components
  double origMin, origMax;
  dataArray->GetTuple(0, value);
  origMin = origMax = value[component];

  /*  
  for (int i=1; i<numTuples; i++)
  {
    dataArray->GetTuple(i, value);
    if (value[component] < origMin)
      origMin = value[component];
    if (value[component] > origMax)
      origMax = value[component];
  }*/
  double tensorRange[2];
  dataArray->GetRange(tensorRange);
  origMin = tensorRange[0];
  origMax = tensorRange[1];

  double origRange = origMax-origMin;
  double factor  = (max-min)/origRange;


  // create new array of scalars that represent the volume - choose the appropriate data type according to the parameter type
  // fast solution, but not best looking
  vtkDataArray* createdArray = NULL;
  if (type == 0)
    {
      vtkUnsignedShortArray *newScalars = vtkUnsignedShortArray::New();
      for (int i=0; i<numTuples; i++)
      {
        dataArray->GetTuple(i, value);
        int cvalue = min + (int)(factor * value[component] + 0.5);
        newScalars->InsertValue(i, cvalue);
      }
      createdArray = newScalars;
    } 
  else 
    {
    vtkIntArray *newScalars = vtkIntArray::New();
    for (int i=0; i<numTuples; i++)
    {
      dataArray->GetTuple(i, value);
      int cvalue = min + (int)(factor * value[component] + 0.5);
      newScalars->InsertValue(i, cvalue);
    }
    createdArray = newScalars;
  } 

  // replace the old array
  volume->GetPointData()->SetScalars(createdArray);
  volume->Update();
  createdArray->Delete();
}


//------------------------------------------------------------------------------
void GetArrayLimits(vtkDataArray* dataArray, int& min, int& max)
//------------------------------------------------------------------------------
{
  double range[2];
  if (!dataArray)
    return;
  dataArray->GetRange(range);
  min = range[0];
  max = range[1];
}