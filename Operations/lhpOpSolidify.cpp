/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpSolidify.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
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

#include "lhpOpSolidify.h"
#include <wx/busyinfo.h>

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"


#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"

#include "vtkPolyData.h"
#include "vtkMEDPolyDataMirror.h"
#include "mafVectors.h"
#include <vector>


#include "mafVMESurface.h"
#include "mafVMEOutputSurface.h"
#include "mafTransformBase.h"

#include "vtkMAFSmartPointer.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"

#define SOL_ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
        a[k][l]=h+s*(g-h*tau)

#define SOL_MAX_ROTATIONS 20

//#undef VTK_MAX_ROTATIONS

//#define VTK_MAX_ROTATIONS 50

// Jacobi iteration for the solution of eigenvectors/eigenvalues of a nxn
// real symmetric matrix. Square nxn matrix a; size of matrix in n;
// output eigenvalues in w; and output eigenvectors in v. Resulting
// eigenvalues/vectors are sorted in decreasing order; eigenvectors are
// normalized.
template<class T>
int solJacobiN(T **a, int n, T *w, T **v)
{
  int i, j, k, iq, ip, numPos;
  T tresh, theta, tau, t, sm, s, h, g, c, tmp;
  T bspace[4], zspace[4];
  T *b = bspace;
  T *z = zspace;

  // only allocate memory if the matrix is large
  if (n > 4)
    {
    b = new T[n];
    z = new T[n]; 
    }

  // initialize
  for (ip=0; ip<n; ip++) 
    {
    for (iq=0; iq<n; iq++)
      {
      v[ip][iq] = 0.0;
      }
    v[ip][ip] = 1.0;
    }
  for (ip=0; ip<n; ip++) 
    {
    b[ip] = w[ip] = a[ip][ip];
    z[ip] = 0.0;
    }

  // begin rotation sequence
  for (i=0; i<SOL_MAX_ROTATIONS; i++) 
    {
    sm = 0.0;
    for (ip=0; ip<n-1; ip++) 
      {
      for (iq=ip+1; iq<n; iq++)
        {
        sm += fabs(a[ip][iq]);
        }
      }
    if (sm == 0.0)
      {
      break;
      }

    if (i < 3)                                // first 3 sweeps
      {
      tresh = 0.2*sm/(n*n);
      }
    else
      {
      tresh = 0.0;
      }

    for (ip=0; ip<n-1; ip++) 
      {
      for (iq=ip+1; iq<n; iq++) 
        {
        g = 100.0*fabs(a[ip][iq]);

        // after 4 sweeps
        if (i > 3 && (fabs(w[ip])+g) == fabs(w[ip])
        && (fabs(w[iq])+g) == fabs(w[iq]))
          {
          a[ip][iq] = 0.0;
          }
        else if (fabs(a[ip][iq]) > tresh) 
          {
          h = w[iq] - w[ip];
          if ( (fabs(h)+g) == fabs(h))
            {
            t = (a[ip][iq]) / h;
            }
          else 
            {
            theta = 0.5*h / (a[ip][iq]);
            t = 1.0 / (fabs(theta)+sqrt(1.0+theta*theta));
            if (theta < 0.0)
              {
              t = -t;
              }
            }
          c = 1.0 / sqrt(1+t*t);
          s = t*c;
          tau = s/(1.0+c);
          h = t*a[ip][iq];
          z[ip] -= h;
          z[iq] += h;
          w[ip] -= h;
          w[iq] += h;
          a[ip][iq]=0.0;

          // ip already shifted left by 1 unit
          for (j = 0;j <= ip-1;j++) 
            {
            SOL_ROTATE(a,j,ip,j,iq);
            }
          // ip and iq already shifted left by 1 unit
          for (j = ip+1;j <= iq-1;j++) 
            {
            SOL_ROTATE(a,ip,j,j,iq);
            }
          // iq already shifted left by 1 unit
          for (j=iq+1; j<n; j++) 
            {
            SOL_ROTATE(a,ip,j,iq,j);
            }
          for (j=0; j<n; j++) 
            {
            SOL_ROTATE(v,j,ip,j,iq);
            }
          }
        }
      }

    for (ip=0; ip<n; ip++) 
      {
      b[ip] += z[ip];
      w[ip] = b[ip];
      z[ip] = 0.0;
      }
    }

  //// this is NEVER called
  if ( i >= SOL_MAX_ROTATIONS )
    {
    /*vtkGenericWarningMacro(
       "vtkMath::Jacobi: Error extracting eigenfunctions");*/
    system("pause");
    return 0;
    }

  // sort eigenfunctions                 these changes do not affect accuracy 
  for (j=0; j<n-1; j++)                  // boundary incorrect
    {
    k = j;
    tmp = w[k];
    for (i=j+1; i<n; i++)                // boundary incorrect, shifted already
      {
      if (w[i] >= tmp)                   // why exchage if same?
        {
        k = i;
        tmp = w[k];
        }
      }
    if (k != j) 
      {
      w[k] = w[j];
      w[j] = tmp;
      for (i=0; i<n; i++) 
        {
        tmp = v[i][j];
        v[i][j] = v[i][k];
        v[i][k] = tmp;
        }
      }
    }
  // insure eigenvector consistency (i.e., Jacobi can compute vectors that
  // are negative of one another (.707,.707,0) and (-.707,-.707,0). This can
  // reek havoc in hyperstreamline/other stuff. We will select the most
  // positive eigenvector.
  int ceil_half_n = (n >> 1) + (n & 1);
  for (j=0; j<n; j++)
    {
    for (numPos=0, i=0; i<n; i++)
      {
      if ( v[i][j] >= 0.0 )
        {
        numPos++;
        }
      }
//    if ( numPos < ceil(double(n)/double(2.0)) )
    if ( numPos < ceil_half_n)
      {
      for(i=0; i<n; i++)
        {
        v[i][j] *= -1.0;
        }
      }
    }

  if (n > 4)
    {
    delete [] b;
    delete [] z;
    }
  return 1;
}

