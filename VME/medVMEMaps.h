/*=========================================================================

 Program: MAF2Medical
 Module: medVMEMaps
 Authors: Eleonora Mambrini
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medVMEMaps_h
#define __medVMEMaps_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "mafVME.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMESurface;
class mafVMEVolume;
class mafTransform;
class mafVMEOutputSurface;
class mmaMaterial;
class vtkColorTransferFunction;

class vtkMAFDistanceFilter;
class vtkPolyData;
class vtkPolyDataNormals;
class vtkLookupTable;


/** Class name: medVMEMaps.
VME with input a surface and a volume, and producing a distance/density scalar surface.  
*/

class MED_VME_EXPORT medVMEMaps : public mafVME
{
public:
  
  /** IDs for the GUI */
  enum VME_MAPS_WIDGET_ID
  {
    ID_DENSITY_DISTANCE = Superclass::ID_LAST,
    ID_LAST
  };

  mafTypeMacro(medVMEMaps,mafVME);

  medVMEMaps();
  ~medVMEMaps() override;

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;

  /** Copy the contents of another VME-Maps into this one. */
  int DeepCopy(mafNode *a) override;

  /** Compare with another VME-Maps. */
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
  //virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  //virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  mafString GetVisualPipe() override {return mafString(_R("mafPipeSurface"));}

  /** Return pointer to material attribute. */
  std::shared_ptr<mmaMaterial> GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  //static char ** GetIcon();

  /** Set the source volume. */
  void SetVolume(mafVMEVolume *volume);

  /** Get the source volume. */
  mafVMEVolume *GetVolume();

  /** 
  Set data for the give timestamp. 
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. */
  //virtual int SetData(vtkPolyData *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  /** Get distance-density filter mode. */
  int GetDensityDistance(){return m_DensityDistance;};

  /** Set distance-density filter mode. */
  void SetDensityDistance(int densityDistance);

  /** Get first threshold distance-density filter parameter. */
  int GetFirstThreshold(){return m_FirstThreshold;};

  /** Set first threshold distance-density filter parameter. */
  void SetFirstThreshold(int firstThreshold);

  /** Get second threshold distance-density filter parameter. */
  int GetSecondThreshold(){return m_SecondThreshold;};

  /** Set second threshold distance-density filter parameter. */
  void SetSecondThreshold(int secondThreshold);
  
  /** Get max distance distance-density filter parameter. */
  int GetMaxDistance(){return m_MaxDistance;};

  /** Set max distance distance-density filter parameter. */
  void SetMaxDistance(int maxDistance);

  /** Set the link to the maps.*/
  void SetMappedVMELink(mafNode *node);

  /** Get the link to the mapped vme.*/
  mafNode *GetMappedVMELink();

  /** Set the volume link to the maps.*/
  void SetSourceVMELink(mafNode *node);

  /** Get the link to the source vme.*/
  mafNode *GetSourceVMELink();

  /** Get the scalar range of the output vtkDataSet. */
  void GetScalarRange(double range[2]);

private:

  //medVMEMaps();
  //virtual ~medVMEMaps();

  /** Internally used to create a new instance of the GUI.*/
  mafGUI *CreateGui() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** called to prepare the update of the output */
  void InternalPreUpdate() override;

  /** update the output data structure */
  void InternalUpdate() override;

  vtkLookupTable *CreateTable();

  /** Update distance density filter. */
  void UpdateFilter();

  /** private to avoid calling by external classes */
  //virtual int SetData(vtkDataSet *data, mafTimeStamp t, int mode=MAF_VME_COPY_DATA);

  mafString                 m_MappedName;

  vtkPolyDataNormals        *m_Normals;
  vtkMAFDistanceFilter      *m_DistanceFilter;
  mafVMEVolume              *m_Volume;
  mafTransform              *m_Transform;
  vtkPolyData               *m_PolyData;
  vtkLookupTable            *m_Table;

  int m_DensityDistance;
  int m_FirstThreshold;
  int m_SecondThreshold;
  int m_MaxDistance;

};
#endif
