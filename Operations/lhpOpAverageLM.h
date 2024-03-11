/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpAverageLM.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpAverageLM_H__
#define __lhpOpAverageLM_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEAFRefSys;
class mafGui;
class mafEvent;
class mafIntGraphHyer;
class mafVMELandmarkCloud;
class vtkPoints;

//----------------------------------------------------------------------------
// lhpOpRefSys :
//----------------------------------------------------------------------------
/** */
class lhpOpAverageLM: public mafOp
{
public:
  mafTypeMacro(lhpOpAverageLM, mafOp)
  lhpOpAverageLM(const mafString& label = _R(""));
 ~lhpOpAverageLM(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

protected: 

  void OpStop(int result);

  /// limb cloud from motion: animated, we will insert stick tip here
  mafVMELandmarkCloud  *m_LimbCloud;

private:
  /// index of new landmark for undo operation 
  wxInt32                       m_NewIndex;
};
#endif
