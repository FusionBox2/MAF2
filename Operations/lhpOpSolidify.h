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
  ~lhpOpSolidify() override;
  void OnEvent(mafEventBase *maf_event) override;
  mafOp* Copy() override;

  mafTypeMacro(lhpOpSolidify, mafOp);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) override;   

  void OpStop(int result) override;

  /** Builds operation's interface. */
  void OpRun() override;

  /** Execute the operation. */
  void OpDo() override;

  /** Makes the undo for the operation. */
  void OpUndo() override;

  static bool RigidBodyAccept(mafNode* node) {if(node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen() && !((mafVMELandmarkCloud*)node)->IsAnimated())return true; return false;}

protected:
  void SetNodeName(mafVME *pVME, mafString *pName);

  mafVMELandmarkCloud*                         m_Source;
  mafString                                    m_SourceName;
};
#endif
