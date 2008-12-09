/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFMuscleDecomposition.cpp,v $ 
  Language: C++ 
  Date: $Date: 2008-12-09 12:36:54 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "vtkMAFMuscleDecomposition.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkCutter.h"

#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923
#endif


vtkCxxRevisionMacro(vtkMAFMuscleDecomposition, "$Revision: 1.1.2.2 $");
vtkStandardNewMacro(vtkMAFMuscleDecomposition);

#include "mafMemDbg.h"
#include "../mafDbg.h"


vtkMAFMuscleDecomposition::vtkMAFMuscleDecomposition()
{
  this->Resolution = 9;       //nine segments => 10 points
  this->NumberOfFibres = 50;

  this->FibersTemplate = NULL;
  this->OriginArea = NULL;
  this->InsertionArea = NULL;

  this->SmoothFibers = 1;
  this->SmoothSteps = 5;
  this->SmoothFactor = 4.0;
  this->DebugOutputMode = 0;
}

vtkMAFMuscleDecomposition::~vtkMAFMuscleDecomposition()
{
  if (FibersTemplate != NULL) {
    FibersTemplate->Delete();
    FibersTemplate = NULL;
  }

  if (OriginArea != NULL) {
    OriginArea->Delete();
    OriginArea = NULL;
  }

  if (InsertionArea != NULL) {
    InsertionArea->Delete();
    InsertionArea = NULL;
  }
}

//------------------------------------------------------------------------
//Sets new template for muscle fibers
/*virtual*/ void vtkMAFMuscleDecomposition::SetFibersTemplate(vtkMAFMuscleFibers* pTemplate)
//------------------------------------------------------------------------
{
  if (pTemplate != FibersTemplate)
  {
    if (FibersTemplate != NULL)
      FibersTemplate->Delete();      
        
    if (NULL != (FibersTemplate = pTemplate))
      FibersTemplate->Register(this);

    this->Modified();
  }
}

//------------------------------------------------------------------------
//Sets new origin area points for the input muscle
/*virtual*/ void vtkMAFMuscleDecomposition::SetOriginArea(vtkPoints* pPoints)
//------------------------------------------------------------------------
{
  if (pPoints != OriginArea)
  {
    if (OriginArea != NULL)
      OriginArea->Delete();      

    if (NULL != (OriginArea = pPoints))
      OriginArea->Register(this);

    this->Modified();
  }
}


//------------------------------------------------------------------------
//Sets new insertion area points for the input muscle
/*virtual*/ void vtkMAFMuscleDecomposition::SetInsertionArea(vtkPoints* pPoints)
//------------------------------------------------------------------------
{
  if (pPoints != InsertionArea)
  {
    if (InsertionArea != NULL)
      InsertionArea->Delete();      

    if (NULL != (InsertionArea = pPoints))
      InsertionArea->Register(this);

    this->Modified();
  }
}

//------------------------------------------------------------------------
//Samples E2 space <0..1>x<0..1> storing samples into points buffer.
//N samples are created. Points buffer must be capable enough to hold
//2*N doubles (format is x1,y1,x2,y2...).
//The routine is based on the code by Frances Y. Kuo <f.kuo@unsw.edu.au>,
//School of Mathematics and Statistics, University of New South Wales,
//Sydney NSW 2052, Australia
void vtkMAFMuscleDecomposition::CreateSobolPoints(int N, double* points)
//------------------------------------------------------------------------
{
  if (N == 1)
  {
    points[0] = points[1] = 0.5;
    return;
  }

  // L = max number of bits needed 
  int L = (int)ceil(log((double)N)/log(2.0)); 

  //generate an array C, C[i] = index from the right of the first zero bit of i
  int* C = new int[N];  
  for (int i = 0; i < N; i++) 
  {
    C[i] = 1;
    int value = i;
    while (value & 1) {
      value >>= 1;
      C[i]++;
    }
  }

  // Compute direction numbers V[1] to V[L], scaled by pow(2,32)
  typedef unsigned long ULong2[2];
  ULong2* V = new ULong2[L+1]; 
  V[1][0] = V[1][1] = 1 << 31; //32 - 1
  for (int i = 2; i <= L; i++) 
  {
    V[i][0] = 1 << (32 - i); // all m's = 1
    V[i][1] = V[i - 1][1] ^ (V[i - 1][1] >> 1); 
  }  

  //Compute points (from bits)
  ULong2 X = {0, 0};  
  for (int i = 0; i < N; i++) 
  {
    for (unsigned j = 0; j < 2; j++) 
    {
      X[j] = X[j] ^ V[C[i]][j];      
      points[2*i + j] = X[j] / 4294967296.0; //pow(2.0,32);
    }
  }

  // Clean up
  delete [] V;  
  delete [] C;  
}

//------------------------------------------------------------------------
//Sorts the given points according to their iCoord coordinate.
//The resulting order is returned and the user is responsible for its
//deallocation when it is no longer needed. pPoints array is not touched.
int* vtkMAFMuscleDecomposition::SortPoints(const VCoord* pPoints, int nPoints, int iCoord)
//------------------------------------------------------------------------
{
  int* pOrder = new int[nPoints];
  for (int i = 0; i < nPoints; i++){
    pOrder[i] = i;
  }

  //heap-sort  
  //construct heap by successive adding of points
  for (int i = 1; i < nPoints; i++)   //1 point is already sorted
  {    
    int k = i;
    while (k > 0)      
    {
      int kPar = ((k + 1) / 2) - 1; //get parent index
      if (pPoints[pOrder[kPar]][iCoord] >= pPoints[pOrder[k]][iCoord]) 
        break;  //heap restored
      
      int tmp = pOrder[k];
      pOrder[k] = pOrder[kPar];
      pOrder[kPar] = tmp;
      
      k = kPar;
    }
  }

  //successive sorting (swap maximum at first position with the last item)
  //and restore the heap considering one item less
  for (int i = nPoints - 1; i > 0; i--)
  {
    //swap
    int tmp = pOrder[0];
    pOrder[0] = pOrder[i];
    pOrder[i] = tmp;
            
    //move top down in the tree (while there is some child)
    int k = 0;
    while (2*(k + 1) < i) 
    {
      int j = 2*k + 1;
      if (j + 1 < i && pPoints[pOrder[j]][iCoord] < pPoints[pOrder[j + 1]][iCoord]) 
        j++;  //goto the right child

      if (pPoints[pOrder[k]][iCoord] >= pPoints[pOrder[j]][iCoord]) 
        break; //heap restored

      tmp = pOrder[k];
      pOrder[k] = pOrder[j];
      pOrder[j] = tmp;
      
      k = j;
    }  
  }

  return pOrder;
}

