/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafJointAnalysis.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 11:59:34 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
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

#include "mafPlotMath.h"
#include "mafMatrix3x3.h"
#include "mafJointAnalysis.h"
#include "mafVMELandmarkCloud.h"
#include "mmuTimeSet.h"

#include "nr.h"

#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"


namespace
{
//----------------------------------------------------------------------------
inline double _sign(double a) 
//----------------------------------------------------------------------------
{
  return (a>0) ? 1.0 : ((a<0) ? -1.0 : 0.0);
}
}




//----------------------------------------------------------------------------
inline float MathRound(float val)
//----------------------------------------------------------------------------
{
  float rFloor;
  float rCeil;

  rFloor = (float)floor(val);
  rCeil  = (float)ceil(val);
  return (val - rFloor > rCeil - val) ? rCeil : rFloor;
}

//----------------------------------------------------------------------------
float Fix360Difference(float rAold, float rAnew)
//----------------------------------------------------------------------------
{
  float rDiff = rAnew - rAold;
  if(rDiff > 0)
  {
    float rR = rAnew - 360.0 * MathRound((rDiff - fmodf(rDiff, 360.0)) / 360.0);

    if(fabs(rR - rAold) > 180.0)
      return rR - 360.0;
    return rR;//(rA + rRemnant);

  }
  float rR = rAnew - 360.0 * MathRound((rDiff - fmodf(rDiff, 360.0)) / 360.0);

  if(fabs(rR - rAold) > 180.0)
    return rR + 360.0;
  return rR;//(rA + rRemnant);
}

//----------------------------------------------------------------------------
float Fix180Difference(float rAold, float rAnew)
//----------------------------------------------------------------------------
{
  rAnew = Fix360Difference(rAold, rAnew);
  float rDiff = rAnew - rAold;
  if(rDiff > 0)
  {
    float rR = rAnew - 180.0 * MathRound((rDiff - fmodf(rDiff, 180.0)) / 180.0);

    if(fabs(rR - rAold) > 90.0)
      return rR - 180.0;
    return rR;//(rA + rRemnant);

  }
  float rR = rAnew - 180.0 * MathRound((rDiff - fmodf(rDiff, 180.0)) / 180.0);

  if(fabs(rR - rAold) > 90.0)
    return rR + 180.0;
  return rR;//(rA + rRemnant);
}

mafVMERefSysAbstract *GetRefSys(mafVME *vme)
{
  for(int i = 0; i < vme->GetNumberOfChildren(); i++)
  {
    mafVMERefSysAbstract *refsys =mafVMERefSysAbstract::SafeDownCast(vme->GetChild(i));
    if(refsys != NULL)
    {
      return refsys;
    }
  }
  return NULL;
}
mafVMEAFRefSys *GetAFRefSys(mafVME *vme)
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
void GetGlobalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat)
//----------------------------------------------------------------------------
{
  mafMatrix      matrix;
  GetGlobalMatrix(vme, ts, matrix);
  mflMatrixToDi(matrix.GetVTKMatrix(), pMat);
}

//----------------------------------------------------------------------------
void GetLocalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat, mafVME *parent = NULL)
//----------------------------------------------------------------------------
{
  mafMatrix      matrix;
  GetLocalMatrix(vme, ts, matrix, parent);
  mflMatrixToDi(matrix.GetVTKMatrix(), pMat);
}

//----------------------------------------------------------------------------
void GetGlobalMatrix(mafVME *vme, mafTimeStamp ts, mafMatrix& matrix)
//----------------------------------------------------------------------------
{
  matrix.Identity();
  if(vme == NULL)
    return;

  mafVMEAFRefSys *afs = GetAFRefSys(vme);
  if(afs == NULL)
    vme->GetOutput()->GetAbsMatrix(matrix, ts);
  else
    afs->CalculateMatrix(matrix, ts);
}

//----------------------------------------------------------------------------
void GetLocalMatrix(mafVME *vme, mafTimeStamp ts, mafMatrix& matrix, mafVME *parent)
//----------------------------------------------------------------------------
{
  mafMatrix pmatrix;
  mafMatrix cmatrix;

  matrix.Identity();
  if(vme == NULL)
    return;

  if(parent == NULL)
    parent = vme->GetParent();

  GetGlobalMatrix(parent, ts, pmatrix);//in case of GetParent == NULL Global matrix is filled as identity
  GetGlobalMatrix(vme,              ts, cmatrix);

  pmatrix.Invert();
  mafMatrix::Multiply4x4(pmatrix, cmatrix, matrix);
}


