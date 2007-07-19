/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeIntGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-19 12:37:34 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeIntGraph.h"
#include "mafDecl.h"
#include "mafViewIntGraph.h"

#include "mafTransform.h"
#include "mafPlotMath.h"

#include "mafMatrix3x3.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEAFRefSys.h"

#include "vtkMath.h"
#include "vtkMatrix4x4.h"

#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

#ifndef DIM
#define DIM(a)  (sizeof((a)) / sizeof(*(a)))
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------

const float _Conventions[24] = {EulOrdXYZs, EulOrdXYXs, EulOrdXZYs, EulOrdXZXs, EulOrdYZXs, EulOrdYZYs,
                                EulOrdYXZs, EulOrdYXYs, EulOrdZXYs, EulOrdZXZs, EulOrdZYXs, EulOrdZYZs,
                                EulOrdZYXr, EulOrdXYXr, EulOrdYZXr, EulOrdXZXr, EulOrdXZYr, EulOrdYZYr,
                                EulOrdZXYr, EulOrdYXYr, EulOrdYXZr, EulOrdZXZr, EulOrdXYZr, EulOrdZYZr};

//----------------------------------------------------------------------------
inline float _MathRound(float val)
//----------------------------------------------------------------------------
{
  float rFloor;
  float rCeil;

  rFloor = (float)floor(val);
  rCeil  = (float)ceil(val);
  return (val - rFloor > rCeil - val) ? rCeil : rFloor;
}

//----------------------------------------------------------------------------
static float _fix360Difference(float rAold, float rAnew)
//----------------------------------------------------------------------------
{
  float rDiff = rAnew - rAold;
  if(rDiff > 0)
  {
    float rR = rAnew - 360.0 * _MathRound((rDiff - fmodf(rDiff, 360.0)) / 360.0);
  
    if(fabs(rR - rAold) > 180.0)
      return rR - 360.0;
    return rR;//(rA + rRemnant);

  }
  float rR = rAnew - 360.0 * _MathRound((rDiff - fmodf(rDiff, 360.0)) / 360.0);
  
  if(fabs(rR - rAold) > 180.0)
    return rR + 360.0;
  return rR;//(rA + rRemnant);
}

//----------------------------------------------------------------------------
static float _fix180Difference(float rAold, float rAnew)
//----------------------------------------------------------------------------
{
  rAnew = _fix360Difference(rAold, rAnew);
  float rDiff = rAnew - rAold;
  if(rDiff > 0)
  {
    float rR = rAnew - 180.0 * _MathRound((rDiff - fmodf(rDiff, 180.0)) / 180.0);
  
    if(fabs(rR - rAold) > 90.0)
      return rR - 180.0;
    return rR;//(rA + rRemnant);

  }
  float rR = rAnew - 180.0 * _MathRound((rDiff - fmodf(rDiff, 180.0)) / 180.0);
  
  if(fabs(rR - rAold) > 90.0)
    return rR + 180.0;
  return rR;//(rA + rRemnant);
}

//----------------------------------------------------------------------------
mafPipeIntGraph::mafPipeIntGraph():m_variables(GDT_LAST)
//----------------------------------------------------------------------------
{
  m_Selected = false;

  m_PrevStamp   = -1;
  for(unsigned int i = 0; i < m_variables.size(); i++)
    m_variables[i].second = false;
  m_variables[GDT_FRAME].first  = -1;
}
//----------------------------------------------------------------------------
mafPipeIntGraph::~mafPipeIntGraph()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
static void GetGlobalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat)
//----------------------------------------------------------------------------
{
  mafMatrix matrix;

  ///This function can be called with zero this!
  vme->GetOutput()->GetMatrix(matrix, ts);
  mflMatrixToDi(matrix.GetVTKMatrix(), pMat);
  mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(vme);
  mafVMEAFRefSys      *afs = NULL;
  if(lmc == NULL)
    return;

  for(int i = 0; i < lmc->GetNumberOfChildren(); i++)
  {
    mafNode *child = vme->GetChild(i);
    if(child->IsA("mafVMEAFRefSys"))
    {
      afs = mafVMEAFRefSys::SafeDownCast(child);
      break;
    }
  }

  if(afs == NULL)
    return;

  afs->GetOutput()->GetAbsMatrix(matrix, ts);
  mflMatrixToDi(matrix.GetVTKMatrix(), pMat);
}

