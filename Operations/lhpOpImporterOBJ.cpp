#include "lhpOpImporterOBJ.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "vtkSmartPointer.h"
#include "mafFilesDirs.h"

#include "vtkOBJReader.h"
#include "vtkPolyData.h"

#include <fstream>

//----------------------------------------------------------------------------
lhpOpImporterOBJ::lhpOpImporterOBJ(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
  m_FileDir = _R("");//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
lhpOpImporterOBJ::~lhpOpImporterOBJ()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedOBJs.size(); i++)
    m_ImportedOBJs[i].reset();
}
//----------------------------------------------------------------------------
bool lhpOpImporterOBJ::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* lhpOpImporterOBJ::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpImporterOBJ *cp = new lhpOpImporterOBJ(GetLabel());
  cp->m_Files = m_Files;
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpImporterOBJ::OpRun()   
//----------------------------------------------------------------------------
{
  if (!m_TestMode && m_Files.size() == 0)
  {
    mafString wildc = _R("Wavefront (*.obj)|*.obj");
    m_Files.clear();
    mafGetOpenMultiFiles(m_FileDir,wildc, m_Files);
  }
	
	int result = OP_RUN_CANCEL;

	if(m_Files.size() != 0) 
	{
		result = OP_RUN_OK;
    
		ImportOBJ();
	}

	{mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void lhpOpImporterOBJ::OpDo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedOBJs.size(); i++)
  {
    if (m_ImportedOBJs[i])
    {
      mafNode::ReparentTo(m_ImportedOBJs[i], GetInput().get());
    }
  }
  {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void lhpOpImporterOBJ::OpUndo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedOBJs.size(); i++)
  {
    if (m_ImportedOBJs[i])
    {
      mafNode::ReparentTo(m_ImportedOBJs[i], nullptr);
    }
  }
  {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void lhpOpImporterOBJ::ImportOBJ()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Loading file: ...");  
  }

  unsigned int i;
  for(i = 0; i < m_ImportedOBJs.size(); i++)
    m_ImportedOBJs[i].reset();
  m_ImportedOBJs.clear();

  for(unsigned kk = 0; kk < m_Files.size(); kk++)
  {
    mafString fn;
    fn = m_Files[kk];
    
    vtkNew<vtkOBJReader> reader;
	  {mafEvent evUnq(this,BIND_TO_PROGRESSBAR); evUnq.SetVtkObj(reader); InvokeEvent(evUnq);}
    reader->SetFileName(fn.GetCStr());
	  reader->Update();

    mafString path, name, ext;
    mafSplitPath(fn.GetCStr(),&path,&name,&ext);

    auto importedOBJ = mafVMESurface::NewSPtr();
    importedOBJ->SetName(name);
	  importedOBJ->SetDataByDetaching(reader->GetOutput(),0);

    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));
    importedOBJ->GetTagArray()->SetTag(tag_Nature);

    m_ImportedOBJs.push_back(importedOBJ);
  }
}


//----------------------------------------------------------------------------
void lhpOpImporterOBJ::SetFileName(const mafString& file_name)
//----------------------------------------------------------------------------
{
  m_Files.resize(1);
  m_Files[0] = file_name;
}
//----------------------------------------------------------------------------
void lhpOpImporterOBJ::GetImportedOBJ(std::vector<std::shared_ptr<mafVMESurface> > &importedOBJ)
//----------------------------------------------------------------------------
{
  importedOBJ.clear();
  importedOBJ.resize(m_ImportedOBJs.size());
  for (unsigned int i=0; i< m_ImportedOBJs.size(); i++)
  {
    importedOBJ[i] = m_ImportedOBJs[i];
  }
}
