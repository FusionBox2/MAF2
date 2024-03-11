/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafJointAnalysis.h,v $
  Language:  C++
  Date:      $Date: 2008-04-14 11:59:34 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafJoinAnalysis_h
#define __mafJoinAnalysis_h

#include "mafVME.h"
#include "mafVMEAFRefSys.h"
#include "mafVMERefSysAbstract.h"
#include "mafVectors.h"

mafVMERefSysAbstract *GetRefSys(mafVME *vme);
mafVMEAFRefSys *GetAFRefSys(mafVME *vme);
//void GetGlobalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat);
void GetGlobalMatrix(mafVME *vme, mafTimeStamp ts, mafMatrix& matrix, bool useRefSys = true);
//void GetLocalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat);
void GetLocalMatrix(mafVME *vme, mafTimeStamp ts, mafMatrix& matrix, mafVME *parent = NULL, bool useRefSys = true);

mafVME *AutoSelectProximal(mafVME *distal);

void SetOVP(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, V4d<double> *vOVPPos, V4d<double> *vOVPRot);
int FindParentID(int id);
void OVP_GES(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, V4d<double> *vOVPPos, V4d<double> *vOVPRot, V4d<double> *vGESPos, V4d<double> *vGESRot, mafVME *parent = NULL);
int MatrixToHelicalAxis(const mafMatrix &matrix, double helical_axis[3],double point[3], double& phi,double& t);

void GetMeanAxis(mafVME *proximal, mafVME *distal, mafTimeStamp tsMinTime, mafTimeStamp tsMaxTime, double minAngle, V3d<double>& Direction, V3d<double>& StartPoint, double& resang);
void GetMomentAxis(mafVME *proximal, mafVME *distal, mafTimeStamp tsTime, double minAngle, V3d<double>& helicalAxis, V3d<double>& point, double& angle, double& translationAmount, mafTimeStamp tsTimeTo = -1);

bool mafTransfInverseTransformUpright(mafMatrix const *mpIn, V4d<double> *vpPos, V4d<double> *vpRot);
bool mafTransfMatrixToEuler(mafMatrix const *mpR, V4d<double> *vpR, int conv);
bool mafTransfComposeMatrixStright(mafMatrix *mpIn, V4d<double> const *vpRot, V4d<double> const *vpPos);
inline float MathRound(float val);
float Fix360Difference(float rAold, float rAnew);
float Fix180Difference(float rAold, float rAnew);

#endif