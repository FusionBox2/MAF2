/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEBSplineVolume.h,v $
Language:  C++
Date:      $Date: 2007-11-16 10:26:10 $
Version:   $Revision: 1.7 $
Authors:   Daniele Giunchi & Matteo Giacomoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEBSplineVolume_h
#define __mafVMEBSplineVolume_h

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEOutputSurface.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkVolumeSlicer;
class vtkPolyData;
class mafNode;
class vtkPoints;
class mmaMaterial;

/** mafVMEBSplineVolume - a procedural VME computing the slice of its parent VME.
mafVMEBSplineVolume is a node implementing a slicer of a VME (currently only VME-Volume).
The sliced volume is the partent VME.
@sa mafVMEVolume
@todo
- 
*/
template <class T>
class BVolume;
class MAF_EXPORT mafVMEBSplineVolume : public mafVME
{
public:

  mafTypeMacro(mafVMEBSplineVolume,mafVME);

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;

  /** Copy the contents of another VME-Slicer into this one. */
  int DeepCopy(mafNode *a) override;

  /** Compare with another VME-Slicer. */
  bool Equals(mafVME *vme) override;

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
  mafString GetVisualPipe() override;

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Return pointer to material attribute. */
  std::shared_ptr<mmaMaterial> GetMaterial();

  enum SPLINE_WIDGET_ID
  {
    ID_PNTS_GROUP_LINK = Superclass::ID_LAST,
    ID_NUMTESSELX,
    ID_NUMTESSELY,
    ID_NUMTESSELZ,
    ID_NUMSOURCEX,
    ID_NUMSOURCEY,
    ID_NUMSOURCEZ,
    ID_ORDERX,
    ID_ORDERY,
    ID_ORDERZ,
    ID_MODE,
    ID_LAST
  };

  static bool PolylineAccept(mafNode *node);

  /** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event) override;

protected:
  mafVMEBSplineVolume();
  ~mafVMEBSplineVolume() override;

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** used to initialize and create the material attribute if not yet present */
  int InternalInitialize() override;

  /** called to prepare the update of the output */
  void InternalPreUpdate() override;

  /** update the output data structure */
  void InternalUpdate() override;

  /** Internally used to create a new instance of the GUI.*/
  mafGUI    *CreateGui() override;

  std::shared_ptr<mafTransform> m_Transform; ///< pose matrix for the slicer plane

  vtkPolyData       *m_Polygons;
  std::shared_ptr<mafTransform> m_TmpTransform;

  mafString         m_PointsGroupName;
  BVolume<double>   *m_BVolume;
  int               m_OrderX, m_OrderY, m_OrderZ;
  int               m_Mode;
  int               m_TesselX, m_TesselY, m_TesselZ;
  int               m_SourceX, m_SourceY, m_SourceZ;


private:
  mafVMEBSplineVolume(const mafVMEBSplineVolume&); // Not implemented
  void operator=(const mafVMEBSplineVolume&); // Not implemented
};

#endif
