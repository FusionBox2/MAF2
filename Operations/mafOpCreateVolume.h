#pragma once
#include "mafOp.h"

//----------------------------------------------------------------------------
// mafOpCreateVolume :
//----------------------------------------------------------------------------
/** Operation used to create a mafVolumeGray containing the vtkStructuredPoint
dataset filled with a constant scalar value. This is useful for testing purposes.*/
class MAF_EXPORT mafOpCreateVolume: public mafOp
{
public:
  mafOpCreateVolume(const mafString& label = _R("Create Volume"));
  ~mafOpCreateVolume() override; 

  mafTypeMacroN(mafOpCreateVolume);

  /** Receive events coming from the user interface.*/
  void OnEvent(mafEventBase *maf_event) override;

  /** Return a copy of the operation.*/
  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;

  /** Builds operation's interface. */
  void OpRun() override;

  /** Set the volume density from code.*/
  void SetVolumeDensity(double density) {m_Density = density;};

  /** Return the volume density of the volume.*/
  double GetVolumeDensity() {return m_Density;};

  /** Set/Get the volume spacing.*/
  void SetVolumeSpacing(double spc[3]);

  /** Set/Get the volume spacing.*/
  void GetVolumeSpacing(double spc[3]);

  /** Set/Get the volume real dimension.*/
  void SetVolumeDimensions(double dim[3]);

  /** Set/Get the volume real dimension.*/
  void GetVolumeDimensions(double dim[3]);

  /** Create the vtkStructuredPoints and put it into the mafVolumeGray.*/
  void CreateVolume();

private:
  double m_Spacing[3]; ///< Voxel spacing of the created volume
  double m_Dimensions[3]; ///< Dimensions of created volume
  double m_Density; ///< Constant density value that fill the created volume
};
