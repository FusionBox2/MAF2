/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoINPImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
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

#include "mmoINPImporter.h"

#include "mafDecl.h"
#include "mmgDialog.h"

#include "mmaMaterial.h"

#include "mafINPReader.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEGroup.h"
#include "mafTagArray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmoINPImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 mmoINPImporter:: mmoINPImporter(const wxString &label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_File		= "";
	m_Canundo	= true;
	m_Surface = NULL;

 	m_FileDir = mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
 mmoINPImporter::~ mmoINPImporter()
//----------------------------------------------------------------------------
{
  mafDEL(m_Surface);
}	
//----------------------------------------------------------------------------
mafOp * mmoINPImporter::Copy()
//----------------------------------------------------------------------------
{
  mmoINPImporter *cp = new  mmoINPImporter(m_Label);
	cp->m_File		= m_File;
  cp->m_FileDir = m_FileDir;
  return cp; 
}
//----------------------------------------------------------------------------
void  mmoINPImporter::OpRun()   
//----------------------------------------------------------------------------
{
	mafString vrml_wildc	= "AMIRA geometry (*.inp)|*.inp|AMIRA geometry in AF system (*.inp_AFs)|*.inp_AFs";
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
void  mmoINPImporter::SetFileName(const char *file_name)
//----------------------------------------------------------------------------
{
  m_File = file_name;
}
//----------------------------------------------------------------------------
void  mmoINPImporter::ImportData()
//----------------------------------------------------------------------------
{
  mafINPReader *reader = mafINPReader::New();

  reader->SetFileName(m_File);
  wxString path, name, ext;

  wxSplitPath(m_File.GetCStr(),&path,&name,&ext);

  reader->Update();

  mafTimeStamp t;
  t = ((mafVME *)m_Input)->GetTimeStamp();
  mafNEW(m_Surface);
  m_Surface->SetName(name.c_str());
  vtkPolyData *data = reader->GetOutput();
  m_Surface->SetData(data,t);

  m_Output = m_Surface;

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  m_Output->GetTagArray()->SetTag(tag_Nature);

  vtkDEL(reader);
}