int FindParentID(int id)
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
void OVP_GES(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, DiV4d *vOVPPos, DiV4d *vOVPRot, DiV4d *vGESPos, DiV4d *vGESRot, mafVME *parent)
{
  DiV4d vOVPPosInt;
  DiV4d vOVPRotInt;
  DiV4d vGESPosInt;
  DiV4d vGESRotInt;

  DiV4d *vOVPPosOut = (vOVPPos == NULL) ? &vOVPPosInt : vOVPPos;
  DiV4d *vOVPRotOut = (vOVPRot == NULL) ? &vOVPRotInt : vOVPRot;
  DiV4d *vGESPosOut = (vGESPos == NULL) ? &vGESPosInt : vGESPos;
  DiV4d *vGESRotOut = (vGESRot == NULL) ? &vGESRotInt : vGESRot;
  vOVPPosOut->x = 0.0;vOVPPosOut->y = 0.0;vOVPPosOut->z = 0.0;vOVPPosOut->w = 0.0;
  vOVPRotOut->x = 0.0;vOVPRotOut->y = 0.0;vOVPRotOut->z = 0.0;vOVPRotOut->w = 0.0;
  vGESPosOut->x = 0.0;vGESPosOut->y = 0.0;vGESPosOut->z = 0.0;vGESPosOut->w = 0.0;
  vGESRotOut->x = 0.0;vGESRotOut->y = 0.0;vGESRotOut->z = 0.0;vGESRotOut->w = 0.0;
  if(vme == NULL)
    return;

  DiMatrix mLTM;
  DiMatrix mRefLTM;
  DiV4d    vTm;
  DiV4d    vOVPRefPos;
  GetLocalMatrix(vme, ts,    &mLTM, parent);
  GetLocalMatrix(vme, tsRef, &mRefLTM, parent);
  mafTransfInverseTransformUpright(&mLTM, &vTm, vOVPRotOut);

  mafVMEAFRefSys *vmeSys = GetAFRefSys(vme);
  if(vmeSys == NULL || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return;
  if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_PELVIS)
  {
    DiV4dCopy(&vTm, vOVPPosOut);
    DiV4dCopy(vOVPRotOut, vGESRotOut);
    DiV4dCopy(vOVPPosOut, vGESPosOut);
    return;
  }
  if(parent == NULL)
    parent = vme->GetParent();
  if(parent == NULL)
    return;
  mafVMEAFRefSys *parentSys = GetAFRefSys(parent);
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
    DiV4dInverseTransformVectorTo(&vTmp, &parentMatrix, vOVPPosOut);

    DiV4dSub(&vThg, &vPlv, &vTmp);
    DiV4dInverseTransformVectorTo(&vTmp, &parentMatrix, &vOVPRefPos);
  }
  else
  {
    V3d<double> middle;

    middle.x = parentMatrRef.GetVTKMatrix()->GetElement(0, 3);
    middle.y = parentMatrRef.GetVTKMatrix()->GetElement(1, 3);
    middle.z = parentMatrRef.GetVTKMatrix()->GetElement(2, 3);
    
    //if(!parentSys->GetVector("MIDDLE", tsRef, middle))
    //  return;
    DiV4d vMdl, vTmp, vDist, vProx;
    vMdl.x = -middle.x;vMdl.y =  middle.y;vMdl.z =  middle.z;vMdl.w =  1.0;

    DiV4dInverseTransformPointTo(&vMdl, &vmeMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &vmeMatrix, &vDist);

    DiV4dInverseTransformPointTo(&vMdl, &parentMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &parentMatrix, &vProx);

    DiV4dSub(&vDist, &vProx, &vTmp);
    DiV4dInverseTransformVectorTo(&vTmp, &parentMatrix, vOVPPosOut);

    vOVPRefPos.x = 0.0;vOVPRefPos.y = 0.0;vOVPRefPos.z = 0.0;vOVPRefPos.w = 1.0;
  }
  vOVPRefPos.x  = -vOVPRefPos.x;
  vOVPPosOut->x = -vOVPPosOut->x;

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
    float    rAngleX = 0.f  * diPI / 180.f;
    float    rAngleY = 11.f * diPI / 180.f;

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
      DiV4dSub(vOVPPosOut, &vOVPRefPos, &vGSPos);
      DiV4dTransformVectorTo(&vGSPos, &mGSBasic, &vGSPos);
      DiV4dCopy(&vGSPos, vGESPosOut);
    }
    DiV4dCopy(&vGS, vGESRotOut);
    if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LTHIGH ||
      vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LSHANK ||
      vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LFOOT  )
    {
      vGESRotOut->y = -vGESRotOut->y;
      vGESRotOut->x = -vGESRotOut->x;
    }
    if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_RSHANK ||
      vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_LSHANK )
    {
      vGESRotOut->z = -vGESRotOut->z;
    }
  }
}


