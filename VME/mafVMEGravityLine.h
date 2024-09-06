/*=========================================================================

 Program: MAF2
 Module: mafVMEGravityLine
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.
cloud
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVMEGravityLine_h
#define __mafVMEGravityLine_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "vtkSmartPointer.h"
#include "vtkPointSet.h"
#include "../Eigen/Eigen/Dense"
#include "mafVMEPlane.h"
#include "mafVMEMeter.h"
#include "mafVMELineSeg.h"
//#include "vtkAppendPolyData.h"
//#include "../../Eigen/unsupported/Eigen/LevenbergMarquardt"
#include <vector>
#include <iostream>
#include <fstream>
#include "mafVMEPlane.h"
#include "mafVMESurface.h"
//#include "../QuadricSurfaces/vtkContentActor.hpp"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class vtkPolyData;
class vtkPoints;
class mafVME;
class mafVMELandmark;
class mafVMELandmarkCloud;
class vtkLineSource;
class vtkXYPlotActor;
class vtkAppendPolyData;

class vtkXYPlotActor;
class mafRWI;
class mafGUIDialogPreview;




/** mafVMESurfaceParametric - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, cube, cone and plane.
*/
class MAF_EXPORT mafVMEGravityLine : public mafVME
{
public:

	mafTypeMacro(mafVMEGravityLine, mafVME);
    
	void SetSurfaceName(mafVMESurface* ,mafString);
	//void SetPlane(mafVMEPlane*, mafString a);
	void Update() override;

	int DeepCopy(mafNode *a) override;

  /** Compare with another mafVMESurfaceParametric. */
	bool Equals(mafVME *vme) override;

 
  /** return the right type of output */  
 // mafVMEOutputSurface *GetSurfaceOutput();
  mafVMEOutputPolyline *GetPolylineOutput();


	bool IsAnimated() override;

  /** Return the suggested pipe-typename for the visualization of this vme */
  //virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};
	mafString GetVisualPipe() override { return mafString(_R("mafPipePolyline")); };
  //virtual mafString GetVisualPipe() { return mafString("mafPipeMeter"); };
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  //static char ** GetIcon();
	
  /** Precess events coming from other objects */
	void OnEvent(mafEventBase *maf_event) override;

  void SetPlaneLink(const mafString& link_name, mafNode *n);
  void SetSurfaceLink(const mafString& link_name, mafNode *n);
  
  void SetTimeStamp(mafTimeStamp t) override;
  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified 
  
  . When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/

  mafVMEPlane* parallelPlane;
	void SetMatrix(const mafMatrix &mat) override;
	void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;
  /*
  Fit an ellipsoid to a set of points
  */
  mafVMEGravityLine();
	~mafVMEGravityLine() override;
protected:


  enum PARAMETRIC_SURFACE_WIDGET_ID
  {


	CHANGE_VALUE_POINTS1,
	ID_LAST,
	ID_PLANE_LINK,
	ID_P0_LINK,
	ID_Surface_LINK
  };

  enum ID_ORIENTATION_AXIS
  {
    ID_X_AXIS = 0,
    ID_Y_AXIS,
    ID_Z_AXIS,
  };
 // void SetInputPointsCloud(vtkPoints *);

  /** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
  //void EnableQuadricSurfaceGui(int surfaceTypeID);

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

	/** called to prepare the update of the output */
	void InternalPreUpdate() override;

	/** update the output data structure */
 void InternalUpdate() override;

  /** Internally used to create a new instance of the GUI.*/
	mafGUI *CreateGui() override;


  //void CreateGuiSelectPoints();

  mafString m_PlaneVmeName;
  mafString m_SurfaceName;
  //mafGUI *m_GuiSphere;

 // mafGUI *m_GuiSelectPoints;
  double* centerTemp;
  vtkAppendPolyData *m_Goniometer;
  mafString m_P0LandmarkName;

  mafTransform *m_Transform; 
  vtkPolyData  *m_PolyData;
  mafString gLength;
  //int m_GeometryType;

  double PointToPlanPt1[3];
  //double PointToPlanPt2[3];
  //double PointToPlanPt3[3];

  double distToPlan=0;

  vtkNew<vtkPoints> points;

  void SetLandmarkLink(const mafString&, mafNode *);
 
  void UpdateLinks();


  Eigen::Matrix3d rotationMat;
 
  mafVMELandmarkCloud	*m_Cloud2;
  
  mafVMEPlane* plan;
  mafVMESurface* surface;
  

  protected:
	  mafVME* GetP0VME();
	  vtkXYPlotActor *m_PlotActor;
	  mafVMEPlane* GetPlaneVME();
	  mafVMESurface* GetSurfaceVME();
	  
	  mafVME *P0;
	  

private:
	mafVMEGravityLine(const mafVMEGravityLine&); // Not implemented
	void operator=(const mafVMEGravityLine&); // Not implemented
};
#endif