//------------------------------------------------------------------------
//Gets edges from the contour and sorts them to form continuous path. The
//format of the returned array (the caller is responsible for its
//deallocation) is s1,s2,s2,s3,s3,s4, ... sn,s1 - for instance:
//0,2,2,3,3,4,4,6,0;
int* vtkMAFMuscleDecomposition::GetSortedEdges(vtkPolyData* contour)
//------------------------------------------------------------------------
{
  contour->BuildLinks();
  int nCells = contour->GetNumberOfCells();  
  int nEntries = 2*nCells;

  int* pEdgeIds = new int[nEntries];
  for (int i = 0, j = 0; i < nCells; i++, j += 2)
  {
    vtkIdType nPts, *pPts;
    contour->GetCellPoints(i, nPts, pPts);
    
    pEdgeIds[j + 0] = pPts[0];
    pEdgeIds[j + 1] = pPts[1];
  }

  //pEdgeIds now contains edges whose indices are sorted  
  int nSearchVal = pEdgeIds[1];
  int iChain = 0;
  int iStartPos = 2;

  while (iStartPos < nEntries)
  {
    //search for nSearchVal
    int iFoundPos = -1;
    for (int i = iStartPos; i < nEntries; i += 2)
    {
      if (pEdgeIds[i] == nSearchVal) {
        iFoundPos = i; break;
      }
    }

    if (iFoundPos != iStartPos)
    {
      //swap edges
      int tmp = pEdgeIds[iFoundPos];
      pEdgeIds[iFoundPos] = pEdgeIds[iStartPos];
      pEdgeIds[iStartPos] = tmp;

      tmp = pEdgeIds[iFoundPos + 1];
      pEdgeIds[iFoundPos + 1] = pEdgeIds[iStartPos + 1];
      pEdgeIds[iStartPos + 1] = tmp;
    }

    nSearchVal = pEdgeIds[iStartPos + 1]; //continue with this value
    iStartPos += 2;

    //if the contour is in several pieces we might have found the end
    if (nSearchVal == pEdgeIds[iChain])
    { 
      //continue with the next polygon
      if (iStartPos < nEntries)
      {
        //if there is any other polygon
        iChain = iStartPos;
        nSearchVal = pEdgeIds[iStartPos + 1];
        iStartPos += 2;
      }
    }
  }
  
  return pEdgeIds;
}

//------------------------------------------------------------------------
//Divides the rectangle defined by one point and two vectors into
//nPoints edges such that the total square error between lengths of
//contour and rectangle edges is minimized. The routine stores beginning
//points of these rectangle edges into pOutRectPoints.
void vtkMAFMuscleDecomposition::DivideRectangle(double* origin, double* u, double* v, 
              int nPoints, VCoord* pContourPoints, VCoord* pOutRectPoints)
//------------------------------------------------------------------------
{
  //compute contour edges lengths
  double dblCLen = 0.0;
  double* pELens = new double[nPoints];
  for (int i = 0; i < nPoints; i++)
  {
    int i2 = (i + 1) % nPoints;
    pELens[i] = sqrt(vtkMath::Distance2BetweenPoints(pContourPoints[i], pContourPoints[i2]));
    dblCLen += pELens[i];
  }
  
  //compute sides and sizes of rectangle sides
  double Verts[4][3];
  double RSizes[4];
  for (int i = 0; i < 3; i++)
  {
    Verts[0][i] = u[i];
    Verts[1][i] = v[i];
  }

  RSizes[2] = RSizes[0] = vtkMath::Normalize(Verts[0]);
  RSizes[3] = RSizes[1] = vtkMath::Normalize(Verts[1]);  
  for (int i = 0; i < 3; i++)
  {
    Verts[2][i] = -Verts[0][i];
    Verts[3][i] = -Verts[1][i];
  }

  //and the perimeter of rectangle
  double dblRLen = 2*(RSizes[0] + RSizes[1]);
  double dblCoef = dblRLen / dblCLen;
    
  double dblBestERSizes[4] = {0.0, 0.0, 0.0, 0.0};
  double dblBestScore = DBL_MAX;
  int nBestDivision[4] = {0, 0, 0, nPoints};

  double ERSizes[4];
  ERSizes[0] = pELens[0];
  ERSizes[1] = pELens[1];
  ERSizes[2] = pELens[2];
  ERSizes[3] = dblCLen - (ERSizes[0] + ERSizes[1] + ERSizes[2]);

  //check all possible cases
  for (int i = 1; i < nPoints - 2; i++)
  {
    for (int j = i + 1; j < nPoints - 1; j++)
    {
      for (int k = j + 1; k < nPoints; k++)
      {
        //ERSizes contains the undeformed sizes 
        double dblScore = 0.0;        
        for (int m = 0; m < 4; m++)
        {
          double dblDiff = RSizes[m] - ERSizes[m]*dblCoef;
          dblScore += dblDiff*dblDiff;
        }

        if (dblScore < dblBestScore)
        {
          //we found a better solution
          dblBestScore = dblScore;
          for (int m = 0; m < 4; m++){
            dblBestERSizes[m] = ERSizes[m];
          }
           
          nBestDivision[0] = i; 
          nBestDivision[1] = j; 
          nBestDivision[2] = k;
        } //end  if (dblScore < dblBestScore)

        ERSizes[3] -= pELens[k]; //in the last round, this will be 0
        ERSizes[2] += pELens[k]; //in the last round this will be all
      } //end for k

      ERSizes[1] += pELens[j];
      ERSizes[2] -= pELens[j];
      ERSizes[3] = ERSizes[2] - pELens[j + 1];
      ERSizes[2] = pELens[j + 1];
      //in the last round, 0 is of unknown size, 1 is large, 2 contains last edge and 3 is zero
    }
    
    ERSizes[0] += pELens[i];
    ERSizes[1] = pELens[i + 1];
    ERSizes[2] = pELens[i + 2];    
    ERSizes[3] = dblCLen - (ERSizes[0] + ERSizes[1] + ERSizes[2]);
  }
    
  //we have detected best division strategy, now do it
  for (int i = 0; i < 3; i++)
  {
    pOutRectPoints[0][i] = origin[i];
    pOutRectPoints[nBestDivision[0]][i] = origin[i] + u[i];
    pOutRectPoints[nBestDivision[1]][i] = origin[i] + u[i] + v[i];
    pOutRectPoints[nBestDivision[2]][i] = origin[i] + v[i];
  }

  int nIndex = 1;
  int iStartPos = 0;
  for (int i = 0; i < 4; i++)
  {
    int iEndPos = nBestDivision[i];
    double dblCoef = RSizes[i] / dblBestERSizes[i];

    for (int j = iStartPos; j < iEndPos - 1; j++, nIndex++)
    {
      double dblFact = dblCoef*pELens[j];
      for (int k = 0; k < 3; k++)
      {
        pOutRectPoints[nIndex][k] = 
          pOutRectPoints[nIndex - 1][k] + Verts[i][k]*dblFact;
      }
    }

    iStartPos = iEndPos;
    nIndex++;
  }

  delete[] pELens;

  //_RPT0(_CRT_WARN, "====== RECTANGLE - REL. COORDS ======\n");
  //_RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n", 0.0, 0.0, 0.0);
  //_RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n", u[0], u[1], v[2]);
  //_RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n", u[0] + v[0], u[1] + v[1], v[2] + v[2]);
  //_RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n", v[0], v[1], v[2]);

  //_RPT0(_CRT_WARN, "====== DIVIDED RECTANGLE - REL. COORDS ======\n");
  //for (int i = 0; i < nPoints; i++)
  //{
  //  _RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n",
  //    pOutRectPoints[i][0] - origin[0],  
  //    pOutRectPoints[i][1] - origin[1],
  //    pOutRectPoints[i][2] - origin[2]);
  //}
}

