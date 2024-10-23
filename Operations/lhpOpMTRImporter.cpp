/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpMTRImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-19 11:42:56 $
  Version:   $Revision: 1.3 $
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

#include "lhpOpMTRImporter.h"

#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafVMEGroup.h"
#include "mafMtrLMCReader.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpMTRImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 lhpOpMTRImporter:: lhpOpMTRImporter(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  //m_File    = "";
  m_Canundo = true;
  //m_Group   = NULL;

  m_FileDir = mafGetApplicationDirectory();
}
//----------------------------------------------------------------------------
 lhpOpMTRImporter::~lhpOpMTRImporter()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Groups.size(); i++)
    mafDEL(m_Groups[i]);
}  
//----------------------------------------------------------------------------
mafOp * lhpOpMTRImporter::Copy()
//----------------------------------------------------------------------------
{
  lhpOpMTRImporter *cp = new  lhpOpMTRImporter(GetLabel());
  cp->m_Files   = m_Files;
  cp->m_FileDir = m_FileDir;
  return cp; 
}

//----------------------------------------------------------------------------
void  lhpOpMTRImporter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString vrml_wildc  = _R("MTR FARO data (*.mtr)|*.mtr");

  m_Files.clear();
  //if (m_File.IsEmpty())
  {
    mafGetOpenMultiFiles(m_FileDir,vrml_wildc, m_Files);
  }

  int result = OP_RUN_CANCEL;

  if(m_Files.size() != 0) 
  {
    result = OP_RUN_OK;
    ImportData();
  }

  {mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void  lhpOpMTRImporter::ImportData()
//----------------------------------------------------------------------------
{

  for(unsigned i = 0; i < m_Groups.size(); i++)
    mafDEL(m_Groups[i]);
  m_Groups.clear();

  for(unsigned fi = 0; fi < m_Files.size(); fi++)
  {
    if(m_Files[fi].empty())
      continue;
 
    
    mafString vmeName;
    //wxBusyInfo wait("Loading file: ...");  
    mafString path;
    mafString grpName, extension;
    mafVMEGroup *grp;

    mafSplitPath(m_Files[fi],&path,&grpName,&extension);

    mafNEW(grp);
    grp->SetName(grpName);
    m_Groups.push_back(grp);

    while(TRUE)
    {
      mafMTRLMCReader *LMCReader = mafMTRLMCReader::New();
      LMCReader->SetFileName(m_Files[fi].GetCStr());
      mafString name, ext;

      mafSplitPath(m_Files[fi],&path,&name,&ext);

      LMCReader->SetSet(mafMTRLMCReader::SetNotDefined);
      LMCReader->Execute();

      if(LMCReader->GetPointsRead() == 0)
      {
        cppDEL(LMCReader);
        break;
      }

      const std::vector<std::pair<mafVMELandmarkCloud*, int> >& clouds = LMCReader->GetClouds();
      for(int i = 0; i < clouds.size(); i++)
      {
        vmeName = name + _R("_");
        mafString  curNumber = mafToString(i);
        vmeName = vmeName + curNumber;
        mafTagItem tag_Nature;
        tag_Nature.SetName(_R("VME_NATURE"));
        tag_Nature.SetValue(_R("NATURAL"));

        mafVMELandmarkCloud *cloud;
        cloud = clouds[i].first;
        cloud->SetName(vmeName);
        cloud->GetTagArray()->SetTag(tag_Nature);
        cloud->Close();
        cloud->ReparentTo(grp);
      }
      cppDEL(LMCReader);
      break;
    }

    {mafEvent evUnq(this, VME_ADD, grp); InvokeEvent(evUnq);}
    {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
  }
}

//----------------------------------------------------------------------------
void lhpOpMTRImporter::OpDo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Groups.size(); i++)
  {
    if (m_Groups[i])
    {
      m_Groups[i]->ReparentTo(m_Input);
      {mafEvent evUnq(this, VME_ADD, m_Groups[i]); InvokeEvent(evUnq);}
    }
  }
  {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void lhpOpMTRImporter::OpUndo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Groups.size(); i++)
  {
    if (m_Groups[i])
    {
      {mafEvent evUnq(this, VME_REMOVE, m_Groups[i]); InvokeEvent(evUnq);}
    }
  }
  {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
}
