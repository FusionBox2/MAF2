/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeIntGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.5 $
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


mafVMEAFRefSys *GetRefSys(mafVME *vme)
{
  mafVMEAFRefSys *afs = NULL;
  mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(vme);
  if(lmc == NULL)
    return NULL;
  for(int i = 0; i < lmc->GetNumberOfChildren(); i++)
  {
    mafNode *child = vme->GetChild(i);
    if(child->IsA("mafVMEAFRefSys"))
    {
      mafVMEAFRefSys *rs = mafVMEAFRefSys::SafeDownCast(child);
      if(rs->GetActive())
      {
        afs = rs;
        break;
      }
    }
  }
  return afs;
}

//----------------------------------------------------------------------------
static void GetGlobalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat)
//----------------------------------------------------------------------------
{
  DiMatrixIdentity(pMat);
  if(vme == NULL)
    return;

  mafMatrix      matrix;
  mafVMEAFRefSys *afs = GetRefSys(vme);
  if(afs == NULL)
    vme->GetOutput()->GetAbsMatrix(matrix, ts);
  else
    afs->CalculateMatrix(matrix, ts);
  mflMatrixToDi(matrix.GetVTKMatrix(), pMat);
}

//----------------------------------------------------------------------------
static void GetLocalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat)
//----------------------------------------------------------------------------
{
  DiMatrix pmatrix;
  DiMatrix cmatrix;
  DiMatrix pInv;

  DiMatrixIdentity(pMat);
  if(vme == NULL)
    return;

  GetGlobalMatrix(vme->GetParent(), ts, &pmatrix);//in case of GetParent == NULL Global matrix is filled as identity
  GetGlobalMatrix(vme,              ts, &cmatrix);

  DiMatrixInvert(&pmatrix, &pInv);
  DiMatrixMultiply(&cmatrix, &pInv, pMat);
}