//------------------------------------------------------------------------
//Computes new locations of given points (pPoints)lying inside the
//template polygon (pPolyTemplate) within the other polygon
//(pPolyTarget). Both polygons have the same number of edges. New
//coordinates are stored in pPoints buffer.
void vtkMAFMuscleDecomposition::MapPoints(VCoord* pPoints, int nPoints, 
    VCoord* pPolyTemplate, VCoord* pPolyTarget, int nPolyPoints)
//------------------------------------------------------------------------
{
  const static double eps_zero = 1e-8;
/*

  _RPT0(_CRT_WARN, "====== TEMPLATE POLYGON ======\n");
  for (int i = 0; i < nPolyPoints; i++)
  {
    _RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n",
      pPolyTemplate[i][0],  pPolyTemplate[i][1], pPolyTemplate[i][2]);
  }

  _RPT0(_CRT_WARN, "====== TARGET POLYGON ======\n");
  for (int i = 0; i < nPolyPoints; i++)
  {
    _RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n",
      pPolyTarget[i][0],  pPolyTarget[i][1], pPolyTarget[i][2]);
  }

  _RPT0(_CRT_WARN, "====== POINTS TO BE MAPPED ======\n");
  for (int i = 0; i < nPoints; i++)
  {
  _RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n",
  pPoints[i][0],  pPoints[i][1], pPoints[i][2]);
  }
*/
  
  VCoord* s_i = new VCoord[nPolyPoints];   //hold vectors vi, v (vi = points on boundary, v = inner point to map)
  double* r_i = new double[nPolyPoints];   //lengths of s_i vectors
  double* D_i = new double[nPolyPoints];   //dot products of s_i vectors
  double* A_i = new double[nPolyPoints];   //area of i-th triangle  

  //for every inner point
  for (int j = 0; j < nPoints; j++)
  {
    //compute mean value coordinates of the current point (v) within the source polygon 
    //see: K. Hormann, MS. Floater: Mean Value Coordinates for Arbitrary Planar Polygons.
    //http://folk.uio.no/michaelf/papers/barycentric.pdf

    //first, compute s_i, r_i
    for (int i = 0; i < nPolyPoints; i++)
    {
      for (int k = 0; k < 3; k++){
        s_i[i][k] = pPolyTemplate[i][k] - pPoints[j][k];
      }

      r_i[i] = vtkMath::Norm(s_i[i]);
      if (r_i[i] <= eps_zero) //if r_i is zero
      {
        //v = vi => this will be mapped to target point directly
        for (int k = 0; k < 3; k++){
          pPoints[j][k] = pPolyTarget[i][k];          
        }

        goto next_point;  //and proceed with the next point
      }
    }

    //next, compute D_i and A_i
    for (int i = 0; i < nPolyPoints; i++)
    {
      int i_plus = (i + 1) % nPolyPoints;
      D_i[i] = vtkMath::Dot(s_i[i], s_i[i_plus]);         
      A_i[i] = fabs(0.5*(
        (s_i[i][1] - s_i[i][0])*(s_i[i_plus][2] - s_i[i_plus][0]) -
        (s_i[i][2] - s_i[i][0])*(s_i[i_plus][1] - s_i[i_plus][0])
        ));   //it may happen that area is negative; this is caused by 
              //a) points do not lie on a common plane - see the tolerance in ExecuteData
              //b) numeric reasons

      //if s_i and s_i+ does not form a triangle (colinear)
      //v lies on line supported by the edge vi, vi+1
      //if, moreover, D_i is negative, the angle is > 90 degrees
      //which logically implies that the point lies on the edge      
      if (A_i[i] <= eps_zero && D_i[i] < 0.0)
      {
        //v lies on an edge of polygon
        double dblDen = r_i[i] + r_i[i_plus];
        for (int k = 0; k < 3; k++){
          pPoints[j][k] = (r_i[i]*pPolyTarget[i][k] + r_i[i_plus]*pPolyTarget[i_plus][k]) / dblDen;          
        }

        goto next_point;
      }
    } //end for i

    //point is in a general position, we will blend it
    pPoints[j][0] = pPoints[j][1] = pPoints[j][2] = 0.0;

    double dblWTotal = 0.0;    
    for (int i = 0; i < nPolyPoints; i++)
    {
      int i_plus = (i + 1) % nPolyPoints;
      int i_minus = (i + nPolyPoints - 1) % nPolyPoints;

      double w;
      if (fabs(A_i[i_minus]) <= eps_zero)
        w = 0.0;
      else
        w = (r_i[i_minus] - D_i[i_minus]/r_i[i]) / A_i[i_minus];

      if (fabs(A_i[i]) > eps_zero)
        w += (r_i[i_plus] - D_i[i]/r_i[i]) / A_i[i];

      for (int k = 0; k < 3; k++){
        pPoints[j][k] += w*pPolyTarget[i][k];
      }

      dblWTotal += w;
    }

    for (int k = 0; k < 3; k++){
      pPoints[j][k] /= dblWTotal;
    }

next_point:
    ;
  } //end for j (every point)

  delete[] A_i;
  delete[] D_i;
  delete[] r_i;
  delete[] s_i;

/*
  _RPT0(_CRT_WARN, "====== MAPPED POINTS ======\n");
  for (int i = 0; i < nPoints; i++)
  {
    _RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n",
      pPoints[i][0],  pPoints[i][1], pPoints[i][2]);
  }
  */
}

