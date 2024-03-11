/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpCutSurface.h,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:23:48 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpCutSurface_H__
#define __lhpOpCutSurface_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVME;
class mafVMESurfaceParametric;
class vtkMatrix4x4;
class mafVMESurface;

//----------------------------------------------------------------------------
//lhpOpCutSurface :
//----------------------------------------------------------------------------
class lhpOpCutSurface: public mafOp
{
public:
           lhpOpCutSurface(const mafString& label = _R("CutSurface"));
  virtual ~lhpOpCutSurface();
  
  mafTypeMacro(lhpOpCutSurface, mafOp);

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);

  /** Builds operation's interface. */
  void OpRun();

  /** Makes the undo for the operation. */
  void OpUndo();

  void CreateGui();

  /** Execute the operation. */
  void OpDo();

protected:
  void SetNodeName(mafVME *pVME, mafString *pName);
  void OpStop(int result);

  mafVMESurfaceParametric     *m_CutSurf;
  mafString                   m_CutSurfName;

  mafVMESurface *m_OutSurface;
};
#endif