#undef SOL_ROTATE
#undef SOL_MAX_ROTATIONS

//----------------------------------------------------------------------------
int JacobiN(double **a, int n, double *w, double **v)
{
  return solJacobiN(a,n,w,v);
}

 template<class T>
void solSwapVectors3(T v1[3], T v2[3])
{
  for (int i = 0; i < 3; i++)
    {
    T tmp = v1[i];
    v1[i] = v2[i];
    v2[i] = tmp;
    }
}

  // Description:
  // LU Factorization of a 3x3 matrix.  The diagonal elements are the
  // multiplicative inverse of those in the standard LU factorization.
  template<class T>
void solLUFactor3x3(T A[3][3], int index[3])
{
  int i,maxI;
  T tmp,largest;
  T scale[3];

  // Loop over rows to get implicit scaling information

  for ( i = 0; i < 3; i++ ) 
    {
    largest =  fabs(A[i][0]);
    if ((tmp = fabs(A[i][1])) > largest)
      {
      largest = tmp;
      }
    if ((tmp = fabs(A[i][2])) > largest)
      {
      largest = tmp;
      }
    scale[i] = T(1.0)/largest;
    }
  
  // Loop over all columns using Crout's method

  // first column
  largest = scale[0]*fabs(A[0][0]);
  maxI = 0;
  if ((tmp = scale[1]*fabs(A[1][0])) >= largest) 
    {
    largest = tmp;
    maxI = 1;
    }
  if ((tmp = scale[2]*fabs(A[2][0])) >= largest) 
    {
    maxI = 2;
    }
  if (maxI != 0) 
    {
    solSwapVectors3(A[maxI],A[0]);
    scale[maxI] = scale[0];
    }
  index[0] = maxI;

  A[0][0] = T(1.0)/A[0][0];
  A[1][0] *= A[0][0];
  A[2][0] *= A[0][0];
    
  // second column
  A[1][1] -= A[1][0]*A[0][1];
  A[2][1] -= A[2][0]*A[0][1];
  largest = scale[1]*fabs(A[1][1]);
  maxI = 1;
  if ((tmp = scale[2]*fabs(A[2][1])) >= largest) 
    {
    maxI = 2;
    solSwapVectors3(A[2],A[1]);
    scale[2] = scale[1];
    }
  index[1] = maxI;
  A[1][1] = T(1.0)/A[1][1];
  A[2][1] *= A[1][1];

  // third column
  A[1][2] -= A[1][0]*A[0][2];
  A[2][2] -= A[2][0]*A[0][2] + A[2][1]*A[1][2];
  largest = scale[2]*fabs(A[2][2]);
  index[2] = 2;
  A[2][2] = T(1.0)/A[2][2];
}

void LUFactor3x3(double A[3][3], int index[3])
{
  solLUFactor3x3(A,index);
}

  // Description:
  // LU back substitution for a 3x3 matrix.  The diagonal elements are the
  // multiplicative inverse of those in the standard LU factorization.
 template<class T1, class T2>
inline void solLUSolve3x3(const T1 A[3][3], const int index[3], T2 x[3])
{
  T2 sum;

  // forward substitution
  
  sum = x[index[0]];
  x[index[0]] = x[0];
  x[0] = sum;

  sum = x[index[1]];
  x[index[1]] = x[1];
  x[1] = sum - A[1][0]*x[0];

  sum = x[index[2]];
  x[index[2]] = x[2];
  x[2] = sum - A[2][0]*x[0] - A[2][1]*x[1];

  // back substitution
  
  x[2] = x[2]*A[2][2];
  x[1] = (x[1] - A[1][2]*x[2])*A[1][1];
  x[0] = (x[0] - A[0][1]*x[1] - A[0][2]*x[2])*A[0][0];
}  


void LUSolve3x3(const double A[3][3], 
                         const int index[3], double x[3])
{
  solLUSolve3x3(A,index,x);
}



  // Description:
  // Invert a 3x3 matrix.
  template<class T1, class T2>