//------------------------------------------------------------------------
//Smooth the fiber defined by the given points.
void vtkMAFMuscleDecomposition::SmoothFiber(VCoord* pPoints, int nPoints)
//------------------------------------------------------------------------
{
  if (nPoints <= 3)
    return; //cannot smooth

  double dblTotalW = this->SmoothFactor + 2;
  for (int i = 0; i < SmoothSteps; i++)
  { 
    double x[3];  //buffer for one point
    for (int k = 0; k < 3; k++){
      x[k] = pPoints[0][k];
    }

    //for every inner point Pj of the curve, we set its coordinates into:
    //Pj' = 1/6*(Pj-1 + 4*Pj + Pj+1)  -- see Coons curve
    for (int j = 1; j < nPoints - 1; j++)
    {
      //j+1 is at iteration i-1
      //j-1 is at iteration i-1 and its iteration i is in x and should be saved now
      //j is at iteration i-1 and its iteration should be stored in x
      for (int k = 0; k < 3; k++)
      {
        double dblTmp = x[k];
        x[k] = (pPoints[j - 1][k] + this->SmoothFactor*pPoints[j][k] + 
          pPoints[j + 1][k]) / dblTotalW;
        pPoints[j - 1][k] = dblTmp;
      }
    } //end for points
  } //end for SmoothSteps
}


//------------------------------------------------------------------------
//By default, UpdateInformation calls this method to copy information
//unmodified from the input to the output.
/*virtual*/void vtkMAFMuscleDecomposition::ExecuteInformation()
//------------------------------------------------------------------------
{
  //check input
  vtkPolyData* input = GetInput();
  if (input == NULL)
  {
    vtkErrorMacro(<< "Invalid input for vtkMAFPolyDataDeformation.");
    return;   //we have no input
  }

  //check output
  vtkPolyData* output = GetOutput();
  if (output == NULL)
    SetOutput(vtkPolyData::New());

  if (this->FibersTemplate == NULL)
    this->FibersTemplate = vtkMAFPennateMuscleFibers::New();  //default is a pennate muscle

  //copy input to output
  Superclass::ExecuteInformation();  
}


