/*=========================================================================

 Program: MAF2
 Module: mafVMESlicer
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMESlicer_h
#define __mafVMESlicer_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEVolume.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkMAFVolumeSlicer;
class vtkTransformPolyDataFilter;
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;

/** mafVMESlicer - a procedural VME computing the slice of its parent VME.
  mafVMESlicer is a node implementing a slicer of a VME (currently only VME-Volume).
  The sliced volume is the partent VME.
  @sa mafVMEVolume
  @todo
  - 
  */
class MAF_EXPORT mafVMESlicer : public mafVME
{
public:

  /** Enable/Disable slicer texture vtk property automatic update from material (default behavior is 
  updating from material) */
  void SetUpdateVTKPropertiesFromMaterial(bool updateFromMaterial) {m_UpdateVTKPropertiesFromMaterial = updateFromMaterial;};
  bool GetUpdateVTKPropertiesFromMaterial() {return m_UpdateVTKPropertiesFromMaterial;};

  mafTypeMacro(mafVMESlicer,mafVME);

  enum SLICER_WIDGET_ID
  {
    ID_VOLUME_LINK = Superclass::ID_LAST,
    ID_LAST
  };

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;

  /** Copy the contents of another VME-Slicer into this one. */
  int DeepCopy(mafNode *a) override;

  /** Compare with another VME-Slicer. */
  bool Equals(mafVME *vme) override;

  /** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event) override;

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const mafMatrix &mat) override;

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  bool IsAnimated() override;

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  bool IsDataAvailable() override;

  /** Return the suggested pipe-typename for the visualization of this vme */
  mafString GetVisualPipe() override {return mafString(_R("mafPipeSurfaceTextured"));};
  
  /** Return pointer to material attribute. */
  std::shared_ptr<mmaMaterial> GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Set the link to the slicer.*/
  void SetSlicedVMELink(mafNode *node);

  /** Get the link to the slicing vme.*/
  mafNode *GetSlicedVMELink();

  static bool VolumeAccept(mafNode* node) {return(node != NULL  && node->IsMAFType(mafVMEVolume));};


  /** Set tri-linear interpolation to on */
  void SetTrilinearInterpolationOn(){m_TrilinearInterpolationOn = true;};

  /** Set tri-linear interpolation to off */
  void SetTrilinearInterpolationOff(){m_TrilinearInterpolationOn = false;};

  /** Set tri-linear interpolation */
  void SetTrilinearInterpolation(bool on);

protected:
  mafVMESlicer();
  ~mafVMESlicer() override;

  /** Internally used to create a new instance of the GUI.*/
  mafGUI *CreateGui() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** called to prepare the update of the output */
  void InternalPreUpdate() override;

  /** update the output data structure */
  void InternalUpdate() override;

	std::shared_ptr<mafTransform>     m_CopyTransform; ///< pose matrix for the slicer plane
  std::shared_ptr<mafTransform>     m_Transform; ///< pose matrix for the slicer plane
  vtkMAFVolumeSlicer*  m_PSlicer;  ///< slicer object used to extract the cut contour
  vtkMAFVolumeSlicer*  m_ISlicer;  ///< slicer object used to compute the slice image

  vtkTransformPolyDataFilter *m_BackTransform;
  //vtkTransformPolyDataFilter *m_BackTransformParent;

  mafString         m_SlicedName;
  int               m_TextureRes;
  double            m_Xspc;
  double            m_Yspc;

  bool m_UpdateVTKPropertiesFromMaterial;

  bool m_TrilinearInterpolationOn;

private:
  mafVMESlicer(const mafVMESlicer&); // Not implemented
  void operator=(const mafVMESlicer&); // Not implemented
};
#endif