void SetOVP(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, DiV4d *vOVPPos, DiV4d *vOVPRot)
{
  if(vme == NULL)
    return;

  mafMatrix vmeNodeLMatr;
  mafMatrix vmeNodeGMatr;
  vme->GetOutput()->GetMatrix(vmeNodeLMatr, ts);
  vme->GetOutput()->GetAbsMatrix(vmeNodeGMatr, ts);

  DiMatrix vmeNodeLMatrix;
  DiMatrix vmeNodeGMatrix;
  mflMatrixToDi(vmeNodeLMatr.GetVTKMatrix(), &vmeNodeLMatrix);
  mflMatrixToDi(vmeNodeGMatr.GetVTKMatrix(), &vmeNodeGMatrix);

  DiMatrix mLTM;
  DiMatrix mLTMTrg;
  DiMatrix mRefLTM;
  DiMatrix mGTM;

  DiMatrix mAFCL;
  DiMatrix mAFCLInv;
  DiMatrix mTmp;

  GetLocalMatrix(vme,  ts,    &mLTM);
  GetGlobalMatrix(vme, ts,    &mGTM);
  GetLocalMatrix(vme,  tsRef, &mRefLTM);

  //mafTransfInverseTransformUpright(&mLTM, &vTm, vOVPRotOut);
  DiV4d    vPosComp, vRotComp;
  //vPosComp.x = 0.0;vPosComp.y = 0.0;vPosComp.z = 0.0;vPosComp.w = 1.0;
  vPosComp.x = vOVPPos->x;vPosComp.y = vOVPPos->y;vPosComp.z = vOVPPos->z;vPosComp.w = 1.0;
  vRotComp.x = vOVPRot->x;vRotComp.y = vOVPRot->y;vRotComp.z = vOVPRot->z;vRotComp.w = 1.0;
  mafTransfTransformUpright(&vPosComp, &vRotComp, &mLTMTrg);
  
  DiMatrix mOVPMatrix;
  DiMatrix mOVPMatrixTrg;
  DiMatrix mDelta;

  DiMatrixInvert(&vmeNodeGMatrix, &mTmp);
  DiMatrixMultiply(&mGTM, &mTmp, &mAFCL);
  DiMatrixInvert(&mAFCL, &mAFCLInv);

  DiMatrixMultiply(&mAFCLInv, &mLTM,    &mOVPMatrix);
  DiMatrixMultiply(&mAFCLInv, &mLTMTrg, &mOVPMatrixTrg);
  DiMatrixInvert(&mOVPMatrix, &mTmp);
  DiMatrixMultiply(&mOVPMatrixTrg, &mTmp, &mDelta);

  DiMatrixCopy(&vmeNodeLMatrix, &mTmp);
  DiMatrixMultiply(&mDelta, &mTmp, &vmeNodeLMatrix);
  DiMatrixToVTK(&vmeNodeLMatrix, vmeNodeLMatr.GetVTKMatrix());
  vme->SetPose(vmeNodeLMatr, ts);

  mafVMEAFRefSys *vmeSys = GetAFRefSys(vme);
  if(vmeSys == NULL || vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED)
    return;
  if(vmeSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_PELVIS)
  {
    return;
  }
  mafVME *parent = vme->GetParent();
  if(parent == NULL)
    return;
  mafVMEAFRefSys *parentSys = GetAFRefSys(parent);
  if(parentSys == NULL || parentSys->GetBoneID() == mafVMEAFRefSys::ID_AFS_NOTDEFINED || parentSys->GetBoneID() != FindParentID(vmeSys->GetBoneID()))
    return;

  mafMatrix parNodeLMatr;
  mafMatrix parNodeGMatr;

  vme->GetOutput()->GetMatrix(vmeNodeLMatr, ts);
  vme->GetOutput()->GetAbsMatrix(vmeNodeGMatr, ts);
  parent->GetOutput()->GetMatrix(parNodeLMatr, ts);
  parent->GetOutput()->GetAbsMatrix(parNodeGMatr, ts);
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
  DiMatrix parNodeGMatrix;
  mflMatrixToDi(vmeMatr.GetVTKMatrix(), &vmeMatrix);
  mflMatrixToDi(vmeMatrRef.GetVTKMatrix(), &vmeMatrixRef);
  mflMatrixToDi(parentMatr.GetVTKMatrix(), &parentMatrix);
  mflMatrixToDi(parentMatrRef.GetVTKMatrix(), &parentMatrixRef);
  mflMatrixToDi(vmeNodeLMatr.GetVTKMatrix(), &vmeNodeLMatrix);
  mflMatrixToDi(vmeNodeGMatr.GetVTKMatrix(), &vmeNodeGMatrix);
  mflMatrixToDi(parNodeGMatr.GetVTKMatrix(), &parNodeGMatrix);

  DiV4d vOVPPosTrg;
  vOVPPosTrg.x = -vOVPPos->x;vOVPPosTrg.y = vOVPPos->y;vOVPPosTrg.z = vOVPPos->z;vOVPPosTrg.w = 1.0;

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

    DiV4d vPlv, vThg, vTmp, vDist, vDistCur, vDelta, vProx;
    vPlv.x = -pelvisPnt.x;vPlv.y =  pelvisPnt.y;vPlv.z =  pelvisPnt.z;vPlv.w =  1.0;
    vThg.x = -thighPnt.x; vThg.y =  thighPnt.y; vThg.z =  thighPnt.z; vThg.w =  1.0;
    DiV4dInverseTransformPointTo(&vThg, &vmeMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &vmeMatrix, &vDistCur);

    DiV4dInverseTransformPointTo(&vPlv, &parentMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &parentMatrix, &vProx);



    DiV4dTransformVectorTo(&vOVPPosTrg, &parentMatrix, &vTmp);
    DiV4dAdd(&vTmp, &vProx, &vDist);
    DiV4dSub(&vDist, &vDistCur, &vTmp);
    DiV4dInverseTransformVectorTo(&vTmp, &parNodeGMatrix, &vDelta);
    DiV4dCopy(&vmeNodeLMatrix.vPos, &vTmp);
    DiV4dAdd(&vTmp, &vDelta, &vmeNodeLMatrix.vPos);
    DiMatrixToVTK(&vmeNodeLMatrix, vmeNodeLMatr.GetVTKMatrix());
    vme->SetPose(vmeNodeLMatr, ts);
  }
  else
  {
    V3d<double> middle;

    middle.x = parentMatrRef.GetVTKMatrix()->GetElement(0, 3);
    middle.y = parentMatrRef.GetVTKMatrix()->GetElement(1, 3);
    middle.z = parentMatrRef.GetVTKMatrix()->GetElement(2, 3);

    //if(!parentSys->GetVector("MIDDLE", tsRef, middle))
    //  return;
    DiV4d vMdl, vTmp, vDist, vDistCur, vDelta, vProx;
    vMdl.x = -middle.x;vMdl.y =  middle.y;vMdl.z =  middle.z;vMdl.w =  1.0;

    DiV4dInverseTransformPointTo(&vMdl, &vmeMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &vmeMatrix, &vDistCur);

    DiV4dInverseTransformPointTo(&vMdl, &parentMatrixRef, &vTmp);
    DiV4dTransformPointTo(&vTmp, &parentMatrix, &vProx);

    DiV4dTransformVectorTo(&vOVPPosTrg, &parentMatrix, &vTmp);
    DiV4dAdd(&vTmp, &vProx, &vDist);
    DiV4dSub(&vDist, &vDistCur, &vTmp);
    DiV4dInverseTransformVectorTo(&vTmp, &parNodeGMatrix, &vDelta);
    DiV4dCopy(&vmeNodeLMatrix.vPos, &vTmp);
    DiV4dAdd(&vTmp, &vDelta, &vmeNodeLMatrix.vPos);
    DiMatrixToVTK(&vmeNodeLMatrix, vmeNodeLMatr.GetVTKMatrix());
    vme->SetPose(vmeNodeLMatr, ts);
  }
}