//#define _DEBUG_CREATE_CONTOURS
//------------------------------------------------------------------------
//This method is the one that should be used by subclasses, right now the 
//default implementation is to call the backwards compatibility method
/*virtual*/void vtkMAFMuscleDecomposition::ExecuteData(vtkDataObject *output)
{
#pragma region Input Checks
  //check whether output is valid
  vtkPolyData* input = GetInput();
  if (input == NULL)
    return;

  vtkPolyData* pPoly = vtkPolyData::SafeDownCast(output);
  if (pPoly == NULL)
  {
    vtkWarningMacro(<< "Invalid output for vtkMAFMuscleDecomposition.");
    return;   //we have no valid output
  }  

  if (NumberOfFibres <= 0 || Resolution <= 0) {
    vtkWarningMacro(<< "NumberOfFibres or Resolution is invalid.");
    return;
  }
#pragma endregion Input Checks

#pragma region Template Cube and Target Cube Construction
 //we now will create fiber points in the template cube (1x1x1)
  int nFVerts = (Resolution + 1)*NumberOfFibres;
  VCoord* pFVerts = new VCoord[nFVerts];  

  double* r_s = new double[2*NumberOfFibres]; 
  CreateSobolPoints(NumberOfFibres, r_s);

  int nIndex = 0;
  double delta_t = 1.0 / Resolution;
  for (int i = 0; i < NumberOfFibres; i++)
  {
    double r = r_s[2*i];
    double s = r_s[2*i + 1];
    for (int j = 0; j <= Resolution; j++, nIndex++) 
    { 
      double x[3];
      FibersTemplate->GetPoint(r, s, j*delta_t, x);

      //fibers templates use different coordinate system (they use left-handed)
      //whilst the rest of our application uses right-handed
      pFVerts[nIndex][0] = x[0];
      pFVerts[nIndex][1] = x[2];
      pFVerts[nIndex][2] = x[1];
    }
  }

  delete[] r_s;   //no longer needed

  //we need to find minimal oriented box that fits the input point data
  //so that all points are inside of this box (or on its boundary)
  //and the total squared distance of template origin points from
  //the input mesh origin points and the total squared distance of 
  //template insertion points from the input mesh origin points
  //are minimized
  LOCAL_FRAME lf, lfNorm;
  ComputeFittingOB(input->GetPoints(), lf);

  //we are going to transform the template cube (1x1x1) into the target cube using
  //the local frame lf and by "projection" into the muscle volume, the
  //projection will be done in the plane perpendicular to the longest
	//direction => get this direction
  lfNorm = lf;
  int iPlane = 0;
  double dblAxisLength = vtkMath::Normalize(lfNorm.uvw[0]);
  for (int i = 1; i < 3; i++)
  {
    double dblTmp = vtkMath::Normalize(lfNorm.uvw[i]);
    if (dblTmp > dblAxisLength)
    {
      dblAxisLength = dblTmp;
      iPlane = i;
    }
  }

  int iPlane2 = (iPlane + 2) % 3; //fiber templates use different coordinate system
  int iPlane3 = (iPlane + 1) % 3;
      
  //lfNorm now contains the local frame with normalized axis
  //and iPlane is the index of longest axis
#pragma endregion Template Cube and Target Cube Construction
  
#pragma region Computation of Target Fibres by Projection
  //sort points according to their coordinate in this axis, which will help
  //us to reduce the number of required cuts (slow) by exploiting the
  //coherence of the data
  int* pOrder = SortPoints(pFVerts, nFVerts, iPlane);  
  
  //construct mesh cutter
  vtkPlane* cutPlane = vtkPlane::New();
  cutPlane->SetNormal(lfNorm.uvw[iPlane]);

  vtkCutter* cutter = vtkCutter::New();
  cutter->SetCutFunction(cutPlane);
  cutter->SetInput(input);  

#ifdef _DEBUG_CREATE_CONTOURS
  vtkPoints* pPoints = vtkPoints::New();
  vtkCellArray* pCells = vtkCellArray::New();
#endif
  
  
  VCoord* pTrPoints = new VCoord[nFVerts];  //to avoid allocation and deallocation inside the loop
  
  int nInvalidPoints = 0;
  int iStartPos = 0;
  while (iStartPos < nFVerts)
  {
    //tolerance - points within this tolerance uses the same cutting plane
    const static double dblTolerance = 0.005;

    double dblAvgCoord = pFVerts[pOrder[iStartPos]][iPlane];
    
    int iEndPos = iStartPos + 1;  //exclusive
    while (iEndPos < nFVerts &&
      pFVerts[pOrder[iStartPos]][iPlane] + dblTolerance > pFVerts[pOrder[iEndPos]][iPlane])
    {
      dblAvgCoord +=  pFVerts[pOrder[iEndPos]][iPlane];
      iEndPos++;
    }

    dblAvgCoord /= (iEndPos - iStartPos);

    //all points at iStartPos - iEndPos-1 will be projected using the same
    //plane (constructed in their average); find the origin of the
    //rectangle created by the intersection of target cube and plane with
    //normal lf.uvw[iPlane] going through transformed dblAvgCoord
    double origin[3];
    for (int j = 0; j < 3; j++){
      origin[j] = lf.O[j] + lf.uvw[iPlane][j]*dblAvgCoord;
    }

    cutPlane->SetOrigin(origin);
    cutter->Update();   //cut the mesh by the plane => we should have a contour

#pragma region Projection
    vtkPolyData* contour = vtkPolyData::SafeDownCast(cutter->GetOutput());            
    int nPoints = contour->GetNumberOfPoints();
    if (nPoints == 0)
    {
      //due to some numeric problems, there is no intersection
      //mark every point as invalid
      for (int i = iStartPos; i < iEndPos; i++)
      {
        pFVerts[pOrder[i]][iPlane] = DBL_MAX; //invalid
        nInvalidPoints++;
      }
    }
    else if (nPoints == 1)
    {
      //singular case - to be handled separately
      const double* pcoords = contour->GetPoint(0);
      for (int i = iStartPos; i < iEndPos; i++)
      {
        for (int j = 0; j < 3; j++){
          pFVerts[pOrder[i]][j] = pcoords[j];
        }        
      }
    }
    else
    { 
      //we have a general (expected) case      
      
      //find the point on the contour that is the closest to our origin
      //from this point we will do our mapping
      VCoord* pTargetPolyBuf = new VCoord[2*nPoints];  //twice because of mapping
      VCoord* pTargetPoly = pTargetPolyBuf;
      
      //unfortunately, points on the contour are not ordered
      //so we will need to do this first
      int* pEdgesOrder = GetSortedEdges(contour);     

      int iMapStartPos = 0;
      double dblMinDist = DBL_MAX;
      for (int j = 0; j < nPoints; j++)
      {
        contour->GetPoint(pEdgesOrder[2*j], pTargetPoly[j]);
        double dblDist = vtkMath::Distance2BetweenPoints(origin, pTargetPoly[j]);
        if (dblDist < dblMinDist)
        {
          iMapStartPos = j;
          dblMinDist = dblDist;
        }        
      }

      //we need to shift pTargetPoly so, the closest point is the first one
      if (iMapStartPos != 0)
      {        
        memcpy(pTargetPoly + nPoints, pTargetPoly, iMapStartPos*sizeof(VCoord));
        pTargetPoly += iMapStartPos;
      }

      delete[] pEdgesOrder; //no longer needed      

        
      //_RPT0(_CRT_WARN, "====== CONTOUR - REL. COORDS ======\n");
      //for (int i = 0; i < nPoints; i++)
      //{
      //  _RPT3(_CRT_WARN, "%.2f,%.2f,%.2f\n",
      //    pTargetPoly[(i + iMapStartPos) % nPoints][0] - origin[0],  
      //    pTargetPoly[(i + iMapStartPos) % nPoints][1] - origin[1],
      //    pTargetPoly[(i + iMapStartPos) % nPoints][2] - origin[2]);        
      //}

#ifdef _DEBUG_CREATE_CONTOURS
      //we save contours instead of fibers
      int nShift = pPoints->GetNumberOfPoints();
      for (int i = 0; i < nPoints; i++){
        pPoints->InsertNextPoint(pTargetPoly[(i + iMapStartPos) % nPoints]);
      }

      vtkIdType ptIds[2] = {nShift, nShift + 1};
      for (int i = 1; i < nPoints; i++)
      {
        pCells->InsertNextCell(2, ptIds);
        ptIds[0]++; ptIds[1]++;
      }
#endif

      //TODO: handle singular case when target has 2 or 3 points only

            
      //divide the rectangle into nPoints segments         
      VCoord* pTemplatePoly = new VCoord[nPoints];
      DivideRectangle(origin, lf.uvw[iPlane2], lf.uvw[iPlane3],
        nPoints, pTargetPoly, pTemplatePoly);
                              

      //so we can now perform "warping" of points from template cube      
      for (int i = iStartPos, nIndex = 0; i < iEndPos; i++, nIndex++)
      {
        //transformation of point x (onto target cube)                
        for (int k = 0; k < 3; k++){
          pTrPoints[nIndex][k] = lf.O[k] + 
            lf.uvw[0][k]*pFVerts[pOrder[i]][0] + 
            lf.uvw[1][k]*pFVerts[pOrder[i]][1] + 
            lf.uvw[2][k]*pFVerts[pOrder[i]][2];
        }
      } //end for i (points)

      if (DebugOutputMode == 0)
      {
        MapPoints(pTrPoints, iEndPos - iStartPos, 
          pTemplatePoly, pTargetPoly, nPoints);      
      }

      //store results
      for (int i = iStartPos, nIndex = 0; i < iEndPos; i++, nIndex++)
      {
        for (int j = 0; j < 3; j++){
          pFVerts[pOrder[i]][j] = pTrPoints[nIndex][j];
        }
      }
      
      delete[] pTargetPolyBuf;
      delete[] pTemplatePoly;      
    } // end else (nPoints > 1)
#pragma endregion Projection

    iStartPos = iEndPos;
  } //end while

  delete[] pTrPoints;
  delete[] pOrder;  //no longer needed

  cutter->Delete();
  cutPlane->Delete();
#pragma endregion Computation of Target Fibres by Projection

#pragma region Cleaning Fibres
  //we are going to remove all INF coordinates  
  for (int i = 0, nIndex = 0; i < NumberOfFibres; i++)
  {
    //starting at nIndex and ending at nIndex + Resolution
    //are points for one curve, all INF points are to be moved at the end

    int nNextValidPoint = nIndex;
    int nLastPoint = nIndex + Resolution;
    while (nIndex <= nLastPoint)
    {
      if (pFVerts[nIndex][iPlane] != DBL_MAX)
      {
        if (nIndex != nNextValidPoint)
        {
          //there are some invalid points prior to this one, move this to that place
          for (int k = 0; k < 3; k++){
            pFVerts[nNextValidPoint][k] = pFVerts[nIndex][k];
          }
        }

        nNextValidPoint++;
      }

      nIndex++;
    }

    //now invalidate every point after valid points
    while (nNextValidPoint <= nLastPoint) 
    {
      pFVerts[nNextValidPoint][iPlane] = DBL_MAX;
      nNextValidPoint++;
    }   
  } //end for i

#pragma endregion Cleaning Fibres
  
#pragma region Saving the Target Fibres into Output PolyData
  //save the result 
#ifndef _DEBUG_CREATE_CONTOURS  
  vtkPoints* pPoints = vtkPoints::New();
  pPoints->SetNumberOfPoints(nFVerts - nInvalidPoints);    

  vtkCellArray* pCells = vtkCellArray::New();  
  vtkIdType* pIds = new vtkIdType[Resolution + 1];

  nIndex = 0;
  int nValidPtIndex = 0;    
  for (int i = 0; i < NumberOfFibres; i++)
  {
    //get number of points in the current fibre
    int nValidPoints = 0;
    while (nValidPoints <= Resolution && 
      pFVerts[nIndex + nValidPoints][iPlane] != DBL_MAX) {
        nValidPoints++;
    }

    if (this->SmoothFibers != 0){
      SmoothFiber(&pFVerts[nIndex], nValidPoints);
    }

    for (int j = 0; j < nValidPoints; j++, nIndex++)
    {         
      pPoints->SetPoint(nValidPtIndex, pFVerts[nIndex]);
      pIds[j] = nValidPtIndex++;        
    } //end for j
    
    pCells->InsertNextCell(nValidPoints, pIds);
    nIndex += (Resolution + 1 - nValidPoints);  //advance to the next
  } //end for i
  
  delete[] pIds;    
#endif

  pPoly->SetLines(pCells);
  pPoly->SetPoints(pPoints);

  pCells->Delete();
  pPoints->Delete();

  delete[] pFVerts;
#pragma endregion Saving the Target Fibres into Output PolyData
} //end: void vtkMAFMuscleDecomposition::ExecuteData(vtkDataObject *output)

