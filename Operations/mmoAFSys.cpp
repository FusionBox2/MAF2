/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoAFSys.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-19 10:11:03 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmoAFSys.h"
#include "wx/busyinfo.h"
#include "wx/textfile.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"
#include "mafDictionary.h"
#include "mafPlotMath.h"

#include "mafSmartPointer.h"

#include "mafVMEAFRefSys.h"
#include "mafVMELandmarkCloud.h"

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Forward Refs
//----------------------------------------------------------------------------

#define ADD_PREDEF(name) m_predefinedScripts.push_back(std::make_pair(#name, std::vector<mafString>(&_##name[0], &_##name[0] + sizeof(_##name)/sizeof(_##name[0]))))
void mmoAFSys::InitPredefined()
{
  mafString _IPE[] = {"ASSV PN0 RIAS", "ASSV PN1 LIAS", "ASSV PN2 RIPS", "ASSV PN3 LIPS", "LNCMB MIDDLEA 0.5 PN0 0.5 PN1", "LNCMB MIDDLEP 0.5 PN2 0.5 PN3", "ASSV P1 PN0", "ASSV P2 PN1", "ASSV P3 MIDDLEP", "ASSV P4 PN1", "ASSV P5 PN0", "LNCMB A 1 P2 -1 P1", "NRML A", "LNCMB B 1 P3 -1 P2", "NRML B", "CROSS X1 A B", "NRML X1", "LNCMB R 1 P5 -1 P4", "NRML R", "CROSS Y1 X1 R", "NRML Y1", "CROSS Z1 X1 Y1", "NRML Z1", "ASSV X Y1", "ASSV Y X1", "LNCMB Z -1 Z1 0 Y1", "ASSV P MIDDLEA"};
  mafString _LFT[] = {"ASSV PN0 LFCC", "ASSV PN1 LFM5", "ASSV PN2 LFM2", "ASSV PN3 LFM1", "ASSV P1 PN3", "ASSV P2 PN1", "ASSV P3 PN0", "ASSV P4 PN2", "ASSV P5 PN0", "LNCMB A 1 P2 -1 P1", "NRML A", "LNCMB B 1 P3 -1 P2", "NRML B", "CROSS X1 A B", "NRML X1", "LNCMB R 1 P5 -1 P4", "NRML R", "CROSS Y1 X1 R", "NRML Y1", "CROSS Z1 X1 Y1", "NRML Z1", "ASSV X Z1", "ASSV Y X1", "ASSV Z Y1", "ASSV P PN0"};
  mafString _LSH[] = {"ASSV PN0 LFAX", "ASSV PN1 LTTC", "ASSV PN2 LTAM", "ASSV PN3 LFAL", "LNCMB MIDDLE 0.5 PN2 0.5 PN3", "ASSV P1 PN2", "ASSV P2 PN3", "ASSV P3 PN0", "ASSV P4 MIDDLE", "ASSV P5 PN1", "LNCMB A 1 P2 -1 P1", "NRML A", "LNCMB B 1 P3 -1 P2", "NRML B", "CROSS X1 A B", "NRML X1", "LNCMB R 1 P5 -1 P4", "NRML R", "CROSS Y1 X1 R", "NRML Y1", "CROSS Z1 X1 Y1", "NRML Z1", "ASSV X X1", "LNCMB Y -1 Z1 0 Y1", "ASSV Z Y1", "ASSV P MIDDLE"};
  mafString _LTH[] = {"ASSV PN0 LFME", "ASSV PN1 LFLE", "ASSV PN2 LFCH", "LNCMB MIDDLE 0.5 PN0 0.5 PN1", "ASSV P1 PN0", "ASSV P2 PN1", "ASSV P3 PN2", "ASSV P4 MIDDLE", "ASSV P5 PN2", "LNCMB A 1 P2 -1 P1", "NRML A", "LNCMB B 1 P3 -1 P2", "NRML B", "CROSS X1 A B", "NRML X1", "LNCMB R 1 P5 -1 P4", "NRML R", "CROSS Y1 X1 R", "NRML Y1", "CROSS Z1 X1 Y1", "NRML Z1", "ASSV X X1", "LNCMB Y -1 Z1 0 Y1", "ASSV Z Y1", "ASSV P MIDDLE"};
  mafString _RFT[] = {"ASSV PN0 RFCC", "ASSV PN1 RFM1", "ASSV PN2 RFM2", "ASSV PN3 RFM5", "ASSV P1 PN3", "ASSV P2 PN1", "ASSV P3 PN0", "ASSV P4 PN2", "ASSV P5 PN0", "LNCMB A 1 P2 -1 P1", "NRML A", "LNCMB B 1 P3 -1 P2", "NRML B", "CROSS X1 A B", "NRML X1", "LNCMB R 1 P5 -1 P4", "NRML R", "CROSS Y1 X1 R", "NRML Y1", "CROSS Z1 X1 Y1", "NRML Z1", "ASSV X Z1", "ASSV Y X1", "ASSV Z Y1", "ASSV P PN0"};
  mafString _RSH[] = {"ASSV PN0 RFAX", "ASSV PN1 RTTC", "ASSV PN2 RFAL", "ASSV PN3 RTAM", "LNCMB MIDDLE 0.5 PN2 0.5 PN3", "ASSV P1 PN2", "ASSV P2 PN3", "ASSV P3 PN0", "ASSV P4 MIDDLE", "ASSV P5 PN1", "LNCMB A 1 P2 -1 P1", "NRML A", "LNCMB B 1 P3 -1 P2", "NRML B", "CROSS X1 A B", "NRML X1", "LNCMB R 1 P5 -1 P4", "NRML R", "CROSS Y1 X1 R", "NRML Y1", "CROSS Z1 X1 Y1", "NRML Z1", "ASSV X X1", "LNCMB Y -1 Z1 0 Y1", "ASSV Z Y1", "ASSV P MIDDLE"};
  mafString _RTH[] = {"ASSV PN0 RFLE", "ASSV PN1 RFME", "ASSV PN2 RFCH", "LNCMB MIDDLE 0.5 PN0 0.5 PN1", "ASSV P1 PN0", "ASSV P2 PN1", "ASSV P3 PN2", "ASSV P4 MIDDLE", "ASSV P5 PN2", "LNCMB A 1 P2 -1 P1", "NRML A", "LNCMB B 1 P3 -1 P2", "NRML B", "CROSS X1 A B", "NRML X1", "LNCMB R 1 P5 -1 P4", "NRML R", "CROSS Y1 X1 R", "NRML Y1", "CROSS Z1 X1 Y1", "NRML Z1", "ASSV X X1", "LNCMB Y -1 Z1 0 Y1", "ASSV Z Y1", "ASSV P MIDDLE"};
  ADD_PREDEF(IPE);
  ADD_PREDEF(RTH);
  ADD_PREDEF(LTH);
  ADD_PREDEF(RSH);
  ADD_PREDEF(LSH);
  ADD_PREDEF(RFT);
  ADD_PREDEF(LFT);
}

