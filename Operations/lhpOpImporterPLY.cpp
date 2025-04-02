#include "lhpOpImporterPLY.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafTagItem.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "vtkSmartPointer.h"
#include "mafFilesDirs.h"

#include "vtkPLYReader.h"
#include "vtkPolyData.h"

#include <fstream>

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
    m_ImportedPLYs[i].reset();
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

	{mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void lhpOpImporterPLY::OpDo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
  {
    if (m_ImportedPLYs[i])
    {
      mafNode::ReparentTo(m_ImportedPLYs[i], GetInput().get());
    }
  }
  {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void lhpOpImporterPLY::OpUndo()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_ImportedPLYs.size(); i++)
  {
    if (m_ImportedPLYs[i])
    {
      mafNode::ReparentTo(m_ImportedPLYs[i], nullptr);
    }
  }
  {mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
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
    m_ImportedPLYs[i].reset();
  m_ImportedPLYs.clear();

  for(unsigned kk = 0; kk < m_Files.size(); kk++)
  {
    mafString fn;
    fn = m_Files[kk];
    
    vtkNew<vtkPLYReader> reader;
	  {mafEvent evUnq(this,BIND_TO_PROGRESSBAR); evUnq.SetVtkObj(reader); InvokeEvent(evUnq);}
    reader->SetFileName(fn.GetCStr());
	  reader->Update();

    mafString path, name, ext;
    mafSplitPath(fn.GetCStr(),&path,&name,&ext);

    auto importedPLY = mafVMESurface::NewSPtr();
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
void lhpOpImporterPLY::GetImportedPLY(std::vector<std::shared_ptr<mafVMESurface> > &importedPLY)
//----------------------------------------------------------------------------
{
  importedPLY.clear();
  importedPLY.resize(m_ImportedPLYs.size());
  for (unsigned int i=0; i< m_ImportedPLYs.size(); i++)
  {
    importedPLY[i] = m_ImportedPLYs[i];
  }
}
