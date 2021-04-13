/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPlotMath.h,v $
  Language:  C++
  Date:      $Date: 2008-02-19 11:42:32 $
  Version:   $Revision: 1.2 $
  Authors:   Vladik Aranov
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPlotMath_H__
#define __mafPlotMath_H__

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "vtkMatrix4x4.h"
#include "mafVectors.h"

#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */

#define _X_IDX                       0
#define _Y_IDX                       1
#define _Z_IDX                       2
#define _W_IDX                       3
enum QuatPart { X, Y, Z, W };
#define _MATR_EL(xyzw,rua)           (*((&mpMat->vRight.x) + (sizeof(mpMat->vRight)/sizeof(double)) * (rua) + (xyzw)))

#define EulFrmS      0
#define EulFrmR      1
#define EulParEven   0
#define EulParOdd    1
#define EulSafe      "\000\001\002\000"
#define EulNext      "\001\002\000\001"
#define EulRepNo     0
#define EulRepYes    1
#define EulGetOrd(ord,i,j,k,h,n,s,f) {unsigned int o=ord;f=o&1;o>>=1;s=o&1;o>>=1;\
    n=o&1;o>>=1;i=EulSafe[o&3];j=EulNext[i+n];k=EulNext[i+1-n];h=s?k:i;}

// EulOrd creates an order value between 0 and 23 from 4-tuple choices.
#define EulOrd(i,p,r,f)    (((((((i)<<1)+(p))<<1)+(r))<<1)+(f))
#define _QUAT_COMP_ASSIGN(idx,val)   *((double *)qpQuat+(idx)) = (val)
#define EPSILON                     0.0004f

#define FPEqualTo(a,b)          (fabs((a)-(b)) < 1e-4)  
#define FPNotEqualTo(a,b)       (fabs((a)-(b)) > 1e-4)  

#define EulFrm(ord)  ((unsigned)(ord)&1)
#define EulRep(ord)  (((unsigned)(ord)>>1)&1)
#define EulPar(ord)  (((unsigned)(ord)>>2)&1)
#define EulAxI(ord)  ((int)(EulSafe[(((unsigned)(ord)>>3)&3)]))
#define EulAxJ(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)==EulParOdd)]))
#define EulAxK(ord)  ((int)(EulNext[EulAxI(ord)+(EulPar(ord)!=EulParOdd)]))
#define EulAxH(ord)  ((EulRep(ord)==EulRepNo)?EulAxK(ord):EulAxI(ord))

//#define Sb(a)  (*((DiSplitBits *)(&(a))))

// Static axes 
#define EulOrdXYZs    EulOrd(_X_IDX,EulParEven,EulRepNo,EulFrmS)
#define EulOrdXYXs    EulOrd(_X_IDX,EulParEven,EulRepYes,EulFrmS)
#define EulOrdXZYs    EulOrd(_X_IDX,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdXZXs    EulOrd(_X_IDX,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdYZXs    EulOrd(_Y_IDX,EulParEven,EulRepNo,EulFrmS)
#define EulOrdYZYs    EulOrd(_Y_IDX,EulParEven,EulRepYes,EulFrmS)
#define EulOrdYXZs    EulOrd(_Y_IDX,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdYXYs    EulOrd(_Y_IDX,EulParOdd,EulRepYes,EulFrmS)
#define EulOrdZXYs    EulOrd(_Z_IDX,EulParEven,EulRepNo,EulFrmS)
#define EulOrdZXZs    EulOrd(_Z_IDX,EulParEven,EulRepYes,EulFrmS)
#define EulOrdZYXs    EulOrd(_Z_IDX,EulParOdd,EulRepNo,EulFrmS)
#define EulOrdZYZs    EulOrd(_Z_IDX,EulParOdd,EulRepYes,EulFrmS)

