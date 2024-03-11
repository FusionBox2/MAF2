/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpSolidify.h,v $
  Language:  C++
  Date:      $Date: 2007-12-28 12:55:08 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpSolidify_H__
#define __lhpOpSolidify_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVMELandmarkCloud.h"
#include "mafVectors.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpOpSolidify :
//----------------------------------------------------------------------------
/** */
class lhpOpSolidify: public mafOp
{
public:
  lhpOpSolidify(const mafString& label = _R("Solidify landmarks cloud"));
  ~lhpOpSolidify(); 
  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  mafTypeMacro(lhpOpSolidify, mafOp);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

  void OpStop(int result);

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  static bool RigidBodyAccept(mafNode* node) {if(node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen() && !((mafVMELandmarkCloud*)node)->IsAnimated())return true; return false;}

protected:
  void SetNodeName(mafVME *pVME, mafString *pName);

  mafVMELandmarkCloud*                         m_Source;
  mafString                                    m_SourceName;
};
#endif