//modified by STEFY 10-7-2003(begin)//modified because Helical Axis conversion must be made 
//from a translation matrix (not from a rotation matrix)
int MatrixToHelicalAxis(const mafMatrix &matrix, double helical_axis[3],double point[3], double& phi,double& t)
//----------------------------------------------------------------------------
{
  // input: 
  // mafMatrix &matrix (translation matrix)
  // int intersect:location of the screw axis where it intersects either the X, Y, or the Z plane
  // default: intersect = Z

  // output:
  // helical_axis[3] is the unit vector with direction of helical axis
  // point[3] is the point on helical axis 
  // phi is the rotation angle (in deg)
  // t is the amount of translation along screw axis


  double tmp[3];

  tmp[0] = matrix.GetElement(2,1) - matrix.GetElement(1,2);
  tmp[1] = matrix.GetElement(0,2) - matrix.GetElement(2,0);
  tmp[2] = matrix.GetElement(1,0) - matrix.GetElement(0,1);

  double quad_sum;
  quad_sum = 0;
  int i;
  for (i=0; i<3; i++)
  {

    quad_sum += (tmp[i] * tmp[i]);
  }


  quad_sum = sqrt(quad_sum);

  for (i=0; i<3; i++)
  {
    tmp[i] = tmp[i]/quad_sum;

  }



  helical_axis[0] = tmp[0];
  helical_axis[1] = tmp[1];
  helical_axis[2] = tmp[2];



  //if (quad_sum <= sqrt(2.0)) 
  {
    //phi=asin(0.5*quad_sum);
    //phi = phi * mafMatrix3x3::RadiansToDegrees();

  /*}

  else  
  {*/
    double sum = matrix.GetElement(0,0)+matrix.GetElement(1,1)+matrix.GetElement(2,2)-1;
    //phi=acos(0.5*sum);
    phi = atan2(0.5*quad_sum, 0.5*sum);//fix of a bug
    phi = phi * mafMatrix3x3::RadiansToDegrees();
  }



  //if phi approaches 180 deg it is better to use the following:
  if (phi>135)
  {
    mafMatrix b_mat;

    double phi_rad = phi * mafMatrix3x3::DegreesToRadians();

    double b00 = 0.5 * (matrix.GetElement(0,0)+matrix.GetElement(0,0)) - cos(phi_rad); 
    double b01 = 0.5 * (matrix.GetElement(0,1)+matrix.GetElement(1,0)); 
    double b02 = 0.5 * (matrix.GetElement(0,2)+matrix.GetElement(2,0)); 

    double b10 = 0.5 * (matrix.GetElement(1,0)+matrix.GetElement(0,1)); 
    double b11 = 0.5 * (matrix.GetElement(1,1)+matrix.GetElement(1,1)) - cos(phi_rad); 
    double b12 = 0.5 * (matrix.GetElement(1,2)+matrix.GetElement(2,1)); 

    double b20 = 0.5 * (matrix.GetElement(2,0)+matrix.GetElement(0,2)); 
    double b21 = 0.5 * (matrix.GetElement(2,1)+matrix.GetElement(1,2)); 
    double b22 = 0.5 * (matrix.GetElement(2,2)+matrix.GetElement(2,2)) - cos(phi_rad); 



    b_mat.SetElement(0,0,b00);
    b_mat.SetElement(0,1,b01);
    b_mat.SetElement(0,2,b02);

    b_mat.SetElement(1,0,b10);
    b_mat.SetElement(1,1,b11);
    b_mat.SetElement(1,2,b12);

    b_mat.SetElement(2,0,b20);
    b_mat.SetElement(2,1,b21);
    b_mat.SetElement(2,2,b22);



    double btmp[3];

    btmp[0] = b00*b00 + b10*b10 + b20*b20;
    btmp[1] = b01*b01 + b11*b11 + b21*b21;
    btmp[2] = b02*b02 + b12*b12 + b22*b22;


    double bmax = 0.0;
    int index = 0;

    for (i=0; i<3; i++)
    {

      if (btmp[i] > bmax)
      {

        bmax = btmp[i];
        index = i;

      }

    }


    helical_axis[0] = (b_mat.GetElement(0,index))/sqrt(bmax);
    helical_axis[1] = (b_mat.GetElement(1,index))/sqrt(bmax);
    helical_axis[2] = (b_mat.GetElement(2,index))/sqrt(bmax);


    if ( _sign(matrix.GetElement(2,1)- matrix.GetElement(1,2)) != _sign(helical_axis[0]) )
    {
      helical_axis[0] = (-1) * helical_axis[0];
      helical_axis[1]	= (-1) * helical_axis[1];
      helical_axis[2]	= (-1) * helical_axis[2];

    }

  }


  // calculation of t:amount of translation along screw axis

  t = helical_axis[0] * matrix.GetElement(0,3) + helical_axis[1] * matrix.GetElement(1,3)	+ 
    helical_axis[2] * matrix.GetElement(2,3);

  //calculating where the screw axis intersects the plane as defined in 'intersect'

  double q_mat[3][3];
  double q_inv[3][3];
  double cphi = cos(phi * mafMatrix3x3::DegreesToRadians());
  double sphi = sin(phi * mafMatrix3x3::DegreesToRadians());

  q_mat[0][0] = 1 - cphi;
  q_mat[0][1] =  sphi * helical_axis[2];
  q_mat[0][2] = -sphi * helical_axis[1];

  q_mat[1][0] = -sphi * helical_axis[2];
  q_mat[1][1] = 1- cphi;
  q_mat[1][2] =  sphi * helical_axis[0];

  q_mat[2][0] =  sphi * helical_axis[1];
  q_mat[2][1] = -sphi * helical_axis[0];
  q_mat[2][2] = 1- cphi;

  mafMatrix3x3::Invert(q_mat, q_inv);


  double v03 = (matrix.GetElement(0,3)) - t * helical_axis[0];
  double v13 = (matrix.GetElement(1,3)) - t * helical_axis[1];
  double v23 = (matrix.GetElement(2,3)) - t * helical_axis[2];


  // calculting the point on helical axis 

  point[0] = q_inv[0][0] * v03 + q_inv[0][1] * v13 + q_inv[0][2] * v23;  
  point[1] = q_inv[1][0] * v03 + q_inv[1][1] * v13 + q_inv[1][2] * v23;
  point[2] = q_inv[2][0] * v03 + q_inv[2][1] * v13 + q_inv[2][2] * v23;

  return 1;
}