//----------------------------------------------------------------------------
mmoAFSys::mmoAFSys(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType   = OPTYPE_OP;
  m_Canundo  = true;
  m_RefSys   = NULL;
  InitPredefined();
  m_Radio    = m_predefinedScripts.size();
}

//----------------------------------------------------------------------------
mmoAFSys::~mmoAFSys()
//----------------------------------------------------------------------------
{
  mafDEL(m_RefSys);
}

//----------------------------------------------------------------------------
mafOp* mmoAFSys::Copy()   
//----------------------------------------------------------------------------
{
  return new mmoAFSys(m_Label);
}

//----------------------------------------------------------------------------
bool mmoAFSys::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;
  if(mafVMELandmarkCloud::SafeDownCast(vme) == NULL)
    return false;

  /*for(int i = 0; i < vme->GetNumberOfChildren(); i++)
  {
    if(vme->GetChild(i)->IsA("mafVMEAFRefSys"))
      return false;
  }*/
  
  return true;
}

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_RADIO_SCRIPT,
  ID_LOAD_DICTIONARY,
  ID_LOAD_SCRIPT,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

//----------------------------------------------------------------------------
void mmoAFSys::OpRun()   
//----------------------------------------------------------------------------
{
  mafString strBase(m_Input->GetName());
  mafNEW(m_RefSys);
  strBase += "_AF_Frame";
  mafString str = strBase;
  unsigned ind = 0;
  unsigned i;
  do
  {
    for(i = 0; i < m_Input->GetNumberOfChildren(); i++)
    {
      mafNode *node = m_Input->GetChild(i);
      if(strcmp(node->GetName(), str.GetCStr()) == 0)
      {
        str.Printf("%s_%u", strBase.GetCStr(), ind);
        ind++;
        break;
      }
    }
  }
  while(i < m_Input->GetNumberOfChildren() && ind != UINT_MAX);
  m_RefSys->SetName(str.GetCStr());
  for(unsigned nm = 0; nm < m_predefinedScripts.size(); nm++)
  {
    if(stricmp(m_predefinedScripts[nm].first.GetCStr(), m_Input->GetName()) == 0)
    {
      m_Radio = nm;
      m_RefSys->SetScriptText(m_predefinedScripts[m_Radio].second);
      break;
    }
  }
  CreateGui();
}