// Rotating axes 
#define EulOrdZYXr    EulOrd(_X_IDX,EulParEven,EulRepNo,EulFrmR)
#define EulOrdXYXr    EulOrd(_X_IDX,EulParEven,EulRepYes,EulFrmR)
#define EulOrdYZXr    EulOrd(_X_IDX,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdXZXr    EulOrd(_X_IDX,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdXZYr    EulOrd(_Y_IDX,EulParEven,EulRepNo,EulFrmR)
#define EulOrdYZYr    EulOrd(_Y_IDX,EulParEven,EulRepYes,EulFrmR)
#define EulOrdZXYr    EulOrd(_Y_IDX,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdYXYr    EulOrd(_Y_IDX,EulParOdd,EulRepYes,EulFrmR)
#define EulOrdYXZr    EulOrd(_Z_IDX,EulParEven,EulRepNo,EulFrmR)
#define EulOrdZXZr    EulOrd(_Z_IDX,EulParEven,EulRepYes,EulFrmR)
#define EulOrdXYZr    EulOrd(_Z_IDX,EulParOdd,EulRepNo,EulFrmR)
#define EulOrdZYZr    EulOrd(_Z_IDX,EulParOdd,EulRepYes,EulFrmR)

#define FLT_GARB         888.0f  
#define DiMax(a, b)      (((a) > (b))?(a):(b))
#define DiMin(a, b)      (((a) < (b))?(a):(b))
#define DiSign(Value)    (((Value) < 0.f)? -1.0f:(((Value) > 0.f)?1.0f:0.0f))
#define DiRound(val)     (((val) - floor(val) > ceil(val) - (val)) ? ceil(val) : floor(val))
#define diPI             3.14159265358979323846f

#ifndef DIM
  #define DIM(a)  (sizeof((a)) / sizeof(*(a)))
#endif
#define DiD2L(val)       ((int)(val))
#define DiL2D(val)       ((double)(val))

#ifdef __cplusplus
template <bool test> struct Compiled_Time_Assertion_Failed;

template <> struct Compiled_Time_Assertion_Failed<true> 
{ 
  enum 
  { 
    value = 1 
  }; 
};

template <int x> struct DiCompileAssertTest
{
};

#define DICOMPILEASSERT(expression) \
  {                                       \
    typedef DiCompileAssertTest<sizeof(Compiled_Time_Assertion_Failed<(bool)(expression)>)> DiCompileAssertType; \
  }
#else
#define DICOMPILEASSERT(expression) ((void)0)
#endif

enum DiOpCombainType
{
  diREPLACE,
  diPRECONCAT,
  diPOSTCONCAT,
  di_OP_FORCEDWORD = 0x7FFFFFFF
};


//----------------------------------------------------------------------------
// Inline functions
//----------------------------------------------------------------------------

/**
Fits variable into diapason type regardless
*/
#define DiFFitIn(val, from, to)   DiMax(from, DiMin(val, to))

//----------------------------------------------------------------------------
// type definitions
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// types definitions
//----------------------------------------------------------------------------
typedef V4d<double> DiV4d;

struct DiMatrix
{
  DiV4d     vRight;
  DiV4d     vUp;
  DiV4d     vAt;
  DiV4d     vPos;
  //DiV4d& operator[](int i){assert(i >= 0 && i <4);if(i == 0) return vRight; else if(i == 1) return vUp; else if(i == 2) return vUp; else return vPos;}
  //const DiV4d& operator[](int i)const{assert(i >= 0 && i <4);if(i == 0) return vRight; else if(i == 1) return vUp; else if(i == 2) return vUp; else return vPos;}
} ;

typedef DiV4d DiQuaternion;


/**
 *This macro initalizes matrix.
 *@memo     Initalize matrix
 *@param    m      matrix
 *@param    t      init type
 */
#define DiMatrixInitialize(m)       \
{                                   \
  memset(m, 0, sizeof(DiMatrix)); \
}


//----------------------------------------------------------------------------
// external variables
//----------------------------------------------------------------------------
extern DiV4d const *vp4GZero;

//----------------------------------------------------------------------------
// forward references
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Functions references
//----------------------------------------------------------------------------