//Routines related to matching template cube and input data bounding box
#pragma region Matching
//------------------------------------------------------------------------
//Computes the principal axis for the given point set.
//N.B. the direction is normalized
void vtkMAFMuscleDecomposition::ComputeAxisLine(vtkPoints* points, 
                                                double* origin, double* direction)
//------------------------------------------------------------------------
{
  //the line goes through the centroid
  int N = points->GetNumberOfPoints();
  origin[0] = origin[1] = origin[2] = 0.0;
  for (int i = 0; i < N; i++)
  {
    const double* pcoords = points->GetPoint(i);
    for (int j = 0; j < 3; j++){
      origin[j] += pcoords[j];
    }
  }

  for (int j = 0; j < 3; j++){
    origin[j] /= N;
  }
  
  //compute the covariance matrix, which is a symmetric matrix
  double A[3][3], eigenvals[3], eigenvects[3][3];
    
  //fill the matrix
  memset(A, 0, sizeof(A));
  for (int k = 0; k < N; k++)
  {
    const double* pcoords = points->GetPoint(k);
    
    for (int i = 0; i < 3; i++) 
    {
      for (int j = 0; j < 3; j++) {        
        A[i][j] += (pcoords[i] - origin[i])*(pcoords[j] - origin[j]);
      }
    }
  }

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++){
      A[i][j] /= (N - 1);
    }
  }  

  //compute eigen vectors, the principal axis is the first one
  double *ATemp[3], *V[3];
  for (int i = 0; i < 3; i++)
  {
    ATemp[i] = A[i];
    V[i] = eigenvects[i];
  }
  
  vtkMath::Jacobi(ATemp, eigenvals, V);

  //copy the result
  //N.B. Jacobi returns vectors in columns!
  for (int i = 0; i < 3; i++) {    
    direction[i] = eigenvects[i][0];
  }
}


  
//------------------------------------------------------------------------
//Computes 4*nFrames vectors by rotating u around r vector. All vectors are
//normalized and stored in the order A, B, C, D where B is the vector
//opposite to A, C is vector perpendicular to A and r and D is vector
//opposite to C. N.B. vectors u and r must be normalized and perpendicular!
//The buffer pVectors must be capable enough to hold all vectors.
void vtkMAFMuscleDecomposition::ComputeDirectionVectors(
  double* u, double* r, int nFrames, VCoord* pVectors)
//------------------------------------------------------------------------
{   
  //the matrix for the rotation can be computed by procedure given in
  //Bloomenthal J. Calculation of reference frames along a
  //space curve. Graphics Gems I, 1990, 567–571.
  //http://www.unchainedgeometry.com/jbloom/pdf/ref-frames.pdf
  //or also by Schneider: Geometric Tools for Computer Games  
  double cos_delta = cos(M_PI_2 / (nFrames + 1));
  double sin_delta = sqrt(1 - cos_delta*cos_delta);
  double cos_theta = 1.0;

  double sqx = r[0]*r[0];
  double sqy = r[1]*r[1];
  double sqz = r[2]*r[2];
  
  double M[3][3]; //rotation matrix
  for (int i = 0, index = 0; i < nFrames; i++)
  {
    double sin_w = sqrt(1.0 - cos_theta*cos_theta);
    double cos_w1 = 1.0 - cos_theta;

    double xycos1 = r[0]*r[1]*cos_w1;
    double yzcos1 = r[1]*r[2]*cos_w1;
    double zxcos1 = r[2]*r[0]*cos_w1;

    double xsin = r[0]*sin_w;
    double ysin = r[1]*sin_w;
    double zsin = r[2]*sin_w;

    M[0][0] = sqx + (1.0 - sqx)*cos_theta;
    M[1][0] = xycos1 + zsin;
    M[2][0] = zxcos1 - ysin;

    M[0][1] = xycos1 - zsin;
    M[1][1] = sqy + (1.0 - sqy)*cos_theta;
    M[2][1] = yzcos1 + xsin;

    M[0][2] = zxcos1 + ysin;
    M[1][2] = yzcos1 - xsin;
    M[2][2] = sqz + (1 - sqz)*cos_theta;

    //by multiplying vector u by the matrix we get the new vector
    for (int j = 0; j < 3; j++) 
    { 
      pVectors[index][j] = 0.0;
      for (int k = 0; k < 3; k++) {
        pVectors[index][j] += u[k]*M[k][j];        
      }
    }

    //get the other axis
    vtkMath::Cross(pVectors[index], r, pVectors[index + 2]);
    vtkMath::Normalize(pVectors[index + 2]);

    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 3; k++){
        pVectors[index + 1][k] = -pVectors[index][k];
      }

      index += 2;
    }

    //in the next loop we will need to create LFS for angle theta + delta
    //cos(theta + delta) = cos(theta)*cos(delta)-sin(theta)*sin(delta)
    cos_theta = cos_theta*cos_delta - sin_w*sin_delta;
  }  
}

