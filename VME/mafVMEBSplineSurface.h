/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEBSplineSurface.h,v $
  Language:  C++
  Date:      $Date: 2007-11-16 10:26:10 $
  Version:   $Revision: 1.7 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEBSplineSurface_h
#define __mafVMEBSplineSurface_h

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

/** mafVMEBSplineSurface - a procedural VME computing the slice of its parent VME.
  mafVMEBSplineSurface is a node implementing a slicer of a VME (currently only VME-Volume).
  The sliced volume is the partent VME.
  @sa mafVMEVolume
  @todo
  - 
  */
template <class T>
class BSurface;
class MAF_EXPORT mafVMEBSplineSurface : public mafVME
{
public:

  mafTypeMacro(mafVMEBSplineSurface,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Slicer into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Slicer. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

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
  void SetPointsGroupLink(mafNode *n);

  void SetElemLink(mafNode *n, int index);


  /** 
  Get links for the Polyline*/
	mafVME *GetPointsGroupLink();

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  enum SPLINE_WIDGET_ID
  {
    ID_PNTS_GROUP_LINK = Superclass::ID_LAST,
    ID_NUMTESSELX,
    ID_NUMTESSELY,
    ID_ORDERX,
    ID_ORDERY,
    ID_MODE,
    ID_SUBMODEU,
    ID_SUBMODEV,
    ID_SMOOTHX,
    ID_SMOOTHY,
    ID_USEBSLINES,
    ID_LAST
  };

  static bool PolylineAccept(mafNode *node);

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

protected:
  mafVMEBSplineSurface();
  virtual ~mafVMEBSplineSurface();

  virtual int InternalStore(mafStorageElementBuilder& parent);
  virtual int InternalRestore(const mafStorageElement& node);

  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  mafTransform*     m_Transform; ///< pose matrix for the slicer plane

  vtkPolyData       *m_Polygons;
  mafTransform      *m_TmpTransform;

  mafString         m_PointsGroupName;
  BSurface<double>  *m_BSurface;
  int               m_OrderX, m_OrderY;
  int               m_Mode;
  int               m_SubModeU;
  int               m_SubModeV;
  int               m_useBSLines;
  double            m_SmoothX, m_SmoothY;
  int               m_TesselX, m_TesselY;


private:
  mafVMEBSplineSurface(const mafVMEBSplineSurface&); // Not implemented
  void operator=(const mafVMEBSplineSurface&); // Not implemented
};

#endif
