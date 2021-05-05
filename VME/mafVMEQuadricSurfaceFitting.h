/*=========================================================================

 Program: MAF2
 Module: mafVMEQuadricSurfaceFitting
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVMEQuadricSurfaceFitting_h
#define __mafVMEQuadricSurfaceFitting_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "vtkMAFSmartPointer.h"

#include "../Eigen/Eigen/Dense"
#include "mafVMEEllipsoid.h"

//#include "../QuadricSurfaces/vtkContentActor.hpp"
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Matrix4d;
using Eigen::Matrix3d;
using Eigen::Matrix2d;
using Eigen::VectorXd;
using Eigen::Vector3d;
using Eigen::VectorXcd;
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


/** mafVMESurfaceParametric - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, cube, cone and plane.
*/
class MAF_EXPORT mafVMEQuadricSurfaceFitting : public mafVME
{
public:

	mafTypeMacro(mafVMEQuadricSurfaceFitting, mafVME);
    

  enum PARAMETRIC_SURFACE_TYPE_ID
  {
	  
	  POINTS_SELECTION_Ellipsoid1=0,
	  POINTS_SELECTION_Ellipsoid2,
	  POINTS_SELECTION_Ellipsoid3,
	  POINTS_SELECTION_Ellipsoid4,
	  POINTS_SELECTION_Ellipsoid5,
	  PARAMETRIC_SPHERE,
	  PARENT_POINTS,
	  POINTS_SELECTION_Ellipsoid7,
	
  };

  

  /** Set the geometry type to be generated: use PARAMETRIC_SURFACE_TYPE_ID as arg*/
  void SetGeometryType(int parametricSurfaceTypeID);

  /** Return the type of the parametric object.*/
  int GetGeometryType(){return m_GeometryType;};

  /** Set the radius for the parametric sphere.*/
  void SetSphereRadius(double radius);

  /** Return the radius of the parametric sphere.*/
  double GetSphereRadius() {return m_SphereRadius;};

  /** Return the radius of the parametric cylinder.*/
  double GetCylinderRadius() {return m_CylinderRadius;};

  /** Return the cylinder orientation axis.*/
  int GetCylinderAxis() {return m_CylinderOrientationAxis;};

  /** Copy the contents of another mafVMESurfaceParametric into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString(_R("mafPipeSurface"));};

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
  /*
  Fit an ellipsoid to a set of points
  */
  void fitEllipsoid(vtkMAFSmartPointer<vtkPoints> points, double* xres, double* yres, double* zres, double*r1, double* r2, double* r3, Matrix3d* rotationMatrix);

	mafVMEQuadricSurfaceFitting();
	virtual ~mafVMEQuadricSurfaceFitting();
protected:
  enum PARAMETRIC_SURFACE_WIDGET_ID
  {
    CHANGE_PARAMETER = Superclass::ID_LAST,
	CHANGE_VALUE_SPHERE,
	CHANGE_VALUE_CONE,
	CHANGE_VALUE_CYLINDER,
	CHANGE_VALUE_CUBE,
	CHANGE_VALUE_PLANE,
	CHANGE_VALUE_ELLIPSOID,
	CHANGE_VALUE_POINTS2,
	CHANGE_VALUE_POINTS3,
	CHANGE_VALUE_POINTS4,
	CHANGE_VALUE_POINTS5,
	CHANGE_VALUE_POINTS6,
	CHANGE_VALUE_POINTS7,
	CHANGE_VALUE_POINTS10,
	CHANGE_VALUE_POINTS1,
	CHANGE_VALUE_PARENTPOINTS,
    ID_GEOMETRY_TYPE,
    ID_LAST
  };

  enum ID_ORIENTATION_AXIS
  {
    ID_X_AXIS = 0,
    ID_Y_AXIS,
    ID_Z_AXIS,
  };
  void SetInputPointsCloud(vtkPoints *);

