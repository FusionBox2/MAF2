/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpLMProj.h,v $
  Language:  C++
  Date:      $Date: 2007-12-28 12:55:08 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpLMProj_H__
#define __lhpOpLMProj_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;

//----------------------------------------------------------------------------
// lhpOpLMProj :
//----------------------------------------------------------------------------
/** */
class lhpOpLMProj: public mafOp
{
public:
  lhpOpLMProj(bool internalproj = true, const mafString& label = _R("Landmarks projection"));
  ~lhpOpLMProj() override;
  void OnEvent(mafEventBase *maf_event) override;
  mafOp* Copy() override;

  mafTypeMacro(lhpOpLMProj, mafOp);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) override;   

  void OpStop(int result) override;

  /** Builds operation's interface. */
  void OpRun() override;

  /** Execute the operation. */
  void OpDo() override;

  /** Makes the undo for the operation. */
  void OpUndo() override;

protected:
  void SetNodeName(mafVME *pVME, mafString *pName);

  mafVME*                                      m_Source;
  mafString                                    m_SourceName;
  int                                          m_MultiTime;
  bool                                         m_InternalProjection;
};
#endif