void solInvert3x3(const T1 A[3][3], T2 AI[3][3])
{
  int index[3];
  T2 tmp[3][3];

  for (int k = 0; k < 3; k++)
    {
    AI[k][0] = A[k][0];
    AI[k][1] = A[k][1];
    AI[k][2] = A[k][2];
    }
  // invert one column at a time
  LUFactor3x3(AI,index);
  for (int i = 0; i < 3; i++)
    {
    T2 *x = tmp[i];
    x[0] = x[1] = x[2] = 0.0;
    x[i] = 1.0;
    LUSolve3x3(AI,index,x);
    }
  for (int j = 0; j < 3; j++)
    {
    T2 *x = tmp[j];
    AI[0][j] = x[0];
    AI[1][j] = x[1];
    AI[2][j] = x[2];      
    }
}


void Invert3x3(const double A[3][3], double AI[3][3])
{
  solInvert3x3(A,AI);
}

template<class T, class T2, class T3>
void solMultiplyMatrix3x3(const T A[3][3], const T2 B[3][3],
                                        T3 C[3][3])
{
  T3 D[3][3];

  for (int i = 0; i < 3; i++)
    {
    D[0][i] = A[0][0]*B[0][i] + A[0][1]*B[1][i] + A[0][2]*B[2][i];
    D[1][i] = A[1][0]*B[0][i] + A[1][1]*B[1][i] + A[1][2]*B[2][i];
    D[2][i] = A[2][0]*B[0][i] + A[2][1]*B[1][i] + A[2][2]*B[2][i];
    }

  for (int j = 0; j < 3; j++)
    {
    C[j][0] = D[j][0];
    C[j][1] = D[j][1];
    C[j][2] = D[j][2];
    }
}

void Multiply3x3(const double A[3][3], 
                          const double B[3][3], double C[3][3])
{
  solMultiplyMatrix3x3(A,B,C);
}



void Perpendiculars(const double x[3], double y[3], double z[3],
                             double theta)
{
  int dx,dy,dz;
  double x2 = x[0]*x[0];
  double y2 = x[1]*x[1];
  double z2 = x[2]*x[2];
  double r = sqrt(x2 + y2 + z2);

  // transpose the vector to avoid divide-by-zero error
  if (x2 > y2 && x2 > z2)
  {
    dx = 0; dy = 1; dz = 2;
  }
  else if (y2 > z2) 
  {
    dx = 1; dy = 2; dz = 0;
  }
  else 
  {
    dx = 2; dy = 0; dz = 1;
  }

  double a = x[dx]/r;
  double b = x[dy]/r;
  double c = x[dz]/r;

  double tmp = sqrt(a*a+c*c);

  if (theta != 0)
    {
    double sintheta = sin(theta);
    double costheta = cos(theta);

    if (y)
    {
      y[dx] = (c*costheta - a*b*sintheta)/tmp;
      y[dy] = sintheta*tmp;
      y[dz] = (-a*costheta - b*c*sintheta)/tmp;
    }

    if (z)
      {
      z[dx] = (-c*sintheta - a*b*costheta)/tmp;
      z[dy] = costheta*tmp;
      z[dz] = (a*sintheta - b*c*costheta)/tmp;
      }
    }
  else
    {
    if (y)
    {
      y[dx] = c/tmp;
      y[dy] = 0;
      y[dz] = -a/tmp;
    }

    if (z)
      {
      z[dx] = -a*b/tmp;
      z[dy] = tmp;
      z[dz] = -b*c/tmp;
      }
    }      
}


