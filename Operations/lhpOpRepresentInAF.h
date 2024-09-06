/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRepresentInAF.h,v $
  Language:  C++
  Date:      $Date: 2007-07-09 16:58:31 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani      - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpRepresentInAF_H__
#define __lhpOpRepresentInAF_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMERefSysAbstract.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpOpRepresentInAF :
//----------------------------------------------------------------------------
/** */
class lhpOpRepresentInAF: public mafOp
{
public:
  lhpOpRepresentInAF(const mafString& label = _L("Represent in RefSys"));
 ~lhpOpRepresentInAF() override;
  void OnEvent(mafEventBase *maf_event) override;
  
  mafTypeMacro(lhpOpRepresentInAF, mafOp);
  
  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) override;   

  /** Builds operation's interface. */
  void OpRun() override;

  /** Execute the operation. */
  void OpDo() override;

  /** Makes the undo for the operation. */
  void OpUndo() override;

  static bool RefSysAccept(mafNode* node) {mafVMERefSysAbstract *afsys = mafVMERefSysAbstract::SafeDownCast(node); return (afsys != NULL);}

protected:
  int                     m_MultiTime;
  int                     m_ApplyChildren;
  mafVMERefSysAbstract    *m_RefSys;
  mafString               m_RefsysName;
 };
#endif