bool mafTransfInverseTransformUpright(mafMatrix const *mpIn, V4d<double> *vpPos, V4d<double> *vpRot)
{
  DiMatrix dm;
  mflMatrixToDi(mpIn->GetVTKMatrix(), &dm);
  return mafTransfInverseTransformUpright(&dm, vpPos, vpRot);
}

bool mafTransfMatrixToEuler(mafMatrix const *mpR, V4d<double> *vpR, int conv)
{
  DiMatrix dm;
  mflMatrixToDi(mpR->GetVTKMatrix(), &dm);
  return mafTransfMatrixToEuler(&dm, vpR, conv);
}
bool mafTransfComposeMatrixStright(mafMatrix *mpIn, V4d<double> const *vpRot, V4d<double> const *vpPos)
{
  DiMatrix dm;
  DiMatrix dm1;
  bool result = mafTransfComposeMatrixStright(&dm1, vpRot, vpPos);
  mafTransfRightLeftConv(&dm1, &dm);
  DiMatrixToVTK(&dm, mpIn->GetVTKMatrix());
  return result;
}


namespace
{
  //----------------------------------------------------------------------------
  int ExtractMatchingPoints(mafVMELandmarkCloud *src, mafTimeStamp srctime, mafTimeStamp trgtime, const mafMatrix& mpar1, const mafMatrix& mpar2, vtkPoints *pSrc, vtkPoints *pTrg)
    //----------------------------------------------------------------------------
  {
    double lmsrc[4];
    double lmtrg[4];
    double lmsrctrf[4];
    double lmtrgtrf[4];
    int    i;
    int    npSource    = src->GetNumberOfLandmarks();
    int    npTarget    = npSource;
    int    ncp         = 0;

    mafMatrix mcur;
    mafMatrix mnxt;

    mafMatrix mparcur;
    mafMatrix mparnxt;

    mparcur = mpar1;
    mparnxt = mpar2;

    mparcur.Invert();
    mparnxt.Invert();

    src->GetOutput()->GetAbsMatrix(mcur, srctime);
    src->GetOutput()->GetAbsMatrix(mnxt, trgtime);

    pSrc->Reset();
    pTrg->Reset();

    for(i = 0; i < npSource; i++)
    {
      if(!src->GetLandmarkVisibility(i,srctime) || !src->GetLandmarkVisibility(i,trgtime))
        continue;

      //add new points to arrays if no one is zero (zero indicates that data are invalid)
      src->GetLandmark(i, lmsrc, srctime);
      src->GetLandmark(i, lmtrg, trgtime);

      lmsrc[3] = 1.0;
      lmtrg[3] = 1.0;
      mcur.MultiplyPoint(lmsrc, lmsrctrf);
      mnxt.MultiplyPoint(lmtrg, lmtrgtrf);

      mparcur.MultiplyPoint(lmsrctrf, lmsrc);
      mparnxt.MultiplyPoint(lmtrgtrf, lmtrg);

      pSrc->InsertNextPoint(lmsrc);
      pTrg->InsertNextPoint(lmtrg);
      ncp++;
    }

    return ncp;
  }
  //----------------------------------------------------------------------------
  double RegisterPoints(vtkPoints *pSrc, vtkPoints *pTrg, vtkMatrix4x4 *res_matrix)
    //----------------------------------------------------------------------------
  {
    int i;
    double deviation = 0.0;
    double dx, dy, dz;
    assert(pSrc && pTrg);

    vtkWeightedLandmarkTransform *RegisterTransform = vtkWeightedLandmarkTransform::New();

    //setup transform calculator
    RegisterTransform->SetSourceLandmarks(pSrc);
    RegisterTransform->SetTargetLandmarks(pTrg);

    RegisterTransform->SetModeToRigidBody();// Similarity();
    //calculate transform
    RegisterTransform->Update();
    RegisterTransform->GetMatrix(res_matrix);

    //calculate deviation
    for(i = 0; i < pSrc->GetNumberOfPoints(); i++)
    {
      double coord[4];
      double result[4];
      double target[3];
      pSrc->GetPoint(i, coord);
      coord[3] = 1.0;

      pTrg->GetPoint(i, target);

      //transform point
      res_matrix->MultiplyPoint(coord, result);

      dx = target[0] - result[0];
      dy = target[1] - result[1];
      dz = target[2] - result[2];

      deviation += dx * dx + dy * dy + dz * dz;
    }
    if(pSrc->GetNumberOfPoints() != 0)
      deviation /= pSrc->GetNumberOfPoints();
    deviation = sqrt(deviation);

    vtkDEL(RegisterTransform);
    return deviation;
  }

