#pragma once

#include "mafVME.h"
#include "mafEvent.h"

#include <vtkNew.h>

#include <array>

class mmaMaterial;
class mafVMEOutputSurface;
class vtkPolyData;

/** mafVMESurfaceParametric - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, cube, cone and plane.
*/
class MAF_EXPORT mafVMESurfaceParametric : public mafVME
{
public:

	mafTypeMacro(mafVMESurfaceParametric, mafVME)

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
	int GetGeometryType() { return m_GeometryType; }

	/** Set the radius for the parametric sphere.*/
	void SetSphereRadius(double radius);

	/** Return the radius of the parametric sphere.*/
	double GetSphereRadius() { return m_SphereRadius; }

	/** Return the radius of the parametric cylinder.*/
	double GetCylinderRadius() { return m_CylinderRadius; }

	/** Return the cylinder orientation axis.*/
	int GetCylinderAxis() { return m_CylinderOrientationAxis; }

	/** Copy the contents of another mafVMESurfaceParametric into this one. */
	int DeepCopy(mafNode* a) override;

	/** Compare with another mafVMESurfaceParametric. */
	bool Equals(mafVME* vme) override;

	/** return the right type of output */
	mafVMEOutputSurface* GetSurfaceOutput();

	/**
	Return the list of timestamps for this VME. Timestamps list is
	obtained merging timestamps for matrixes and VME items*/
	void GetLocalTimeStamps(std::vector<mafTimeStamp>& kframes) override;

	/** return always false since (currently) the slicer is not an animated VME (position
	is the same for all timestamps). */
	bool IsAnimated() override;

	/** Return the suggested pipe-typename for the visualization of this vme */
	mafString GetVisualPipe() override { return mafString(_R("mafPipeSurface")); }

	/** Return pointer to material attribute. */
	std::shared_ptr<mmaMaterial> GetMaterial();

	/** return an xpm-icon that can be used to represent this node */
	static const char** GetIcon();

	/** Precess events coming from other objects */
	void OnEvent(mafEventBase* maf_event) override;

	/**
	Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
	set or get the Pose for a specified time. When setting, if the time does not exist
	the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
	interpolates on the fly according to the matrix interpolator.*/
	void SetMatrix(const mafMatrix& mat) override;

	std::vector<std::unique_ptr<IProperty>> getProperties() override;

protected:

	mafVMESurfaceParametric();
	~mafVMESurfaceParametric() override;

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


	/** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
	void EnableParametricSurfaceGui(int surfaceTypeID);

	void InternalStore(mafStorageElementBuilder& parent) override;
	void InternalRestore(const mafStorageElement& node) override;

	/** called to prepare the update of the output */
	void InternalPreUpdate() override;

	/** update the output data structure */
	void InternalUpdate() override;

	/** Internally used to create a new instance of the GUI.*/
	mafGUI* CreateGui() override;

	void CreateGuiPlane(mafGUI* gui);
	void EnableGuiPlane();

	void CreateGuiCube(mafGUI* gui);
	void EnableGuiCube();

	void CreateGuiCylinder(mafGUI* gui);
	void EnableGuiCylinder();

	void CreateGuiCone(mafGUI* gui);
	void EnableGuiCone();

	void CreateGuiSphere(mafGUI* gui);
	void EnableGuiSphere();

	void CreateGuiEllipsoid(mafGUI* gui);
	void EnableGuiEllipsoid();

	mafGUI* m_GuiSphere;
	mafGUI* m_GuiCone;
	mafGUI* m_GuiCylinder;
	mafGUI* m_GuiCube;
	mafGUI* m_GuiPlane;
	mafGUI* m_GuiEllipsoid;

	std::shared_ptr<mafTransform> m_Transform;
	vtkNew<vtkPolyData> m_PolyData;

	int m_GeometryType = PARAMETRIC_SPHERE;
	double m_SphereRadius = 2.0;
	double m_SpherePhiRes = 10;
	double m_SphereTheRes = 10;
	double m_ConeHeight = 5.0;
	double m_ConeRadius = 2.0;
	int m_ConeCapping = 0;
	double m_ConeRes = 20;
	int m_ConeOrientationAxis = ID_X_AXIS;
	double m_CylinderHeight = 5.0;
	double m_CylinderRadius = 2.0;
	double m_CylinderRes = 20;
	int m_CylinderOrientationAxis = ID_Y_AXIS;
	double m_CubeXLength = 2.0;
	double m_CubeYLength = 2.0;
	double m_CubeZLength = 2.0;
	double m_PlaneXRes = 2;
	double m_PlaneYRes = 2;
	std::array<double,3> m_PlaneOrigin = {0.0, 0.0, 0.0};
	std::array<double, 3> m_PlanePoint1 = {2.0, 0.0, 0.0};
	std::array<double, 3> m_PlanePoint2 = {0.0, 3.0, 0.0};
	double m_EllipsoidXLength = 1.0;
	double m_EllipsoidYLength = 2.0;
	double m_EllipsoidZLenght = 3.0;
	double m_EllipsoidPhiRes = 10;
	double m_EllipsoidTheRes = 10;
	int m_EllipsoidOrientationAxis = ID_X_AXIS;

private:
	mafVMESurfaceParametric(const mafVMESurfaceParametric&) = delete;
	void operator=(const mafVMESurfaceParametric&) = delete;
};
