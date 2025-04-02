#pragma once

#include "mafOp.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafEvent;
class mafVMEExternalData;
class mafOp;

//----------------------------------------------------------------------------
// mafOpImporterExternalFile :
//----------------------------------------------------------------------------
/** Operation to import external files, such as Word or Excel files*/
class MAF_EXPORT mafOpImporterExternalFile: public mafOp
{
public:
  mafOpImporterExternalFile(const mafString &label = _R("ExternalFileImporter"));
 ~mafOpImporterExternalFile() override; 

  mafTypeMacroN(mafOpImporterExternalFile);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) override {return true;};

  /** Set the External filename to be imported. 
  This is used when the operation is executed not using user interface. */
  void SetFileName(const char *name) {m_File = _R(name);};

	/** Builds operation's interface. */
  void OpRun() override;

	/** Execute the operation. */
  void ImportExternalFile();

protected:
  mafString m_FileDir;
	mafString m_File;

	std::shared_ptr<mafVMEExternalData> m_Vme; 
};