  void findTransform(mafVMELandmarkCloud *src, mafTimeStamp srctime, mafTimeStamp trgtime, const mafMatrix& mpar1, const mafMatrix& mpar2, mafMatrix& result)
  {
    vtkPoints    *pointsCur = vtkPoints::New();
    vtkPoints    *pointsNxt = vtkPoints::New();
    ExtractMatchingPoints(src, srctime, trgtime, mpar1, mpar2, pointsCur, pointsNxt);
    RegisterPoints(pointsCur, pointsNxt, result.GetVTKMatrix());
    vtkDEL(pointsCur);
    vtkDEL(pointsNxt);
  }

  void GetLocalTransform(mafVME *proximalVME, mafVME *vme, mafTimeStamp ts1, mafTimeStamp ts2, mafMatrix& transf)
  {
    mafMatrix     mt;
    mafMatrix     mt1;
    mafMatrix     tmp;

    transf.Identity();
    if(vme == NULL)
      return;


    mafVMELandmarkCloud *lm = mafVMELandmarkCloud::SafeDownCast(vme);
    if(lm != NULL && GetAFRefSys(vme) == NULL)
    {
      mafMatrix mpar1, mpar2;
      mpar1.Identity();
      mpar2.Identity();
      if(proximalVME != NULL)
      {
        GetGlobalMatrix(proximalVME, ts1, mpar1);
        GetGlobalMatrix(proximalVME, ts2, mpar2);
      }
      findTransform(lm, ts1, ts2, mpar1, mpar2, transf);
      return;
    }

    GetLocalMatrix(vme, ts1, mt, proximalVME);
    GetLocalMatrix(vme, ts2, mt1, proximalVME);
    mt.Invert();
    mafMatrix::Multiply4x4(mt1, mt, transf);
  }

  bool MultiplyMatVec(Mat_I_DP& m, Vec_I_DP& vi, Vec_O_DP& vo)
  {
    assert(m.ncols() == vi.size());
    assert(m.nrows() == vo.size());
    if((m.ncols() != vi.size()) || (m.nrows() != vo.size()))
      return false;
    for(int i = 0; i < vo.size(); i++)
    {
      vo[i] = 0.0;
      for(int j = 0; j < vi.size(); j++)
      {
        vo[i] += m[i][j] * vi[j];
      }
    }
    return true;
  }

  bool MultiplyMatrix(Mat_I_DP& i1, Mat_I_DP& i2, Mat_O_DP& o)
  {
    assert(i1.ncols() == i2.nrows());
    assert(i1.nrows() == o.nrows());
    assert(i2.ncols() == o.ncols());
    if((i1.ncols() != i2.nrows()) || (i1.nrows() != o.nrows()) || (i2.ncols() != o.ncols()))
      return false;
    for(int i = 0; i < o.nrows(); i++)
    {
      for(int j = 0; j < o.ncols(); j++)
      {
        o[i][j] = 0.0;
        for(int k = 0; k < i1.ncols(); k++)
        {
          o[i][j] += i1[i][k] * i2[k][j];
        }
      }
    }
    return true;
  }

  bool AddMatrix(Mat_I_DP& i1, Mat_I_DP& i2, Mat_O_DP& o)
  {
    assert(i1.nrows() == i2.nrows());
    assert(i1.ncols() == i2.ncols());
    assert(i2.nrows() == o.nrows());
    assert(i2.ncols() == o.ncols());
    if((i1.nrows() != i2.nrows()) || (i1.ncols() != i2.ncols()) || (i2.nrows() != o.nrows()) || (i2.ncols() != o.ncols()))
      return false;
    for(int i = 0; i < o.nrows(); i++)
    {
      for(int j = 0; j < o.ncols(); j++)
      {
        o[i][j] = i1[i][j] + i2[i][j];
      }
    }
    return true;
  }

  bool SubMatrix(Mat_I_DP& i1, Mat_I_DP& i2, Mat_O_DP& o)
  {
    assert(i1.nrows() == i2.nrows());
    assert(i1.ncols() == i2.ncols());
    assert(i2.nrows() == o.nrows());
    assert(i2.ncols() == o.ncols());
    if((i1.nrows() != i2.nrows()) || (i1.ncols() != i2.ncols()) || (i2.nrows() != o.nrows()) || (i2.ncols() != o.ncols()))
      return false;
    for(int i = 0; i < o.nrows(); i++)
    {
      for(int j = 0; j < o.ncols(); j++)
      {
        o[i][j] = i1[i][j] - i2[i][j];
      }
    }
    return true;
  }

  bool IdentityMatrix(Mat_IO_DP& mi)
  {
    assert(mi.nrows() == mi.ncols());
    if(mi.nrows() != mi.ncols())
      return false;
    for(int i = 0; i < mi.nrows(); i++)
    {
      for(int j = 0; j < mi.ncols(); j++)
      {
        if(i == j)
          mi[i][j] = 1.0;
        else
          mi[i][j] = 0.0;
      }
    }
    return true;
  }


  bool TransposeMatrix(Mat_I_DP& mi, Mat_O_DP& o)
  {
    assert(mi.ncols() == o.nrows());
    assert(mi.nrows() == o.ncols());
    if((mi.ncols() != o.nrows()) || (mi.nrows() != o.ncols()))
      return false;
    for(int i = 0; i < o.nrows(); i++)
    {
      for(int j = 0; j < o.ncols(); j++)
      {
        o[i][j] = mi[j][i];
      }
    }
    return true;
  }