double Residual(const std::vector<std::vector<double> >& result,const std::vector<V3d<double> >& sourcelandmarks,const std::vector<V3d<double> >& targetlandmarks,int nb_points)
{
  double **a, **b, **product, **bcalc, **diff, **diffsquare;
  double sum=0,dof,res;
  int i,j,k;


  a=(double**)malloc(4*sizeof(double*));
	for(i=0;i<4;i++)
        a[i]=(double*)malloc(nb_points*sizeof(double));

  b=(double**)malloc(3*sizeof(double*));
	for(i=0;i<3;i++)
        b[i]=(double*)malloc(nb_points*sizeof(double));

  product=(double**)malloc(4*sizeof(double*));
	for(i=0;i<4;i++)
        product[i]=(double*)malloc(nb_points*sizeof(double));

  bcalc=(double**)malloc(3*sizeof(double*));
	for(i=0;i<3;i++)
        bcalc[i]=(double*)malloc(nb_points*sizeof(double));

  diff=(double**)malloc(3*sizeof(double*));
	for(i=0;i<3;i++)
        diff[i]=(double*)malloc(nb_points*sizeof(double));

  diffsquare=(double**)malloc(3*sizeof(double*));
	for(i=0;i<3;i++)
        diffsquare[i]=(double*)malloc(nb_points*sizeof(double));

  
    for(i=0;i<nb_points;i++)
      { for(j=0;j<3;j++)
        {
          a[j][i]=sourcelandmarks[i].components[j];
          b[j][i]=targetlandmarks[i].components[j];
        }
          a[3][i]=1;
      }

    for(i=0;i<4;i++)
      for(j=0;j<nb_points;j++)
        {
          product[i][j]=0;
          for(k=0;k<4;k++)
            product[i][j] += result[i][k]*a[k][j];
          if(i != 3)
            bcalc[i][j]=product[i][j];
        }
    
    for(i=0;i<3;i++)
      for(j=0;j<nb_points;j++)
        {
          diff[i][j]=b[i][j]-bcalc[i][j];
          diffsquare[i][j]=diff[i][j]*diff[i][j];
          sum += diffsquare[i][j];
        }

      dof= 3*nb_points-6;
      res=sqrt(sum/dof);

      for(i=0;i<4;i++)
        {
          free(a[i]);
          a[i]=NULL;
          free(product[i]);
          product[i]=NULL;
        }
      free(a);
      a=NULL;
      free(product);
      product=NULL;
      for(i=0;i<3;i++)
        {
          free(b[i]);
          b[i]=NULL;
          free(bcalc[i]);
          bcalc[i]=NULL;
          free(diff[i]);
          diff[i]=NULL;
          free(diffsquare[i]);
          diffsquare[i]=NULL;
        }
      b=NULL;
      bcalc=NULL;
      diff=NULL;
      diffsquare=NULL;

      return res;



}
std::vector<std::vector<double> > Transformation_Matrix(const std::vector<V3d<double> >& sourcelandmarks,const std::vector<V3d<double> >& targetlandmarks,int nb_points,int type_mode)
{
int i,j;
//vector<vector<double> > result(4,vector<double>(4));
//V3d<double> source_centroid(0,0,0);
//V3d<double> target_centroid(0,0,0);
//vector<vector<double> > matrice(3,vector<double>(3,0));
//vector<vector<double> > aat(3,vector<double>(3,0));
double source_centroid[3]={0,0,0};
double target_centroid[3]={0,0,0};
double aat[3][3],matrice[3][3];
//double **matrice1;
std::vector<std::vector<double> > matrice1(4,std::vector<double>(4,0));
double a[3],b[3];
double sa=0.0,sb=0.0;
//V3d<double> a;
//V3d<double> b;

	for(i=0;i<4;i++)
  {
          matrice1[i][i]=1;
  }
/*
matrice1=(double**)malloc(4*sizeof(double*));
	for(i=0;i<4;i++)
  {
        matrice1[i]=(double*)malloc(4*sizeof(double));
        for(int k = 0; k < 4; k++)
          matrice1[i][k]=0;
          matrice1[i][i]=1;
  }*/
//double matrice[3][3];
//double aat[3][3];
  for(i=0;i<3;i++) 
    {
    aat[i][0] = matrice[i][0]=0.0; // fill matrice with zeros
    aat[i][1] = matrice[i][1]=0.0; 
    aat[i][2] = matrice[i][2]=0.0; 
    }
for(i=0;i<nb_points;i++)
{
  source_centroid[0] +=sourcelandmarks[i].x;
  source_centroid[1] +=sourcelandmarks[i].y;
  source_centroid[2] +=sourcelandmarks[i].z;

  target_centroid[0] +=targetlandmarks[i].x;
  target_centroid[1] +=targetlandmarks[i].y;
  target_centroid[2] +=targetlandmarks[i].z;
}

source_centroid[0] /=nb_points;
source_centroid[1] /=nb_points;
source_centroid[2] /=nb_points;

target_centroid[0] /=nb_points;
target_centroid[1] /=nb_points;
target_centroid[2] /=nb_points;

for(i=0;i<nb_points;i++)
{
  a[0]=sourcelandmarks[i].x;
  a[1]=sourcelandmarks[i].y;
  a[2]=sourcelandmarks[i].z;

  b[0]=targetlandmarks[i].x;
  b[1]=targetlandmarks[i].y;
  b[2]=targetlandmarks[i].z;

  a[0] -= source_centroid[0];
  a[1] -= source_centroid[1];
  a[2] -= source_centroid[2];

  b[0] -= target_centroid[0];
  b[1] -= target_centroid[1];
  b[2] -= target_centroid[2];

  for(j=0;j<3;j++)
  {
    matrice[j][0] += a[j]*b[0];
    matrice[j][1] += a[j]*b[1];
    matrice[j][2] += a[j]*b[2];
  if(type_mode==0)
      {
        aat[j][0] += a[j]*a[0];
        aat[j][1] += a[j]*a[1];
        aat[j][2] += a[j]*a[2];
      }
  }

  sa += a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
  sb += b[0]*b[0]+b[1]*b[1]+b[2]*b[2];
}
if(type_mode==0)
    {
      Invert3x3(aat,aat);
      Multiply3x3(aat,matrice,matrice);

      for(i=0;i<3;++i)
         for(j=0;j<3;++j)
          {
            matrice1[i][j]=matrice[j][i];
          }
    }
else
{
double scale = (double)sqrt(sb/sa);

double Ndata[4][4];
    double *N[4];
    for(i=0;i<4;i++)
      {
      N[i] = Ndata[i];
      N[i][0]=0.0; // fill N with zeros
      N[i][1]=0.0;
      N[i][2]=0.0;
      N[i][3]=0.0;
      }
    // on-diagonal elements
    N[0][0] = matrice[0][0]+matrice[1][1]+matrice[2][2];
    N[1][1] = matrice[0][0]-matrice[1][1]-matrice[2][2];
    N[2][2] = -matrice[0][0]+matrice[1][1]-matrice[2][2];
    N[3][3] = -matrice[0][0]-matrice[1][1]+matrice[2][2];
    // off-diagonal elements
    N[0][1] = N[1][0] = matrice[1][2]-matrice[2][1];
    N[0][2] = N[2][0] = matrice[2][0]-matrice[0][2];
    N[0][3] = N[3][0] = matrice[0][1]-matrice[1][0];

    N[1][2] = N[2][1] = matrice[0][1]+matrice[1][0];
    N[1][3] = N[3][1] = matrice[2][0]+matrice[0][2];
    N[2][3] = N[3][2] = matrice[1][2]+matrice[2][1];

    // -- eigen-decompose N (is symmetric) --

    double eigenvectorData[4][4];
    double *eigenvectors[4],eigenvalues[4];

    eigenvectors[0] = eigenvectorData[0];
    eigenvectors[1] = eigenvectorData[1];
    eigenvectors[2] = eigenvectorData[2];
    eigenvectors[3] = eigenvectorData[3];
  
    JacobiN(N,4,eigenvalues,eigenvectors);

    double w,x,y,z;

    // first: if points are collinear, choose the quaternion that 
    // results in the smallest rotation.
    if (eigenvalues[0] == eigenvalues[1] || nb_points == 2)
      {double s0[3],t0[3],s1[3],t1[3];

    s0[0]=sourcelandmarks[0].x;    
    s0[1]=sourcelandmarks[0].y;    
    s0[2]=sourcelandmarks[0].x;  

    t0[0]=targetlandmarks[0].x;
    t0[1]=targetlandmarks[0].y;
    t0[2]=targetlandmarks[0].z;

    s1[0]=sourcelandmarks[1].x;    
    s1[1]=sourcelandmarks[1].y;    
    s1[2]=sourcelandmarks[1].z;   

    t1[0]=targetlandmarks[1].x;
    t1[1]=targetlandmarks[1].y;
    t1[2]=targetlandmarks[1].z;

    double ds[3],dt[3];
      double rs = 0, rt = 0;
      for (i = 0; i < 3; i++)
        {
        ds[i] = s1[i] - s0[i];      // vector between points
        rs += ds[i]*ds[i];
        dt[i] = t1[i] - t0[i];
        rt += dt[i]*dt[i];
        }

      // normalize the two vectors
      rs = sqrt(rs);
      ds[0] /= rs; ds[1] /= rs; ds[2] /= rs; 
      rt = sqrt(rt);
      dt[0] /= rt; dt[1] /= rt; dt[2] /= rt; 

      // take dot & cross product
      w = ds[0]*dt[0] + ds[1]*dt[1] + ds[2]*dt[2];
      x = ds[1]*dt[2] - ds[2]*dt[1];
      y = ds[2]*dt[0] - ds[0]*dt[2];
      z = ds[0]*dt[1] - ds[1]*dt[0];
    
      double r = sqrt(x*x + y*y + z*z);
      double theta = atan2(r,w);

      // construct quaternion
      w = cos(theta/2);
      if (r != 0)
        {
        r = sin(theta/2)/r;
        x = x*r;
        y = y*r;
        z = z*r;
        }
       else // rotation by 180 degrees: special case
        {
        // rotate around a vector perpendicular to ds
        Perpendiculars(ds,dt,0,0);
        r = sin(theta/2);
        x = dt[0]*r;
        y = dt[1]*r;
        z = dt[2]*r;
        }
      }
    else // points are not collinear
      {
      w = eigenvectors[0][0];
      x = eigenvectors[1][0];
      y = eigenvectors[2][0];
      z = eigenvectors[3][0];
      }
    double ww = w*w;
    double wx = w*x;
    double wy = w*y;
    double wz = w*z;

    double xx = x*x;
    double yy = y*y;
    double zz = z*z;

    double xy = x*y;
    double xz = x*z;
    double yz = y*z;
    
    matrice1[0][0] = ww + xx - yy - zz; 
    matrice1[1][0] = 2.0*(wz + xy);
    matrice1[2][0] = 2.0*(-wy + xz);

    matrice1[0][1] = 2.0*(-wz + xy);  
    matrice1[1][1] = ww - xx + yy - zz;
    matrice1[2][1] = 2.0*(wx + yz);

    matrice1[0][2] = 2.0*(wy + xz);
    matrice1[1][2] = 2.0*(-wx + yz);
    matrice1[2][2] = ww - xx - yy + zz;

  if(type_mode!=1)
    {     for(i=0;i<3;i++) 
            {
              matrice1[i][0] *= scale;
              matrice1[i][1] *= scale;
              matrice1[i][2] *= scale;
            }
    }
}

  // the translation is given by the difference in the transformed source
  // centroid and the target centroid
  double sx, sy, sz;

  sx = matrice1[0][0] * source_centroid[0] +
       matrice1[0][1] * source_centroid[1] +
       matrice1[0][2] * source_centroid[2];
  sy = matrice1[1][0] * source_centroid[0] +
       matrice1[1][1] * source_centroid[1] +
       matrice1[1][2] * source_centroid[2];
  sz = matrice1[2][0] * source_centroid[0] +
       matrice1[2][1] * source_centroid[1] +
       matrice1[2][2] * source_centroid[2];

  matrice1[0][3] = target_centroid[0] - sx;
  matrice1[1][3] = target_centroid[1] - sy;
  matrice1[2][3] = target_centroid[2] - sz;

  // fill the bottom row of the 4x4 matrix
  matrice1[3][0] = 0.0;
  matrice1[3][1] = 0.0;
  matrice1[3][2] = 0.0;
  matrice1[3][3] = 1.0;



  return matrice1;
}

