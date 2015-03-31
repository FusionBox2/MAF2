/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPlotMath.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-19 11:42:32 $
  Version:   $Revision: 1.2 $
  Authors:   Vladik Aranov
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/textfile.h"
#include "wx/arrimpl.cpp"

//from mafView
#include "mafPlotMath.h"

#include "vtkTransform.h"
#include "vtkMath.h"

static DiV4d __vGZero(0.0, 0.0, 0.0, 0.0);
DiV4d const *vp4GZero = &__vGZero;

//----------------------------------------------------------------------------
// members
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 * This function is used to calculate cross product
 * 
 * @memo    Calculate cross product
 * @return  none
 * @param   vpVect1 vector [in]
 * @param   vpVect2 vector [in]
 * @param   vpOut   vector [out]
 * @see     
 */
//----------------------------------------------------------------------------
void DiV4dCrossProduct(const DiV4d *vpVect1, const DiV4d *vpVect2, DiV4d *vpOut)
//----------------------------------------------------------------------------
{
  vpOut->x = vpVect1->y * vpVect2->z - vpVect1->z * vpVect2->y;
  vpOut->y = vpVect1->z * vpVect2->x - vpVect1->x * vpVect2->z;
  vpOut->z = vpVect1->x * vpVect2->y - vpVect1->y * vpVect2->x;
  vpOut->w = 0.0f;
}

/**
 * This function identities the whole matrix.
 *
 * @memo    Identity whole matrix
 * @return  none
 * @param   mpM pointer to the matrix to be identitied
 */
void DiMatrixIdentity(DiMatrix *mpM)
{
  wxASSERT(mpM != NULL);
  
  mpM->vRight.y = mpM->vRight.z =
  mpM->vUp.x    = mpM->vUp.z    =
  mpM->vAt.x    = mpM->vAt.y    =
  mpM->vPos.x   = mpM->vPos.y   = mpM->vPos.z = 0.0f;
  mpM->vRight.x = mpM->vUp.y = mpM->vAt.z = mpM->vPos.w = 1.0f;
  mpM->vRight.w = mpM->vAt.w = mpM->vUp.w = 0.0f;

  return;
} // end of DiMatrixIdentity

/**
 * This function is used to test given matrix integrity
 *
 * @memo    test matrix integrity
 * @return  none
 * @param   mpMatrix  matrix to test
 * @see     
 */
void DiMatrixTestIntegrity(const DiMatrix *mpMatrix)
{
  
  wxASSERT(mpMatrix != NULL);
  if (!(mpMatrix->vRight.w == 0 &&
        mpMatrix->vUp.w == 0    &&
        mpMatrix->vAt.w == 0    &&
        mpMatrix->vPos.w == 1))
  {
  	wxASSERT(false);
  } 

  return;
} // end of DiMatrixTestIntegrity


/**
 * This function is used to test given matrix integrity
 *
 * @memo    test matrix integrity
 * @return  none
 * @param   mpMatrix  matrix to test
 * @see     
 */
bool DiMatrixTestIdentity(const DiMatrix *mpMatrix)
{
  
  wxASSERT(mpMatrix != NULL);
  const double eps = 1.e-6;
  if(fabs(mpMatrix->vRight.x - 1.0) > eps)
    return false;
  if(fabs(mpMatrix->vRight.y - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vRight.z - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vRight.w - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.x - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.y - 1.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.z - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.w - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.x - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.y - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.z - 1.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.w - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vPos.x - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vPos.y - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vPos.z - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vPos.w - 1.0) > eps)
    return false;
  return true;
} // end of DiMatrixTestIdentity
bool DiMatrixTestRotIdentity(const DiMatrix *mpMatrix)
{
  wxASSERT(mpMatrix != NULL);
  const double eps = 1.e-6;
  if(fabs(mpMatrix->vRight.x - 1.0) > eps)
    return false;
  if(fabs(mpMatrix->vRight.y - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vRight.z - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vRight.w - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.x - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.y - 1.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.z - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vUp.w - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.x - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.y - 0.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.z - 1.0) > eps)
    return false;
  if(fabs(mpMatrix->vAt.w - 0.0) > eps)
    return false;
  return true;
} // end of DiMatrixTestIdentity

/**
 * This function scales matrix according to the specified operation.
 *
 * @memo    Scales matrix according to specified operation
 * @return  TRUE on success, FALSE otherwise
 * @param   mpMat      [in/out] pointer to matrix to be scaled
 * @param   vpScale    [in] pointer to scale constant (vector)
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixIntergrityTest
 * @see     DiMatrixIdentity
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixAdd
 * @see     DiMatrixSubtract
 * @see     DiMatrixScale
 * @see     DiMatrixTranslate
 */
