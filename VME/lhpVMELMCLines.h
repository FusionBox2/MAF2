/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVMELMCLines.h,v $
  Language:  C++
  Date:      $Date: 2007-11-16 10:26:10 $
  Version:   $Revision: 1.7 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpVMELMCLines_h
#define __lhpVMELMCLines_h

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkVolumeSlicer;
class vtkPolyData;
class mafNode;
class vtkPoints;
class mmaMaterial;

/** lhpVMELMCLines - a procedural VME computing the slice of its parent VME.
  lhpVMELMCLines is a node implementing a slicer of a VME (currently only VME-Volume).
  The sliced volume is the partent VME.
  @sa mafVMEVolume
  @todo
  - 
  */
class MAF_EXPORT lhpVMELMCLines : public mafVME
{
public:

  mafTypeMacro(lhpVMELMCLines,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Slicer into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Slicer. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputPolyline *GetPolylineOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

	/** 
  Set links for the Polyline*/
  void SetPointsCloudLink(mafNode *n);

  /** 
  Set links for the Polyline*/
  void SetCloud(mafVMELandmarkCloud *cloud);


  /** 
  Get links for the Polyline*/
  mafVMELandmarkCloud *GetPointsCloudLink();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  enum SPLINE_WIDGET_ID
  {
    ID_PNTS_CLOUD_LINK = Superclass::ID_LAST,
    ID_LOOPED,
    ID_LAST
  };

  static bool PolylineAccept(mafNode *node);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

protected:
  lhpVMELMCLines();
  virtual ~lhpVMELMCLines();

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** this creates the Material attribute at the right time... */
  virtual int InternalInitialize();

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  mafTransform*     m_Transform; ///< pose matrix for the slicer plane

  vtkPolyData           *m_Polyline;
  mafTransform          *m_TmpTransform;

  mafString             m_PointsCloudName;
  int                   m_Looped;

private:
  lhpVMELMCLines(const lhpVMELMCLines&); // Not implemented
  void operator=(const lhpVMELMCLines&); // Not implemented
};

#endif
