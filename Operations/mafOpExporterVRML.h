/*=========================================================================

 Program: MAF2
 Module: mafOpExporterVRML
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExporterVRML_H__
#define __mafOpExporterVRML_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVMEGroup;
class mmaMaterial;
//----------------------------------------------------------------------------
//mafOpExporterVRML :
//----------------------------------------------------------------------------
/**
	VRML files are commonly called worlds and have the .wrl extension. 
	

*/
class MAF_EXPORT mafOpExporterVRML : public mafOp
{
public:
           mafOpExporterVRML(const mafString& label = _R("VRMLExporter"));
           ~mafOpExporterVRML() override;
	
  mafTypeMacro(mafOpExporterVRML, mafOp);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) override {return true;};
	void OnEvent(mafEventBase *maf_event) override;
	/** Builds operation's interface. */
  void OpRun() override;

  /** Set the filename for the .vrl to import */
  void SetFileName(const mafString& file_name);

  /** Export vrml data. */
  void ExportVRML();

protected:
	void OpStop(int result) override;
	mafString m_File;
	mafString m_FileDir;
  
	std::shared_ptr<mmaMaterial> m_SurfaceMaterial;
};
#endif
