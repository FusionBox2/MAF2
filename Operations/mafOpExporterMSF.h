#pragma once
//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "ftk/Base/String.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafEvent;

//----------------------------------------------------------------------------
// mafOpExporterMSF :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpExporterMSF: public mafOp
{
public:
  mafOpExporterMSF(const mafString& label = _R("MSFExporter"));
 ~mafOpExporterMSF() override; 
  
  mafTypeMacroN(mafOpExporterMSF);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *vme) override;

	/** Builds operation's interface. */
  void OpRun() override;

  /** Set the filename for the .msf to export */
  void SetFileName(const char *file_name) {m_MSFFile = _R(file_name);}

  /** Get the filename for the .msf to export */
  mafString GetFileName() {return m_MSFFile;}

  /** Export selected subtree-tree to a .msf file*/
  int ExportMSF();

  typedef struct 
  {
    mafNode *oldID;
    mafNode *newID;
  } idValues;

protected:
  mafString m_MSFFile;
	mafString m_MSFFileDir;
};
