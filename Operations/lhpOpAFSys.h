/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpAFSys.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:18 $
  Version:   $Revision: 1.3 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __lhpOpAFSys_H__
#define __lhpOpAFSys_H__

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEAFRefSys;
class mafGui;
class mafEvent;


#include <strstream>
#include <vector>
#include <map>
#include <string>

//----------------------------------------------------------------------------
// class lhpOpAFSys
//----------------------------------------------------------------------------
class lhpOpAFSys: public mafOp
{
public:
  lhpOpAFSys(wxString label = "AFSys");
 ~lhpOpAFSys(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

protected: 
  struct PredefinedScripts
  {
    PredefinedScripts(const mafString& name, const std::vector<mafString>& script, int boneID):m_Name(name),m_Script(script),m_BoneID(boneID){}
    mafString              m_Name;
    std::vector<mafString> m_Script;
    int                    m_BoneID;
  };
  bool ReadScript(const mafString& filename, std::vector<mafString>& output);
  void InitPredefined();
  void OpStop(int result);

  int                            m_Radio;
  mafVMEAFRefSys                 *m_RefSys;
  mafString                       m_ScriptFName;
  std::vector<PredefinedScripts>  m_predefinedScripts;
};
#endif
