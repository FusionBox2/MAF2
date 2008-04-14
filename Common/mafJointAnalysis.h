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

#include "mafPlotMath.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEAFRefSys.h"

mafVMEAFRefSys *GetRefSys(mafVME *vme);
void GetGlobalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat);
void GetLocalMatrix(mafVME *vme, mafTimeStamp ts, DiMatrix *pMat);
void SetOVP(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, DiV4d *vOVPPos, DiV4d *vOVPRot);
int FindParentID(int id);
void OVP_GES(mafVME *vme, mafTimeStamp ts, mafTimeStamp tsRef, DiV4d *vOVPPos, DiV4d *vOVPRot, DiV4d *vGESPos, DiV4d *vGESRot);



#endif