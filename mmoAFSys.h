/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoAFSys.h,v $
  Language:  C++
  Date:      $Date: 2007-07-10 19:13:46 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mmoAFSys_H__
#define __mmoAFSys_H__

#ifdef __GNUG__
    #pragma interface "mmoAFSys.cpp"
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "mafVME.h"
#include "mafPlotMath.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEAFRefSys;
class mafOp;
class mafGui;
class mafEvent;

class mmoAFSys: public mafOp
{
public:
  mmoAFSys(wxString label);
 ~mmoAFSys(); 

   virtual void OnEvent(mafEventBase *maf_event);
  mafOp* Copy();

  bool Accept(mafNode* vme);   
  void OpRun();
  void OpDo();
  void OpUndo();
  void CreateGui();

protected: 
  void OpStop(int result);

  std::vector<std::pair<wxString, wxString> >  m_dictionary;
  mafVMEAFRefSys  *m_RefSys;
  mafString       m_DictionaryFName;
};
#endif
