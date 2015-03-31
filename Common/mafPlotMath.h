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
#include "vectors.h"

#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */

#define _X_IDX                       0
#define _Y_IDX                       1
#define _Z_IDX                       2
#define _W_IDX                       3
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

#endif
