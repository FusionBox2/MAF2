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
           ~lhpOpCutSurface() override;
  
  mafTypeMacro(lhpOpCutSurface, mafOp);

           void OnEvent(mafEventBase *maf_event) override;
  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) override;

  /** Builds operation's interface. */
  void OpRun() override;

  /** Makes the undo for the operation. */
  void OpUndo() override;

  void CreateGui();

  /** Execute the operation. */
  void OpDo() override;

protected:
  void SetNodeName(mafVME *pVME, mafString *pName);
  void OpStop(int result) override;

  mafVMESurfaceParametric     *m_CutSurf;
  mafString                   m_CutSurfName;

  mafVMESurface *m_OutSurface;
};
#endif
