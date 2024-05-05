/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpINPImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:19:11 $
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

#include "lhpOpINPImporter.h"

#include "mafDecl.h"
#include "mafGUIDialog.h"

#include "mmaMaterial.h"

#include "mafINPReader.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEGroup.h"
#include "mafTagArray.h"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpINPImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 lhpOpINPImporter::lhpOpINPImporter(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	//m_File		= "";
	m_Canundo	= true;
	//m_Surface = NULL;

 	m_FileDir = mafGetApplicationDirectory();
}
//----------------------------------------------------------------------------
 lhpOpINPImporter::~lhpOpINPImporter()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Surfaces.size(); i++)
    mafDEL(m_Surfaces[i]);
}
//----------------------------------------------------------------------------
mafOp * lhpOpINPImporter::Copy()
//----------------------------------------------------------------------------
{
  lhpOpINPImporter *cp = new  lhpOpINPImporter(GetLabel());
  cp->m_Files   = m_Files;
  cp->m_FileDir = m_FileDir;
  return cp; 
}
//----------------------------------------------------------------------------
void  lhpOpINPImporter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString vrml_wildc = _R("AMIRA geometry (*.inp)|*.inp|AMIRA geometry in AF system (*.inp_AFs)|*.inp_AFs");

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

  mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void  lhpOpINPImporter::ImportData()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Surfaces.size(); i++)
    mafDEL(m_Surfaces[i]);
  m_Surfaces.clear();
  for(unsigned i = 0; i < m_Files.size(); i++)
  {
    if(m_Files[i].empty())
      continue;
    mafINPReader  *reader = mafINPReader::New();
    mafVMESurface *surface;

    reader->SetFileName(m_Files[i].GetCStr());
    mafString path, name, ext;

    mafSplitPath(m_Files[i],&path,&name,&ext);

    reader->Update();

    mafTimeStamp t;
    t = ((mafVME *)m_Input)->GetTimeStamp();
    mafNEW(surface);
    surface->SetName(name);
    vtkPolyData *data = reader->GetOutput();
    surface->SetData(data,t);

    m_Surfaces.push_back(surface);

    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));

    surface->GetTagArray()->SetTag(tag_Nature);

    vtkDEL(reader);
  }
}
//----------------------------------------------------------------------------
void lhpOpINPImporter::OpDo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Surfaces.size(); i++)
  {
    if (m_Surfaces[i])
    {
      m_Surfaces[i]->ReparentTo(m_Input);
      mafEventMacro(mafEvent(this, VME_ADD, m_Surfaces[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void lhpOpINPImporter::OpUndo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Surfaces.size(); i++)
  {
    if (m_Surfaces[i])
    {
      mafEventMacro(mafEvent(this, VME_REMOVE, m_Surfaces[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
