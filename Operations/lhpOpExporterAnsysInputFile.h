/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpExporterAnsysInputFile.h,v $
  Language:  C++
  Date:      $Date: 2008-11-14 15:17:04 $
  Version:   $Revision: 1.6.2.1 $
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
/** Write a mafVMEMesh in ANSYS .inp format by calling ansysWriter.py which is a 
grammar based Ansys .inp format writer written in Python */
class lhpOpExporterAnsysInputFile : public mafOp
{
public:
	lhpOpExporterAnsysInputFile(const mafString& label = _R("lhpOpExporterAnsysInputFile"));
	~lhpOpExporterAnsysInputFile(); 
	
  mafTypeMacro(lhpOpExporterAnsysInputFile, mafOp);

  /** Apply vme abs matrix to data geometry */
  void ApplyABSMatrixOn() {m_ABSMatrixFlag = 1;};
  void ApplyABSMatrixOff() {m_ABSMatrixFlag = 0;};
  void SetApplyABSMatrix(int apply_matrix) {m_ABSMatrixFlag = apply_matrix;};

  /** Set/Get output file name*/
  void SetOutputFileName(const char *outputFileName) {m_AnsysOutputFileNameFullPath = _R(outputFileName);};
  const char *GetOutputFileName() {return m_AnsysOutputFileNameFullPath.GetCStr();};

  /** Export the input mesh by writing it in Ansys .inp format */
  int Write();

  /** Builds operation's interface. */
	void OpRun();

  /** Return the "pid" of the wxExecute() ansysWriter.py process; use only for debugging
  purposes */
  long GetPid();

  virtual void OnEvent(mafEventBase *maf_event);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

protected:

  /** Set/Get nodes file name*/
  void SetInputNodesFileName(const char *name)   {this->m_NodesFileName = _R(name);}
  const char *GetInputNodesFileName() {return this->m_NodesFileName.GetCStr();}

  /** Set/Get elements file name*/
  void SetInputElementsFileName(const char *name)   {this->m_ElementsFileName = _R(name);}
  const char *GetInputElementsFileName() {return this->m_ElementsFileName.GetCStr();}

  /** Set/Get materials file name*/
  void SetInputMaterialsFileName(const char *name) {this->m_MaterialsFileName = _R(name);}
  const char *GetInputMaterialsFileName() {return this->m_MaterialsFileName.GetCStr();}

  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  
  
  void OpStop(int result);

  void OnOK();

  mafString m_FileDir;
  mafString m_AnsysOutputFileNameFullPath;

  mafString m_AnsysPythonExporterFullPathFileName;

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
  
  int				 m_ABSMatrixFlag;

  mafString m_PythonExe; //>python  executable
  mafString m_PythonwExe; //>pythonw  executable

};
#endif