void     mflMatrixToDi(vtkMatrix4x4 const *pMatrix, DiMatrix *mp);
void     DiMatrixToVTK(DiMatrix const *mp, vtkMatrix4x4 *pMatrix);

bool     mafTransfMirrorMatrixYOX(DiMatrix *mpIn, DiMatrix *mpOut);
bool     mafTransfMatrixToEuler(DiMatrix const *mpR, DiV4d *vpR, int conv);
bool     mafTransfEulerToMatrix(DiV4d *vpRot, DiMatrix *mpMat, int conv);
bool     mafTransfEulerToQuaternion(DiV4d *vpRot, DiQuaternion *qpQuat, int conv);
void     mafTransfOrtoNormalizeMatrix(DiMatrix *mpMat, DiV4d const*vpRot, bool bLeftRight);
void     mafTransfRightLeftConv(DiMatrix *mpSource, DiMatrix *mpDest);
void     mafTransfMatrixToGES(DiMatrix *mpParent, DiMatrix *mpChild, DiMatrix *mpBasic, DiV4d *vpR);
void     mafAttVecToVTKMat(double ThetaIn[3], vtkMatrix4x4 *pMat);
bool     mafTransfTransformUpright(DiV4d const*vpPos, DiV4d const*vpRot, DiMatrix *mpOut);
bool     mafTransfInverseTransformUpright(DiMatrix const *mpIn, DiV4d *vpPos, DiV4d *vpRot);
void     DiMatrixTransposeRotationalSubmatrix(const DiMatrix *mpMatSrc, DiMatrix *mpMatDst);
void     DiMatrixOrtoNormalizeVectSys(DiV4d  *vpMain, DiV4d  *vpSub1, DiV4d  *vpSub2, bool bLeftRight);
void     DiV4dTransformVectorTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut);
void     DiV4dInverseTransformVectorTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut);
void     DiV4dTransformPointTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut);
void     DiV4dInverseTransformPointTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut);
bool     DiMatrixInvert(const DiMatrix *mpIn, DiMatrix *mpOut);
bool     DiMatrixTransform(DiMatrix *mpDst, DiMatrix *mpSrc, int op);
void     DiV4dCrossProduct(const DiV4d *vpVect1, const DiV4d *vpVect2, DiV4d *vpOut);
void     DiMatrixMultiply(const DiMatrix *mpA, const DiMatrix *mpB, DiMatrix *mpOut);
void     DiMatrixTestIntegrity(const DiMatrix *mpMatrix);
bool     DiMatrixTestIdentity(const DiMatrix *mpMatrix);
bool     DiMatrixTestRotIdentity(const DiMatrix *mpMatrix);
void     DiMatrixIdentity(DiMatrix *mpM);
void     DiQuatBuildFromMatrix(DiMatrix const *mpMatIn, DiQuaternion *qpQuat);
void     DiQuatBuildMatrix(DiQuaternion *qpQuat, DiMatrix *mpMatrix);
bool     mafTransfDecomposeMatrix(DiMatrix *mpIn, DiV4d *vpRot, DiV4d *vpPos);
bool     mafTransfDecomposeMatrixStright(DiMatrix *mpIn, DiV4d *vpRot, DiV4d *vpPos);
bool     mafTransfComposeMatrix(DiMatrix *mpIn, DiV4d *vpRot, DiV4d *vpPos);
bool     mafTransfComposeMatrixStright(DiMatrix *mpIn, DiV4d const *vpRot, DiV4d const *vpPos);
bool     mafTransfIsMatrixOrtoNormalized(DiMatrix *mpMat);
void     DiMatrixScale(DiMatrix *mpMat, DiV4d *vpScale, DiOpCombainType nOperation);
void     DiMatrixTranslate(DiMatrix *mpMat, DiV4d *vpTran, DiOpCombainType nOperation);
void     DiMatrixRotateAtSinCosLeft(DiMatrix *mpMat, double rSin, double rCos, DiOpCombainType nOperation);
void     DiMatrixRotateAt(DiMatrix *mpMat, double rAngle, DiOpCombainType nOperation);
void     DiMatrixRotateRightSinCos(DiMatrix *mpMat, double rSin, double  rCos, DiOpCombainType nOperation);
void     DiMatrixRotateRight(DiMatrix *mpMat, double rAngle, DiOpCombainType nOperation);
void     DiMatrixRotateUpSinCosLeft(DiMatrix *mpMat, double rSin, double  rCos, DiOpCombainType nOperation);
void     DiMatrixRotateUp(DiMatrix *mpMat, double rAngle, DiOpCombainType nOperation);