void solidify(const std::vector<V3d<double> >& gold, std::vector<std::vector<V3d<double> > >& motion, std::vector<std::vector<bool> >& visibility)
{
  double  **subtract,** b, **product;

  std::vector<V3d<double> > sourcelandmarks;
  std::vector<V3d<double> > targetlandmarks;
  std::vector<std::vector<double> > Trans_matrix(4,std::vector<double>(4));

  int Ind_Prmt_3[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};
  int Ind_Prmt_4[24][4]={{0,1,2,3},{0,1,3,2},{0,2,1,3},{0,2,3,1},{0,3,2,1},{0,3,1,2},{1,0,2,3},{1,0,3,2},
						   {1,2,0,3},{1,2,3,0},{1,3,2,0},{1,3,0,2},{2,1,0,3},{2,1,3,0},{2,0,1,3},{2,0,3,1},
						   {2,3,0,1},{2,3,1,0},{3,1,2,0},{3,1,0,2},{3,2,1,0},{3,2,0,1},{3,0,2,1},{3,0,1,2}};
	int Ind_Prmt_3_4[24][3]={{0,1,2},{0,1,3},{0,2,3},{1,2,3},{1,3,2},{2,1,3},{2,3,1},{3,2,1},
                           {3,1,2},{0,3,2},{2,0,3},{2,3,0},{3,2,0},{3,0,2},{1,0,3},{1,3,0},
                           {0,3,1},{3,0,1},{3,1,0},{1,2,0},{1,0,2},{2,1,0},{2,0,1},{0,2,1}};


  double R[3][3],RR[3][3],T[3];

  unsigned int ct_nb;
  unsigned int i,j,l,m,n,k,z,t;
  double res,min;
  int inc=0;

  ct_nb=motion[0].size();

               b=(double**)malloc(3*sizeof(double*));
	             for(z=0;z<3;z++)
                  b[z]=(double*)malloc(ct_nb*sizeof(double));
               subtract=(double**)malloc(3*sizeof(double*));
	             for(z=0;z<3;z++)
                 subtract[z]=(double*)malloc(ct_nb*sizeof(double));
               product=(double**)malloc(3*sizeof(double*));
	             for(z=0;z<3;z++)
                 product[z]=(double*)malloc(ct_nb*sizeof(double));

      
  if (ct_nb <3)
    {
      //alert("the number of markers in the frames is less then 3");
      return;
    }
  if(gold.size() < ct_nb)
      {
        //alert("the number of markers in the gold standard is less then ct_nb")
        return;
      }
    else
      {
        
          for(i=0;i<motion.size();i++)
          {
             int ind=0;
             min=pow((double)10,10);
             
               for(j=0;j<visibility[i].size();j++)
                  {
                   if(visibility[i][j] == true)
                     {
                       sourcelandmarks.push_back(motion[i][j]);
                       ind++;
                     }
                  }
                
                 if(ind<3)
                  {
                      //alert("the number of markers visible in the frame i is less then 3");
                      return;
                  }
                 else
                  {
                    if(ct_nb == 3)
                     {
                        if(ind==3)
                          {
                            for(l=0;l<6;l++)
                              {
                                for(m=0;m<3;m++)
                                  {
                                    n=Ind_Prmt_3[l][m];
                                    targetlandmarks.push_back(gold[n]);
                                  }

                                  Trans_matrix=Transformation_Matrix(sourcelandmarks,targetlandmarks,3,1);
                                  res=Residual(Trans_matrix,sourcelandmarks,targetlandmarks,3);
                                 if(res<min)
                                   {if(l==0)
                                        {
                                           min=res;
                                           for(z=0;z<3;z++)
                                              {
                                                for(t=0;t<3;t++)
                                                    R[z][t]=Trans_matrix[z][t];
                                                 T[z]=Trans_matrix[z][3];
                                              }
                                         }
                                     else
                                      {
                                        inc++;
                                        motion[i]=motion[i-1];
                                        targetlandmarks.clear();   
                                        goto next;
                                      }
                                   }

                                   targetlandmarks.clear(); 
                                  
                              }
                          }
                        
                     }
                    else 
                     {
                        if(ct_nb == 4)
                           {
                             if(ind==4)
                               {
                                  for(l=0;l<24;l++)
                                    {
                                     for(m=0;m<4;m++)
                                        {
                                           n=Ind_Prmt_4[l][m];
                                           targetlandmarks.push_back(gold[n]);
                                        }
                                      Trans_matrix=Transformation_Matrix(sourcelandmarks,targetlandmarks,4,1);
                                      res=Residual(Trans_matrix,sourcelandmarks,targetlandmarks,4);
                                   if(res<min)
                                   {if(l==0)
                                        {
                                           min=res;
                                           for(z=0;z<3;z++)
                                              {
                                                for(t=0;t<3;t++)
                                                    R[z][t]=Trans_matrix[z][t];
                                                 T[z]=Trans_matrix[z][3];
                                              }
                                         }
                                     else
                                      {
                                        inc++;
                                        motion[i]=motion[i-1];
                                        targetlandmarks.clear();   
                                        goto next;
                                      }
                                   }
                                         targetlandmarks.clear();                 
                                 }
                                
                               }
                               else
                                 {
                                   if(ind==3)
                                    {
                                      for(l=0;l<24;l++)
                                         {
                                           for(m=0;m<3;m++)
                                              {
                                                n=Ind_Prmt_3_4[l][m];
                                                targetlandmarks.push_back(gold[n]);
                                              }
                                           Trans_matrix=Transformation_Matrix(sourcelandmarks,targetlandmarks,3,1);
                                           res=Residual(Trans_matrix,sourcelandmarks,targetlandmarks,3);
                                          if(res<min)
                                             {if(l<=3)
                                                  {
                                                     min=res;
                                                     for(z=0;z<3;z++)
                                                        {
                                                          for(t=0;t<3;t++)
                                                              R[z][t]=Trans_matrix[z][t];
                                                           T[z]=Trans_matrix[z][3];
                                                        }
                                                   }
                                               else
                                                {
                                                  inc++;
                                                  motion[i]=motion[i-1];
                                                  targetlandmarks.clear();   
                                                  goto next;
                                                }
                                             }
                                            targetlandmarks.clear(); 
                                                                               
                                          }
                                     }
                                                       
                                }
                           }
                      



                 }


         }

                             

               for(z=0;z<3;z++) 
                 for(t=0;t<3;t++) 
                   RR[t][z]=R[z][t];

                  for(z=0;z<ct_nb;z++) 
                     for(t=0;t<3;t++)
                       b[t][z]=gold[z].components[t];
                      

                  for(z=0;z<3;z++) 
                    for(t=0;t<ct_nb;t++)
                      {
                        subtract[z][t]=b[z][t]-T[z];
                      }
         
               for(z=0;z<3;z++)
                  for(t=0;t<ct_nb;t++)
                    {
                      product[z][t]=0;
                      for(k=0;k<3;k++)
                        product[z][t] += RR[z][k]*subtract[k][t];
                      motion[i][t].components[z]=product[z][t];
                    }
                
                                          
                 
                next :
               sourcelandmarks.clear();
          //if(i==993)
            //goto exitt;

          }
            


  }

      for(i=0;i<3;i++)
        {
          free(b[i]);
          b[i]=NULL;
          free(subtract[i]);
          subtract[i]=NULL;
          free(product[i]);
          product[i]=NULL;
        }
      b=NULL;
      subtract=NULL;
      product=NULL;

//exitt:
  return;
}






