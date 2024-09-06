/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpJoinSurf.h,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:23:48 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpJoinSurf_H__
#define __lhpOpJoinSurf_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVME;
class vtkMatrix4x4;
class mafVMESurface;

//----------------------------------------------------------------------------
//lhpOpJoinSurf :
//----------------------------------------------------------------------------
class lhpOpJoinSurf: public mafOp
{
public:
           lhpOpJoinSurf(const mafString& label = _R("CutSurface"));
           ~lhpOpJoinSurf() override;
  
  mafTypeMacro(lhpOpJoinSurf, mafOp);

  enum 
  {
    ID_DEFAULT = MINID,
    ID_JOINSURF,
    ID_ADD_SURF,
    ID_REMOVE_SURF,
    ID_LAST,
    ID_FORCED_DWORD = 0x7fffffff
  };


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

  wxListBox *m_ListBox;
  std::vector<mafVME*> m_JoinSurf;
  mafVMESurface        *m_OutSurface;
};
#endif