static int FindParentID(int id)
{
  switch(id)
  {
  case mafVMEAFRefSys::ID_AFS_LTHIGH:
  case mafVMEAFRefSys::ID_AFS_RTHIGH:
    return mafVMEAFRefSys::ID_AFS_PELVIS;
  case mafVMEAFRefSys::ID_AFS_LSHANK:
    return mafVMEAFRefSys::ID_AFS_LTHIGH;
  case mafVMEAFRefSys::ID_AFS_RSHANK:
    return mafVMEAFRefSys::ID_AFS_RTHIGH;
  case mafVMEAFRefSys::ID_AFS_LFOOT:
    return mafVMEAFRefSys::ID_AFS_LSHANK;
  case mafVMEAFRefSys::ID_AFS_RFOOT:
    return mafVMEAFRefSys::ID_AFS_RSHANK;
  default:
    return mafVMEAFRefSys::ID_AFS_NOTDEFINED;
  }
}
static void OVP_GES(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, DiV4d *vOVPPos, DiV4d *vOVPRot, DiV4d *vGESPos, DiV4d *vGESRot)
{
  vOVPPos->x = 0.0;vOVPPos->y = 0.0;vOVPPos->z = 0.0;vOVPPos->w = 0.0;
  vOVPRot->x = 0.0;vOVPRot->y = 0.0;vOVPRot->z = 0.0;vOVPRot->w = 0.0;
  vGESPos->x = 0.0;vGESPos->y = 0.0;vGESPos->z = 0.0;vGESPos->w = 0.0;
  vGESRot->x = 0.0;vGESRot->y = 0.0;vGESRot->z = 0.0;vGESRot->w = 0.0;
  if(vme == NULL)
    return;

  DiMatrix mLTM;
  DiMatrix mRefLTM;
  DiV4d    vTm;
  DiV4d    vOVPRefPos;
  GetLocalMatrix(vme, ts,    &mLTM);
  GetLocalMatrix(vme, tsRef, &mRefLTM);
  mafTransfInverseTransformUpright(&mLTM, &vTm, vOVPRot);

  mafVMEAFRefSys *vmeSys = GetRefSys(vme);
  if(vmeSys == NULL || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_PELVIS)
    return;
  mafVME *parent = vme->GetParent();
  if(parent == NULL)
    return;
  mafVMEAFRefSys *parentSys = GetRefSys(parent);
  if(parentSys == NULL || parentSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED || parentSys->GetBoneID() != FindParentID(vmeSys->GetBoneID()))
    return;

  mafMatrix vmeMatr;
  mafMatrix vmeMatrRef;
  mafMatrix parentMatr;
  mafMatrix parentMatrRef;
  vmeSys->CalculateMatrix(vmeMatr, ts);
  vmeSys->CalculateMatrix(vmeMatrRef, tsRef);
  parentSys->CalculateMatrix(parentMatr, ts);
  parentSys->CalculateMatrix(parentMatrRef, tsRef);
  DiMatrix vmeMatrix;
  DiMatrix vmeMatrixRef;
  DiMatrix parentMatrix;
  DiMatrix parentMatrixRef;
  mflMatrixToDi(vmeMatr.GetVTKMatrix(), &vmeMatrix);
  mflMatrixToDi(vmeMatrRef.GetVTKMatrix(), &vmeMatrixRef);
  mflMatrixToDi(parentMatr.GetVTKMatrix(), &parentMatrix);
  mflMatrixToDi(parentMatrRef.GetVTKMatrix(), &parentMatrixRef);
  if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LTHIGH || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RTHIGH)
  {
    V3d<double> pelvisPnt;
    V3d<double> thighPnt;
    if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RTHIGH)
    {
      if(!parentSys->GetVector("RIAC", tsRef, pelvisPnt) || !vmeSys->GetVector("RFCH", tsRef, thighPnt))
        return;
    }
    else
    {
      if(!parentSys->GetVector("LIAC", tsRef, pelvisPnt) || !vmeSys->GetVector("LFCH", tsRef, thighPnt))
        return;
    }

    DiV4d vPlv, vThg, vTmp, vDist, vProx;
    vPlv.x = -pelvisPnt.x;vPlv.y =  pelvisPnt.y;vPlv.z =  pelvisPnt.z;vPlv.w =  1.0;
    vThg.x = -thighPnt.x; vThg.y =  thighPnt.y; vThg.z =  thighPnt.z; vThg.w =  1.0;
    DiV4dInverseTransformPointTo(&vThg, &vmeMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &vmeMatrix, &vDist);

    DiV4dInverseTransformPointTo(&vPlv, &parentMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &parentMatrix, &vProx);

    DiV4dSub(&vDist, &vProx, &vTmp);
    DiV4dInverseTransformVectorTo(&vTmp, &parentMatrix, vOVPPos);

    DiV4dSub(&vThg, &vPlv, &vTmp);
    DiV4dInverseTransformVectorTo(&vTmp, &parentMatrix, &vOVPRefPos);
  }
  else
  {
    V3d<double> middle;

    if(!parentSys->GetVector("MIDDLE", tsRef, middle))
      return;
    DiV4d vMdl, vTmp, vDist, vProx;
    vMdl.x = -middle.x;vMdl.y =  middle.y;vMdl.z =  middle.z;vMdl.w =  1.0;

    DiV4dInverseTransformPointTo(&vMdl, &vmeMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &vmeMatrix, &vDist);

    DiV4dInverseTransformPointTo(&vMdl, &parentMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &parentMatrix, &vProx);

    DiV4dSub(&vDist, &vProx, &vTmp);
    DiV4dInverseTransformVectorTo(&vTmp, &parentMatrix, vOVPPos);

    vOVPRefPos.x = 0.0;vOVPRefPos.y = 0.0;vOVPRefPos.z = 0.0;vOVPRefPos.w = 1.0;
  }
  vOVPRefPos.x = -vOVPRefPos.x;
  vOVPPos->x   = -vOVPPos->x;

  //GES
  {
    DiMatrix mPGTM;
    DiMatrix mDLTM;
    DiMatrix mDURLTM;
    DiMatrix mDURLTMInv;
    DiMatrix mDGTM;
    DiMatrix mDGTMInv;
    DiMatrix mGSBasic;
    DiMatrix mInitRotX;
    DiMatrix mInitRotY;
    DiMatrix mInitRot;
    DiV4d    vGS, vGSPos;
    DiFloat  rAngleX = 0.f  * diPI / 180.f;
    DiFloat  rAngleY = 11.f * diPI / 180.f;

    DiMatrixIdentity(&mInitRotX);
    DiMatrixIdentity(&mInitRotY);
    DiMatrixRotateRight(&mInitRotX, rAngleX, diREPLACE);
    DiMatrixRotateUp(&mInitRotY, rAngleY, diREPLACE);
    DiMatrixMultiply(&mInitRotX, &mInitRotY, &mInitRot);

    DiMatrixIdentity(&mPGTM);
    DiMatrixMultiply(&mLTM, &mInitRot, &mDLTM);
    DiMatrixCopy(&mRefLTM, &mDURLTM);
    DiMatrixTransposeRotationalSubmatrix(&mDURLTM, &mDURLTMInv);
    DiMatrixMultiply(&mDURLTMInv, &mDLTM, &mDGTMInv);
    mafTransfRightLeftConv(&mDGTMInv, &mDGTM);

    mafTransfMatrixToGES(&mPGTM, &mDGTM, &mGSBasic, &vGS);

    if(vmeSys->GetBoneID() != mafVMEAFRefSys::ID_AFS_NOTDEFINED && vmeSys->GetBoneID() != mafVMEAFRefSys::ID_AFS_PELVIS)//applying additional 
    {
      DiV4dSub(vOVPPos, &vOVPRefPos, &vGSPos);
      DiV4dTransformVectorTo(&vGSPos, &mGSBasic, &vGSPos);
      DiV4dCopy(&vGSPos, vGESPos);
    }
    DiV4dCopy(&vGS, vGESRot);
    if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LTHIGH ||
       vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LSHANK ||
       vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LFOOT  )
    {
      vGESRot->y = -vGESRot->y;
      vGESRot->x = -vGESRot->x;
    }
    if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RSHANK ||
       vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LSHANK )
    {
      vGESRot->z = -vGESRot->z;
    }
  }
}