//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpSolidify);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpSolidify::lhpOpSolidify(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType             = OPTYPE_OP;
  m_Canundo            = true;
  m_Source             = NULL;
  m_SourceName         = _R("none");
}
//----------------------------------------------------------------------------
lhpOpSolidify::~lhpOpSolidify()
//----------------------------------------------------------------------------
{
  mafDEL(m_Output);
}
//----------------------------------------------------------------------------
mafOp* lhpOpSolidify::Copy()
//----------------------------------------------------------------------------
{
  lhpOpSolidify *cp = new lhpOpSolidify(GetLabel());
  cp->m_Canundo   = m_Canundo;
  cp->m_OpType    = m_OpType;
  cp->SetListener(GetListener());
  cp->m_Next      = NULL;
  return cp;
}
//----------------------------------------------------------------------------
bool lhpOpSolidify::Accept(mafNode* node)   
//----------------------------------------------------------------------------
{
  return  (node && (node->IsMAFType(mafVMELandmarkCloud)));
}
//----------------------------------------------------------------------------
enum SURFACE_PROJ_ID
//----------------------------------------------------------------------------
{
  ID_CHOOSE = MINID,
};
//----------------------------------------------------------------------------
void lhpOpSolidify::OpRun()
//----------------------------------------------------------------------------
{  
  if(!m_TestMode)
  {
    // interface:
    m_Gui = new mafGUI(this);
    m_Gui->SetListener(this);
    m_Gui->Label(_L("source :"),true);
    m_Gui->Label(&m_SourceName);
    m_Gui->Button(ID_CHOOSE,_L("source "));
    m_Gui->Label(_R(""));
    m_Gui->OkCancel();
    m_Gui->Enable(wxOK, false);

    ShowGui();
  }

  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void lhpOpSolidify::OpDo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    m_Output->ReparentTo(m_Input->GetParent());
    //mafEventMacro(mafEvent(this, VME_ADD, m_Output));
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void lhpOpSolidify::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_Output)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_Output));
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}

