#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
//mafOpImporterVRML :
//----------------------------------------------------------------------------
/**
	VRML files are commonly called worlds and have the .wrl extension. 
	Although VRML worlds use a text format they may often be compressed using gzip so 
	that they transfer over the internet more quickly. 
	This modality in not supported by maf vrml importer, it can only import uncompressed wrl.

*/
class MAF_EXPORT mafOpImporterVRML: public mafOp
{
public:
           mafOpImporterVRML(const mafString& label = _R("VRMLImporter"));
           ~mafOpImporterVRML() override;
	
  mafTypeMacroN(mafOpImporterVRML);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) override {return true;};

	/** Builds operation's interface. */
  void OpRun() override;

  /** Set the filename for the .vrl to import */
  void SetFileName(const mafString& file_name);

  /** Import vrml data. */
  void ImportVRML();

protected:
	mafString m_File;
	mafString m_FileDir;
};