struct  DiMatrixInterpolationData
{
  // Matrices
  DiQuaternion  qtCurQuat;
  DiQuaternion  qtPrevQuat;
  // Linear
  DiV4d         vPosDelta;
  DiV4d         vPrevPos;
};
void DiMatrixBuildInterpolated(DiMatrixInterpolationData *midpData, double rT, DiMatrix *mpNewMatrix);
void DiMatrixBuildInterpolationData(DiMatrix const *mpCurMatrix, DiMatrix const *mpPrevMatrix, DiMatrixInterpolationData *midpData);

#define            DiMatrixRotateUpSinCosRight(a, b, c, d)  DiMatrixRotateUpSinCosLeft((a), -(b), (c), (d))
#define            DiMatrixRotateUpSinCos(a, b, c, d)  DiMatrixRotateUpSinCosLeft((a), (b), (c), (d))
#define            DiMatrixRotateAtSinCosRight(a, b, c, d)  DiMatrixRotateAtSinCosLeft((a), -(b), (c), (d))
#define            DiMatrixRotateAtSinCos(a, b, c, d)  DiMatrixRotateAtSinCosLeft((a), (b), (c), (d))

double Determinant(DiMatrix const * inmat, int n);


inline     void DiMatrixCopy(const DiMatrix *mpSrc, DiMatrix *mpDst)
{
  wxASSERT(mpSrc); wxASSERT(mpDst);  *mpDst = *mpSrc;
}
inline     void DiV4dNegate(const DiV4d *vpIn, DiV4d *vpOut)
{
  vpOut->x = -vpIn->x;  vpOut->y = -vpIn->y;  vpOut->z = -vpIn->z;  vpOut->w = 1.0f;
}
inline     void DiV4dCopy(const DiV4d *vpIn, DiV4d *vpOut)
{
  vpOut->x = vpIn->x;  vpOut->y = vpIn->y;  vpOut->z = vpIn->z;  vpOut->w = vpIn->w;
}
inline     double DiV4dPointDistance2(const DiV4d *vpP0, const DiV4d *vpP1)
{
  DiV4d vV; double rDist;
  vV.x = vpP0->x - vpP1->x;  vV.y = vpP0->y - vpP1->y;  vV.z = vpP0->z - vpP1->z;
  rDist = vV.x * vV.x + vV.y * vV.y + vV.z * vV.z;
  return (rDist);
} 
inline     double DiV4dDotProduct(const DiV4d *vpVect1, const DiV4d *vpVect2)
{
  return(vpVect1->x * vpVect2->x + vpVect1->y * vpVect2->y + vpVect1->z * vpVect2->z);
} 
inline     void DiV4dMakeUnit(DiV4d *vpVect)
{
  double   rLen, rRecLen;
  rLen = DiV4dDotProduct(vpVect, vpVect);
  wxASSERT(rLen > 0.f);

  rRecLen = 1.0 / sqrt(rLen);
  vpVect->x = vpVect->x * rRecLen;
  vpVect->y = vpVect->y * rRecLen;
  vpVect->z = vpVect->z * rRecLen;
  vpVect->w = 0.0f;
}
inline     double DiV4dNormalize(const DiV4d *vpIn, DiV4d *vpOut)
{
  double   rLen, rRecLen;
  rLen = DiV4dDotProduct(vpIn, vpIn);
  rLen = sqrt(rLen);
  wxASSERT(rLen > 0.f);
  rRecLen = 1.0f / rLen;  
  vpOut->x = vpIn->x * rRecLen;
  vpOut->y = vpIn->y * rRecLen;
  vpOut->z = vpIn->z * rRecLen;
  vpOut->w = 0.0f;
  return rLen;
} 