//----------------------------------------------------------------------------
void lhpOpSolidify::OpStop(int result)
//----------------------------------------------------------------------------
{
  HideGui();
  if (result == OP_RUN_CANCEL)
  {
    mafEventMacro(mafEvent(this,result));
    return;
  }

  std::vector<mafTimeStamp> stmps;
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);
  mafVMELandmarkCloud *newcloud;// = mafVMELandmarkCloud::New();
  mafString           ncname;
  ncname.Append(cloud->GetName());
  ncname.Append(_R("_solidified_with_"));
  ncname.Append(m_SourceName);
  mafNEW(newcloud);
  newcloud->SetName(ncname);
  //newcloud->DeepCopy(cloud);
  cloud->GetTimeStamps(stmps);
  std::vector<V3d<double> > gold;
  std::vector<std::vector<V3d<double> > > motion;
  std::vector<std::vector<bool> >         visibl;
  gold.resize(m_Source->GetNumberOfLandmarks());
  for(int i = 0; i < m_Source->GetNumberOfLandmarks(); i++)
  {
    m_Source->GetLandmark(i, gold[i].components);
  }
  motion.resize(stmps.size());
  visibl.resize(stmps.size());
  for(int i = 0; i < stmps.size(); i++)
  {
    mafMatrix m;
    cloud->GetOutput()->GetMatrix(m, stmps[i]);
    m.SetTimeStamp(stmps[i]);
    //newcloud->SetMatrix(m);
    motion[i].resize(cloud->GetNumberOfLandmarks());
    visibl[i].resize(cloud->GetNumberOfLandmarks());
    for(int j = 0; j < cloud->GetNumberOfLandmarks(); j++)
    {
      V3d<double> lm;
      cloud->GetLandmark(j, lm.components, stmps[i]);
      V4d<double> lm4, lm41;
      lm4 = V4d<double>(lm[0], lm[1], lm[2], 1.0);
      m.MultiplyPoint(lm4.components, lm41.components);
      motion[i][j] = V3d<double>(lm41.components);
      visibl[i][j] = cloud->GetLandmarkVisibility(j, stmps[i]);
    }
  }
  solidify(gold, motion, visibl);
  for(int i = 0; i < stmps.size(); i++)
  {
    for(int j = 0; j < cloud->GetNumberOfLandmarks(); j++)
    {
      V3d<double> lm = motion[i][j];
      if(newcloud->FindLandmarkIndex(cloud->GetLandmarkName(j)) == -1)
        newcloud->AppendLandmark(cloud->GetLandmarkName(j));
      newcloud->SetLandmark(cloud->GetLandmarkName(j), lm[0], lm[1], lm[2], stmps[i]);
      newcloud->SetLandmarkVisibility(cloud->GetLandmarkName(j), visibl[i][j], stmps[i]);
    }
  }
  newcloud->Modified();
  newcloud->Update();
  //newcloud->Register(this);
  m_Output = newcloud;

  mafEventMacro(mafEvent(this,result));
}


