#pragma once

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafVMELandmarkCloud;
//class mafVMEPointSet;
class mafVMEPolyline;
class mafVMESurface;
class mafVMEVolumeGray;
class mafVMEVolumeRGB;
class mafVMEMesh;
class mafVMEGeneric;
class mafVMEImage;

//----------------------------------------------------------------------------
// mafOpImporterVTK :
//----------------------------------------------------------------------------
/** Import operation that try to read VTK data file and set it into the corresponding VME 
that accept the VTK data format. If no VME can accept the format a message box will be shown 
to the user and no data will be imported.*/
class MAF_EXPORT mafOpImporterVTK: public mafOp 
{
public:
  mafOpImporterVTK(const mafString& label = _R("VTKImporter"));
 ~mafOpImporterVTK() override; 
  
  mafTypeMacro(mafOpImporterVTK, mafOp);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* node) override {return true;};

	/** Builds operation's interface. */
  void OpRun() override;

	/** Import vtk data, return MAF_OK on success. */
  virtual int ImportVTK();

  /** Set the vtk filename to be imported. 
      This is used when the operation is executed not using user interface. */
  void SetFileName(const mafString& name) {m_File = name;};

protected:
  mafString m_File;
  mafString m_FileDir;
};