#define diSHIFT_COMB(rA, rB, rC)      ((rA)*(rB)+(rC))
inline     void DiV4dShiftComb(const DiV4d *vpVect1, const DiV4d *vpVect2, double rCoef, DiV4d *vpOut)
{
  vpOut->x = diSHIFT_COMB(vpVect2->x, rCoef, vpVect1->x);
  vpOut->y = diSHIFT_COMB(vpVect2->y, rCoef, vpVect1->y);
  vpOut->z = diSHIFT_COMB(vpVect2->z, rCoef, vpVect1->z);
  vpOut->w = 1.0f;
}
inline     void DiV4dScale(const DiV4d *vpIn, double rScale, DiV4d *vpOut)
{
  vpOut->x = vpIn->x * rScale;  vpOut->y = vpIn->y * rScale;  vpOut->z = vpIn->z * rScale;  vpOut->w = 1.0f;
}

inline     void DiV4dSub(const DiV4d *vpA, const DiV4d *vpB, DiV4d *vpC)
{
  vpC->x = vpA->x - vpB->x;  vpC->y = vpA->y - vpB->y;  vpC->z = vpA->z - vpB->z;  vpC->w = 1.0f;
} 

inline     void DiV4dAdd(const DiV4d *vpA, const DiV4d *vpB, DiV4d *vpC)
{
  vpC->x = vpA->x + vpB->x;  vpC->y = vpA->y + vpB->y;  vpC->z = vpA->z + vpB->z;  vpC->w = 1.0f;
} 

inline     void DiV4dLineComb(const DiV4d *vpVect1, double rCoef1, const DiV4d *vpVect2, double rCoef2, DiV4d *vpOut)
{
  vpOut->x = diSHIFT_COMB(vpVect1->x, rCoef1, vpVect2->x * rCoef2);
  vpOut->y = diSHIFT_COMB(vpVect1->y, rCoef1, vpVect2->y * rCoef2);
  vpOut->z = diSHIFT_COMB(vpVect1->z, rCoef1, vpVect2->z * rCoef2);
  vpOut->w = 1.0f;
} // end of DiV4dLineComb

inline     void DiQuatLineComb(const DiQuaternion *vpVect1, double rCoef1, const DiQuaternion *vpVect2, double rCoef2, DiQuaternion *vpOut)
{
  vpOut->x = diSHIFT_COMB(vpVect1->x, rCoef1, vpVect2->x * rCoef2);
  vpOut->y = diSHIFT_COMB(vpVect1->y, rCoef1, vpVect2->y * rCoef2);
  vpOut->z = diSHIFT_COMB(vpVect1->z, rCoef1, vpVect2->z * rCoef2);
  vpOut->w = diSHIFT_COMB(vpVect1->w, rCoef1, vpVect2->w * rCoef2);
} // end of DiV4dLineComb

template<class Type>
Type& Fix2PI(Type& src, const Type& trg)
{
  const static auto pivl = 4 * atan(Type(1));
  while (src > trg + pivl) src -= 2 * pivl;
  while (src < trg - pivl) src += 2 * pivl;
  return src;
}
template <class Type>
auto norm2(const Type& v) -> decltype(v * v)
{
  return v * v;
}

