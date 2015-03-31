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
  lhpOpLMProj(bool internalproj = true, const mafString& label = "Landmarks projection");
  ~lhpOpLMProj(); 
  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  mafTypeMacro(lhpOpLMProj, mafOp);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

  void OpStop(int result);

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

protected:
  void SetNodeName(mafVME *pVME, mafString *pName);

  mafVME*                                      m_Source;
  mafString                                    m_SourceName;
  int                                          m_MultiTime;
  bool                                         m_InternalProjection;
};
#endif