//----------------------------------------------------------------------------
static void GetLocalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat)
//----------------------------------------------------------------------------
{
  DiMatrix pmatrix;
  DiMatrix cmatrix;
  DiMatrix pInv;

  GetGlobalMatrix(vme->GetParent(), ts, &pmatrix);
  GetGlobalMatrix(vme, ts, &cmatrix);
  DiMatrixInvert(&pmatrix, &pInv);
  DiMatrixMultiply(&cmatrix, &pInv, pMat);
}

//----------------------------------------------------------------------------
void  mafPipeIntGraph::StoreValueByIdx(int nObjectOrderID, IDType nVarID, int nGraphIndex, mafTimeStamp nTimeStamp, mafTimeStamp nPrevTimeStamp)
//----------------------------------------------------------------------------
{
  mafTimeStamp     ts;
  mafTimeStamp     pts;
  DiInt32          nObjIDByVar = nVarID[0];
  mafGraphDescType tVarType = mafGraphDescType(nVarID[1]);
  DiMatrix         mat;
  DiV4d            vPos; 
  DiV4d            vRot; 

  ts  = (nTimeStamp < 0) ? m_Vme->GetOutput()->GetTimeStamp() : nTimeStamp;
  pts = (nPrevTimeStamp < 0) ? ts : nPrevTimeStamp;

  DiMatrixIdentity(&mat);

  // all variables have native order mentioned in mafGraphDescType. 
  // It means we always have GDT_LAST variables per any element with active pipe
  // Time is repeated for each pipe!
  if(nObjIDByVar != nObjectOrderID)
  {
    //we have nothing to do coz requested variable not from this pipe
    return;
  }

  // Zero nVarID is always time
  // nVarID == GDT_LAST * k, where k from N is also the EXACTLY same time!
  //always add time on first object

  if(fabs(ts - m_variables[GDT_FRAME].first) > 1e-6)
  {
    for(unsigned int i = 0; i < m_variables.size(); i++)
      m_variables[i].second = false;
    m_variables[GDT_FRAME].first  = ts;
    m_variables[GDT_FRAME].second = true;
  }

  if(nVarID.isZero())
  {
    m_View->GetGraph()->SetAddCoord(nGraphIndex, ts);
    return;
  }

  if(!m_variables[tVarType].second)
  {
    switch (tVarType)
    {
    case   GDT_GTM_POSX   :
    case   GDT_GTM_POSY   :
    case   GDT_GTM_POSZ   :
    case   GDT_GTM_ROTX   :
    case   GDT_GTM_ROTY   :
    case   GDT_GTM_ROTZ   :
     {
        //get full trio in proper convention and axises
        GetGlobalMatrix(m_Vme, ts, &mat);
        mafTransfInverseTransformUpright(&mat, &vPos, &vRot);

        m_variables[GDT_GTM_POSX].first  = vPos.x;
        m_variables[GDT_GTM_POSX].second = true;
        m_variables[GDT_GTM_POSY].first  = vPos.y;
        m_variables[GDT_GTM_POSY].second = true;
        m_variables[GDT_GTM_POSZ].first  = vPos.z;
        m_variables[GDT_GTM_POSZ].second = true;
        m_variables[GDT_GTM_ROTX].first  = vRot.x * mafMatrix3x3::RadiansToDegrees();
        m_variables[GDT_GTM_ROTX].second = true;
        m_variables[GDT_GTM_ROTY].first  = vRot.y * mafMatrix3x3::RadiansToDegrees();
        m_variables[GDT_GTM_ROTY].second = true;
        m_variables[GDT_GTM_ROTZ].first  = vRot.z * mafMatrix3x3::RadiansToDegrees();
        m_variables[GDT_GTM_ROTZ].second = true;
        break;
      }
      //helical
      case   GDT_HEL_ROTX   :
      case   GDT_HEL_ROTY   :
      case   GDT_HEL_ROTZ   :
        {    
          DiMatrix     mt;
          DiMatrix     mt1;
          DiMatrix     tmp;
          mafTransform *pTransf;
          vtkMatrix4x4 *mVTK             = NULL;

          double       helicalAxis[3]    = { 0, 0, 0 };
          double       point[3]          = { 0, 0, 0 };
          double       angle             = 0;
          double       translationAmount = 0;

          mafNEW(pTransf);

          GetLocalMatrix(m_Vme, ts, &mt);
          GetLocalMatrix(m_Vme, ts + 1, &mt1);
          DiMatrixInvert(&mt, &tmp);
          DiMatrixMultiply(&mt1, &tmp, &mt);
          vtkNEW(mVTK);
          DiMatrixToVTK(&mt, mVTK);
          pTransf->SetMatrix(mVTK);
          if(!DiMatrixTestIdentity(&mt))
          {
            pTransf->MatrixToHelicalAxis(pTransf->GetMatrix(), helicalAxis, point, angle, translationAmount, 2);
          }
          else
          {
            helicalAxis[0] = 1.0;
            helicalAxis[1] = 0.0;
            helicalAxis[2] = 0.0;
            angle          = 0.0;
          }
          if(angle < 0)
          {
            helicalAxis[0] = -helicalAxis[0];
            helicalAxis[1] = -helicalAxis[1];
            helicalAxis[2] = -helicalAxis[2];
            angle          = -angle;
          }
          vtkDEL(mVTK);
          mafDEL(pTransf);

          m_variables[GDT_HEL_ROTX].first  = helicalAxis[0] * angle;
          m_variables[GDT_HEL_ROTX].second = true;
          m_variables[GDT_HEL_ROTY].first  = helicalAxis[1] * angle;
          m_variables[GDT_HEL_ROTY].second = true;
          m_variables[GDT_HEL_ROTZ].first  = helicalAxis[2] * angle;
          m_variables[GDT_HEL_ROTZ].second = true;
          break;
        }
      //  //LTM
    case   GDT_LTM_POSX   :
    case   GDT_LTM_POSY   :
    case   GDT_LTM_POSZ   :
    case   GDT_LTM_ROTX   :
    case   GDT_LTM_ROTY   :
    case   GDT_LTM_ROTZ   :
      {
        DiMatrix                   mLTM;
        GetLocalMatrix(m_Vme, ts, &mLTM);
        mafTransfInverseTransformUpright(&mLTM, &vPos, &vRot);

        m_variables[GDT_LTM_POSX].first  = vPos.x;
        m_variables[GDT_LTM_POSX].second = true;
        m_variables[GDT_LTM_POSY].first  = vPos.y;
        m_variables[GDT_LTM_POSY].second = true;
        m_variables[GDT_LTM_POSZ].first  = vPos.z;
        m_variables[GDT_LTM_POSZ].second = true;
        m_variables[GDT_LTM_ROTX].first  = vRot.x * mafMatrix3x3::RadiansToDegrees();
        m_variables[GDT_LTM_ROTX].second = true;
        m_variables[GDT_LTM_ROTY].first  = vRot.y * mafMatrix3x3::RadiansToDegrees();
        m_variables[GDT_LTM_ROTY].second = true;
        m_variables[GDT_LTM_ROTZ].first  = vRot.z * mafMatrix3x3::RadiansToDegrees();
        m_variables[GDT_LTM_ROTZ].second = true;
        break;
      }


      case   GDT_OVP_ROTX   :
      case   GDT_OVP_ROTY   :
      case   GDT_OVP_ROTZ   :
        {
          DiMatrix                   mLTM;
          GetLocalMatrix(m_Vme, ts, &mLTM);
          mafTransfInverseTransformUpright(&mat, &vPos, &vRot);
          m_variables[GDT_OVP_ROTX].first  = vRot.x * mafMatrix3x3::RadiansToDegrees();
          m_variables[GDT_OVP_ROTX].second = true;
          m_variables[GDT_OVP_ROTY].first  = vRot.y * mafMatrix3x3::RadiansToDegrees();
          m_variables[GDT_OVP_ROTY].second = true;
          m_variables[GDT_OVP_ROTZ].first  = vRot.z * mafMatrix3x3::RadiansToDegrees();
          m_variables[GDT_OVP_ROTZ].second = true;
          break;
        }
      //Euler
    case   GDT_EUL_ROTXXYZs   :
    case   GDT_EUL_ROTYXYZs   :
    case   GDT_EUL_ROTZXYZs   :

    case   GDT_EUL_ROTXXYXs   :
    case   GDT_EUL_ROTYXYXs   :
    case   GDT_EUL_ROTZXYXs   :

    case   GDT_EUL_ROTXXZYs   :
    case   GDT_EUL_ROTYXZYs   :
    case   GDT_EUL_ROTZXZYs   :

    case   GDT_EUL_ROTXXZXs   :
    case   GDT_EUL_ROTYXZXs   :
    case   GDT_EUL_ROTZXZXs   :

    case   GDT_EUL_ROTXYZXs   :
    case   GDT_EUL_ROTYYZXs   :
    case   GDT_EUL_ROTZYZXs   :

    case   GDT_EUL_ROTXYZYs   :
    case   GDT_EUL_ROTYYZYs   :
    case   GDT_EUL_ROTZYZYs   :

    case   GDT_EUL_ROTXYXZs   :
    case   GDT_EUL_ROTYYXZs   :
    case   GDT_EUL_ROTZYXZs   :

    case   GDT_EUL_ROTXYXYs   :
    case   GDT_EUL_ROTYYXYs   :
    case   GDT_EUL_ROTZYXYs   :

    case   GDT_EUL_ROTXZXYs   :
    case   GDT_EUL_ROTYZXYs   :
    case   GDT_EUL_ROTZZXYs   :

    case   GDT_EUL_ROTXZXZs   :
    case   GDT_EUL_ROTYZXZs   :
    case   GDT_EUL_ROTZZXZs   :

    case   GDT_EUL_ROTXZYXs   :
    case   GDT_EUL_ROTYZYXs   :
    case   GDT_EUL_ROTZZYXs   :

    case   GDT_EUL_ROTXZYZs   :
    case   GDT_EUL_ROTYZYZs   :
    case   GDT_EUL_ROTZZYZs   :


      // Rotating axes 
    case   GDT_EUL_ROTXZYXr   :
    case   GDT_EUL_ROTYZYXr   :
    case   GDT_EUL_ROTZZYXr   :

    case   GDT_EUL_ROTXXYXr   :
    case   GDT_EUL_ROTYXYXr   :
    case   GDT_EUL_ROTZXYXr   :

    case   GDT_EUL_ROTXYZXr   :
    case   GDT_EUL_ROTYYZXr   :
    case   GDT_EUL_ROTZYZXr   :

    case   GDT_EUL_ROTXXZXr   :
    case   GDT_EUL_ROTYXZXr   :
    case   GDT_EUL_ROTZXZXr   :

    case   GDT_EUL_ROTXXZYr   :
    case   GDT_EUL_ROTYXZYr   :
    case   GDT_EUL_ROTZXZYr   :

    case   GDT_EUL_ROTXYZYr   :
    case   GDT_EUL_ROTYYZYr   :
    case   GDT_EUL_ROTZYZYr   :

    case   GDT_EUL_ROTXZXYr   :
    case   GDT_EUL_ROTYZXYr   :
    case   GDT_EUL_ROTZZXYr   :

    case   GDT_EUL_ROTXYXYr   :
    case   GDT_EUL_ROTYYXYr   :
    case   GDT_EUL_ROTZYXYr   :

    case   GDT_EUL_ROTXYXZr   :
    case   GDT_EUL_ROTYYXZr   :
    case   GDT_EUL_ROTZYXZr   :

    case   GDT_EUL_ROTXZXZr   :
    case   GDT_EUL_ROTYZXZr   :
    case   GDT_EUL_ROTZZXZr   :

    case   GDT_EUL_ROTXXYZr   :
    case   GDT_EUL_ROTYXYZr   :
    case   GDT_EUL_ROTZXYZr   :

    case   GDT_EUL_ROTXZYZr   :
    case   GDT_EUL_ROTYZYZr   :
    case   GDT_EUL_ROTZZYZr   :


      {
        DiMatrix                   mLTM;
        DiFloat                    oldValueX = 0.f;
        DiFloat                    oldValueY = 0.f;
        DiFloat                    oldValueZ = 0.f;

        vRot.w = _Conventions[(tVarType - GDT_EUL_ROTXXYZs) / 3];

        GetLocalMatrix(m_Vme, ts, &mLTM);
        mafTransfMatrixToEuler(&mLTM, &vRot);

        vRot.x *= mafMatrix3x3::RadiansToDegrees();
        vRot.y *= mafMatrix3x3::RadiansToDegrees();
        vRot.z *= mafMatrix3x3::RadiansToDegrees();

        int xindex = tVarType - (tVarType - GDT_EUL_ROTXXYZs) % 3;
        if(m_View->GetGraph()->GetUsedMemSpace() != 0 && pts < ts)
        {
          oldValueX  = m_variables[xindex + 0].first;
          oldValueY  = m_variables[xindex + 1].first;
          oldValueZ  = m_variables[xindex + 2].first;
        }
        vRot.x = _fix180Difference( oldValueX, vRot.x);
        vRot.y = _fix180Difference(-oldValueY, vRot.y);
        vRot.z = _fix180Difference( oldValueZ, vRot.z);
        m_variables[xindex + 0].first  = vRot.x;
        m_variables[xindex + 0].second = true;
        m_variables[xindex + 1].first  = vRot.y;
        m_variables[xindex + 1].second = true;
        m_variables[xindex + 2].first  = vRot.z;
        m_variables[xindex + 2].second = true;
        break;
      }

    default:
      {
        wxASSERT(false);
        break;
      }
    }
  }
  wxASSERT(m_variables[tVarType].second);
  m_View->GetGraph()->SetAddCoord(nGraphIndex, m_variables[tVarType].first);
}

//----------------------------------------------------------------------------
DiVoid mafPipeIntGraph::GrabData(wxInt32 nIdx, mafTimeStamp nTimeStamp)
//----------------------------------------------------------------------------
{
  mafTimeStamp ts;
  DiInt32   nYIdx;
  IDType    nYVarID;

  ts = (nTimeStamp < 0) ? m_Vme->GetOutput()->GetTimeStamp() : nTimeStamp;

  //do not grab data for time steps outside the range
  if(m_View->GetPlotFrameStart() > ts || ts < m_View->GetPlotFrameStart())
  {
    return;
  }

  if(ts != m_PrevStamp)
  {
    //store X variable
    //StoreValueByIdx(nIdx, m_View->GetGraph()->GetXID(0), m_View->GetGraph()->GetXIndex(0));
    for(nYIdx = 0; nYIdx < m_View->GetGraph()->GetDim(); nYIdx++)
    {
      nYVarID = m_View->GetGraph()->GetID(nYIdx);
      //nYIndex = m_View->GetGraph()->GetIndex(nYIdx);
      StoreValueByIdx(nIdx, nYVarID, nYIdx, ts, m_PrevStamp);
    }
    m_PrevStamp = ts;  
  }
}