template<class Type, class VecType = V3d<Type> >
VecType EulerAngles(const vtkMatrix4x4& M, int order, const VecType& prev = VecType())
{
  VecType u = VecType();
  VecType v = VecType();
  VecType p = prev;
  const static auto pivl = 4 * atan(Type(1));

  int i, j, k, h, n, s, f;
  EulGetOrd(order, i, j, k, h, n, s, f);
  if (n == EulParOdd) { p = -p; }
  if (f == EulFrmR)   { std::swap(p[0], p[2]); }
  if (s == EulRepYes)
  {
    auto sy = sqrt(M[i][j] * M[i][j] + M[i][k] * M[i][k]);
    if (sy > 16 * FLT_EPSILON)
    {
      u[0] = atan2(M[i][j], M[i][k]);
      u[1] = atan2(sy, M[i][i]);
      u[2] = atan2(M[j][i], -M[k][i]);
    }
    else
    {
      auto sd = atan2(-M[j][k], M[j][j]);
      u[1] = atan2(sy, M[i][i]);
      if (M[i][i] > 0)
      {
        u[0] = (p[0] - p[2] + sd) / 2;
        u[2] = (p[2] - p[0] + sd) / 2;
      }
      else
      {
        u[0] = (p[0] + p[2] + sd) / 2;
        u[2] = (p[0] + p[2] - sd) / 2;
      }
    }
    v[1] = /*2 * pivl*/ -u[1];
    v[0] = pivl + u[0];
    v[2] = pivl + u[2];
  }
  else
  {
    auto cy = sqrt(M[i][i] * M[i][i] + M[j][i] * M[j][i]);
    if (cy > 16 * FLT_EPSILON)
    {
      u[0] = atan2(M[k][j], M[k][k]);
      u[1] = atan2(-M[k][i], cy);
      u[2] = atan2(M[j][i], M[i][i]);
    }
    else
    {
      auto sd = atan2(-M[j][k], M[j][j]);
      u[1] = atan2(-M[k][i], cy);
      if (M[k][i] > 0)
      {
        u[0] = (p[0] - p[2] + sd) / 2;
        u[2] = (p[2] - p[0] + sd) / 2;
      }
      else
      {
        u[0] = (p[0] + p[2] + sd) / 2;
        u[2] = (p[0] + p[2] - sd) / 2;
      }
    }
    v[1] = pivl - u[1];
    v[0] = pivl + u[0];
    v[2] = pivl + u[2];
  }

  for (size_t i = 0; i < 3; ++i)
  {
    Fix2PI(u[i], p[i]);
    Fix2PI(v[i], p[i]);
  }

  if (norm2(v - p) < norm2(u - p))
    u = v;

  if (n == EulParOdd) { u = -u; }
  if (f == EulFrmR)   { std::swap(u[0], u[2]); }
  return u;
}
template<int order, class Type, class VecType = V3d<Type> >
VecType EulerAngles(const vtkMatrix4x4& M, const VecType& prev = VecType())
{
  return EulerAngles(order, M, prev);
}
template<class VecType>
VecType EulerAnglesFix(int order, VecType& curr, const VecType& prv)
{
  typedef decltype(VecType()[0] * VecType()[0]) Type;
  auto prev = prv;
  auto curs = curr;
  auto pivl = 4 * atan(Type(1));
  int i, j, k, h, n, s, f;
  EulGetOrd(order, i, j, k, h, n, s, f);
  if (n == EulParOdd) { curr = -curr; }
  if (f == EulFrmR) { std::swap(curr[0], curr[2]); }
  if (n == EulParOdd) { prev = -prev; }
  if (f == EulFrmR) { std::swap(prev[0], prev[2]); }
  if (s == EulRepYes) {
    auto sy = sin(curr[1]);
    if (sy <= 16 * FLT_EPSILON)
    {
      auto cy = cos(curr[1]);
      auto sd = curr[0];
      if (cy > 0)
      {
        curr[0] = (prev[0] - prev[2] + sd) / 2;
        curr[2] = (prev[2] - prev[0] + sd) / 2;
      }
      else
      {
        curr[0] = (prev[0] + prev[2] + sd) / 2;
        curr[2] = (prev[0] + prev[2] - sd) / 2;
      }
    }
    curs[1] = /*2 * pivl*/ -curr[1];
    curs[0] = pivl + curr[0];
    curs[2] = pivl + curr[2];
  }
  else {
    auto cy = cos(curr[1]);
    if (cy <= 16 * FLT_EPSILON)
    {
      auto sy = sin(curr[1]);
      auto sd = curr[0];
      if (sy < 0)
      {
        curr[0] = (prev[0] - prev[2] + sd) / 2;
        curr[2] = (prev[2] - prev[0] + sd) / 2;
      }
      else
      {
        curr[0] = (prev[0] + prev[2] + sd) / 2;
        curr[2] = (prev[0] + prev[2] - sd) / 2;
      }
    }
    curs[1] = pivl - curr[1];
    curs[0] = pivl + curr[0];
    curs[2] = pivl + curr[2];
  }
  for (size_t i = 0; i < 3; ++i)
  {
    Fix2PI(curr[i], prev[i]);
    Fix2PI(curs[i], prev[i]);
  }

  auto sx = curs[0] - prev[0];
  auto sy = curs[0] - prev[0];
  auto sz = curs[0] - prev[0];
  auto rx = curr[0] - prev[0];
  auto ry = curr[0] - prev[0];
  auto rz = curr[0] - prev[0];
  if (sx * sx + sy * sy + sz * sz < rx * rx + ry * ry + rz * rz)
    curr = curs;

  if (n == EulParOdd) { curr = -curr; }
  if (f == EulFrmR) { std::swap(curr[0], curr[2]); }
  return curr;
}

