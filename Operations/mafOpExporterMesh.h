/*=========================================================================

 Program: MAF2
 Module: mafOpExporterMesh
 Authors: Simone Brazzale
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterMesh_H__
#define __mafOpExporterMesh_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMEMesh;

//----------------------------------------------------------------------------
// mafOpExporterMesh :
//----------------------------------------------------------------------------
/** Export a Generic mesh */
class MAF_EXPORT mafOpExporterMesh: public mafOp
{
public:
	mafOpExporterMesh(const mafString& label = _R("MeshExporter"));
	~mafOpExporterMesh(); 
	
  mafTypeMacro(mafOpExporterMesh, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Set the filename for the mesh to export */
  void SetFileName(const char *file_name) {this->m_File = _R(file_name);}

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

	/** Export the mesh*/
	int Write();

protected:

  mafString	m_FileDir;
  mafString m_File;

  /** Nodes file name*/
  mafString m_NodesFileName;

  /** Elements file name*/
  mafString m_ElementsFileName;

  /** Materials file name*/
  mafString m_MaterialsFileName;
};
#endif
