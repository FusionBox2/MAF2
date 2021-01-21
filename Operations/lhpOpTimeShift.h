/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpTimeShift.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpTimeShift_H__
#define __lhpOpTimeShift_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGui;
class mafEvent;
class mafVMELandmarkCloud;
class vtkPoints;

//----------------------------------------------------------------------------
// mmoRefSys :
//----------------------------------------------------------------------------
/** */
class lhpOpTimeShift: public mafOp
{
public:
  mafTypeMacro(lhpOpTimeShift, mafOp)
  lhpOpTimeShift(const mafString& label = _R("TimeReduce"));
 ~lhpOpTimeShift(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

protected: 
  //void OpStop(int result);

private:
  double m_Shift;
};
#endif