template<class VecType>
void eulerTransform(vtkMatrix4x4& M, int conv, const VecType& eul, const VecType& trn = VecType())
{
  using Type = decltype(VecType() * VecType());
  VecType ea(eul);
  Type ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
  int i, j, k, h, n, s, f;
  EulGetOrd(conv, i, j, k, h, n, s, f);
  if (f == EulFrmR) { std::swap(ea[0], ea[2]); }
  if (n == EulParOdd) { ea = -ea; }
  ti = ea[0];    tj = ea[1];    th = ea[2];
  ci = cos(ti); cj = cos(tj); ch = cos(th);
  si = sin(ti); sj = sin(tj); sh = sin(th);
  cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
  if (s == EulRepYes)
  {
    M[i][i] = cj;       M[i][j] = sj * si;       M[i][k] = sj * ci;
    M[j][i] = sj * sh;  M[j][j] = -cj * ss + cc; M[j][k] = -cj * cs - sc;
    M[k][i] = -sj * ch; M[k][j] = cj * sc + cs;  M[k][k] = cj * cc - ss;
  }
  else
  {
    M[i][i] = cj * ch; M[i][j] = sj * sc - cs; M[i][k] = sj * cc + ss;
    M[j][i] = cj * sh; M[j][j] = sj * ss + cc; M[j][k] = sj * cs - sc;
    M[k][i] = -sj;     M[k][j] = cj * si;      M[k][k] = cj * ci;
  }
  M[0][3] = trn[0];  M[1][3] = trn[1]; M[2][3] = trn[2];
  return M;
}
template<int conv, class VecType>
void eulerTransform(vtkMatrix4x4& M, const VecType& eul, const VecType& trn = VecType())
{
  eulerTransform(M, conv, eul, trn);
}