//------------------------------------------------------------------------
//Adjusts the length of direction vectors (in pVects) to fit the given point set. 
//For each direction vector, the algorighm find a plane defined by the center 
//(it should be the centroid of points) and a normal of non-unit size that is 
//collinear with the input direction vector. This normal is chosen so that the 
//no point lies in the positive halfspace of the plane (i.e. in the direction 
//of normal from the plane). The computed normals are returned in pVects.
//N.B. the input vectors must be of unit size!
void vtkMAFMuscleDecomposition::FitDirectionVectorsToData(vtkPoints* points, 
                                  double* center, int nVects, VCoord* pVects)
//------------------------------------------------------------------------
{
  double* pLengths = new double[nVects];
  memset(pLengths, 0, nVects*sizeof(double));

  int N = points->GetNumberOfPoints();
  for (int i = 0; i < N; i++)
  {
    const double* pcoords = points->GetPoint(i);
    for (int j = 0; j < nVects; j++)
    {
      //compute dot product
      double dblDot = 0.0;
      for (int k = 0; k < 3; k++){
        dblDot += (pcoords[k] - center[k])*pVects[j][k];
      }
      
      //the point lies outside the area => enlarge the vector length
      if (dblDot > pLengths[j])
        pLengths[j] = dblDot;
    }
  }

  for (int i = 0; i < nVects; i++)
  {
    _ASSERT(pLengths[i] != 0.0);

    for (int j = 0; j < 3; j++){
      pVects[i][j] *= pLengths[i];
    }
  }

  delete[] pLengths;
}

//------------------------------------------------------------------------
//Computes local frame systems for various cubes defined by their center
//and two direction vectors in w and 4 direction vectors in u and v axis.
//Direction vectors in u and v are given in uv_dirs and have the structure
//compatible with the output of ComputeDirectionVectors method. 
//The computed LFs are stored in pLFS buffer. The buffer must be capable to
//hold 8*nCubes (= nCubes in ComputeDirectionVectors) entries.
//------------------------------------------------------------------------
void vtkMAFMuscleDecomposition::ComputeLFS(double* center, VCoord* w_dir, 
                int nCubes, VCoord* uv_dirs, LOCAL_FRAME* pLFS)
{
  double w01[3], cv0[3], cv1[3];  //temp vars
  for (int i = 0; i < 3; i++)
  {
    w01[i] = w_dir[0][i] - w_dir[1][i];
    cv0[i] = center[i] + w_dir[0][i];
    cv1[i] = center[i] + w_dir[1][i];
  }

  for (int i = 0, index = 0; i < 4*nCubes; i += 4, index += 8)
  {
    for (int j = 0; j < 3; j++)
    {
      pLFS[index + 0].O[j] = cv1[j] + uv_dirs[i + 1][j] + uv_dirs[i + 3][j];
      pLFS[index + 1].O[j] = cv1[j] + uv_dirs[i + 0][j] + uv_dirs[i + 3][j];
      pLFS[index + 2].O[j] = cv1[j] + uv_dirs[i + 0][j] + uv_dirs[i + 2][j];
      pLFS[index + 3].O[j] = cv1[j] + uv_dirs[i + 1][j] + uv_dirs[i + 2][j];
      pLFS[index + 4].O[j] = cv0[j] + uv_dirs[i + 1][j] + uv_dirs[i + 3][j];
      pLFS[index + 5].O[j] = cv0[j] + uv_dirs[i + 0][j] + uv_dirs[i + 3][j];
      pLFS[index + 6].O[j] = cv0[j] + uv_dirs[i + 0][j] + uv_dirs[i + 2][j];
      pLFS[index + 7].O[j] = cv0[j] + uv_dirs[i + 1][j] + uv_dirs[i + 2][j];

      pLFS[index + 0].uvw[0][j] = pLFS[index + 3].uvw[1][j] = 
        pLFS[index + 4].uvw[1][j] = pLFS[index + 7].uvw[0][j] = uv_dirs[i + 0][j] - uv_dirs[i + 1][j];

      pLFS[index + 1].uvw[1][j] = pLFS[index + 2].uvw[0][j] = 
        pLFS[index + 5].uvw[0][j] = pLFS[index + 6].uvw[1][j] = -pLFS[index + 0].uvw[0][j];

      pLFS[index + 0].uvw[1][j] = pLFS[index + 1].uvw[0][j] = 
        pLFS[index + 4].uvw[0][j] = pLFS[index + 5].uvw[1][j] = uv_dirs[i + 2][j] - uv_dirs[i + 3][j];

      pLFS[index + 2].uvw[1][j] = pLFS[index + 3].uvw[0][j] = 
        pLFS[index + 6].uvw[0][j] = pLFS[index + 7].uvw[1][j] = -pLFS[index + 0].uvw[1][j];     

      for (int k = 0; k < 4; k++)
      {
        pLFS[index + k].uvw[2][j] = w01[j];
        pLFS[index + 4 + k].uvw[2][j] = -w01[j];
      }
    }
  }
}

