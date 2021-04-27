/*=========================================================================

 Program: MAF2
 Module: mafOpImporterExternalFile
 Authors: Paolo Quadrani     Stefano Perticoni   Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpImporterExternalFile.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEExternalData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterExternalFile);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterExternalFile::mafOpImporterExternalFile(const mafString &label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Input = NULL; 

  m_Vme = NULL;
	m_FileDir = _R("");//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterExternalFile::~mafOpImporterExternalFile( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Vme);
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterExternalFile::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterExternalFile *cp =  new mafOpImporterExternalFile(GetLabel());
  cp->m_File = m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterExternalFile::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = _L("All Files (*.*) |*.*");
  mafString f;
  if (m_File.IsEmpty())
  {
    f = mafGetOpenFile(m_FileDir,wildc);
    m_File = f;
  }
  
  int result = OP_RUN_CANCEL;

  if(!m_File.IsEmpty()) 
  {
    ImportExternalFile();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void mafOpImporterExternalFile::ImportExternalFile()
//----------------------------------------------------------------------------
{
	mafString path, name, ext;
	mafSplitPath(m_File, &path, &name, &ext);
  
	mafString vmeName = name + _R(".") + ext;

  mafNEW(m_Vme);
	m_Vme->SetExtension(ext.GetCStr());
	m_Vme->SetFileName(name.GetCStr());
  m_Vme->SetCurrentPath(path);
	m_Vme->SetName(vmeName);
  m_Vme->ReparentTo(m_Input);
  m_Vme->Update();

  m_Output = m_Vme;
}

//----------------------------------------------------------------------------
void mafOpImporterExternalFile::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_Vme);
  m_Vme->ReparentTo(NULL);
}
