/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceParametric
 Authors: Roberto Mucci , Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVMELineSeg_h
#define __mafVMELineSeg_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "vtkCellArray.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEOutputPolyline.h"
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;
//class mafVMEOutputSurface;

class vtkPolyData;
class vtkLineSource;
class vtkAppendPolyData;
class vtkPolyLine;
class vtkPoints;
/** mafVMESurfaceParametric - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, line,  and plane.
*/
class MAF_EXPORT mafVMELineSeg : public mafVME
{
public:

	mafTypeMacro(mafVMELineSeg, mafVME);
    

  enum PARAMETRIC_SURFACE_TYPE_ID
  {
    PARAMETRIC_LINE=0
 
  };

  /** Set the geometry type to be generated: use PARAMETRIC_SURFACE_TYPE_ID as arg*/
  //void SetGeometryType(int parametricSurfaceTypeID);

  /** Return the type of the parametric object.*/
 // int GetGeometryType(){return m_GeometryType;};



  /** Copy the contents of another mafVMESurfaceParametric into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputPolyline* GetPolylineOutput();

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return _R("mafPipePolyline");};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);
  void SetPoint1(double, double, double);
  void SetPoint2(double, double, double);
  void setPoints(vtkPoints*);
  void resetPoints();
  vtkPoints* getPoints();
  vtkPolyData* getVTKPolydata();
  void Update();
  
protected:
	vtkPolyData* m_PolyData;
	mafVMELineSeg();
	virtual ~mafVMELineSeg();

  enum PARAMETRIC_SURFACE_WIDGET_ID
  {
    CHANGE_PARAMETER = Superclass::ID_LAST,
    CHANGE_VALUE_LINE,
    ID_GEOMETRY_TYPE,
    ID_LAST
  };

  enum ID_ORIENTATION_AXIS
  {
    ID_X_AXIS = 0,
    ID_Y_AXIS,
    ID_Z_AXIS,
  };
  

  /** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
  //void EnableParametricSurfaceGui(int surfaceTypeID);

	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  static bool VMEAccept(mafNode *node) { return(node != NULL && node->IsMAFType(mafVME)); };
  static bool VolumeAccept(mafNode *node) { return(node != NULL && node->IsMAFType(mafVMEVolumeGray)); };
  
  void CreateGuiLine();
  void EnableGuiLine();
  
  
  mafGUI *m_GuiLine;

  vtkCellArray* cellArray;
  mafTransform *m_Transform; 
 

  int m_GeometryType;

  vtkPoints* pts;

  


  double PosPt1[3];
  double PosPt2[3];


private:
  mafVMELineSeg(const mafVMELineSeg&); // Not implemented
  void operator=(const mafVMELineSeg&); // Not implemented
};
#endif