//------------------------------------------------------------------------
//Finds the best local frame system from those passed in pLFS that best
//maps template origin and insertion points to target origin and insertion
//points. N.B. any point set can be NULL, if it is not needed. Special
//case is when both target sets or template sets are NULL, then the
//routine returns the first LF.
int vtkMAFMuscleDecomposition::FindBestMatch(vtkPoints* template_O, vtkPoints* template_I,
                  int nLFS, LOCAL_FRAME* pLFS, vtkPoints* target_O, vtkPoints* target_I)
//------------------------------------------------------------------------
{
  //check, if the input is valid for matching
  vtkPoints* tmpPts[4] = { template_O, template_I, target_O, target_I };   
  int nPoints[4];       //number of points in template_O, I, target_O, I  
  
  for (int i = 0; i < 4; i++)
  {    
    nPoints[i] = 0; 

    if (tmpPts[i] != NULL) {
      nPoints[i] = tmpPts[i]->GetNumberOfPoints();    
    }
  }

  //check, if the input is valid for matching
  if ((nPoints[0] == 0 || nPoints[2] == 0) &&
    (nPoints[1] == 0 || nPoints[3] == 0))  
    return 0;

  //we have at least one point to match
  //extract coordinates
  VCoord* pPoints[4];
  for (int i = 0; i < 4; i++)
  {
    pPoints[i] = NULL;    
    if (nPoints[i] != 0) 
    {
      pPoints[i] = new VCoord[nPoints[i]];
      for (int j = 0; j < nPoints[i]; j++){
        tmpPts[i]->GetPoint(j, pPoints[i][j]);
      } 
    }
  }

  //Algorithm:
  //Every point from template_X is transformed using the current LF
  //and the final Euclidian coordinates are compared to coordinates
  //of points from target_X in order to find the closest target point.

  //Distances between points from template_X and their closest counterparts
  //from target_X are sorted and M first smallest distances are summed to
  //get the matching score. As our fiber templates have no more than 10 points,
  //M = all points.The LF with the lowest score is chosen as the best.
      
  int iBestLF = 0;
  double dblBestLFScore = DBL_MAX;   //infinite

  for (int iSys = 0; iSys < nLFS; iSys++)
  {
    double dblScore = 0.0;
    for (int i = 0; i < 2; i++)
    {
      if (nPoints[i] == 0 || nPoints[i + 2] == 0)
        continue; //invalid matching, skip it

      for (int j = 0; j < nPoints[i]; j++)
      {        
        //transform point j
        //NOTE: fiber templates use left hand oriented coordinate system
        double x[3];        
        for (int k = 0; k < 3; k++){
          x[k] = pLFS[iSys].O[k] + pLFS[iSys].uvw[0][k]*pPoints[i][j][0] +
            pLFS[iSys].uvw[1][k]*pPoints[i][j][2] + pLFS[iSys].uvw[2][k]*pPoints[i][j][1];
        }

        //find the closest point
        //brute-force as the number of points should be lower than 10
        double dblMinDist = DBL_MAX;
        for (int m = 0; m < nPoints[i + 2]; m++)
        {
          double dblDist = vtkMath::Distance2BetweenPoints(x, pPoints[i + 2][m]);
          if (dblDist < dblMinDist)
            dblMinDist = dblDist;
        }

        dblScore += sqrt(dblMinDist);
      } //end for j (points in template)      
    } //end for i (templates)
    
    if (dblScore < dblBestLFScore)
    {
      dblBestLFScore = dblScore;
      iBestLF = iSys;
    }    
  } //end for iSys

  for (int i = 0; i < 4; i++) {
    delete[] pPoints[i];
  }
  return iBestLF;
}


//------------------------------------------------------------------------
//Computes the minimal oriented box that fits the input data so that all
//points are inside of this box (or on its boundary) and the total squared
//distance of template origin points from the input mesh origin points and
//the total squared distance of template insertion points from the input
//mesh origin points are minimized
//------------------------------------------------------------------------
void vtkMAFMuscleDecomposition::ComputeFittingOB(vtkPoints* points, LOCAL_FRAME& out_lf)
{      
  //get the principal axis  
  double l_pos[3], l_dir[3];  
  ComputeAxisLine(points, l_pos, l_dir);
  
  //compute initial vectors representing initial axis u (perpendicular to
  //l_dir) u is a projection into one of of XZ, XY or YZ plane + 90 degrees
  //rotation the optimal plane is the one closest to the plane where u lies
  int iPlane = 0;
  for (int i = 1; i < 3; i++) {
    if (fabs(l_dir[i]) < fabs(l_dir[iPlane]))
      iPlane = i; //new minimum
  }

  double u[3];
  int i1 = (iPlane + 1) % 3;
  int i2 = (iPlane + 2) % 3;
  u[i1] = l_dir[i2];
  u[i2] = -l_dir[i1];
  u[iPlane] = 0.0;

  vtkMath::Normalize(u);
  
  //create various systems by rotating them around w axis by small angle (5 degrees)    
  const int nCubes = 10;       //number of frames to be created in Pi/4 area
  int nVects = (4*nCubes + 2); //total number of vectors
  
  VCoord* pVects = new VCoord[nVects];
  for (int i = 0; i < 3; i++)
  {
    pVects[0][i] = l_dir[i];
    pVects[1][i] = -l_dir[i];
  }

  ComputeDirectionVectors(u, l_dir, nCubes, &pVects[2]);
    
  //for every direction, we now need to find a plane such that
  //all input points are in the negative half-space
  //the routine shorten (or prolong) input vectors so they 
  //define the data bounding object
  FitDirectionVectorsToData(points, l_pos, nVects, pVects);

  //it is possible to construct cubes by linear combination
  //of the origin l_pos and vectors pVects[0], pVects[1], 
  //pVects[i], pVects[i + 1], pVects[i + 2] and pVects[i + 3]
  //we will construct LFS    
  LOCAL_FRAME* pLFS = new LOCAL_FRAME[8*nCubes];
  ComputeLFS(l_pos, pVects, nCubes, &pVects[2], pLFS);
  delete[] pVects;  //no longer needed
  
  //find the best system for the input data
  vtkPoints* tempO = vtkPoints::New();
  vtkPoints* tempI = vtkPoints::New();
  FibersTemplate->GetOriginLandmarks(tempO);
  FibersTemplate->GetInsertionLandmarks(tempI);

  out_lf = pLFS[FindBestMatch(tempO, tempI, 
    8*nCubes, pLFS, OriginArea, InsertionArea)];
    
  tempI->Delete();
  tempO->Delete();  
  delete[] pLFS;
  //and we have it here 
}
#pragma endregion 