/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoAFSys.h,v $
  Language:  C++
  Date:      $Date: 2007-10-19 10:11:03 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoAFSys_H__
#define __mmoAFSys_H__

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
// class mmoAFSys
//----------------------------------------------------------------------------
class mmoAFSys: public mafOp
{
public:
  mmoAFSys(wxString label = "AFSys");
 ~mmoAFSys(); 

  virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

protected: 
  bool ReadScript(const mafString& filename, std::vector<mafString>& output);
  void InitPredefined();
  void OpStop(int result);

  int                                                         m_Radio;
  mafVMEAFRefSys                                              *m_RefSys;
  mafString                                                   m_ScriptFName;
  std::vector<std::pair<mafString, std::vector<mafString> > > m_predefinedScripts;
};
#endif