  bool VecMatrix(Vec_I_DP& vi, Mat_O_DP& o)
  {
    assert(vi.size() == o.nrows());
    assert(o.ncols() == 1);
    if((vi.size() != o.nrows()) || (o.ncols() != 1))
      return false;
    for(int i = 0; i < o.nrows(); i++)
    {
      o[i][0] = vi[i];
    }
    return true;
  }


  bool MaxEigen(int N, int NDIM, Mat_I_DP& A, double EPS, Vec_O_DP& E_vec, double& E_lam, int& Ier, int& K_iter)
  {
    Ier = 1001;
    assert(E_vec.size() == 3);
    if(E_vec.size() != 3)
      return false;
    for(int i = 0; i < 3; i++)
      E_vec[i] = sqrt(1.0 * N);
    K_iter = 0;
    double R, S = 1.0 + EPS;
    while(S > EPS && K_iter <= Ier)
    {
      K_iter = K_iter + 1;
      Vec_DP Wrk(E_vec);
      MultiplyMatVec(A, Wrk, E_vec);
      R = sqrt(E_vec[0] * E_vec[0] + E_vec[1] * E_vec[1] + E_vec[2] * E_vec[2]);
      if(R <= EPS)
      {
        Ier = 1; 
        return true;
      }
      R = 1.0 / R;
      for(int i = 0; i < 3; i++)
        E_vec[i] *= R;
      for(int i = 0; i < 3; i++)
        Wrk[i] -= E_vec[i];
      S = sqrt(Wrk[0] * Wrk[0] + Wrk[1] * Wrk[1] + Wrk[2] * Wrk[2]);
    }
    if(S <= EPS)
      Ier = 0;
    E_lam = R;

    return true;
  }
  void GetProxDistTs(mafVME *prox, mafVME *dist, std::vector<mafTimeStamp> &stamps)
  {
    std::vector<mafTimeStamp> pt;
    std::vector<mafTimeStamp> dt;
    if(prox)
      prox->GetAbsTimeStamps(pt);
    if(dist)
      dist->GetAbsTimeStamps(dt);
    mmuTimeSet::Merge(dt, pt, stamps);
  }

}


//----------------------------------------------------------------------------
void GetMomentAxis(mafVME *proximal, mafVME *distal, mafTimeStamp tsTime, double minAngle, 
                   V3d<double>& helicalAxis, V3d<double>& point, double& angle, double& translationAmount, mafTimeStamp tsTimeTo)
                   //----------------------------------------------------------------------------
{
  mafMatrix                 mt;
  mafMatrix                 mtNxt;
  std::vector<mafTimeStamp> timeStamps;

  mafTimeStamp tscur, tsnxt;
  bool         inited = false;

  helicalAxis       = V3d<double>(1.0, 0.0, 0.0);
  point             = V3d<double>(0.0, 0.0, 0.0);
  angle             = 0;
  translationAmount = 0;

  if(distal == NULL)
    return;

  size_t       cur, nxt;

  GetProxDistTs(proximal, distal, timeStamps);
  if(timeStamps.size() == 0)
    return;

  cur   = 0;
  while((cur + 1 < timeStamps.size()) && (timeStamps[cur + 1] <= tsTime))
    cur++;
  tscur = timeStamps[cur];

  nxt = cur + 1;
  while(nxt < timeStamps.size() && (timeStamps[nxt] < tsTimeTo))
    nxt++;
  tscur = timeStamps[cur];

  for(/*nxt = cur + 1*/; nxt < timeStamps.size(); nxt++)
  {
    tsnxt = timeStamps[nxt];
    GetLocalTransform(proximal, distal, tscur, tsnxt, mt);

    angle       = 0.0;
    helicalAxis = V3d<double>(1.0, 0.0, 0.0);
    double cq = 0.5 * (mt.GetElement(0, 0) + mt.GetElement(1, 1) + mt.GetElement(2, 2) - 1.0);
    if(fabs(cq) < 1.0)
      angle = fabs(acos(cq) * mafMatrix3x3::RadiansToDegrees());

    if(angle >= minAngle)
    {
      MatrixToHelicalAxis(mt, helicalAxis.components, point.components, angle, translationAmount);
    }
    if(angle < 0)
    {
      helicalAxis       = -helicalAxis;
      angle             = -angle;
      translationAmount = -translationAmount;
    }
    if(angle >= minAngle)
      break;
  }

  return;
}