//----------------------------------------------------------------------------
void mmoAFSys::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->SetListener(this);

  std::vector<wxString> list;
  for(unsigned i = 0; i < m_predefinedScripts.size(); i++)
  {
    list.push_back(m_predefinedScripts[i].first.GetCStr());
  }
  list.push_back("Custom");
  m_Gui->Radio(ID_RADIO_SCRIPT, "",&m_Radio, list.size(), &list[0]);
  m_Gui->FileOpen(ID_LOAD_SCRIPT, "Script", &m_ScriptFName);
  m_Gui->Label("");

  m_Gui->Enable(ID_LOAD_SCRIPT, m_Radio == m_predefinedScripts.size());
  m_Gui->OkCancel();
  ShowGui();
}

//----------------------------------------------------------------------------
void mmoAFSys::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    if(m_RefSys->GetParent())
    {
      mafEventMacro(mafEvent(this, VME_REMOVE, m_RefSys));
    }
    mafEventMacro(mafEvent(this,result));
  }
  else if (result == OP_RUN_OK)
  {
    if(m_Radio == m_predefinedScripts.size() && m_ScriptFName == "")
    {
      wxMessageBox("Method is not specified","Alert", wxOK , NULL);
      return;
    }
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
}

bool mmoAFSys::ReadScript(const mafString& filename, std::vector<mafString>& output)
{
  FILE *fp = fopen(filename, "rt");
  if(fp == NULL)
  {
    return false;
  }

  int const maxStrLen = 1000;
  char      sLine[maxStrLen];
  char      *pRet;

  while(true)
  {
    pRet = fgets(sLine, maxStrLen, fp);
    if(pRet == NULL)
      break;
    output.push_back(mafString(pRet));
  }
  fclose(fp);
  return true;
}


//----------------------------------------------------------------------------
void mmoAFSys::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
    case wxOK:          
      OpStop(OP_RUN_OK);
      break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
      break;
    case ID_LOAD_SCRIPT:
      {
        std::vector<mafString> tmp;
        if(m_ScriptFName != "" && ReadScript(m_ScriptFName, tmp))
          m_RefSys->SetScriptText(tmp);
      }
      break;
    case ID_RADIO_SCRIPT:
      {
        m_Gui->Enable(ID_LOAD_SCRIPT, m_Radio == m_predefinedScripts.size());
        if(m_Radio != m_predefinedScripts.size())
        {
          m_RefSys->SetScriptText(m_predefinedScripts[m_Radio].second);
        }
        else
        {
          std::vector<mafString> tmp;
          if(m_ScriptFName != "" && ReadScript(m_ScriptFName, tmp))
            m_RefSys->SetScriptText(tmp);
        }
        m_Gui->Update();
      }
      break;
    default:
      mafEventMacro(*maf_event); 
      break;
  }
}

//----------------------------------------------------------------------------
void mmoAFSys::OpDo()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Please wait, working...");

  assert(m_RefSys);
  m_RefSys->ReparentTo(m_Input);
  m_RefSys->SetScaleFactor(100.0);
  mafEventMacro(mafEvent(this, VME_ADD, m_RefSys));
  m_RefSys->SetActive(1);
}
//----------------------------------------------------------------------------
void mmoAFSys::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_RefSys);
  mafEventMacro(mafEvent(this, VME_REMOVE, m_RefSys));
}