template<class Type>
Type sign(Type v)
{
  if (v > 0)
    return Type(1);
  if (v < 0)
    return Type(-1);
  return Type(0);
}
template<class Type, class VecType = V3d<Type> >
VecType OVPAngles(const vtkMatrix4x4& M, const VecType& prev = VecType())
{
  VecType u = VecType();
  Type    a = Type(0.5) * (M[2][1] - M[1][2]);
  Type    b = Type(0.5) * (M[0][2] - M[2][0]);
  Type    c = Type(0.5) * (M[1][0] - M[0][1]);
  Type    s = sqrt(a * a + b * b + c * c);
  Type    co = Type(0.5) * (M[0][0] + M[1][1] + M[2][2] - 1);
  Type    fi = atan2(s, co);
  Type    t;

  co = std::min(Type(1), std::max(Type(-1), co));
  Type v = Type(1) - co;

  if (s > Type(0.1e-12))
  {
    u[0] = a / s;
    u[1] = b / s;
    u[2] = c / s;
  }
  else if (fabs(fi) > Type(0.1e-12) && co > 0)
  {
    t = Type(1) / v;
    u[0] = sign(M[2][1] - M[1][2]) * sqrt(abs((M[0][0] - co) * t));
    u[1] = sign(M[0][2] - M[2][0]) * sqrt(abs((M[1][1] - co) * t));
    u[2] = sign(M[1][0] - M[0][1]) * sqrt(abs((M[2][2] - co) * t));
  }
  else if (fabs(fi) > Type(0.1e-12) && co < 0)
  {
    t = Type(1) / v;
    u[0] = sqrt(abs((M[0][0] - co) * t));
    u[1] = sqrt(abs((M[1][1] - co) * t));
    u[2] = sqrt(abs((M[2][2] - co) * t));

    if (M[2][1] - M[1][2] >= 0)
      s = Type(1);
    else
      s = Type(-1);
    u[0] = u[0] * s;
    u[1] = u[1] * sign(M[1][0] + M[0][1]) * s;
    u[2] = u[2] * sign(M[2][1] + M[0][2]) * s;
  }
  return Fix2PI(fi, u * prev) * u;
  return Fix2PI(fi, (u * (u * fi - prev))) * u;
  //const static auto pivl = 4 * atan(Type(1));
  //auto res = u * fi;
  //auto k = (u * (u * fi - prev)) / (2 * pivl);
  //return (fi - round(k) * 2 * pivl) * u;
}

//ovp angles based
template<class VecType>
void ovpTransform(vtkMatrix4x4& M, const VecType& ovp, const VecType& trn = VecType())
{
  using Type = decltype(VecType() * VecType());
  M[0][0] = M[0][1] = M[0][2] = Type();
  M[1][0] = M[1][1] = M[1][2] = Type();
  M[2][0] = M[2][1] = M[2][2] = Type();
  M[0][0] = M[1][1] = M[2][2] = Type(1);
  M[0][3] = trn[0];  M[1][3] = trn[1]; M[2][3] = trn[2];
  Type fi = norm(ovp);
  if (fi < Type(1e-8))
    return M;
  Type cfi = cos(fi);
  Type sinc = sin(fi) / fi;
  Type cosc = (Type(1) - cfi) / (fi * fi);

  V3d<Type> A[] = { V3d<Type>(Type(0), -ovp[2], ovp[1]),
    V3d<Type>(ovp[2], Type(0), -ovp[0]),
    V3d<Type>(-ovp[1], ovp[0], Type(0)) };
  V3d<Type> E[] = { V3d<Type>(Type(1), Type(0), Type(0)),
    V3d<Type>(Type(0), Type(1), Type(0)),
    V3d<Type>(Type(0), Type(0), Type(1)) };
  V3d<Type> T[] = { V3d<Type>(ovp[0] * ovp[0], ovp[0] * ovp[1], ovp[0] * ovp[2]),
    V3d<Type>(ovp[1] * ovp[0], ovp[1] * ovp[1], ovp[1] * ovp[2]),
    V3d<Type>(ovp[2] * ovp[0], ovp[2] * ovp[1], ovp[2] * ovp[2]) };
  for (size_t i = 0; i < 3; ++i)
  {
    M[0][i] = cfi * E[0][i] + sinc * A[0][i] + cosc * T[0][i];
    M[1][i] = cfi * E[1][i] + sinc * A[1][i] + cosc * T[1][i];
    M[2][i] = cfi * E[2][i] + sinc * A[2][i] + cosc * T[2][i];
    M[i][3] = trn[i];
  }
  return M;
}

#endif