void GetMeanAxis(mafVME *proximal, mafVME *distal, mafTimeStamp tsMinTime, mafTimeStamp tsMaxTime, double minAngle, V3d<double>& Direction, V3d<double>& StartPoint, double& resang)
{
  std::vector<mafTimeStamp> stamps;
  unsigned                  taken = 0;

  Direction  = V3d<double>(1.0, 0, 0);
  StartPoint = V3d<double>(0, 0, 0);
  resang     = 0.0;

  if(distal == NULL)
    return;

  GetProxDistTs(proximal, distal, stamps);

  Mat_DP Qt_t(0.0, 3, 3);
  Mat_DP R_MHA(0.0, 3, 3);
  Vec_DP Qv_t(0.0, 3);
  unsigned tstart = 0;
  unsigned tend   = 0;

  Vec_DP rc(0.0, 3);
  Mat_DP Qmatr(0.0, 3, 3);
  Vec_O_INT vidx(3);
  DP dp;

  unsigned start_index;
  unsigned end_index;
  for(start_index = 0; start_index < stamps.size(); start_index++)
  {
    if(stamps[start_index] >= tsMinTime)
      break;
  }
  for(end_index = stamps.size(); end_index > 0; end_index--)
  {
    if(stamps[end_index - 1] <= tsMaxTime)
      break;
  }
  tstart = start_index;
  for(unsigned t = start_index + 1; t < end_index; t++)
  {
    mafMatrix mt;
    tend = t;
    GetLocalTransform(proximal, distal, stamps[tstart], stamps[tend], mt);
    V3d<double> helicalAxis; 
    V3d<double> point; 
    double angle; 
    double translationAmount;

    helicalAxis = V3d<double>(1.0, 0.0, 0.0);
    angle       = 0.0;
    double cq = 0.5 * (mt.GetElement(0, 0) + mt.GetElement(1, 1) + mt.GetElement(2, 2) - 1.0);
    if(fabs(cq) < 1.0)
      angle = fabs(acos(cq) * mafMatrix3x3::RadiansToDegrees());


    if(angle >= minAngle)
      MatrixToHelicalAxis(mt, helicalAxis.components, point.components, angle, translationAmount);
    else
      continue;
    if(angle < 0)
    {
      helicalAxis       = -helicalAxis;
      angle             = -angle;
      translationAmount = -translationAmount;
    }

    tstart = tend;
    taken++;

    Mat_DP R(0.0, 3, 3);
    Mat_DP E(0.0, 3, 3);
    Mat_DP Q(0.0, 3, 3);
    Mat_DP Q1(0.0, 3, 3);
    IdentityMatrix(E);
    for(int i = 0; i < 3; i++)
    {
      for(int j = 0; j < 3; j++)
      {
        R[i][j] = mt.GetElement(i, j);
      }
    }
    SubMatrix(R, E, Q);
    TransposeMatrix(Q, Q1);

    Mat_DP Qt(0.0, 3, 3);
    Mat_DP Qtt(Qt_t);
    Vec_DP Qv(0.0, 3);
    Vec_DP v(0.0, 3);
    Vec_DP u(0.0, 3);
    Mat_DP um(0.0, 3, 1);
    Mat_DP umt(0.0, 1, 3);
    Mat_DP uut(0.0, 3, 3);
    Mat_DP RMH(0.0, 3, 3);
    Mat_DP RMHA(R_MHA);
    v[0] = mt.GetElement(0, 3);
    v[1] = mt.GetElement(1, 3);
    v[2] = mt.GetElement(2, 3);
    MultiplyMatrix(Q, Q1, Qt);
    MultiplyMatVec(Q1, v, Qv);
    AddMatrix(Qtt, Qt, Qt_t);
    Qv_t[0] -= Qv[0];
    Qv_t[1] -= Qv[1];
    Qv_t[2] -= Qv[2];

    u[0] = helicalAxis[0];
    u[1] = helicalAxis[1];
    u[2] = helicalAxis[2];
    VecMatrix(u, um);
    TransposeMatrix(um, umt);
    MultiplyMatrix(um, umt, uut);
    SubMatrix(E, uut, RMH);
    AddMatrix(RMHA, RMH, R_MHA);
  }
  if(taken >= 2)
  {
    rc    = Qv_t;
    Qmatr = Qt_t;
    NR::ludcmp(Qmatr, vidx, dp);
    NR::lubksb(Qmatr, vidx, rc);

    Vec_DP e1(0.0, 3);
    Vec_DP e2(0.0, 3);
    Vec_DP e3(0.0, 3);
    e1[0] = 1.0;
    e2[1] = 1.0;
    e3[2] = 1.0;

    Mat_DP RMHAMatr(R_MHA);

    NR::ludcmp(RMHAMatr, vidx, dp);
    NR::lubksb(RMHAMatr, vidx, e1);
    NR::lubksb(RMHAMatr, vidx, e2);
    NR::lubksb(RMHAMatr, vidx, e3);
    for(int i = 0; i < 3; i++)
    {
      RMHAMatr[i][0] = e1[i];
      RMHAMatr[i][1] = e2[i];
      RMHAMatr[i][2] = e3[i];
    }

    Vec_DP E_vec(0.0, 3);
    double E_lam;
    int Ier, K_iter;
    MaxEigen(3, 3, RMHAMatr, 1e-7, E_vec, E_lam, Ier, K_iter);
    Direction  = V3d<double>(E_vec[0], E_vec[1], E_vec[2]);
    Direction /= sqrt(Direction.length2());
    StartPoint = V3d<double>(rc[0], rc[1], rc[2]);
  }

  if(taken == 0)
    resang = 0.0;
  else
    resang = 1.0;
}

mafVME *AutoSelectProximal(mafVME *distal)
{
  if(!distal)
    return NULL;
  mafVME *parent = distal->GetParent();
  if(!parent)
    return NULL;
  if(mafVMELandmarkCloud *lm = mafVMELandmarkCloud::SafeDownCast(parent))
    return lm;
  mafVME *grandparent = parent->GetParent();
  if(!grandparent)
    return parent;
  for(unsigned i = 0; i < grandparent->GetNumberOfChildren(); i++)
  {
    mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(grandparent->GetChild(i));
    if(!lmc)
      continue;
    for(unsigned j = 0; j < lmc->GetNumberOfChildren(); j++)
    {
      if(mafVMEAFRefSys *afs = mafVMEAFRefSys::SafeDownCast(lmc->GetChild(j)))
      {
        if(afs->GetActive())
          return lmc;
      }
    }
  }
  for(unsigned i = 0; i < grandparent->GetNumberOfChildren(); i++)
  {
    if(mafVMELandmarkCloud *lmc = mafVMELandmarkCloud::SafeDownCast(grandparent->GetChild(i)))
      return lmc;
  }
  return parent;
}