void mafPipeIntGraph::InvalidateAllVars()
{
  for(unsigned int i = 0; i < m_variables.size(); i++)
    m_variables[i].second = false;
}

//----------------------------------------------------------------------------
void mafPipeIntGraph::StoreValueByIdx(int nObjectOrderID, IDType nVarID, int nGraphIndex, mafTimeStamp nTimeStamp, mafTimeStamp nPrevTimeStamp)
//----------------------------------------------------------------------------
{
  mafTimeStamp     ts  = nTimeStamp;
  mafTimeStamp     pts = nPrevTimeStamp;
  DiInt32          nObjIDByVar = nVarID[0];
  mafGraphDescType tValIDByVar = mafGraphDescType(nVarID[1]);

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

  if(nVarID.isZero())//if timestamp
  {
    m_variables[GDT_FRAME].first  = ts;
    m_variables[GDT_FRAME].second = true;
    m_View->GetGraph()->SetAddCoord(nGraphIndex, ts);//insert timestamp value and stop
    return;
  }

  if(m_variables[tValIDByVar].second)
  {
    m_View->GetGraph()->SetAddCoord(nGraphIndex, m_variables[tValIDByVar].first);
    return;
  }
  
  //GTM
  if(GDT_GTM_POSX <= tValIDByVar && tValIDByVar <= GDT_GTM_ROTZ)
  {
    DiMatrix mat;
    DiV4d    vPos, vRot;
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
  }
  //Helical axis
  else if(GDT_HEL_ROTX <= tValIDByVar && tValIDByVar <= GDT_HEL_ROTZ)
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
  }

  //LTM
  else if(GDT_LTM_POSX <= tValIDByVar && tValIDByVar <= GDT_LTM_ROTZ)
  {
    DiMatrix mLTM;
    DiV4d    vPos, vRot;
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
  }

  //OVP and GES
  else if(GDT_OVP_POSX <= tValIDByVar && tValIDByVar <= GDT_GES_ROTZ)
  {
    DiV4d vOVPRot, vOVPPos;
    DiV4d vGESRot, vGESPos;
    OVP_GES(m_Vme, ts, 0, &vOVPPos, &vOVPRot, &vGESPos, &vGESRot);
    m_variables[GDT_OVP_ROTX].first  = vOVPRot.x * mafMatrix3x3::RadiansToDegrees();
    m_variables[GDT_OVP_ROTX].second = true;
    m_variables[GDT_OVP_ROTY].first  = vOVPRot.y * mafMatrix3x3::RadiansToDegrees();
    m_variables[GDT_OVP_ROTY].second = true;
    m_variables[GDT_OVP_ROTZ].first  = vOVPRot.z * mafMatrix3x3::RadiansToDegrees();
    m_variables[GDT_OVP_ROTZ].second = true;
    m_variables[GDT_OVP_POSX].first  = vOVPPos.x;
    m_variables[GDT_OVP_POSX].second = true;
    m_variables[GDT_OVP_POSY].first  = vOVPPos.y;
    m_variables[GDT_OVP_POSY].second = true;
    m_variables[GDT_OVP_POSZ].first  = vOVPPos.z;
    m_variables[GDT_OVP_POSZ].second = true;

    m_variables[GDT_GES_ROTX].first  = vGESRot.x * mafMatrix3x3::RadiansToDegrees();
    m_variables[GDT_GES_ROTX].second = true;
    m_variables[GDT_GES_ROTY].first  = vGESRot.y * mafMatrix3x3::RadiansToDegrees();
    m_variables[GDT_GES_ROTY].second = true;
    m_variables[GDT_GES_ROTZ].first  = vGESRot.z * mafMatrix3x3::RadiansToDegrees();
    m_variables[GDT_GES_ROTZ].second = true;
    m_variables[GDT_GES_POSX].first  = vGESPos.x;
    m_variables[GDT_GES_POSX].second = true;
    m_variables[GDT_GES_POSY].first  = vGESPos.y;
    m_variables[GDT_GES_POSY].second = true;
    m_variables[GDT_GES_POSZ].first  = vGESPos.z;
    m_variables[GDT_GES_POSZ].second = true;
  }
    //Euler
  else if(GDT_EUL_ROTXXYZs <= tValIDByVar && tValIDByVar <= GDT_EUL_ROTZZYZr)
  {
    DiMatrix mLTM;
    DiV4d    vRot;
    DiFloat  oldValueX = 0.f;
    DiFloat  oldValueY = 0.f;
    DiFloat  oldValueZ = 0.f;

    vRot.w = _Conventions[(tValIDByVar - GDT_EUL_ROTXXYZs) / 3];

    GetLocalMatrix(m_Vme, ts, &mLTM);
    mafTransfMatrixToEuler(&mLTM, &vRot);

    vRot.x *= mafMatrix3x3::RadiansToDegrees();
    vRot.y *= mafMatrix3x3::RadiansToDegrees();
    vRot.z *= mafMatrix3x3::RadiansToDegrees();

    int xindex = tValIDByVar - (tValIDByVar - GDT_EUL_ROTXXYZs) % 3;
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
  }

  //not known value!!
  else
  {
    wxASSERT(false);
  }

  //variable should be calculated and validated here 
  wxASSERT(m_variables[tValIDByVar].second);

  m_View->GetGraph()->SetAddCoord(nGraphIndex, m_variables[tValIDByVar].first);
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
    m_PrevStamp = (m_PrevStamp < 0) ? ts : m_PrevStamp;//Use current time stamp as previous if array is empty
    //store X variable
    //StoreValueByIdx(nIdx, m_View->GetGraph()->GetXID(0), m_View->GetGraph()->GetXIndex(0));
    if(fabs(ts - m_variables[GDT_FRAME].first) > 1e-6)//invalidate variables if ts is updated
    {
      InvalidateAllVars();
      for(nYIdx = 0; nYIdx < m_View->GetGraph()->GetDim(); nYIdx++)
      {
        nYVarID = m_View->GetGraph()->GetID(nYIdx);
        //nYIndex = m_View->GetGraph()->GetIndex(nYIdx);
        StoreValueByIdx(nIdx, nYVarID, nYIdx, ts, m_PrevStamp);
      }
      m_PrevStamp = ts;  
    }
  }
}