  /** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
  void EnableQuadricSurfaceGui(int surfaceTypeID);

	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  void CreateGuiPlane();
  void EnableGuiPlane();

  void CreateGuiParentVerticesResult();
  void EnableGuiParentVerticesResult();

  void CreateGuiCube();
  void EnableGuiCube();

  void CreateGuiCylinder();
  void EnableGuiCylinder();

  void CreateGuiSelectPoints();
  void EnableGuiSelectPoints();

  void CreateGuiCone();
  void EnableGuiCone();

  void CreateGuiSphere();
  void EnableGuiSphere();

  void CreateGuiEllipsoid();
  void EnableGuiEllipsoid();
  
  mafGUI *m_GuiSphere;
  mafGUI *m_GuiCone;
  mafGUI *m_GuiCylinder;
  mafGUI *m_GuiCube;
  mafGUI *m_GuiPlane;
  mafGUI *m_GuiEllipsoid;
  mafGUI *m_GuiSelectPoints;
  mafGUI *m_GuiParentVerticesResult;

	mafTransform *m_Transform; 
	vtkPolyData  *m_PolyData;
  
  int m_GeometryType;
  double m_PointsXRes;
  double m_PointsYRes;
  double m_PointsZRes;
  double m_PointsPhiRes;
  double m_PointsThetaRes;


  vtkMAFSmartPointer<vtkPoints> points;
  double m_Points1Point1[3];
  double m_Points1Point2[3];
  double m_Points1Point3[3];
  double m_Points1Point4[3];
  double m_Points1Point5[3];
  double m_Points1Point6[3];
  
  double m_Points2Point1[3];
  double m_Points2Point2[3];
  double m_Points2Point3[3];
  double m_Points2Point4[3];
  double m_Points2Point5[3];
  double m_Points2Point6[3];

  double m_Points3Point1[3];
  double m_Points3Point2[3];
  double m_Points3Point3[3];
  double m_Points3Point4[3];
  double m_Points3Point5[3];
  double m_Points3Point6[3];

  double m_Points4Point1[3];
  double m_Points4Point2[3];
  double m_Points4Point3[3];
  double m_Points4Point4[3];
  double m_Points4Point5[3];
  double m_Points4Point6[3];


  double m_Points5Point1[3];
  double m_Points5Point2[3];
  double m_Points5Point3[3];
  double m_Points5Point4[3];
  double m_Points5Point5[3];
  double m_Points5Point6[3];


  double m_Points6Point1[3];
  double m_Points6Point2[3];
  double m_Points6Point3[3];
  double m_Points6Point4[3];
  double m_Points6Point5[3];
  double m_Points6Point6[3];

  double m_Points7Point1[3];
  double m_Points7Point2[3];
  double m_Points7Point3[3];
  double m_Points7Point4[3];
  double m_Points7Point5[3];
  double m_Points7Point6[3];


  double m_PointsR1;
  double m_PointsR2;
  double m_PointsR3;
  //mafVMEEllipsoid *ellipseQ;

	double m_SphereRadius;
  double m_SpherePhiRes;
  double m_SphereTheRes;
  double m_ConeHeight;
  double m_ConeRadius;
  int m_ConeCapping;
  double m_ConeRes;
  int m_ConeOrientationAxis;
  double m_CylinderHeight;
  double m_CylinderRadius;
  double m_CylinderRes;
  int m_CylinderOrientationAxis;
  double m_CubeXLength;
  double m_CubeYLength;
  double m_CubeZLength;
  double m_PlaneXRes;
  double m_PlaneYRes;
  double m_PlaneOrigin[3];
  double m_PlanePoint1[3];
  double m_PlanePoint2[3];
  double m_EllipsoidXLenght;
  double m_EllipsoidYLenght;
  double m_EllipsoidZLenght;
  double m_EllipsoidPhiRes;
  double m_EllipsoidTheRes;
  int m_EllipsoidOrientationAxis;

  Eigen::Matrix3d rotationMat;
  mafVMELandmarkCloud	*m_Cloud;
  mafVMELandmarkCloud	*m_Cloud3;
private:
	mafVMEQuadricSurfaceFitting(const mafVMEQuadricSurfaceFitting&); // Not implemented
	void operator=(const mafVMEQuadricSurfaceFitting&); // Not implemented
};
#endif
