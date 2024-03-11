/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterAnsysInputFile.h,v $
  Language:  C++
  Date:      $Date: 2008-11-14 15:17:05 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni   
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpImporterAnsysInputFile_H__
#define __lhpOpImporterAnsysInputFile_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;
class mafEvent;

//----------------------------------------------------------------------------
// lhpOpImporterAnsysInputFile :
//----------------------------------------------------------------------------
/** */
class lhpOpImporterAnsysInputFile : public mafOp
{
public:
	lhpOpImporterAnsysInputFile(const mafString& label = _R("MeshImporter"));
	~lhpOpImporterAnsysInputFile(); 
	
  mafTypeMacro(lhpOpImporterAnsysInputFile, mafOp);

  virtual void OnEvent(mafEventBase *maf_event);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Set the filename for the mesh to import */
  void SetFileName(const char *file_name);

  /** Set/Get nodes file name*/
  void SetNodesFileName(const char *name)   {this->m_NodesFileName = _R(name);};
  const char *GetNodesFileName() {return this->m_NodesFileName.GetCStr();};

  /** Set/Get elements file name*/
  void SetElementsFileName(const char *name)   {this->m_ElementsFileName = _R(name);};
  const char *GetElementsFileName() {return this->m_ElementsFileName.GetCStr();};

  /** Set/Get materials file name*/
  void SetMaterialsFileName(const char *name) {this->m_MaterialsFileName = _R(name);};
  const char *GetMaterialsFileName() {return this->m_MaterialsFileName.GetCStr();};

  /** Builds operation's interface. */
	void OpRun();

	/** Import the mesh*/
	int Read();

  /** Return the "pid" of the wxExecute() */
  long GetPid();

protected:
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  

  mafString m_FileDir;
  mafString m_AnsysInputFileNameFullPath;

  mafString m_AnsysPythonImporterFullPathFileName;

  int m_ImporterType;
	mafVMEMesh *m_ImportedVmeMesh;

  /** Ansys input file name */
  mafString m_AnsysInputFileName;

  mafString m_CacheDir;

  /** Nodes file name*/
  mafString m_NodesFileName;

  /** Elements file name*/
  mafString m_ElementsFileName;

  /** Materials file name*/
  mafString m_MaterialsFileName;

  long m_Pid;
  
  mafString m_PythonExe; //>python  executable
  mafString m_PythonwExe; //>pythonw  executable

};
#endif
