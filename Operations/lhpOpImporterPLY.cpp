/*=========================================================================

 Program: MAF2
 Module: lhpOpImporterPLY
 Authors: Paolo Quadrani
 
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

#include "lhpOpImporterPLY.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafSmartPointer.h"
#include "vtkMAFSmartPointer.h"
#include "mafFilesDirs.h"

#include "vtkPLYReader.h"
#include "vtkPolyData.h"

#include <fstream>

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpImporterPLY);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpImporterPLY::lhpOpImporterPLY(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
  m_FileDir = _R("");//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
lhpOpImporterPLY::~lhpOpImporterPLY()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
    mafDEL(m_ImportedPLYs[i]);
}
//----------------------------------------------------------------------------
bool lhpOpImporterPLY::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* lhpOpImporterPLY::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpImporterPLY *cp = new lhpOpImporterPLY(GetLabel());
  cp->m_Files = m_Files;
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpImporterPLY::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode && m_Files.size() == 0)
  {
    mafString wildc = _R("Wavefront (*.ply)|*.ply");
    m_Files.clear();
    mafGetOpenMultiFiles(m_FileDir,wildc, m_Files);
  }
	
	int result = OP_RUN_CANCEL;

	if(m_Files.size() != 0) 
	{
		result = OP_RUN_OK;
    
		ImportPLY();
	}

	mafEventMacro(mafEvent(this,result));
}

//----------------------------------------------------------------------------
void lhpOpImporterPLY::OpDo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
  {
    if (m_ImportedPLYs[i])
    {
      m_ImportedPLYs[i]->ReparentTo(m_Input);
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void lhpOpImporterPLY::OpUndo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
  {
    if (m_ImportedPLYs[i])
    {
      m_ImportedPLYs[i]->ReparentTo(NULL);
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void lhpOpImporterPLY::ImportPLY()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Loading file: ...");  
  }

  unsigned int i;
  for(i = 0; i < m_ImportedPLYs.size(); i++)
    mafDEL(m_ImportedPLYs[i]);
  m_ImportedPLYs.clear();

  for(unsigned kk = 0; kk < m_Files.size(); kk++)
  {
    mafString fn;
    fn = m_Files[kk];
    
    vtkMAFSmartPointer<vtkPLYReader> reader;
	  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,reader));
    reader->SetFileName(fn.GetCStr());
	  reader->Update();

    mafString path, name, ext;
    mafSplitPath(fn.GetCStr(),&path,&name,&ext);

    mafVMESurface *importedPLY;
    mafNEW(importedPLY);
    importedPLY->SetName(name);
	  importedPLY->SetDataByDetaching(reader->GetOutput(),0);

    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));
    importedPLY->GetTagArray()->SetTag(tag_Nature);

    m_ImportedPLYs.push_back(importedPLY);
  }
}


//----------------------------------------------------------------------------
void lhpOpImporterPLY::SetFileName(const mafString& file_name)
//----------------------------------------------------------------------------
{
  m_Files.resize(1);
  m_Files[0] = file_name;
}
//----------------------------------------------------------------------------
void lhpOpImporterPLY::GetImportedPLY(std::vector<mafVMESurface*> &importedPLY)
//----------------------------------------------------------------------------
{
  importedPLY.clear();
  importedPLY.resize(m_ImportedPLYs.size());
  for (unsigned int i=0; i< m_ImportedPLYs.size(); i++)
  {
    importedPLY[i] = m_ImportedPLYs[i];
  }
}
