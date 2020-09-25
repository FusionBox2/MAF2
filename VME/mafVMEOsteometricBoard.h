


#ifndef __mafVMEOsteometricBoard_h
#define __mafVMEOsteometricBoard_h
// ----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEPlane.h"
#include "mafEvent.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "vtkPlaneSource.h"



//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class mafVMESurface;
class mafVMEGravityLine;
class vtkPolyData;

/** mafVMESurfaceParametric - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, cube, cone and plane.
*/
class MAF_EXPORT mafVMEOsteometricBoard : public mafVME
{
public:

	mafTypeMacro(mafVMEOsteometricBoard, mafVME);


	enum PARAMETRIC_SURFACE_TYPE_ID
	{
		
		PARAMETRIC_PLANE=0,
		
	};

	/** Set the geometry type to be generated: use PARAMETRIC_SURFACE_TYPE_ID as arg*/
	void SetGeometryType(int parametricSurfaceTypeID);

	/** Return the type of the parametric object.*/
//	int GetGeometryType(){ return m_GeometryType; };

	/** Set the radius for the parametric sphere.*/
	

	/** Copy the contents of another mafVMESurfaceParametric into this one. */
	virtual int DeepCopy(mafNode *a);
	void UpdateLinks();
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
	virtual mafString GetVisualPipe() { return mafString("mafPipeSurface"); };

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
	static bool VMEAccept(mafNode *node) { return(node != NULL && node->IsMAFType(mafVME)); };

	mafVMEGravityLine *gline1,*gline2,*gline3;

	vtkPlaneSource* surf;


	void push(double s);
	void SetUVector(double,int);
	double GetUVector(int);
	void SetSurfaceLink(const char *link_name, mafNode *n);
protected:
	double tr=0;
	mafVMEOsteometricBoard();
	virtual ~mafVMEOsteometricBoard();

	enum PARAMETRIC_SURFACE_WIDGET_ID
	{
		CHANGE_PARAMETER = Superclass::ID_LAST,
		
		CHANGE_VALUE_PLANE,
		ID_P1_LINK,
		ID_P2_LINK,
		ID_P3_LINK,
		ID_P4_LINK,
		ID_P5_LINK,
		ID_P6_LINK,
		ID_Surface_LINK,
		
		//ID_P4_LINK,
		//CHANGE_VALUE_ELLIPSOID,
		//ID_GEOMETRY_TYPE,
		ID_LAST
	};

	bool pts_change;
	void SetLandmarkLink(const char *, mafNode *);
	//void SetPlaneLink(const char *, mafNode *);
	mafString m_p1LandmarkName;
	mafString m_p2LandmarkName;
	mafString m_p3LandmarkName;
	mafString m_p4LandmarkName;
	mafString m_p5LandmarkName;
	mafString m_p6LandmarkName;
	mafString m_SurfaceName;
	mafString m_plan1Name;
	mafString angleBPlanes;
	double angPlanes;
	//mafString m_p4LandmarkName;
	mafVMELandmarkCloud	*m_Cloud1;
	//mafVMELandmarkCloud	*m_Cloud2;
	//mafVMELandmarkCloud	*m_Cloud3;
//	mafVME *p1,*p2,*p3,*p4,*p5,*p6;

	mafVMEPlane* plan1;
	mafVMEPlane* plan2;
	mafVMEPlane* plan3;

	mafVMESurface* surface;
	//mafVME* p4;
	/** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
	
	mafVMELandmark *GetLandmarkVME(mafString str);
	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

	/** Internally used to create a new instance of the GUI.*/
	virtual mafGUI *CreateGui();


	mafVMESurface* GetSurfaceVME();

	mafGUI *m_Gui;

	
	mafTransform *m_Transform;
	vtkPolyData  *m_PolyData;

	//int m_GeometryType;

	double m_PlaneXRes;
	double m_PlaneYRes;
	double m_PlaneOrigin[3];
	double m_PlanePoint1[3];
	double m_PlanePoint2[3];
	double m_PlanePoint3[3];
	double m_PlanePoint4[3];
	double m_PlanePoint5[3];

	double pos[3];
	double pos2[3];
	double pos3[3];
	//double m_ParallelPlaneOrigin[3];

private:
	mafVMEOsteometricBoard(const mafVMEOsteometricBoard&); // Not implemented
	void operator=(const mafVMEOsteometricBoard&); // Not implemented
};
#endif