void DiMatrixScale(DiMatrix *mpMat, DiV4d *vpScale, DiOpCombainType nOperation)
{
  wxASSERT(mpMat);
  wxASSERT(vpScale);

  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;
    diDECLARE(DiV4d,    vB, 16) = *vpScale;

    _diMatrixScaleOld(&mA, &vB, nOperation);
#endif

  switch (nOperation)
  {
    case diREPLACE:
    {
      DiMatrixIdentity(mpMat);
      
      mpMat->vRight.x = vpScale->x;
      mpMat->vUp.y    = vpScale->y;
      mpMat->vAt.z    = vpScale->z;
      
      
      break;
    } // end case diREPLACE:

    case diPRECONCAT:
    {
      DiMatrixTestIntegrity(mpMat);

      DiV4dScale(&mpMat->vRight, vpScale->x, &mpMat->vRight);
      DiV4dScale(&mpMat->vUp, vpScale->y, &mpMat->vUp);
      DiV4dScale(&mpMat->vAt, vpScale->z, &mpMat->vAt);
       

      // eliminate w=1 in rotation
      mpMat->vRight.w = mpMat->vUp.w = mpMat->vAt.w = 0.f;
      
      break;
    } // end case diPRECONCAT:

    case diPOSTCONCAT:
    {
      DiMatrixTestIntegrity(mpMat);

      mpMat->vRight.x = mpMat->vRight.x * vpScale->x;
      mpMat->vRight.y = mpMat->vRight.y * vpScale->y;
      mpMat->vRight.z = mpMat->vRight.z * vpScale->z;

      mpMat->vUp.x    = mpMat->vUp.x * vpScale->x;
      mpMat->vUp.y    = mpMat->vUp.y * vpScale->y;
      mpMat->vUp.z    = mpMat->vUp.z * vpScale->z;

      mpMat->vAt.x    = mpMat->vAt.x * vpScale->x;
      mpMat->vAt.y    = mpMat->vAt.y * vpScale->y;
      mpMat->vAt.z    = mpMat->vAt.z * vpScale->z;

      mpMat->vPos.x   = mpMat->vPos.x * vpScale->x;
      mpMat->vPos.y   = mpMat->vPos.y * vpScale->y;
      mpMat->vPos.z   = mpMat->vPos.z * vpScale->z;

      break;
    } // end case diPOSTCONCAT:
  } // end switch (nOperation)

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixScale

/**
 * This function translates matrix according to the specified operation
 *
 * @memo    Translate matrix according to specified operation
 * @return  none
 * @param   mpMat      [in/out] pointer to matrix to be translated
 * @param   vpTran     [in] pointer to translate point
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixIntergrityTest
 * @see     DiMatrixIdentity
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixAdd
 * @see     DiMatrixSubtract
 * @see     DiMatrixScale
 * @see     DiMatrixTranslate
 */
void DiMatrixTranslate(DiMatrix *mpMat, DiV4d *vpTran, DiOpCombainType nOperation)
{
  wxASSERT(mpMat);
  wxASSERT(vpTran);
  
  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;
    diDECLARE(DiV4d,    vB, 16) = *vpTran;

    _diMatrixTranslateOld(&mA, &vB, nOperation);
#endif

  DiMatrixTestIntegrity(mpMat);

  switch (nOperation)
  {
    case diREPLACE:
    {
      mpMat->vPos.x = vpTran->x;
      mpMat->vPos.y = vpTran->y;
      mpMat->vPos.z = vpTran->z;

      break;
    } // end case diREPLACE:

    case diPRECONCAT:
    {
      mpMat->vPos.x += vpTran->x * mpMat->vRight.x +
                       vpTran->y * mpMat->vUp.x +
                       vpTran->z * mpMat->vAt.x;
      mpMat->vPos.y += vpTran->x * mpMat->vRight.y +
                       vpTran->y * mpMat->vUp.y +
                       vpTran->z * mpMat->vAt.y;
      mpMat->vPos.z += vpTran->x * mpMat->vRight.z +
                       vpTran->y * mpMat->vUp.z +
                       vpTran->z * mpMat->vAt.z;
      
      break;
    } // end case diPRECONCAT:

    case diPOSTCONCAT:
    {
      DiV4dAdd(&(mpMat->vPos), vpTran, &(mpMat->vPos));
      
      break;
    } // end case diPOSTCONCAT:
  } // end switch (nOperation)

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixTranslate

/*
 * This function rotates matrix around according to specified operation.
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to the matrix to be rotated
 * @param   rSin       [in] sine angle of rotation
 * @param   rCos       [in] cosine angle of rotation
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 */
void _diMatrixRotateAtSinCosLeft(DiMatrix *mpMat, double rSin, double rCos, DiOpCombainType nOperation)
{
  DiV4d vRight;
  DiV4d vUp;
  DiV4d vAt;
  DiV4d vPos;


  DiMatrixTestIntegrity(mpMat);

  // flip the angle as "Right" axis actually points to the left
  rSin = -rSin;

  wxASSERT((rSin*rSin + rCos*rCos) < 1.003f);
  wxASSERT((rSin*rSin + rCos*rCos) > 0.997f);

  switch (nOperation)
  {
    case diREPLACE:
    {
      // Fill matrix rotate around At
      mpMat->vAt.x    = mpMat->vAt.y =
      mpMat->vRight.z = mpMat->vUp.z = 0.0f;
      mpMat->vAt.z    = 1.0f;
      mpMat->vRight.x =  rCos;
      mpMat->vUp.y    =  rCos;
      mpMat->vRight.y = -rSin;
      mpMat->vUp.x    =  rSin;
      
       
      
      break;
    } // end case diREPLACE:

    case diPRECONCAT:
    {
      // enhance data access
      vRight = mpMat->vRight;
      vUp    = mpMat->vUp;

      // perform rotation
      DiV4dLineComb(&vRight, rCos, &vUp, -rSin, &(mpMat->vRight));
      mpMat->vRight.w = 0.f;
      DiV4dLineComb(&vRight, rSin, &vUp, rCos, &(mpMat->vUp));
      mpMat->vUp.w = 0.f;
       
      
      break;
    } // end case diPRECONCAT:

    case diPOSTCONCAT:
    {
      // enhance data access
      vRight = mpMat->vRight;
      vUp    = mpMat->vUp;
      vAt    = mpMat->vAt;
      vPos   = mpMat->vPos;

      // perform rotation
      mpMat->vRight.x =  vRight.x * rCos + vRight.y * rSin;
      mpMat->vUp.x    =  vUp.x    * rCos + vUp.y    * rSin;
      mpMat->vAt.x    =  vAt.x    * rCos + vAt.y    * rSin;
      mpMat->vPos.x   =  vPos.x   * rCos + vPos.y   * rSin;
      mpMat->vRight.y = -vRight.x * rSin + vRight.y * rCos;
      mpMat->vUp.y    = -vUp.x    * rSin + vUp.y    * rCos;
      mpMat->vAt.y    = -vAt.x    * rSin + vAt.y    * rCos;
      mpMat->vPos.y   = -vPos.x   * rSin + vPos.y   * rCos;
      
      break;
    } // end case diPOSTCONCAT:
  } // end switch (nOperation)


  return;
} // end of _diMatrixRotateAtSinCosLeft

/**
 * This function rotates matrix around according to specified operation.
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to the matrix to be rotated
 * @param   rSin       [in] sine angle of rotation
 * @param   rCos       [in] cosine angle of rotation
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixRotateAtSinCos
 * @see     DiMatrixRotateAt
 * @see     DiMatrixRotateRightSinCos
 * @see     DiMatrixRotateRight
 * @see     DiMatrixRotateUpSinCos
 * @see     DiMatrixRotateUp
 */
void DiMatrixRotateAtSinCosLeft(DiMatrix *mpMat, double rSin, double rCos, DiOpCombainType nOperation)
{
  wxASSERT(mpMat);

  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;

    _diMatrixRotateAtSinCosOld(&mA, rSin, rCos, nOperation);
#endif

  // body of function
  _diMatrixRotateAtSinCosLeft(mpMat, rSin, rCos, nOperation);

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixRotateAtSinCosLeft

/**
 * This function rotates matrix around, according to specified operation
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to matrix to be rotated
 * @param   rAngle     [in] angle of rotation (in radians)
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixRotateAtSinCos
 * @see     DiMatrixRotateAt
 * @see     DiMatrixRotateRightSinCos
 * @see     DiMatrixRotateRight
 * @see     DiMatrixRotateUpSinCos
 * @see     DiMatrixRotateUp
 */
void DiMatrixRotateAt(DiMatrix *mpMat, double rAngle, DiOpCombainType nOperation)
{
  double rSin, rCos;

  wxASSERT(mpMat);

  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;

    _diMatrixRotateAtOld(&mA, rAngle, nOperation);
#endif

  // untegrity
  DiMatrixTestIntegrity(mpMat);

  rSin = sin(rAngle);
  rCos = cos(rAngle);

  // body of function
  DiMatrixRotateAtSinCos(mpMat, rSin, rCos, nOperation);

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixRotateAt

/*
 * This function rotates matrix around Right (?) according to specified operation
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to matrix to be rotated
 * @param   rSin       [in] rotation angle sine
 * @param   rCos       [in] rotation angle cosine
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 */
void _diMatrixRotateRightSinCos(DiMatrix *mpMat, double rSin, 
                                           double  rCos, DiOpCombainType nOperation)
{
  DiV4d vRight;
  DiV4d vUp;
  DiV4d vAt;
  DiV4d vPos;

 
  DiMatrixTestIntegrity(mpMat);

  // flip the angle as "Right" axis actually points to the left
  rSin = -rSin;
  
  //ensure parameters
  wxASSERT((rSin*rSin + rCos*rCos) < 1.003f);
  wxASSERT((rSin*rSin + rCos*rCos) > 0.997f);

  switch (nOperation)
  {
    case diREPLACE:
    {
      // Fill rotate around Up matrix
      mpMat->vRight.y = mpMat->vRight.z =
      mpMat->vUp.x    = mpMat->vAt.x = 0.0f;
      mpMat->vRight.x = 1.0f;
      mpMat->vUp.y    =  rCos;
      mpMat->vAt.z    =  rCos;
      mpMat->vUp.z    = -rSin;
      mpMat->vAt.y    =  rSin;
  
      break;
    } // end case diREPLACE:

    case diPRECONCAT:
    {
      // enhance data access
      vUp    = mpMat->vUp;
      vAt    = mpMat->vAt;
      
      // perform rotation
      DiV4dLineComb(&vUp, rCos, &vAt, -rSin, &(mpMat->vUp));
      mpMat->vUp.w = 0.f;
      DiV4dLineComb(&vUp, rSin, &vAt, rCos, &(mpMat->vAt));
      mpMat->vAt.w = 0.f;
      
      break;
    } // end case diPRECONCAT:

    case diPOSTCONCAT:
    {
      // enhance data access
      vRight = mpMat->vRight;
      vUp    = mpMat->vUp;
      vAt    = mpMat->vAt;
      vPos   = mpMat->vPos;
      
      // perform rotation
      mpMat->vRight.y =  vRight.y * rCos + vRight.z * rSin;
      mpMat->vUp.y    =  vUp.y    * rCos + vUp.z    * rSin;
      mpMat->vAt.y    =  vAt.y    * rCos + vAt.z    * rSin;
      mpMat->vPos.y   =  vPos.y   * rCos + vPos.z   * rSin;
      mpMat->vRight.z = -vRight.y * rSin + vRight.z * rCos;
      mpMat->vUp.z    = -vUp.y    * rSin + vUp.z    * rCos;
      mpMat->vAt.z    = -vAt.y    * rSin + vAt.z    * rCos;
      mpMat->vPos.z   = -vPos.y   * rSin + vPos.z   * rCos;

      break;
    } // end case diPOSTCONCAT:
  } // end switch (nOperation)

  return;
} // end _diMatrixRotateRightSinCos

/**
 * This function rotates matrix around Right (?) according to specified operation
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to matrix to be rotated
 * @param   rSin       [in] rotation angle sine
 * @param   rCos       [in] rotation angle cosine
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixRotateAtSinCos
 * @see     DiMatrixRotateAt
 * @see     DiMatrixRotateRightSinCos
 * @see     DiMatrixRotateRight
 * @see     DiMatrixRotateUpSinCos
 * @see     DiMatrixRotateUp
 */
void DiMatrixRotateRightSinCos(DiMatrix *mpMat, double rSin, 
                                 double  rCos, DiOpCombainType nOperation)
{
  wxASSERT(mpMat);

  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;

    _diMatrixRotateRightSinCosOld(&mA, rSin, rCos, nOperation);
#endif

  // body of function
  _diMatrixRotateRightSinCos(mpMat, rSin, rCos, nOperation);

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixRotateRightSinCos

/**
 * This function rotates matrix around Right (?) according to specified operation
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to matrix to be rotated
 * @param   rAngle     [in] rotation angle (in radians)
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixRotateAtSinCos
 * @see     DiMatrixRotateAt
 * @see     DiMatrixRotateRightSinCos
 * @see     DiMatrixRotateRight
 * @see     DiMatrixRotateUpSinCos
 * @see     DiMatrixRotateUp
 */
void DiMatrixRotateRight(DiMatrix *mpMat, double rAngle, DiOpCombainType nOperation)
{
  double rSin, rCos;


  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;

    _diMatrixRotateRightOld(&mA, rAngle, nOperation);
#endif

  // untegrity
  DiMatrixTestIntegrity(mpMat);

  // calc sin/cos
  rSin = sin(rAngle);
  rCos = cos(rAngle);

  // body of function
  _diMatrixRotateRightSinCos(mpMat, rSin, rCos, nOperation);

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixRotateRight

/*
 * This function rotates matrix around Up according to specified operation.
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to matrix to be rotated
 * @param   rSin       [in] rotation angle sine
 * @param   rCos       [in] rotation angle cosine
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 */
void _diMatrixRotateUpSinCosLeft(DiMatrix *mpMat, double rSin, 
                                        double  rCos, DiOpCombainType nOperation)
{
  DiV4d vRight;
  DiV4d vUp;
  DiV4d vAt;
  DiV4d vPos;

  DiMatrixTestIntegrity(mpMat);

  wxASSERT((rSin*rSin + rCos*rCos) < 1.003f);
  wxASSERT((rSin*rSin + rCos*rCos) > 0.997f);

  switch (nOperation)
  {
    case diREPLACE:
    {
      // Fill matrix rotate around Up
      mpMat->vRight.y = mpMat->vUp.z =
      mpMat->vUp.x    = mpMat->vAt.y = 0.0f;
      mpMat->vUp.y    = 1.0f;
      mpMat->vRight.x =  rCos;
      mpMat->vAt.z    =  rCos;
      mpMat->vRight.z = -rSin;
      mpMat->vAt.x    =  rSin;
      
      break;
    } // end case diREPLACE:

    case diPRECONCAT:
    {
      // enhance data access
      vRight = mpMat->vRight;
      vAt    = mpMat->vAt;

      // perform rotation
      DiV4dLineComb(&vRight, rCos, &vAt, -rSin, &(mpMat->vRight));
      mpMat->vRight.w = 0.f;
      DiV4dLineComb(&vRight, rSin, &vAt, rCos, &(mpMat->vAt));
      mpMat->vAt.w = 0.f;
      
      break;
    } // end case diPRECONCAT:

    case diPOSTCONCAT:
    {
      // enhance data access
      vRight = mpMat->vRight;
      vUp    = mpMat->vUp;
      vAt    = mpMat->vAt;
      vPos   = mpMat->vPos;

      // perform rotation
      mpMat->vRight.x =  vRight.x * rCos + vRight.z * rSin;
      mpMat->vUp.x    =  vUp.x    * rCos + vUp.z    * rSin;
      mpMat->vAt.x    =  vAt.x    * rCos + vAt.z    * rSin;
      mpMat->vPos.x   =  vPos.x   * rCos + vPos.z   * rSin;
      mpMat->vRight.z = -vRight.x * rSin + vRight.z * rCos;
      mpMat->vUp.z    = -vUp.x    * rSin + vUp.z    * rCos;
      mpMat->vAt.z    = -vAt.x    * rSin + vAt.z    * rCos;
      mpMat->vPos.z   = -vPos.x   * rSin + vPos.z   * rCos;

      break;
    } // end case diPOSTCONCAT:
  } // end switch (nOperation)

  return;
} // end of _diMatrixRotateUpSinCosLeft

/**
 * This function rotates matrix around Up according to specified operation.
 *
 * @memo    Rotate matrix
 * @return  none
 * @param   mpMat      [in/out] pointer to matrix to be rotated
 * @param   rSin       [in] rotation angle sine
 * @param   rCos       [in] rotation angle cosine
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixRotateAtSinCos
 * @see     DiMatrixRotateAt
 * @see     DiMatrixRotateRightSinCos
 * @see     DiMatrixRotateRight
 * @see     DiMatrixRotateUpSinCos
 * @see     DiMatrixRotateUp
 */
void DiMatrixRotateUpSinCosLeft(DiMatrix *mpMat, double rSin, 
                              double  rCos, DiOpCombainType nOperation)
{
  wxASSERT(mpMat);

  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;

    _diMatrixRotateUpSinCosOld(&mA, rSin, rCos, nOperation);
#endif

  // body of function
  _diMatrixRotateUpSinCosLeft(mpMat, rSin, rCos, nOperation);

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixRotateUpSinCosLeft

/**
 * This function Rotates matrix around Up according to specified operation
 *
 * @memo    Rotate matrix around Up
 * @return  none
 * @param   mpMat      [in/out] pointer to the matrix to be rotated
 * @param   rAngle     [in] angle of rotation (in radians)
 * @param   nOperation [in] operation to be perfomed
 * @author  BVS, Miron
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixRotateAtSinCos
 * @see     DiMatrixRotateAt
 * @see     DiMatrixRotateRightSinCos
 * @see     DiMatrixRotateRight
 * @see     DiMatrixRotateUpSinCos
 * @see     DiMatrixRotateUp
 */
void DiMatrixRotateUp(DiMatrix *mpMat, double rAngle, DiOpCombainType nOperation)
{
  double rSin, rCos;


  // TEST START
#ifdef diMATRIX_NEW_MODE_TEST
  {
    diDECLARE(DiMatrix, mA, 16) = *mpMat;

    _diMatrixRotateUpOld(&mA, rAngle, nOperation);
#endif

  // untegrity
  DiMatrixTestIntegrity(mpMat);

  // calc sin/cos
  rSin = sin(rAngle);
  rCos = cos(rAngle);

  // body of function
  DiMatrixRotateUpSinCos(mpMat, rSin, rCos, nOperation);

  // TEST FINISH
#ifdef diMATRIX_NEW_MODE_TEST
    wxASSERT(_diMatrixCmpMatrix(mpMat, &mA));
  }
#endif

  return;
} // end of DiMatrixRotateUp




/**
 * This function is used to copy matrices
 *
 * @memo    Copy matrices
 * @return  none
 * @param   mpSrc source matrix [in]
 * @param   mpDst dest matrix [out]
 */
//----------------------------------------------------------------------------
void  DiMatrixMultiply(const DiMatrix *mpA, const DiMatrix *mpB, DiMatrix *mpOut)
//----------------------------------------------------------------------------
{
  int     nI;
  const DiV4d *vpSrcVector;
  DiV4d       *vpDstVector;

  vpSrcVector = &(mpA->vRight);
  vpDstVector = &(mpOut->vRight);
  //test
  DiMatrixTestIntegrity(mpA);
  DiMatrixTestIntegrity(mpB);
  
  for (nI = 0; nI < 4; nI++)
  {
    vpDstVector->x = vpSrcVector->x * mpB->vRight.x +
                     vpSrcVector->y * mpB->vUp.x +
                     vpSrcVector->z * mpB->vAt.x +
                     vpSrcVector->w * mpB->vPos.x;

    vpDstVector->y = vpSrcVector->x * mpB->vRight.y +
                     vpSrcVector->y * mpB->vUp.y +
                     vpSrcVector->z * mpB->vAt.y +
                     vpSrcVector->w * mpB->vPos.y;

    vpDstVector->z = vpSrcVector->x * mpB->vRight.z +
                     vpSrcVector->y * mpB->vUp.z +
                     vpSrcVector->z * mpB->vAt.z +
                     vpSrcVector->w * mpB->vPos.z;

    vpDstVector->w = vpSrcVector->x * mpB->vRight.w +
                     vpSrcVector->y * mpB->vUp.w +
                     vpSrcVector->z * mpB->vAt.w +
                     vpSrcVector->w * mpB->vPos.w;
    vpSrcVector++;
    vpDstVector++;
  }
  return;
} // end of DiMatrixMultiply

//----------------------------------------------------------------------------
void  mflMatrixToDi(vtkMatrix4x4 const *pMatrix, DiMatrix *mp)
//----------------------------------------------------------------------------
{
  DiMatrixIdentity(mp);

  mp->vPos  .x = -pMatrix->GetElement(0, 3);
  mp->vPos  .y =  pMatrix->GetElement(1, 3);
  mp->vPos  .z =  pMatrix->GetElement(2, 3);

  mp->vRight.x =  pMatrix->GetElement(0, 0);
  mp->vRight.y = -pMatrix->GetElement(1, 0);
  mp->vRight.z = -pMatrix->GetElement(2, 0);

  mp->vUp   .x = -pMatrix->GetElement(0, 1);
  mp->vUp   .y =  pMatrix->GetElement(1, 1);
  mp->vUp   .z =  pMatrix->GetElement(2, 1);

  mp->vAt   .x = -pMatrix->GetElement(0, 2);
  mp->vAt   .y =  pMatrix->GetElement(1, 2);
  mp->vAt   .z =  pMatrix->GetElement(2, 2);
}

//----------------------------------------------------------------------------
void  DiMatrixToVTK(DiMatrix const *mp, vtkMatrix4x4 *pMatrix)
//----------------------------------------------------------------------------
{
  pMatrix->SetElement(0, 3, -mp->vPos  .x);
  pMatrix->SetElement(1, 3,  mp->vPos  .y);
  pMatrix->SetElement(2, 3,  mp->vPos  .z);

  pMatrix->SetElement(0, 0,  mp->vRight.x);
  pMatrix->SetElement(1, 0, -mp->vRight.y);
  pMatrix->SetElement(2, 0, -mp->vRight.z);
                                        
  pMatrix->SetElement(0, 1, -mp->vUp   .x);
  pMatrix->SetElement(1, 1,  mp->vUp   .y);
  pMatrix->SetElement(2, 1,  mp->vUp   .z);
                                        
  pMatrix->SetElement(0, 2, -mp->vAt   .x);
  pMatrix->SetElement(1, 2,  mp->vAt   .y);
  pMatrix->SetElement(2, 2,  mp->vAt   .z);
}


#define _QUAT_COMP_ASSIGN(idx,val)   *((double *)qpQuat+(idx)) = (val)

#define EPSILON                     0.0004f

#define FPEqualTo(a,b)          (fabs((a)-(b)) < 1e-4)  
#define FPNotEqualTo(a,b)       (fabs((a)-(b)) > 1e-4)  

#ifdef OLD_COMPOSER
#define AllTriplets 8

struct EulerAngles
{
  double theta;
  double psi;
  double phi;
};
#endif
#define DET(xx, xy, yx, yy) ((xx) * (yy) - (xy) * (yx))

#define m_X_X mpIn->vRight.x
#define m_X_Y mpIn->vRight.y
#define m_X_Z mpIn->vRight.z

#define m_Y_X mpIn->vUp.x
#define m_Y_Y mpIn->vUp.y
#define m_Y_Z mpIn->vUp.z

#define m_Z_X mpIn->vAt.x
#define m_Z_Y mpIn->vAt.y
#define m_Z_Z mpIn->vAt.z

#define T_X mpIn->vPos.x
#define T_Y mpIn->vPos.y
#define T_Z mpIn->vPos.z

#define i_X_X mpOut->vRight.x
#define i_X_Y mpOut->vRight.y
#define i_X_Z mpOut->vRight.z
#define i_X_W mpOut->vRight.w

#define i_Y_X mpOut->vUp.x
#define i_Y_Y mpOut->vUp.y
#define i_Y_Z mpOut->vUp.z
#define i_Y_W mpOut->vUp.w

#define i_Z_X mpOut->vAt.x
#define i_Z_Y mpOut->vAt.y
#define i_Z_Z mpOut->vAt.z
#define i_Z_W mpOut->vAt.w

#define i_W_X mpOut->vPos.x
#define i_W_Y mpOut->vPos.y
#define i_W_Z mpOut->vPos.z
#define i_W_W mpOut->vPos.w


/**
 * detailed description
 *
 * @memo    <memo>
 * @return  <return>
 * @param   mpMatrix
 * @param   qpQuat
 */
//----------------------------------------------------------------------------
void DiQuatBuildFromMatrix(DiMatrix const *mpMatIn, DiQuaternion *qpQuat)
//----------------------------------------------------------------------------
{
  double         sbTrace;
  DiMatrix        mStore;
  DiMatrix const  *mpMat = &mStore;
  double         rS, rSub, rV;
  int         nI, nJ, nK;
  static int  _naNext[3] = {_Y_IDX, _Z_IDX, _X_IDX};

  wxASSERT(mpMat != NULL);
  wxASSERT(qpQuat != NULL);
  
  // ensure we have a correct picture of matrix and quaternion layouts
  wxASSERT(offsetof(DiMatrix,vRight) + sizeof(DiV4d)==offsetof(DiMatrix,vUp) &&
           offsetof(DiMatrix,vUp) + sizeof(DiV4d)==offsetof(DiMatrix,vAt));
  wxASSERT(offsetof(DiQuaternion,w) == _W_IDX * sizeof(double));// fail => _QUAT_COMP_ASSIGN wrong
  //access
  mpMat = mpMatIn;
 
  // get trace
  sbTrace = mpMat->vRight.x + mpMat->vUp.y + mpMat->vAt.z;

  // positive trace case is straightforward
  if (sbTrace > 0.0f)
  {
    // apply solution
    rS = sqrt(sbTrace + 1.0f);
    qpQuat->w = rS * 0.5f;
    rS = 0.5f / rS;
    qpQuat->x = rS * (mpMat->vAt.y    - mpMat->vUp.z);
    qpQuat->y = rS * (mpMat->vRight.z - mpMat->vAt.x);
    qpQuat->z = rS * (mpMat->vUp.x    - mpMat->vRight.y);
  }
  else // have a negative trace
  {
    //obv. table
    wxASSERT(_MATR_EL(0,0) == mpMat->vRight.x);
    wxASSERT(_MATR_EL(1,0) == mpMat->vRight.y);
    wxASSERT(_MATR_EL(2,0) == mpMat->vRight.z);
    wxASSERT(_MATR_EL(0,1) == mpMat->vUp.x);
    wxASSERT(_MATR_EL(1,1) == mpMat->vUp.y);
    wxASSERT(_MATR_EL(2,1) == mpMat->vUp.z);
    wxASSERT(_MATR_EL(0,2) == mpMat->vAt.x);
    wxASSERT(_MATR_EL(1,2) == mpMat->vAt.y);
    wxASSERT(_MATR_EL(2,2) == mpMat->vAt.z);

    // get start index
    nI = _X_IDX;
    if (_MATR_EL(_Y_IDX,_Y_IDX) > _MATR_EL(_X_IDX,_X_IDX))
    {
      nI = _Y_IDX;
    }
    if (_MATR_EL(_Z_IDX,_Z_IDX) > _MATR_EL(nI,nI))
    {
      nI = _Z_IDX;
    }
    // construct two rest indices
    nJ = _naNext[nI];
    nK = _naNext[nJ];

    // apply solution
    rSub      = _MATR_EL(nJ,nJ) + _MATR_EL(nK,nK);
    rS        = sqrt(_MATR_EL(nI,nI) - rSub + 1.0f);
    _QUAT_COMP_ASSIGN(nI, rS * 0.5f);
    rS        = 0.5f / rS;
    qpQuat->w = rS * (_MATR_EL(nJ,nK)  - _MATR_EL(nK,nJ));
    rV        = rS * (_MATR_EL(nI,nJ)  + _MATR_EL(nJ,nI));
    _QUAT_COMP_ASSIGN(nJ, rV);
    rV        = rS * (_MATR_EL(nI,nK)  + _MATR_EL(nK,nI));
    _QUAT_COMP_ASSIGN(nK, rV);
  } // trace cases

  wxASSERT(fabs((qpQuat->x * qpQuat->x + qpQuat->y * qpQuat->y + qpQuat->z * qpQuat->z + qpQuat->w * qpQuat->w) - 1.0f) <  EPSILON * 100);
 
  return;
} // end of DiQuatBuildFromMatrix

/**
 * This function is used to build matrix by the
 * given quaternion. 
 *
 * @memo    Build matrix by the given quaternion
 * @return  none
 * @param   qpQuat   quaternion [in]
 * @param   mpMatrix matrix [out]
 */
//----------------------------------------------------------------------------
void  DiQuatBuildMatrix(DiQuaternion *qpQuat, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  double   fXX , fXY , fXZ;
  double   fYY , fYZ;
  double   fZZ ;
  double   fWX , fWY , fWZ;
  double   fS;
  double   rXX, rYY, rZZ, rDiv;

  wxASSERT(mpMatrix != NULL);
  wxASSERT(qpQuat != NULL);

	// fixed part
	// - vectors
  mpMatrix->vRight.w  = 
  mpMatrix->vUp.w     = 
  mpMatrix->vAt.w     = 
  // - position
  mpMatrix->vPos.x = 
  mpMatrix->vPos.y = 
  mpMatrix->vPos.z = 0.f;
  mpMatrix->vPos.w = 1.f;

	// math part
  rXX = qpQuat->x * qpQuat->x;
  rYY = qpQuat->y * qpQuat->y;
  rZZ = qpQuat->z * qpQuat->z;

  rDiv = rXX + rYY + rZZ + (qpQuat->w * qpQuat->w);
  wxASSERT(fabs(rDiv) > 1e-7f);
  fS = 2.0f / rDiv;
  fYY = fS * (rYY);
  fZZ = fS * (rZZ);

  // right vector
  mpMatrix->vRight.x = (1.0f - (fYY + fZZ) );
  fXY = fS * (qpQuat -> x * qpQuat -> y);
  fWZ = fS * (qpQuat -> w * qpQuat -> z);
  mpMatrix->vRight.y = (fXY - fWZ          );
  fXZ = fS * (qpQuat -> x * qpQuat -> z);
  fWY = fS * (qpQuat -> w * qpQuat -> y);
  mpMatrix->vRight.z = (fXZ + fWY          );

  // up vector
  mpMatrix->vUp.x = (fXY + fWZ           );
  fXX = fS * (rXX);
  mpMatrix->vUp.y = (1.0f - (fXX + fZZ)  );
  fYZ = fS * (qpQuat -> y * qpQuat -> z);
  fWX = fS * (qpQuat -> w * qpQuat -> x);
  mpMatrix->vUp.z = (fYZ - fWX           );

  // at vector
  mpMatrix->vAt.x = (fXZ - fWY           );
  mpMatrix->vAt.y = (fYZ + fWX           );
  mpMatrix->vAt.z = (1.0f - (fXX + fYY)  );

  return;
} // end of DiQuatBuildMatrix

/*
 * detailed description
 *
 * @memo    Perform fortran like sign function
 * @return  None
 * @param   rA
 * @param   rB
 * @author  Earnol
 */
//----------------------------------------------------------------------------
static double _mafIntGraphTransfSign(double rA, double rB)
//----------------------------------------------------------------------------
{
  double rRet;

  if(rB > 0.0f)
  {
    rRet = fabs(rA);
  }
  else if(rB < 0.0f)
  {
    rRet = -fabs(rA);
  }
  else
  {
    rRet = 0.0f;
  }
  return (rRet);
} // end of _mafIntGraphTransfSign

/**
 * detailed description
 *
 * @memo    Mirror matrix mpIn and store into mpOut
 * @return  None
 * @param   mpIn
 * @param   mpOut
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
bool mafTransfMirrorMatrixYOX(DiMatrix *mpIn, DiMatrix *mpOut)
//----------------------------------------------------------------------------
{
  mpOut->vRight.x = -mpIn->vRight.x;
  mpOut->vRight.y =  mpIn->vRight.y;
  mpOut->vRight.z =  mpIn->vRight.z;
  mpOut->vRight.w = 0.0;

  mpOut->vUp   .x = -mpIn->vUp   .x;
  mpOut->vUp   .y =  mpIn->vUp   .y;
  mpOut->vUp   .z =  mpIn->vUp   .z;
  mpOut->vUp   .w = 0.0;

  mpOut->vAt   .x = -mpIn->vAt   .x;
  mpOut->vAt   .y =  mpIn->vAt   .y;
  mpOut->vAt   .z =  mpIn->vAt   .z;
  mpOut->vAt   .w = 0.0;

  mpOut->vPos  .x = -mpIn->vPos  .x;
  mpOut->vPos  .y =  mpIn->vPos  .y;
  mpOut->vPos  .z =  mpIn->vPos  .z;
  mpOut->vPos  .w = 1.0;

  return (TRUE);
}

/*
 * This function ortonormalizes vector system.
 *
 * @memo    Ortonormalize vector system
 * @return  none
 * @param   vpMain     [in] pointer to the Main vector
 * @param   vpSub1     [out] pointer to Sub vector N 1
 * @param   vpSub2     [out] pointer to Sub vector N 2
 * @param   bLeftRight [in] What kind of system do we want? :
                        <br>FALSE:  3rd vector = 1st vector x 2nd vector
 *                      <br>TRUE:   3rd vector = 2nd vector x 1st vector
 * @author  BVS, Miron
 */
//----------------------------------------------------------------------------
void DiMatrixOrtoNormalizeVectSys(DiV4d  *vpMain, DiV4d  *vpSub1, DiV4d  *vpSub2, bool bLeftRight)
//----------------------------------------------------------------------------
{
  double rMain_Sub1, rSub1Norma, rCoeff;

  wxASSERT(DiV4dDotProduct(vpMain, vpMain) > 0.00001f);
  wxASSERT(DiV4dDotProduct(vpSub1, vpSub1) > 0.00001f);

  // Normalize main vector
  // DiV4dNormalize(vpMain, vpMain);
  DiV4dMakeUnit(vpMain);

  // Ortogonalize sub vector N 1
  rMain_Sub1 = DiV4dDotProduct(vpMain, vpSub1);
  DiV4dShiftComb(vpSub1, vpMain, -rMain_Sub1, vpSub1);

  // Normalize sub vector N 1
  rSub1Norma = sqrt(DiV4dDotProduct(vpSub1, vpSub1));

  if (rSub1Norma < 0.0001f)
  {
    rMain_Sub1 = DiV4dDotProduct(vpMain, vpSub2);
    DiV4dShiftComb(vpSub2, vpMain, -rMain_Sub1, vpSub2);
    DiV4dNormalize(vpSub2, vpSub1);
  }
  else
  {
    rCoeff = 1.f / rSub1Norma;
    DiV4dScale(vpSub1, rCoeff, vpSub1);
  } // end if (rSub1Norma == 0.f)

  // Get sub vector N 2 as cross product of MainVector x SubVector1
  if (!bLeftRight)
  {
    DiV4dCrossProduct(vpMain, vpSub1, vpSub2);
  }
  else
  {
    DiV4dCrossProduct(vpSub1, vpMain, vpSub2);
  } // end if (!bLeftRight)

  return;
} // end of DiMatrixOrtoNormalizeVectSys

/**
 * @memo    Smart ortonormalization
 * @return  None
 * @param   vpFirst
 * @param   vpSecond
 * @param   vpThird
 * @param   bLeftRight
 * @author  Earnol
 */
//----------------------------------------------------------------------------
void mafTransfOrtoNormalizeMatrix(DiMatrix *mpMat, DiV4d const*vpRot, bool bLeftRight)
//----------------------------------------------------------------------------
{
  DiMatrix mA   ;
  DiMatrix mB   ;
  DiMatrix mC   ;
  DiV4d    vTmp ;
  DiV4d    vARot;
  DiV4d    vBRot;
  DiV4d    vCRot;
  double rA, rB, rC;

  //compare 1
  DiMatrixCopy(mpMat, &mA);
  DiMatrixOrtoNormalizeVectSys(&mA.vRight, &mA.vUp, &mA.vAt, bLeftRight);
  mafTransfInverseTransformUpright(&mA, &vTmp, &vARot);
  rA = DiV4dPointDistance2(&vARot, vpRot);
  
  //compare 2
  DiMatrixCopy(mpMat, &mB);
  DiMatrixOrtoNormalizeVectSys(&mB.vUp, &mB.vAt, &mB.vRight, bLeftRight);
  mafTransfInverseTransformUpright(&mB, &vTmp, &vBRot);
  rB = DiV4dPointDistance2(&vBRot, vpRot);

  //compare 3
  DiMatrixCopy(mpMat, &mC);
  DiMatrixOrtoNormalizeVectSys(&mC.vAt, &mC.vRight, &mC.vUp, bLeftRight);
  mafTransfInverseTransformUpright(&mC, &vTmp, &vCRot);
  rC = DiV4dPointDistance2(&vCRot, vpRot);

  if(rA <= rB && rA <= rC)
  {
    DiMatrixCopy(&mA, mpMat);
  }
  else if(rB <= rA && rB <= rC)
  {
    DiMatrixCopy(&mB, mpMat);
  }
  else if(rC <= rA && rC <= rB)
  {
    DiMatrixCopy(&mC, mpMat);
  }
  // fix bug of DiMatrixOrtoNormalizeVectSys
  mpMat->vAt.w = 0.0f;
  mpMat->vUp.w = 0.0f;
  mpMat->vRight.w = 0.0f;
  return;
} // end of mafTransfOrtoNormalizeMatrix


//----------------------------------------------------------------------------
void mafAttVecToVTKMat(double ThetaIn[3], vtkMatrix4x4 *pMat)
//----------------------------------------------------------------------------
{
	const double rCoeff = vtkMath::Pi()/180.0;
  DiV4d    vRot;
  DiMatrix mMat;

  vRot.x = rCoeff * ThetaIn[0];
  vRot.y = rCoeff * ThetaIn[1];
  vRot.z = rCoeff * ThetaIn[2];
  mafTransfTransformUpright(vp4GZero, &vRot, &mMat);
  DiMatrixToVTK(&mMat, pMat);
}

/**
 * detailed description
 *
 * @memo    Transform RAW data for upright matrix
 * @return  None
 * @param   vpPos IN
 * @param   vpRot IN
 * @param   mpOut OUT
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
bool mafTransfTransformUpright(DiV4d const *vpPos, DiV4d const *vpRot, DiMatrix *mpOut)
//----------------------------------------------------------------------------
{
  DiV4d     vRot;
  DiMatrix  mC;
  double  rNorm, rLen;
  double  rLCos, rLSin;
  double  rLCosPerNorm, rLSinPerLen;
  DiMatrix *mpMat;

  if(vpRot->x == 0.0f && vpRot->y == 0.0f && vpRot->z == 0.0f)
  {
    DiMatrixIdentity(&mC);
  }
  else
  {
    //for sure
    mpMat = &mC;
    wxASSERT(_MATR_EL(0,0) == mpMat->vRight.x);
    wxASSERT(_MATR_EL(1,0) == mpMat->vRight.y);
    wxASSERT(_MATR_EL(2,0) == mpMat->vRight.z);
    wxASSERT(_MATR_EL(0,1) == mpMat->vUp.x);
    wxASSERT(_MATR_EL(1,1) == mpMat->vUp.y);
    wxASSERT(_MATR_EL(2,1) == mpMat->vUp.z);
    wxASSERT(_MATR_EL(0,2) == mpMat->vAt.x);
    wxASSERT(_MATR_EL(1,2) == mpMat->vAt.y);
    wxASSERT(_MATR_EL(2,2) == mpMat->vAt.z);
    DiMatrixInitialize(mpOut);
  
    // copy to local data
    DiV4dCopy(vpRot, &vRot);
    //vRot.x = -vRot.x;

    //force entire calculations to double
    rNorm = ((double)vRot.x) * vRot.x + vRot.y * vRot.y + vRot.z * vRot.z;
    rLen = sqrt(rNorm);
    //prepare trigonomery
    rLCos = cos(rLen);
    rLSin = sin(rLen);
  
    //prepare sin(Theta)/theta
    //prepare 1 - cos(Theta)/(Theta * theta)
    //precalculations
    rLSinPerLen  = rLSin / rLen;
    rLCosPerNorm = (1.0 - rLCos) / rNorm;

    //add first compound
    _MATR_EL(0,0) = (0.0                     + rLCosPerNorm * (double)(vRot.x) * vRot.x + rLCos);  
    _MATR_EL(1,0) = (rLSinPerLen * (-vRot.z) + rLCosPerNorm * (double)(vRot.x) * vRot.y        );
    _MATR_EL(2,0) = (rLSinPerLen * vRot.y    + rLCosPerNorm * (double)(vRot.x) * vRot.z        );
    _MATR_EL(3,0) = ( 0.0                    + 0.0                                          );
    _MATR_EL(0,1) = (rLSinPerLen * vRot.z    + rLCosPerNorm * (double)(vRot.y) * vRot.x        );
    _MATR_EL(1,1) = (0.0                     + rLCosPerNorm * (double)(vRot.y) * vRot.y + rLCos);
    _MATR_EL(2,1) = (rLSinPerLen * (-vRot.x) + rLCosPerNorm * (double)(vRot.y) * vRot.z        );
    _MATR_EL(3,1) = (0.0                     + 0.0                                          );
    _MATR_EL(0,2) = (rLSinPerLen * (-vRot.y) + rLCosPerNorm * (double)(vRot.z) * vRot.x        );
    _MATR_EL(1,2) = (rLSinPerLen * (vRot.x)  + rLCosPerNorm * (double)(vRot.z) * vRot.y        );
    _MATR_EL(2,2) = (0.0                     + rLCosPerNorm * (double)(vRot.z) * vRot.z + rLCos);
    _MATR_EL(3,2) = (0.0                     + 0.0                                          );
  }
  
  //set position
  DiV4dCopy(vpPos, &mC.vPos);
  mC.vPos.w = 1.0f;
  DiMatrixTestIntegrity(&mC);
  
  DiMatrixTransposeRotationalSubmatrix(&mC, mpOut);

  //perform mirroring
  mafTransfMirrorMatrixYOX(mpOut, mpOut);
  DiV4dNegate(&mpOut->vRight, &mpOut->vRight);

  //remove calculation errors
  mafTransfOrtoNormalizeMatrix(mpOut, vpRot, FALSE);

  return (TRUE);
} // end of SaTransfTransformUpright;


//----------------------------------------------------------------------------
static  bool _mafTransfInverseTransformUpright(DiMatrix const *mpIn, DiV4d *vpPos, DiV4d *vpRot)
//----------------------------------------------------------------------------
{
  DiMatrix mA;
  DiMatrix mB;
  double rCosQuat, rArcCosFrom1;
  double rQuatCosine, rSinQuat;
  DiMatrix *mpMat;
  double rEps = 1.0e-4f; 
  double rGradToRadCoeff = 1.0f;// / diPI * 180.0f;

  //init
  mpMat = &mA;
  //for sure
  wxASSERT(_MATR_EL(0,0) == mpMat->vRight.x);
  wxASSERT(_MATR_EL(1,0) == mpMat->vRight.y);
  wxASSERT(_MATR_EL(2,0) == mpMat->vRight.z);
  wxASSERT(_MATR_EL(0,1) == mpMat->vUp.x);
  wxASSERT(_MATR_EL(1,1) == mpMat->vUp.y);
  wxASSERT(_MATR_EL(2,1) == mpMat->vUp.z);
  wxASSERT(_MATR_EL(0,2) == mpMat->vAt.x);
  wxASSERT(_MATR_EL(1,2) == mpMat->vAt.y);
  wxASSERT(_MATR_EL(2,2) == mpMat->vAt.z);
  DiMatrixCopy(mpIn, mpMat);
  DiV4dNegate(&mpMat->vRight, &mpMat->vRight);    
  //perform mirroring
  mafTransfMirrorMatrixYOX(mpMat, mpMat);
  
  //init out data
  DiV4dCopy(&mpMat->vPos, vpPos);
  //transpose
  DiMatrixTransposeRotationalSubmatrix(mpMat, &mB);
  DiMatrixCopy(mpMat, &mB);
  //init rotation part
  DiV4dCopy(vp4GZero, vpRot);
  //access
  mpMat        = &mB;
  double   tmp_x, tmp_y, tmp_z;
  double   n_x, n_y, n_z;
  double   n_tmp;
  tmp_x = _MATR_EL(2,1)-_MATR_EL(1,2);
  tmp_y = _MATR_EL(0,2)-_MATR_EL(2,0);
  tmp_z = _MATR_EL(1,0)-_MATR_EL(0,1);
  n_tmp = sqrt(tmp_z*tmp_z + tmp_x*tmp_x + tmp_y*tmp_y);
  if(n_tmp <= 0.1e-7)
  {  
    rSinQuat = asin(0.5*n_tmp);
    vpRot->x = rSinQuat * tmp_x;
    vpRot->y = rSinQuat * tmp_y;
    vpRot->z = rSinQuat * tmp_z;
    vpRot->w = 1.0f;
    return (TRUE);
  }
  else
  {
    n_tmp = 1 / n_tmp;
  }
  n_x = n_tmp * tmp_x;
  n_y = n_tmp * tmp_y;
  n_z = n_tmp * tmp_z;


  rCosQuat     = 0.5f * (_MATR_EL(0,0) + _MATR_EL(1,1) + _MATR_EL(2,2) - 1.0f); // cos(q)
  rArcCosFrom1 = vtkMath::Pi(); // !!! 180 degr
  rQuatCosine = acos(DiFFitIn(rCosQuat, -1, 1)); // Ordinary case
  if(rQuatCosine > vtkMath::Pi())
  {
    double b1_x, b2_x, b3_x;
    double b1_y, b2_y, b3_y;
    double b1_z, b2_z, b3_z;
    b1_x = 0.5 * (_MATR_EL(0,0) + _MATR_EL(0,0)) - cos(rQuatCosine) * 1;
    b1_y = 0.5 * (_MATR_EL(1,0) + _MATR_EL(0,1)) - cos(rQuatCosine) * 0;
    b1_z = 0.5 * (_MATR_EL(2,0) + _MATR_EL(0,2)) - cos(rQuatCosine) * 0;

    b2_x = 0.5 * (_MATR_EL(0,1) + _MATR_EL(1,0)) - cos(rQuatCosine) * 0;
    b2_y = 0.5 * (_MATR_EL(1,1) + _MATR_EL(1,1)) - cos(rQuatCosine) * 1;
    b2_z = 0.5 * (_MATR_EL(2,1) + _MATR_EL(1,2)) - cos(rQuatCosine) * 0;
 
    b3_x = 0.5 * (_MATR_EL(0,2) + _MATR_EL(2,0)) - cos(rQuatCosine) * 0;
    b3_y = 0.5 * (_MATR_EL(1,2) + _MATR_EL(2,1)) - cos(rQuatCosine) * 0;
    b3_z = 0.5 * (_MATR_EL(2,2) + _MATR_EL(2,2)) - cos(rQuatCosine) * 1;
    double b_tmp_x, b_tmp_y, b_tmp_z;
    b_tmp_x = b1_x * b1_x + b1_y * b1_y + b1_z * b1_z;
    b_tmp_y = b2_x * b2_x + b2_y * b2_y + b2_z * b2_z;
    b_tmp_z = b3_x * b3_x + b3_y * b3_y + b3_z * b3_z;
    if(b_tmp_x >= b_tmp_y && b_tmp_x >= b_tmp_z)
    {
      n_x = b1_x / b_tmp_x; 
      n_y = b1_y / b_tmp_x; 
      n_z = b1_z / b_tmp_x; 
    }
    else if(b_tmp_y >= b_tmp_x && b_tmp_y >= b_tmp_z)
    {
      n_x = b2_x / b_tmp_y; 
      n_y = b2_y / b_tmp_y; 
      n_z = b2_z / b_tmp_y; 
    }
    else if(b_tmp_z >= b_tmp_x && b_tmp_z >= b_tmp_y)
    {
      n_x = b3_x / b_tmp_z; 
      n_y = b3_y / b_tmp_z; 
      n_z = b3_z / b_tmp_z; 
    }
    if((_MATR_EL(2,1)-_MATR_EL(1,2)) * n_x < 0)
    {
      n_x = -n_x;
      n_y = -n_y;
      n_z = -n_z;
    }
  }
  if(n_z > 0 && rQuatCosine > vtkMath::Pi() / 1.9)
  {
    n_x = -n_x;
    n_y = -n_y;                    
    n_z = -n_z;
    rQuatCosine = 2.0 * vtkMath::Pi() - rQuatCosine;
  }
  vpRot->x = rQuatCosine * n_x;
  vpRot->y = rQuatCosine * n_y;
  vpRot->z = rQuatCosine * n_z;
  vpRot->w = 1.0f;
  return (TRUE);
} // end of _mafTransfInverseTransformUpright;

/**

 * detailed description
 *
 * @memo    Transform matrix into PGD RAW data for upright 
 * @return  None
 * @param   mpIn  IN
 * @param   vpPos OUT
 * @param   vpRot OUT
 * @author  Earnol
 * @see     Nothing
 */
//----------------------------------------------------------------------------
bool mafTransfInverseTransformUpright(DiMatrix const *mpIn, DiV4d *vpPos, DiV4d *vpRot)
//----------------------------------------------------------------------------
{
  DiMatrix mA;
  DiMatrix mB;
  DiMatrix mC;
  double rCosQuat, rArcCosFrom1;
  double rTeta, rTetaOut;
  double rQuatCosine, rSinQuat, rAQ;
  double rAX, rAY, rAZ;
  DiMatrix *mpMat;
  double rEps = 1.0e-5f; 
  double rGradToRadCoeff = 1.0f;// / diPI * 180.0f;
  int  nI, nJ;
  double rDLength, rMaxSum, rSum;
  int  nIndex;
  double rDotTmp;
  double rALength;

  //init
  mpMat = &mA;
  //for sure
  wxASSERT(_MATR_EL(0,0) == mpMat->vRight.x);
  wxASSERT(_MATR_EL(1,0) == mpMat->vRight.y);
  wxASSERT(_MATR_EL(2,0) == mpMat->vRight.z);
  wxASSERT(_MATR_EL(0,1) == mpMat->vUp.x);
  wxASSERT(_MATR_EL(1,1) == mpMat->vUp.y);
  wxASSERT(_MATR_EL(2,1) == mpMat->vUp.z);
  wxASSERT(_MATR_EL(0,2) == mpMat->vAt.x);
  wxASSERT(_MATR_EL(1,2) == mpMat->vAt.y);
  wxASSERT(_MATR_EL(2,2) == mpMat->vAt.z);
 
  DiMatrixCopy(mpIn, mpMat);
  DiV4dNegate(&mpMat->vRight, &mpMat->vRight);
  //perform mirroring
  mafTransfMirrorMatrixYOX(mpMat, mpMat);
  
  //init out data
  DiV4dCopy(&mpMat->vPos, vpPos);
  //transpose
  DiMatrixTransposeRotationalSubmatrix(mpMat, &mB);
  DiMatrixCopy(mpMat, &mB);
  //init rotation part
  DiV4dCopy(vp4GZero, vpRot);
  //access
  mpMat        = &mB;
  rCosQuat     = 0.5f * (_MATR_EL(0,0) + _MATR_EL(1,1) + _MATR_EL(2,2) - 1.0f); // cos(q)
  rArcCosFrom1 = vtkMath::Pi(); // !!! 180 degr
  rQuatCosine  = 0.5f * (rArcCosFrom1 - _mafIntGraphTransfSign(rArcCosFrom1, rCosQuat));//  !!! ??????????????????????
  if(fabs(rCosQuat) < 1.0f - rEps)  // rCosQuat (-1.,1.], rQuatCosine [0,180.) degr
  {
    rQuatCosine = acos(rCosQuat); // Ordinary case
    rSinQuat = sin(rQuatCosine);
    rAQ = 0.5f * rQuatCosine * rGradToRadCoeff / rSinQuat;
    rAX = _MATR_EL(2,1) - _MATR_EL(1,2);
    rAY = _MATR_EL(0,2) - _MATR_EL(2,0);
    rAZ = _MATR_EL(1,0) - _MATR_EL(0,1);
  }
  else // rCosQuat <= -1., rQuatCosine = 180. degr
  {
    for(nI = 0; nI < 3; nI++)  // Singular case
    {
      for(nJ = 0; nJ < 3; nJ++)
      {
        rDotTmp          = 0.5f * (_MATR_EL(nI, nJ) + _MATR_EL(nJ, nI));
        mpMat            = &mC;
        _MATR_EL(nI, nJ) = (double)rDotTmp;
        mpMat            = &mB;
      }
      mpMat            = &mC;
      _MATR_EL(nI, nI) = _MATR_EL(nI, nI) - (double)cos(rQuatCosine);
      mpMat            = &mB;
    }
    nIndex  = 0;
    rMaxSum = -1.0f;
    for(nJ = 0; nJ < 3; nJ++)
    {
      rSum = 0.0f;
      for(nI = 0; nI < 3; nI++)
      {
        mpMat = &mC;      
        rSum  = rSum + fabs(_MATR_EL(nI, nJ));
        mpMat = &mB;        
      }
      if(rSum > rMaxSum) 
      {
        nIndex  = nJ;
        rMaxSum = rSum;
      }
    }
    rAQ   = rQuatCosine * rGradToRadCoeff;
    mpMat = &mC;
    rDLength = sqrt(_MATR_EL(0, nIndex) * _MATR_EL(0, nIndex) + _MATR_EL(1,nIndex)*_MATR_EL(1,nIndex) + _MATR_EL(2,nIndex)*_MATR_EL(2,nIndex));
    if(rDLength < rEps)
    {
      rALength = 1.0f;
      for(nI = 0; nI < 3; nI++)
      {
        mpMat                = &mC;
        _MATR_EL(nI, nIndex) = 0.0f;
      }
      _MATR_EL(nIndex,nIndex) = 1.0f;
    }
    else
    {
      rALength = 1.0f / rDLength;
    }

    rAX = _MATR_EL(0,nIndex) * rALength;
    rAY = _MATR_EL(1,nIndex) * rALength;
    rAZ = _MATR_EL(2,nIndex) * rALength;
  }

  //store output
  vpRot->x = (double)(rAX*rAQ);
  vpRot->y = (double)(rAY*rAQ);
  vpRot->z = (double)(rAZ*rAQ);
  _mafTransfInverseTransformUpright(mpIn, vpPos, vpRot);
  vpRot->w = 1.0f;
//
  //some data we do not need at all :)))
  rTeta = rQuatCosine * rGradToRadCoeff; // to compare with rTetaOut
  rTetaOut = sqrt(vpRot->x * vpRot->x + vpRot->y * vpRot->y + vpRot->z * vpRot->z);
  
  return (TRUE);
} // end of mafTransfInverseTransformUpright;


bool CompareVectors(const DiV4d &a, const DiV4d &b)
{
	return 
		fabs(a.x - b.x) < 0.001f  &&
		fabs(a.y - b.y) < 0.001f  &&
		fabs(a.z - b.z) < 0.001f;
}	


bool mafTransfEulerToMatrix(DiV4d *vpRot, DiMatrix *mpMat, int conv)
{
  DiQuaternion qQuat;
  DiMatrix mTransp;


  mafTransfEulerToQuaternion(vpRot, &qQuat, conv);
  DiQuatBuildMatrix(&qQuat, &mTransp);
  DiMatrixTransposeRotationalSubmatrix(&mTransp, mpMat);
  return true;
} // end of SaTransfEulerToMatrix


/**
 * detailed description
 *
 * @memo    Transform euler angles to quaternion
 * @return  None
 * @param   vpPos
 * @param   vpRot
 * @param   mpOut
 * @author  Earnol
 * @see     Nothing
 */
bool mafTransfEulerToQuaternion(DiV4d *vpRotIn, DiQuaternion *qpQuat, int conv)
{
  double a[3], ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
  int i,j,k,h,n,s,f;
  double rTmp;
  DiV4d vWork;
  DiV4d *vpRot = &vWork;

  vWork = *vpRotIn;
  EulGetOrd(conv,i,j,k,h,n,s,f);
  if (f == EulFrmR) 
  {
    rTmp = vpRot->x; vpRot->x = vpRot->z; vpRot->z = rTmp;
  }
  if (n==EulParOdd) vpRot->y = -vpRot->y;
  ti = vpRot->x * 0.5f; tj = vpRot->y * 0.5f; th = vpRot->z * 0.5f;
  ci = cos(ti);  cj = cos(tj);  ch = cos(th);
  si = sin(ti);  sj = sin(tj);  sh = sin(th);
  cc = ci*ch; cs = ci*sh; sc = si*ch; ss = si*sh;
  if (s == EulRepYes) 
  {
    a[i] = cj*(cs + sc);    /* Could speed up with */
    a[j] = sj*(cc + ss);    /* trig identities. */
    a[k] = sj*(cs - sc);
    qpQuat->w = cj*(cc - ss);
  } 
  else 
  {
    a[i] = cj*sc - sj*cs;
    a[j] = cj*ss + sj*cc;
    a[k] = cj*cs - sj*sc;
    qpQuat->w = cj*cc + sj*ss;
  }
  if (n==EulParOdd) a[j] = -a[j];
  qpQuat->x = a[0]; qpQuat->y = a[1]; qpQuat->z = a[2];
  return true;
} //end of SaTransfEulerToQuaternion


/**
 * detailed description
 *
 * @memo    See name. Pretty obv. i think
 * @return  None
 * @param   mpMat
 * @author  Earnol
 * @see     Nothing
 */
bool mafTransfIsMatrixOrtoNormalized(DiMatrix *mpMat)
{
  int nI;
  DiV4d   *vp;
  double rNorm;
  DiV4d   vT;


  DiMatrixTestIntegrity(mpMat);
  vp = (DiV4d *) mpMat;
  //check normalization
  for(nI = 0; nI < 3; nI++)
  {
    rNorm = DiV4dDotProduct(vp, vp);
    if(!(0.999f < rNorm && rNorm < 1.001f))
    {
      return (FALSE);
    }
    vp++;
  }
  //check orientation
  DiV4dCrossProduct(&mpMat->vRight, &mpMat->vUp, &vT);
  if(!CompareVectors(vT, mpMat->vAt))
  {
    return (FALSE);
  }
  //check ortogonalization
  rNorm = DiV4dDotProduct(&mpMat->vRight, &mpMat->vUp);
  if(fabs(rNorm) > 0.001f)
  {
    return (FALSE);
  }
  //check ortogonalization
  rNorm = DiV4dDotProduct(&mpMat->vRight, &mpMat->vAt);
  if(fabs(rNorm) > 0.001f)
  {
    return (FALSE);
  }

  //check ortogonalization
  rNorm = DiV4dDotProduct(&mpMat->vAt, &mpMat->vUp);
  if(fabs(rNorm) > 0.001f)
  {
    return (FALSE);
  }
  //the matrix is good
  return (TRUE);
} // end of mafTransfIsMatrixOrtoNormalized




/**
 * detailed description
 *
 * @memo    Decompose matrix into 3 axises to be rotated about
 * @return  None
 * @param   mpIn
 * @param   vpRot
 * @param   vpPos
 * @author  Earnol
 * @see     Nothing
 */
bool mafTransfDecomposeMatrix(DiMatrix *mpIn, DiV4d *vpRot, DiV4d *vpPos)
{
  bool bRet;

  //init
  bRet = FALSE;
  //check is matrix orthonormalized
  //if(mafTransfIsMatrixOrtoNormalized(mpIn))
  {
    bRet = mafTransfDecomposeMatrixStright(mpIn, vpRot, vpPos);
  }
  return (bRet);
} // end of SaTransfDecomposeMatrix

bool mafTransfDecomposeMatrixStright(DiMatrix *mpIn, DiV4d *vpRot, DiV4d *vpPos)
{
  DiV4dCopy(&(mpIn->vPos), vpPos);
  vpPos->w = 1.0;
  mafTransfMatrixToEuler(mpIn, vpRot, EulOrdXYZr);
  vpRot->w = 1.0;
  return true;

}



bool mafTransfComposeMatrixStright(DiMatrix *mpIn, DiV4d const *vpRot, DiV4d const *vpPos)
{
  DiV4d rloc;

  DiV4dCopy(vpRot, &rloc);
  mafTransfEulerToMatrix(&rloc, mpIn, EulOrdXYZr);
  DiV4dCopy(vpPos, &(mpIn->vPos));
  mpIn->vPos.w = 1.0;
  return true;
}
#ifdef OLD_COMPOSER
/**
 * detailed description
 *
 * @memo    Implement direct matrix decomposition
 * @return  None
 * @param   mpIn
 * @param   vpRot
 * @param   vpPos
 * @author  Earnol
 * @see     Nothing
 */
bool mafTransfDecomposeMatrixStright1(DiMatrix *mpIn, DiV4d *vpRot, DiV4d *vpPos)
{
  DiMatrix  mMat;
  double   rCos;

  //copy position
  DiV4dCopy(&mpIn->vPos, vpPos);

  //copy work area
  DiMatrixCopy(mpIn, &mMat);
  //construct orientation: axis apply order X,Z,Y
  if((mMat.vRight.y > 0.0001f) || (mMat.vRight.y < -0.0001f) )
  {
    rCos = 1.0f - mMat.vRight.y * mMat.vRight.y;
    rCos = DiFFitIn(rCos,0.0f,1.0f);
    rCos = sqrt(rCos);
    vpRot->z = asin(mMat.vRight.y);
    DiMatrixRotateAtSinCos(&mMat, -mMat.vRight.y, rCos,diPRECONCAT);
  }
  else
  {
    vpRot->z = 0.0f;
  }
  // Z
  if((mMat.vAt.y > 0.0001f) || (mMat.vAt.y < -0.0001f) )
  {
    rCos = 1.0f - mMat.vAt.y * mMat.vAt.y;
    rCos = DiFFitIn(rCos,0.0f,1.0f);
    rCos = sqrt(rCos);
    vpRot->x = asin(mMat.vAt.y);
    DiMatrixRotateRightSinCos(&mMat, -mMat.vAt.y, rCos,diPRECONCAT);
  }
  else
  {
    vpRot->x = 0.0f;
  }
  //Y
  if((mMat.vAt.x > 0.0001f) || (mMat.vAt.x < -0.0001f) )
  {
    vpRot->y = asin(mMat.vAt.x);
  }
  else
  {
    vpRot->y = 0.0f;
  }

  //support integrity
  vpRot->w = 1.0f;
  return (TRUE);
} // end of SaTransfDecomposeMatrixStright
#endif
/**
 * detailed description
 *
 * @memo    Make matrix from 2 vectors
 * @return  None
 * @param   mpIn
 * @param   vpRot
 * @param   vpPos
 * @author  Earnol
 * @see     Nothing
 */
bool mafTransfComposeMatrix(DiMatrix *mpIn, DiV4d *vpRot, DiV4d *vpPos)
{

  //relay
  mafTransfComposeMatrixStright(mpIn, vpRot, vpPos);
  wxASSERT(mafTransfIsMatrixOrtoNormalized(mpIn));
  return (TRUE);
} // end of SaTransfComposeMatrix
#ifdef OLD_COMPOSER
/**
 * detailed description
 *
 * @memo    See name
 * @return  None
 * @param   mpIn
 * @param   vpRot
 * @param   vpPos
 * @author  Earnol
 * @see     Nothing
 */
bool mafTransfComposeMatrixStright1(DiMatrix *mpIn, DiV4d const *vpRot, DiV4d const *vpPos)
{
  //set rotation
  DiMatrixIdentity(mpIn);
  DiMatrixRotateUp(mpIn, vpRot->y ,diPRECONCAT);
  DiMatrixRotateRight(mpIn, -vpRot->x ,diPRECONCAT);
  DiMatrixRotateAt(mpIn, vpRot->z ,diPRECONCAT);
  //set position
  DiV4dCopy(vpPos, &mpIn->vPos);

  return (TRUE);
} // end of SaTransfComposeMatrixStright

#endif

bool mafTransfMatrixToEuler(DiMatrix const *mpMat, DiV4d *vpR, int conv)
{
    int i,j,k,h,n,s,f;
    double phi, psi, theta;

    EulGetOrd(conv,i,j,k,h,n,s,f);

    if(s == EulRepYes) 
    {
	    double sy = sqrt((double)_MATR_EL(i,j) * (double)_MATR_EL(i, j) + (double)_MATR_EL(i, k) * (double)_MATR_EL(i, k));
	    if(sy > 16 * FLT_EPSILON) 
      {
	      phi = atan2((double)_MATR_EL(i, j), (double)_MATR_EL(i, k));
	      psi = atan2(sy, (double)_MATR_EL(i, i));
	      theta = atan2((double)_MATR_EL(j, i), -(double)_MATR_EL(k, i));
    	} 
      else 
      {
	      phi = atan2(-(double)_MATR_EL(j, k), (double)_MATR_EL(j, j));
	      psi = atan2(sy, (double)_MATR_EL(i, i));
	      theta = 0;
	    }
    } 
    else 
    {
	    double cy = sqrt(_MATR_EL(i, i) * _MATR_EL(i, i) + _MATR_EL(j, i) * _MATR_EL(j, i));
	    if(cy > 16 * FLT_EPSILON) 
      {
	      phi = atan2((double)_MATR_EL(k, j), (double)_MATR_EL(k, k));
	      psi = atan2(-(double)_MATR_EL(k, i), cy);
	      theta = atan2((double)_MATR_EL(j, i), (double)_MATR_EL(i, i));
    	} 
      else 
      {
	      phi = atan2(-(double)_MATR_EL(j, k), (double)_MATR_EL(j, j));
	      psi = atan2(-(double)_MATR_EL(k, i), cy);
	      theta = 0;
    	}
    }
    vpR->x = phi;
    vpR->y = psi;
    vpR->z = theta;
    if(n == EulParOdd)
    {
      vpR->x = -vpR->x;
      vpR->y = -vpR->y;
      vpR->z= - vpR->z;
    }
    if(f == EulFrmR) 
    {
      double rTmp = vpR->x; 
      vpR->x = vpR->z; 
      vpR->z = rTmp;
    }
    return true;
}

#ifdef OLD_COMPOSER
/**
 * detailed description
 *
 * @memo    Calculates Euler angles from matrix
 * @return  None
 * @param   mpR
 * @param   vpR      
 * @author  Earnol
 */
//----------------------------------------------------------------------------
bool mafTransfMatrixToEuler1(DiMatrix *mpR, DiV4d *vpR)
//----------------------------------------------------------------------------
{
  EulerAngles eulerAngles[AllTriplets];
  int     i;
  double    theta1, theta2, psi11, psi12, psi21, psi22, phi11, phi12, phi21, phi22;
  double    phi, psi1, psi2;
  DiMatrix    mA; 
  DiMatrix    *mpMat = &mA;

  DiMatrixCopy(mpR, &mA);

  //make it sane... We cannot transform any matrix to Euler
  DiMatrixOrtoNormalizeVectSys(&mA.vRight, &mA.vUp, &mA.vAt, FALSE);

  if (FPNotEqualTo(_MATR_EL(2,0), 1) && FPNotEqualTo(_MATR_EL(2,0), -1)) 
  {
    // Compute the two possible thetas
    theta1 = -asin(_MATR_EL(2,0));
    theta2 = vtkMath::Pi() - theta1;

    // Compute the four possible psis
    psi11 = asin(_MATR_EL(2,1) / cos(theta1));
    psi12 = vtkMath::Pi() - psi11;
    psi21 = asin(_MATR_EL(2,1) / cos(theta2));
    psi22 = vtkMath::Pi() - psi21;

    // Compute the four possible phis
    phi11 = asin(_MATR_EL(1,0) / cos(theta1));
    phi12 = vtkMath::Pi() - phi11;
    phi21 = asin(_MATR_EL(1,0) / cos(theta2));
    phi22 = vtkMath::Pi() - phi21;

    // Construct all eight triplets
    eulerAngles[0].theta = theta1;
    eulerAngles[0].psi = psi11;
    eulerAngles[0].phi = phi11;

    eulerAngles[1].theta = theta1;
    eulerAngles[1].psi = psi12;
    eulerAngles[1].phi = phi11;

    eulerAngles[2].theta = theta1;
    eulerAngles[2].psi = psi11;
    eulerAngles[2].phi = phi12;

    eulerAngles[3].theta = theta1;
    eulerAngles[3].psi = psi12;
    eulerAngles[3].phi = phi12;

    eulerAngles[4].theta = theta2;
    eulerAngles[4].psi = psi21;
    eulerAngles[4].phi = phi21;

    eulerAngles[5].theta = theta2;
    eulerAngles[5].psi = psi22;
    eulerAngles[5].phi = phi21;

    eulerAngles[6].theta = theta2;
    eulerAngles[6].psi = psi21;
    eulerAngles[6].phi = phi22;

    eulerAngles[7].theta = theta2;
    eulerAngles[7].psi = psi22;
    eulerAngles[7].phi = phi22;

    // Elimate possibilities
    for (i = 0; i < AllTriplets; i++) 
    {
      if (FPNotEqualTo(_MATR_EL(0,0), cos(eulerAngles[i].theta) * cos(eulerAngles[i].phi))) 
        continue;

      if (FPNotEqualTo(_MATR_EL(0,1), sin(eulerAngles[i].psi) * sin(eulerAngles[i].theta) * cos(eulerAngles[i].phi) - 
                         cos(eulerAngles[i].psi) * sin(eulerAngles[i].phi))) 
        continue;

      if (FPNotEqualTo(_MATR_EL(0,2), cos(eulerAngles[i].psi) * sin(eulerAngles[i].theta) * cos(eulerAngles[i].phi) +
                         sin(eulerAngles[i].psi) * sin(eulerAngles[i].phi))) 
        continue;

      if (FPNotEqualTo(_MATR_EL(1,1), sin(eulerAngles[i].psi) * sin(eulerAngles[i].theta) * sin(eulerAngles[i].phi) +
                         cos(eulerAngles[i].psi) * cos(eulerAngles[i].phi))) 
        continue;

      if (FPNotEqualTo(_MATR_EL(1,2), cos(eulerAngles[i].psi) * sin(eulerAngles[i].theta) * sin(eulerAngles[i].phi) - 
                         sin(eulerAngles[i].psi) * cos(eulerAngles[i].phi))) 
        continue;

      if (FPNotEqualTo(_MATR_EL(2,2), cos(eulerAngles[i].psi) * cos(eulerAngles[i].theta))) 
        continue;

      // This is valid triplet.  Return with this one.
      vpR->x = (eulerAngles[i].psi);
      vpR->y = (eulerAngles[i].theta);
      vpR->z = (eulerAngles[i].phi);
      return (TRUE);
    }
    // Return at least something here
    vpR->x = (eulerAngles[0].psi);
    vpR->y = (eulerAngles[0].theta);
    vpR->z = (eulerAngles[0].phi);
    return (FALSE);
  } 
  else 
  {
    // Get the two possible triplets for PI / 2
    theta1 = vtkMath::Pi() / 2;
    phi = 0;
    psi1 = asin(_MATR_EL(0,1));
    psi2 = vtkMath::Pi() - psi1;

    eulerAngles[0].theta = theta1;
    eulerAngles[0].psi = psi1;
    eulerAngles[0].phi = phi;

    eulerAngles[1].theta = theta1;
    eulerAngles[1].psi = psi2;
    eulerAngles[1].phi = phi;

    // See which is a valid solution
    if (FPEqualTo(_MATR_EL(0,2), cos(psi1))) 
    {
      vpR->x = (eulerAngles[0].psi  );
      vpR->y = (eulerAngles[0].theta);
      vpR->z = (eulerAngles[0].phi  );
      return (TRUE);
    }
    if (FPEqualTo(_MATR_EL(0,2), cos(psi2))) 
    {
      vpR->x = (eulerAngles[1].psi  );
      vpR->y = (eulerAngles[1].theta);
      vpR->z = (eulerAngles[1].phi  );
      return (TRUE);
    }

    // Get the two possible triplets for -PI / 2
    theta2 = -vtkMath::Pi() / 2;
    phi = 0;
    psi1 = -asin(_MATR_EL(0,1));
    psi2 = vtkMath::Pi() - psi1;

    eulerAngles[2].theta = theta2;
    eulerAngles[2].psi = psi1;
    eulerAngles[2].phi = phi;

    eulerAngles[3].theta = theta2;
    eulerAngles[3].psi = psi2;
    eulerAngles[3].phi = phi;

    // See which is a valid solution
    if (FPEqualTo(_MATR_EL(0,2), -cos(psi1))) 
    {
      vpR->x = (eulerAngles[2].psi  );
      vpR->y = (eulerAngles[2].theta);
      vpR->z = (eulerAngles[2].phi  );
      return (TRUE);
    }
    if (FPEqualTo(_MATR_EL(0,2), -cos(psi2))) 
    {
      vpR->x = (eulerAngles[3].psi  );
      vpR->y = (eulerAngles[3].theta);
      vpR->z = (eulerAngles[3].phi  );
      return (TRUE);
    }
    //return something
    vpR->x = (eulerAngles[3].psi  );
    vpR->y = (eulerAngles[3].theta);
    vpR->z = (eulerAngles[3].phi  );
    return (FALSE);
  }
  return (FALSE);
}
#endif


/**
 * This function transposes rotational submatrix.
 *
 * @memo    Transpose rotational submatrix
 * @return  none
 * @param   mpMatSrc [in] pointer to the source matrix
 * @param   mpMatDst [out] pointer to the destination matrix
 */
//----------------------------------------------------------------------------
void DiMatrixTransposeRotationalSubmatrix(const DiMatrix *mpMatSrc, DiMatrix *mpMatDst)
//----------------------------------------------------------------------------
{
  DiMatrixTestIntegrity(mpMatSrc);

  // Copy non rotational component
  mpMatDst->vPos   = mpMatSrc->vPos;

  // Copy transposed rotational component
  mpMatDst->vRight.x = mpMatSrc->vRight.x;
  mpMatDst->vRight.y = mpMatSrc->vUp.x;
  mpMatDst->vRight.z = mpMatSrc->vAt.x;
  mpMatDst->vRight.w = 0.f;
  mpMatDst->vUp.x = mpMatSrc->vRight.y;
  mpMatDst->vUp.y = mpMatSrc->vUp.y;
  mpMatDst->vUp.z = mpMatSrc->vAt.y;
  mpMatDst->vUp.w = 0.f;
  mpMatDst->vAt.x = mpMatSrc->vRight.z;
  mpMatDst->vAt.y = mpMatSrc->vUp.z;
  mpMatDst->vAt.z = mpMatSrc->vAt.z;
  mpMatDst->vAt.w = 0.f;

  return;
}

/**
 * This function adds 2 matrices and places result to the third matrix.
 *
 * @memo    Add 2 matrices
 * @return  none
 * @param   mpSrc1 [in] pointer to the first matrix
 * @param   mpSrc2 [in] pointer to the second matrix
 * @param   mpDst  [out] pointer to the resulting matrix
 * @author  BVS, Miron
 * @see     DiMatrixIntergrityTest
 * @see     DiMatrixIdentity
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixAdd
 * @see     DiMatrixSubtract
 * @see     DiMatrixScale
 * @see     DiMatrixTranslate
 */
void DiMatrixAdd(const DiMatrix *mpSrc1, const DiMatrix *mpSrc2, DiMatrix *mpDst)
{
  wxASSERT(mpDst);
  
  // add matrices
  DiV4dAdd(&(mpSrc1->vAt), &(mpSrc2->vAt), &(mpDst->vAt));
  mpDst->vAt.w = 0.0f;
  DiV4dAdd(&(mpSrc1->vUp), &(mpSrc2->vUp), &(mpDst->vUp));
  mpDst->vUp.w = 0.0f;
  DiV4dAdd(&(mpSrc1->vRight), &(mpSrc2->vRight), &(mpDst->vRight));
  mpDst->vRight.w = 0.0f;
  // @NEWVECTORS@
  DiV4dAdd(&(mpSrc1->vPos), &(mpSrc2->vPos), &(mpDst->vPos));
  mpDst->vPos.w = 1.f;

  return;
} // end of DiMatrixAdd

/**
 * This function adds 2 matrices and places result to the third matrix.
 *
 * @memo    Add 2 matrices
 * @return  none
 * @param   mpSrc1 [in] pointer to the first matrix
 * @param   mpSrc2 [in] pointer to the second matrix
 * @param   mpDst  [out] pointer to the resulting matrix
 * @author  BVS, Miron
 * @see     DiMatrixIntergrityTest
 * @see     DiMatrixIdentity
 * @see     DiMatrixRotateIdentity
 * @see     DiMatrixAdd
 * @see     DiMatrixSubtract
 * @see     DiMatrixScale
 * @see     DiMatrixTranslate
 */
void DiMatrixSubtract(const DiMatrix *mpSrc1, const DiMatrix *mpSrc2, DiMatrix *mpDst)
{
  wxASSERT(mpDst);
  
  // add matrices
  DiV4dSub(&(mpSrc1->vAt), &(mpSrc2->vAt), &(mpDst->vAt));
  mpDst->vAt.w = 0.0f;
  DiV4dSub(&(mpSrc1->vUp), &(mpSrc2->vUp), &(mpDst->vUp));
  mpDst->vUp.w = 0.0f;
  DiV4dSub(&(mpSrc1->vRight), &(mpSrc2->vRight), &(mpDst->vRight));
  mpDst->vRight.w = 0.0f;
  // @NEWVECTORS@
  DiV4dSub(&(mpSrc1->vPos), &(mpSrc2->vPos), &(mpDst->vPos));
  mpDst->vPos.w = 1.f;

  return;
} // end of DiMatrixAdd
/**
 * This function is used inver given matrix
 *
 * @memo    Invert given matrix
 * @return  TRUE on success, FALSE otherwise
 * @param   mpIn  matrix [in]
 * @param   mpOut matrix [out]
 * @see     
 */
//----------------------------------------------------------------------------
bool DiMatrixInvert(const DiMatrix *mpIn, DiMatrix *mpOut)
//----------------------------------------------------------------------------
{
  double d;
  double reciprocal_d;
  
  wxASSERT(mpIn);
  wxASSERT(mpOut);
  wxASSERT(mpOut != mpIn);


  // test integrity
  DiMatrixTestIntegrity(mpIn);

  /* Assumes input of form
   [ [ m_X_X, m_X_Y, m_X_Z, 0 ]
     [ m_Y_X, m_Y_Y, m_Y_Z, 0 ]
     [ m_Z_X, m_Z_Y, m_Z_Z, 0 ]
     [ T_X,   T_Y,   T_Z,   1 ] ]
  */

  /* Find adjoint of 3 sub matrix : Transposed matrix of cofactors (cross-products)
   [ [ m_Z_Z m_Y_Y - m_Y_Z m_Z_Y,   m_X_Z m_Z_Y - m_Z_Z m_X_Y,   m_Y_Z m_X_Y - m_X_Z m_Y_Y ]
     [ m_Y_Z m_Z_X - m_Z_Z m_Y_X,   m_Z_Z m_X_X - m_X_Z m_Z_X,   m_X_Z m_Y_X - m_Y_Z m_X_X ]
     [ m_Z_Y m_Y_X - m_Y_Y m_Z_X,   m_X_Y m_Z_X - m_Z_Y m_X_X,   m_Y_Y m_X_X - m_X_Y m_Y_X ] ]
  */

  i_X_X = DET(m_Y_Y, m_Y_Z, m_Z_Y, m_Z_Z);
  i_Y_X = DET(m_Z_X, m_Z_Z, m_Y_X, m_Y_Z);
  i_Z_X = DET(m_Y_X, m_Y_Y, m_Z_X, m_Z_Y);

  i_X_Y = DET(m_Z_Y, m_Z_Z, m_X_Y, m_X_Z);
  i_Y_Y = DET(m_X_X, m_X_Z, m_Z_X, m_Z_Z);
  i_Z_Y = DET(m_Z_X, m_Z_Y, m_X_X, m_X_Y);

  i_X_Z = DET(m_X_Y, m_X_Z, m_Y_Y, m_Y_Z);
  i_Y_Z = DET(m_Y_X, m_Y_Z, m_X_X, m_X_Z);
  i_Z_Z = DET(m_X_X, m_X_Y, m_Y_X, m_Y_Y);

  /*
   Find determinant of 3 sub matrix
  */

  d = i_X_X * m_X_X + i_Y_X * m_X_Y + i_Z_X * m_X_Z;

  /*
   Divide through if non-zero
   to find inverse of 3 sub matrix
  */

  if (d == 0.0f)
  {
    return (FALSE);
  } // end if (d == 0.0f)

  reciprocal_d = 1.0f/d;

  i_X_X *= reciprocal_d;
  i_X_Y *= reciprocal_d;
  i_X_Z *= reciprocal_d;
  i_X_W = 0.f;

  i_Y_X *= reciprocal_d;
  i_Y_Y *= reciprocal_d;
  i_Y_Z *= reciprocal_d;
  i_Y_W = 0.f;

  i_Z_X *= reciprocal_d;
  i_Z_Y *= reciprocal_d;
  i_Z_Z *= reciprocal_d;
  i_Z_W = 0.f;

  /*
   Find translation component of inverse
  */

  i_W_X = - (T_X * i_X_X + T_Y * i_Y_X + T_Z * i_Z_X);
  i_W_Y = - (T_X * i_X_Y + T_Y * i_Y_Y + T_Z * i_Z_Y);
  i_W_Z = - (T_X * i_X_Z + T_Y * i_Y_Z + T_Z * i_Z_Z);
  i_W_W = 1.f;

  return (TRUE);
} // end of DiMatrixInvert


/**
 * @memo    converts right matrix to left
 * @return  true on success
 * @param   mpSource
 * @param   mpDest
 */
//----------------------------------------------------------------------------
void mafTransfRightLeftConv(DiMatrix *mpSource, DiMatrix *mpDest)
//----------------------------------------------------------------------------
{
  //perform mirroring
  DiMatrixCopy(mpSource, mpDest);
  mafTransfMirrorMatrixYOX(mpDest, mpDest);
  DiV4dNegate(&mpDest->vRight, &mpDest->vRight);
  mpDest->vRight.w = 0.f;
  return;
}


/**
 * detailed description
 *
 * @memo    Calculates Grood & Santey angles from proximal and distal matrices
 * @return  None
 * @param   mpParent proximal matrix                  IN
 * @param   mpChild  distal matrix                    IN
 * @param   mpBasic  basic matrix by Grood and Santey OUT
 * @param   vpR      angles vector                    OUT
 */
//----------------------------------------------------------------------------
void mafTransfMatrixToGES(DiMatrix *mpParent, DiMatrix *mpChild, DiMatrix *mpBasic, DiV4d *vpR)
//----------------------------------------------------------------------------
{
  DiMatrix mDistal  ;
  DiMatrix mProximal;
  DiMatrix mBase    ;
  DiV4d    vXP;
  DiV4d    vYP;
  DiV4d    vZP;
  DiV4d    vXD;
  DiV4d    vYD;
  DiV4d    vZD;
  DiV4d    vE1;
  DiV4d    vE2;
  DiV4d    vE3;
  double  rE2xp,rE2yp,rE2zd,rE2xd;
  double  rFE,rAA,rIE,rBet;

  DiMatrixCopy(mpParent, &mProximal);
  DiMatrixCopy(mpChild, &mDistal);
  DiMatrixIdentity(&mBase);

  DiV4dCopy(&mProximal.vAt,    &vZP);
  DiV4dCopy(&mProximal.vUp,    &vYP);
  DiV4dCopy(&mProximal.vRight, &vXP);
  DiV4dCopy(&mDistal.vAt,      &vZD);
  DiV4dCopy(&mDistal.vUp,      &vYD);
  DiV4dCopy(&mDistal.vRight,   &vXD);

  DiV4dMakeUnit(&vXP);
  DiV4dMakeUnit(&vYP);
  DiV4dMakeUnit(&vZP);
  DiV4dMakeUnit(&vXD);
  DiV4dMakeUnit(&vYD);
  DiV4dMakeUnit(&vZD);

  DiV4dCopy(&vZP, &vE1);
  DiV4dCopy(&vYD, &vE3);
  DiV4dCrossProduct(&vE3, &vE1, &vE2);
  DiV4dMakeUnit(&vE2);

  DiV4dCopy(&vE2, &mBase.vRight);
  DiV4dCopy(&vE3, &mBase.vUp);
  DiV4dCopy(&vE1, &mBase.vAt);
  DiMatrixTransposeRotationalSubmatrix(&mBase, mpBasic);

  rE2zd  = -DiV4dDotProduct(&vE2, &vZD);
  rE2xd  =  DiV4dDotProduct(&vE2, &vXD);
  rE2yp  =  DiV4dDotProduct(&vE2, &vYP);
  rE2xp  =  DiV4dDotProduct(&vE2, &vXP);
  rIE    = -(double)atan2(rE2zd, rE2xd);
  rFE    =  (double)atan2(rE2yp, rE2xp);
  rBet   =  DiV4dDotProduct(&vE3, &vE1);
  rAA    =  acos(DiFFitIn(rBet, -1.0, 1.0)) - vtkMath::Pi() * 0.5f;
  vpR->x = rAA;
  vpR->y = rIE;
  vpR->z = rFE;
  vpR->w = 1.0;

  return;
}

/**
 * This function is used to transform vector with the given matrix
 *
 * @memo    Transform vector with the given matrix
 * @return  none
 * @param   vpIn  vector [in]
 * @param   mpMat matrix [in]
 * @param   vpOut vector [out]
 */
//----------------------------------------------------------------------------
void  DiV4dTransformVectorTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut)
//----------------------------------------------------------------------------
{
  double rScale;
  double rImageX;
  double rImageY;
  double rImageZ;

  wxASSERT( mpMat->vRight.w + mpMat->vUp.w + mpMat->vAt.w == 0.f);

  // X
  rScale   = vpIn->x;
  rImageX  = rScale * mpMat->vRight.x;
  rImageY  = rScale * mpMat->vRight.y;
  rImageZ  = rScale * mpMat->vRight.z;

  // Y
  rScale   = vpIn->y;
  rImageX += rScale * mpMat->vUp.x;
  rImageY += rScale * mpMat->vUp.y;
  rImageZ += rScale * mpMat->vUp.z;

  // Z
  rScale   = vpIn->z;
  vpOut->x = rScale * mpMat->vAt.x + rImageX;
  vpOut->y = rScale * mpMat->vAt.y + rImageY;
  vpOut->z = rScale * mpMat->vAt.z + rImageZ;
  vpOut->w = 0.f;

  return;
} // end of DiV4dTransformVectorTo


/**
 * This function is used to transform vector with the given matrix
 *
 * @memo    Transform vector with the given matrix
 * @return  none
 * @param   mpMat   matrix [in]
 * @param   vpIn    vector [in]
 * @param   vpOut   vector [out]
 */
//----------------------------------------------------------------------------
void  DiV4dInverseTransformVectorTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut)
//----------------------------------------------------------------------------
{
  wxASSERT(vpIn != vpOut);
  vpOut->x = DiV4dDotProduct(vpIn, &(mpMat->vRight));
  vpOut->y = DiV4dDotProduct(vpIn, &(mpMat->vUp));
  vpOut->z = DiV4dDotProduct(vpIn, &(mpMat->vAt));
  vpOut->w = 0.0f;
  return;
} // end of DiV4dInverseTransformVectorTo

/**
 * This function is used to transform given point with the given matrix
 *
 * @memo    Transform point with the given matrix
 * @return  none
 * @param   vpIn  point  [in]
 * @param   mpMat matrix [in]
 * @param   vpOut point  [out]
 */
//----------------------------------------------------------------------------
void  DiV4dTransformPointTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut)
//----------------------------------------------------------------------------
{
  double rImageX;
  double rImageY;
  double rImageZ;

  wxASSERT( mpMat->vRight.w + mpMat->vUp.w + mpMat->vAt.w +
            mpMat->vPos.w == 1.f /* 0,0,0 and 1*/);
  wxASSERT(vpIn->w == 1.f ||
           (mpMat->vPos.x == 0.f && mpMat->vPos.y == 0.f && mpMat->vPos.z == 0.f));

  // X
  rImageX  = vpIn->x * mpMat->vRight.x;
  rImageY  = vpIn->x * mpMat->vRight.y;
  rImageZ  = vpIn->x * mpMat->vRight.z;

  // Y
  rImageX += vpIn->y * mpMat->vUp.x;
  rImageY += vpIn->y * mpMat->vUp.y;
  rImageZ += vpIn->y * mpMat->vUp.z;

  // Z
  rImageX += vpIn->z * mpMat->vAt.x;
  rImageY += vpIn->z * mpMat->vAt.y;
  rImageZ += vpIn->z * mpMat->vAt.z;

  // W
  vpOut->x = rImageX + mpMat->vPos.x;
  vpOut->y = rImageY + mpMat->vPos.y;
  vpOut->z = rImageZ + mpMat->vPos.z;
  vpOut->w = 1.f;
  return;
} // end of DiV4dTransformPointTo


/**
 * This function is used to transform point with the given matrix
 *
 * @memo    Transform point with the given matrix
 * @return  none
 * @param   vpIn  point  [in]
 * @param   mpMat matrix [in]
 * @param   vpOut point  [out]
 * @see     
 */
//----------------------------------------------------------------------------
void  DiV4dInverseTransformPointTo(DiV4d const *vpIn, DiMatrix const *mpMat, DiV4d *vpOut)
//----------------------------------------------------------------------------
{
  DiV4d vLPoint;

  DiV4dSub(vpIn, &(mpMat->vPos), &vLPoint);
  vLPoint.w = 0.0f;
  vpOut->x  = DiV4dDotProduct(&vLPoint, &(mpMat->vRight));
  vpOut->y  = DiV4dDotProduct(&vLPoint, &(mpMat->vUp));
  vpOut->z  = DiV4dDotProduct(&vLPoint, &(mpMat->vAt));
  vpOut->w  = 1.0f;

  return;
} // end of DiV4dInverseTransformPointTo


/**
 * This function is used to Calculate determinate using recursive expansion by minors
 *
 * @memo    Transform point with the given matrix
 * @return  none
 * @param   vpIn  point  [in]
 * @param   mpMat matrix [in]
 * @param   vpOut point  [out]
 * @see     
 */
//----------------------------------------------------------------------------
typedef double HMatrix[4][4];

double Determinant(DiMatrix const * inmat, int n)
{
  int i, j, j1, j2;
  double d = 0;
  HMatrix  m;
  HMatrix  const *a = (HMatrix const *)inmat;

  wxASSERT(n > 1);

  if(n == 2)
  { 
    d = (*a)[0][0]*(*a)[1][1] - (*a)[1][0]*(*a)[0][1];
  }
  else 
  {
      d = 0;
      for (j1 = 0; j1 < n; j1++)
      {
          //create minor
          for (i = 1; i < n; i++)
          {
              j2 = 0;
              for (j = 0; j < n; j++)
              {
                  if (j == j1) continue;
                  m[i-1][j2] = (*a)[i][j];
                  ++j2;
              }
          }
          // calculate determinant
          d = d + (((1 + j1) % 2)?-1:1) * (*a)[0][j1] * Determinant((DiMatrix *) m, n-1);
      }
  }
  
  return d;
}

// Matrix interpolation
/**
 * This function fills interpolation data for matrices interpolation:
   2 quaternions & position delta
 *
 * @memo    Fill interpolation data for matrices interpolation:  2 quaternions & position delta
 * @return  none
 * @param   mpCurMatrix  [in] pointer to current matrix
 * @param   mpPrevMatrix [in] pointer to previous matrix
 * @param   midpData     [out] pointer to interpolation data to be filled
 * @author  BVS, Miron
 * @see     DiMatrixBuildInterpolationData
 * @see     DiMatrixBuildInterpolated
 */
void DiMatrixBuildInterpolationData(DiMatrix const            *mpCurMatrix, 
                                      DiMatrix const            *mpPrevMatrix,
                                      DiMatrixInterpolationData *midpData)
{
  DiMatrix mTmp;
  
  // Convert to quaternions
  DiQuatBuildFromMatrix(mpCurMatrix, &(midpData->qtCurQuat));
  DiQuatBuildMatrix(&(midpData->qtCurQuat), &mTmp);
  wxASSERT(fabs(mTmp.vAt.z - mpCurMatrix->vAt.z) < 1e-3f);
  DiQuatBuildFromMatrix(mpPrevMatrix, &(midpData->qtPrevQuat));

  // Save positions
  midpData->vPrevPos = mpPrevMatrix->vPos;
  DiV4dSub(&(mpCurMatrix->vPos), &(midpData->vPrevPos), &(midpData->vPosDelta));

  return;
} // end of DiMatrixBuildInterpolationData

/**
 * detailed description
 *
 * @memo    <memo>
 * @return  <return>
 * @param   qpA
 * @param   qpB
 * @param   rMix
 * @param   qpOut
 * @author  Vlad
 * @see     <see>
 */
void  DiQuatInterpSpherLinear(DiQuaternion  *qpA, 
                                DiQuaternion  *qpB, 
                                double       rMix, 
                                DiQuaternion  *qpOut)
{
  double       rOmega, rRecSinOm, rSinTmp;
  double       sbCosOm;
  double       rKoefA, rKoefB;

  wxASSERT(qpA && qpB && qpOut);

  sbCosOm = qpA->x * qpB->x + qpA->y * qpB->y + qpA->z * qpB->z + qpA->w * qpB->w; 
  
  sbCosOm = DiFFitIn(sbCosOm, -1.0f, 1.0f);

  if (sbCosOm < 0)
  {
    // need to interpolate the other way
    qpA->x = -qpA->x;
    qpA->y = -qpA->y;
    qpA->z = -qpA->z;
    qpA->w = -qpA->w;
    sbCosOm = -sbCosOm;
  }

  if (sbCosOm > 1.0f - EPSILON)
  {// really high
    rKoefA = 1.f - rMix;
    rKoefB = rMix;
  }
  else
  {// rCosOm is ok
    rOmega = acos(sbCosOm);
    rSinTmp = sin(rOmega);
    wxASSERT(fabs(rSinTmp) >= 0.00001f);
    rRecSinOm = 1.0f / rSinTmp;
    rSinTmp = sin((1.0f - rMix) * rOmega);
    rKoefA = rSinTmp * rRecSinOm;
    rSinTmp = sin(rMix * rOmega);
    rKoefB = rSinTmp * rRecSinOm;
  }
  DiQuatLineComb(qpA, rKoefA, qpB, rKoefB, qpOut);

  return;
} // end of DiQuatInterpSpherLinear

/**
 * This function interpolates between to matrices (?) according to
   interpolation data built by DiMatrixBuildInterpolationData
 *
 * @memo    Interpolate between to matrices
 * @return  none
 * @param   midpData     [in] pointer to interpolation data to be filled
 * @param   rT           [in] interpolation parameter
 * @param   mpNewMatrix  [out] pointer to the interpolation result
 * @author  BVS, Miron
 * @see     DiMatrixBuildInterpolationData
 * @see     DiMatrixBuildInterpolated
 */
void DiMatrixBuildInterpolated(DiMatrixInterpolationData *midpData, 
                                 double                   rT,
                                 DiMatrix                  *mpNewMatrix)
{
  DiQuaternion qtNewQuat;

  wxASSERT(0.0f <= rT && rT <= 1.0f);

  // Interpolate quaternion
  DiQuatInterpSpherLinear(&(midpData->qtPrevQuat), &(midpData->qtCurQuat), rT, &qtNewQuat);
  
  // Convert to matrix
  DiQuatBuildMatrix(&qtNewQuat, mpNewMatrix);
  
  // Evaluate position
  DiV4dScale(&(midpData->vPosDelta), rT, &(mpNewMatrix->vPos));
  // @NEWVECTORS@
  DiV4dAdd(&(mpNewMatrix->vPos), &(midpData->vPrevPos), &(mpNewMatrix->vPos));
  mpNewMatrix->vPos.w = 1.f;

  return;
} // end of DiMatrixBuildInterpolated


void DiAlProjectPointOntoPlane(DiV4d const *vpPoint, DiV4d const *vpPlanePoint, DiV4d const *vpPlaneNormal, DiV4d *vpRet)
{
  DiV4d vTemp;

  wxASSERT(fabs(DiV4dDotProduct(vpPlaneNormal, vpPlaneNormal) - 1.f) < 0.001f);
  DiV4dSub(vpPlanePoint, vpPoint, &vTemp);
  DiV4dShiftComb(vpPoint, vpPlaneNormal, DiV4dDotProduct(&vTemp, vpPlaneNormal), vpRet);
}

double BuildFourPointSphere(DiV4d &c, DiV4d *p)
{

  int i;
  double r, m11, m12, m13, m14, m15;
  HMatrix a;

  for (i = 0; i < 4; i++) //find minor 11
  {
      a[i][0] = p[i].x;
      a[i][1] = p[i].y;
      a[i][2] = p[i].z;
      a[i][3] = 1;
  }
  m11 = Determinant((DiMatrix *)&a, 4);

  for(i = 0; i < 4; i++) // find minor 12
  {
      a[i][0] = p[i].x*p[i].x + p[i].y*p[i].y + p[i].z*p[i].z;
      a[i][1] = p[i].y;
      a[i][2] = p[i].z;
      a[i][3] = 1;
  }
  m12 = Determinant((DiMatrix *)&a, 4 );

  for(i = 0; i < 4; i++) //  find minor 13
  {
      a[i][0] = p[i].x*p[i].x + p[i].y*p[i].y + p[i].z*p[i].z;
      a[i][1] = p[i].x;
      a[i][2] = p[i].z;
      a[i][3] = 1;
  }
  m13 = Determinant((DiMatrix *)&a, 4 );

  for(i = 0; i < 4; i++) //  find minor 14
  {
      a[i][0] = p[i].x*p[i].x + p[i].y*p[i].y + p[i].z*p[i].z;
      a[i][1] = p[i].x;
      a[i][2] = p[i].y;
      a[i][3] = 1;
  }
  m14 = Determinant((DiMatrix *)&a, 4 );
                            
  for(i = 0; i < 4; i++) //  find minor 15
  {
      a[i][0] = p[i].x*p[i].x + p[i].y*p[i].y + p[i].z*p[i].z;
      a[i][1] = p[i].x;
      a[i][2] = p[i].y;
      a[i][3] = p[i].z;
  }
  m15 = Determinant((DiMatrix *)&a, 4 );

  if (m11 == 0)
  {
      r = 0;
  }
  else
  {
      c.x =  0.5 * m12 / m11; // center of sphere
      c.y = -0.5 * m13 / m11;
      c.z =  0.5 * m14 / m11;
      c.w =  1.0f;
      r   = sqrt( c.x*c.x + c.y*c.y + c.z*c.z - m15/m11);
  }

  return r;                    //the radius
}


