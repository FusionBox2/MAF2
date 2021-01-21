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
  virtual ~lhpOpJoinSurf();
  
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

  wxListBox *m_ListBox;
  std::vector<mafVME*> m_JoinSurf;
  mafVMESurface        *m_OutSurface;
};
#endif
