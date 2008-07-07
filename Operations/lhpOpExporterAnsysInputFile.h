/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpExporterAnsysInputFile.h,v $
  Language:  C++
  Date:      $Date: 2008-07-07 14:31:46 $
  Version:   $Revision: 1.2 $
  Authors:   Stefano Perticoni   
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpExporterAnsysInputFile_H__
#define __lhpOpExporterAnsysInputFile_H__

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
// lhpOpExporterAnsysInputFile :
//----------------------------------------------------------------------------
/** */
class lhpOpExporterAnsysInputFile : public mafOp
{
public:
	lhpOpExporterAnsysInputFile(const wxString &label = "MeshImporter");
	~lhpOpExporterAnsysInputFile(); 
	
  mafTypeMacro(lhpOpExporterAnsysInputFile, mafOp);

  virtual void OnEvent(mafEventBase *maf_event);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Set the filename for the mesh to import */
  void SetFileName(const char *file_name);

  /** Set/Get nodes file name*/
  void SetNodesFileName(const char *name)   {this->m_NodesFileName = name;};
  const char *GetNodesFileName() {return this->m_NodesFileName;};

  /** Set/Get elements file name*/
  void SetElementsFileName(const char *name)   {this->m_ElementsFileName = name;};
  const char *GetElementsFileName() {return this->m_ElementsFileName;};

  /** Set/Get materials file name*/
  void SetMaterialsFileName(const char *name) {this->m_MaterialsFileName = name;};
  const char *GetMaterialsFileName() {return this->m_MaterialsFileName;};

  /** Builds operation's interface. */
	void OpRun();

	/** Import the mesh*/
	int Read();

  /** Return the "pid" of the wxExecute() */
  long GetPid();

  /** Apply vme abs matrix to data geometry */
  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

protected:
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  
  
  void OpStop(int result);

  void OnOK();

  wxString m_FileDir;
  wxString m_AnsysOutputFileNameFullPath;

  wxString m_AnsysPythonExporterFullPathFileName;

  int m_ImporterType;
	mafVMEMesh *m_ImportedVmeMesh;

  /** Ansys input file name */
  mafString m_AnsysInputFileName;

  mafString m_CacheDir;

  /** Nodes file name*/
  wxString m_NodesFileName;

  /** Elements file name*/
  wxString m_ElementsFileName;

  /** Materials file name*/
  wxString m_MaterialsFileName;

  long m_Pid;
  
  int				 m_ABSMatrixFlag;
};
#endif
