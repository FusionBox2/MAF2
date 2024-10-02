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
	int DeepCopy(mafNode *a) override;

  /** Compare with another mafVMESurfaceParametric. */
	bool Equals(mafVME *vme) override;

  /** return the right type of output */  
  mafVMEOutputPolyline* GetPolylineOutput();

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
	void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
	bool IsAnimated() override;

  /** Return the suggested pipe-typename for the visualization of this vme */
	mafString GetVisualPipe() override {return _R("mafPipePolyline");};

  /** Return pointer to material attribute. */
  std::shared_ptr<mmaMaterial> GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();
	
  /** Precess events coming from other objects */
	void OnEvent(mafEventBase *maf_event) override;

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
	void SetMatrix(const mafMatrix &mat) override;
  void SetPoint1(double, double, double);
  void SetPoint2(double, double, double);
  void setPoints(vtkPoints*);
  void resetPoints();
  vtkPoints* getPoints();
  vtkPolyData* getVTKPolydata();
  void Update() override;
  
protected:
	vtkPolyData* m_PolyData;
	mafVMELineSeg();
	~mafVMELineSeg() override;

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

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

	/** called to prepare the update of the output */
	void InternalPreUpdate() override;

	/** update the output data structure */
	void InternalUpdate() override;

  /** Internally used to create a new instance of the GUI.*/
	mafGUI *CreateGui() override;

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