//----------------------------------------------------------------------------
void lhpOpSolidify::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CHOOSE:
        {
          mafString s(_L("Choose solid landmark"));
          mafEvent e(this,VME_CHOOSE, &s, (intptr_t)&lhpOpSolidify::RigidBodyAccept);
          mafEventMacro(e);
          if(e.GetVme() == NULL)
          {
            return;
          }
          mafNode *sel = e.GetVme();
          if(mafVMELandmarkCloud::SafeDownCast(sel)== NULL)
          {
            wxMessageBox("Selected VME should be mafVMELandmarkCloud.","Warning", wxOK|wxICON_WARNING , NULL);
            return;
          }

          m_Source = mafVMELandmarkCloud::SafeDownCast(sel);
          SetNodeName(m_Source, &m_SourceName);
          m_Gui->Enable(wxOK, true);
          m_Gui->Update();
        }
        break;
      case wxOK:
        if(m_Source == NULL)
          break;
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
        default:
        mafEventMacro(*e);
      break; 
    }
  }  
}

//----------------------------------------------------------------------------
void lhpOpSolidify::SetNodeName(mafVME *pVME, mafString *pName) 
//----------------------------------------------------------------------------
{
  *pName = pVME->GetName();
  if(pVME->GetParent() != NULL)
  {
    *pName += _R(" parent:");
    *pName += pVME->GetParent()->GetName();
  }
}
