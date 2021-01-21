/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurfaceRegParam.h,v $
  Language:  C++
  Date:      $Date: 2008-04-21 12:26:29 $
  Version:   $Revision: 1.4 $
  Authors:   Roberto Mucci
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafVMESurfaceRegParam_h
#define __mafVMESurfaceRegParam_h

#include "lhpDefines.h"

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class vtkPolyData;

/** mafVMESurfaceRegParam - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, cube, cone and plane.
*/
class MAF_EXPORT mafVMESurfaceRegParam : public mafVME
{
public:

  mafTypeMacro(mafVMESurfaceRegParam, mafVME);
    

  enum PARAMETRIC_SURFACE_TYPE_ID
  {
    PARAMETRIC_SPHERE = 0,
    PARAMETRIC_CONE,
    PARAMETRIC_CYLINDER,
    PARAMETRIC_CUBE,
    PARAMETRIC_PLANE,
    PARAMETRIC_ELLIPSOID,
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

  /** Copy the contents of another mafVMESurfaceRegParam into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMESurfaceRegParam. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputSurface *GetSurfaceOutput();

  const double *GetPlaneCenter(){return m_PlaneCenter;}
  const double *GetPlaneNormal(){return m_PlaneNormal;}
  const double *GetPlaneOrigin(){return m_PlaneOrigin;}
  const double *GetPlanePoint1(){return m_PlanePoint1;}
  const double *GetPlanePoint2(){return m_PlanePoint2;}
  const double *GetSphereCenter(){return m_SphereCenter;}
  double GetEllipsoidXSemiAxis() {return m_EllipsoidXLenght;};
  double GetEllipsoidYSemiAxis() {return m_EllipsoidYLenght;};
  double GetEllipsoidZSemiAxis() {return m_EllipsoidZLenght;};

  void SetPlaneCenter(const double *center);
  void SetPlaneNormal(const double *normal);
  void SetPlaneOrigin(const double *origin);
  void SetPlanePoint1(const double *point1);
  void SetPlanePoint2(const double *point2);
  void SetSphereCenter(const double *center);
  void SetEllipsoidXSemiAxis(double axis);
  void SetEllipsoidYSemiAxis(double axis);
  void SetEllipsoidZSemiAxis(double axis);

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


  enum PARAMETRIC_SURFACE_WIDGET_ID
  {
    CHANGE_PARAMETER = Superclass::ID_LAST,
    CHANGE_VALUE_SPHERE,
    CHANGE_VALUE_CONE,
    CHANGE_VALUE_CYLINDER,
    CHANGE_VALUE_CUBE,
    CHANGE_VALUE_PLANE,
    CHANGE_VALUE_ELLIPSOID,
    ID_GEOMETRY_TYPE,
    ID_LAST
  };

  enum ID_ORIENTATION_AXIS
  {
    ID_X_AXIS = 0,
    ID_Y_AXIS,
    ID_Z_AXIS,
  };
  

protected:

  mafVMESurfaceRegParam();
  virtual ~mafVMESurfaceRegParam();

  /** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
  void EnableParametricSurfaceGui(int surfaceTypeID);

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

  void CreateGuiCube();
  void EnableGuiCube();

  void CreateGuiCylinder();
  void EnableGuiCylinder();

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

	mafTransform *m_Transform; 
	vtkPolyData  *m_PolyData;
  
  int m_GeometryType;
	double m_SphereRadius;
  double m_SpherePhiRes;
  double m_SphereTheRes;
  double m_SphereCenter[3];
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
  double m_PlaneXSize;
  double m_PlaneYSize;
  double m_PlaneCenter[3];
  double m_PlaneNormal[3];
  double m_PlaneOrigin[3];
  double m_PlanePoint1[3];
  double m_PlanePoint2[3];
  double m_EllipsoidXLenght;
  double m_EllipsoidYLenght;
  double m_EllipsoidZLenght;
  double m_EllipsoidPhiRes;
  double m_EllipsoidTheRes;
  int m_EllipsoidOrientationAxis;
  
private:
  mafVMESurfaceRegParam(const mafVMESurfaceRegParam&); // Not implemented
  void operator=(const mafVMESurfaceRegParam&); // Not implemented
};
#endif
