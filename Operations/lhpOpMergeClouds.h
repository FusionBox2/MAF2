/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpMergeClouds.h,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:23:48 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpMergeClouds_H__
#define __lhpOpMergeClouds_H__

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
//lhpOpMergeClouds :
//----------------------------------------------------------------------------
class lhpOpMergeClouds: public mafOp
{
public:
           lhpOpMergeClouds(const wxString &label = "Merge clouds");
  virtual ~lhpOpMergeClouds();
  
  mafTypeMacro(lhpOpMergeClouds, mafOp);

  enum 
  {
    ID_DEFAULT = MINID,
    ID_MERGE_CLOUD,
    ID_ADD_CLOUD,
    ID_REMOVE_CLOUD,
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
  void MergeNodes(mafVME *vme1, mafVME *vme2, const mafMatrix &transf);
  void SetNodeName(mafVME *pVME, mafString *pName);
  void OpStop(int result);

  wxListBox            *m_ListBox;
  std::vector<mafVME*>  m_MergeClouds;
};
#endif
