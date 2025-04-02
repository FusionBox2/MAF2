#include "mafOpImporterExternalFile.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEExternalData.h"

//----------------------------------------------------------------------------
mafOpImporterExternalFile::mafOpImporterExternalFile(const mafString &label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;

	m_FileDir = _R("");//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterExternalFile::~mafOpImporterExternalFile( ) 
//----------------------------------------------------------------------------
{
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
  if (m_File.empty())
  {
    f = mafGetOpenFile(m_FileDir,wildc);
    m_File = f;
  }
  
  int result = OP_RUN_CANCEL;

  if(!m_File.empty()) 
  {
    ImportExternalFile();
    result = OP_RUN_OK;
  }
  {mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}

//----------------------------------------------------------------------------
void mafOpImporterExternalFile::ImportExternalFile()
//----------------------------------------------------------------------------
{
	mafString path, name, ext;
	mafSplitPath(m_File, &path, &name, &ext);
  
	mafString vmeName = name + _R(".") + ext;

  m_Vme = mafVMEExternalData::NewSPtr();
	m_Vme->SetExtension(ext.GetCStr());
	m_Vme->SetFileName(name.GetCStr());
  m_Vme->SetCurrentPath(path);
	m_Vme->SetName(vmeName);
  mafNode::ReparentTo(m_Vme, GetInput().get());
  m_Vme->Update();

  SetOutput(m_Vme);
}
