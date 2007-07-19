/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMTRImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-19 12:37:35 $
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

#include "mmoMTRImporter.h"

#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafVMEGroup.h"
#include "mafMtrLMCReader.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoMTRImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 mmoMTRImporter:: mmoMTRImporter(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_File    = "";
  m_Canundo = true;
  m_Group   = NULL;

  m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
 mmoMTRImporter::~ mmoMTRImporter()
//----------------------------------------------------------------------------
{
  mafDEL(m_Group);
}  
//----------------------------------------------------------------------------
mafOp * mmoMTRImporter::Copy()
//----------------------------------------------------------------------------
{
  mmoMTRImporter *cp = new  mmoMTRImporter(m_Label);
  cp->m_File    = m_File;
  cp->m_FileDir = m_FileDir;
  return cp; 
}
//----------------------------------------------------------------------------
void  mmoMTRImporter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString vrml_wildc  = "MTR FARO data (*.mtr)|*.mtr";
  mafString f;

  if (m_File.IsEmpty())
  {
    f = mafGetOpenFile(m_FileDir.GetCStr(),vrml_wildc.GetCStr()).c_str();   
    m_File = f;
  }

  int result = OP_RUN_CANCEL;

  if(!m_File.IsEmpty()) 
  {
    result = OP_RUN_OK;
    ImportData();
  }

  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void  mmoMTRImporter::SetFileName(const char *file_name)
//----------------------------------------------------------------------------
{
  m_File = file_name;
}


//----------------------------------------------------------------------------
void  mmoMTRImporter::ImportData()
//----------------------------------------------------------------------------
{

  wxString vmeName;
  //wxBusyInfo wait("Loading file: ...");  
  wxString path;
  wxString grpName, extension;

  wxSplitPath(m_File.GetCStr(),&path,&grpName,&extension);

  mafNEW(m_Group);
  m_Group->SetName(grpName.c_str());

  while(TRUE)
  {
    mafMTRLMCReader *LMCReader = mafMTRLMCReader::New();
    LMCReader->SetFileName(m_File);
    wxString name, ext;

    wxSplitPath(m_File.GetCStr(),&path,&name,&ext);

    LMCReader->SetSet(mafMTRLMCReader::SetNotDefined);
    LMCReader->Execute();

    if(LMCReader->GetPointsRead() == 0)
    {
      cppDEL(LMCReader);
      break;
    }

    const std::vector<mafVMELandmarkCloud*>& clouds = LMCReader->GetClouds();
    for(int i = 0; i < clouds.size(); i++)
    {
      vmeName = name + "_";
      wxString  curNumber("");
      curNumber.Printf("%d", i);
      vmeName = vmeName + curNumber;
      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");

      mafVMELandmarkCloud *cloud;
      cloud = clouds[i];
      cloud->SetName(vmeName);
      cloud->GetTagArray()->SetTag(tag_Nature);
      cloud->Close();
      cloud->ReparentTo(m_Group);
    }
    cppDEL(LMCReader);
    break;
  }

  mafEventMacro(mafEvent(this, VME_ADD, m_Group));
  mafDEL(m_Group);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
