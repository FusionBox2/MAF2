


#ifndef __mafVMEPlane_h
#define __mafVMEPlane_h
// ----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
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
class vtkPolyData;

/** mafVMESurfaceParametric - this class represents a specific geometry: the plane.
*/
class MAF_EXPORT mafVMEPlane : public mafVME
{
public:

	mafTypeMacro(mafVMEPlane, mafVME);


	enum PARAMETRIC_SURFACE_TYPE_ID
	{
		
		PARAMETRIC_PLANE=0,
		
	};

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
	virtual mafString GetVisualPipe() { return mafString(_R("mafPipeSurface")); };

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
	mafVME *GetVMEPt1();
	double* getPoint1();
	double* getPoint2();
	double* getPoint3();
	void setPoint1(double[3]);
	void setPoint2(double[3]);
	void setPoint3(double[3]);
	//double* getPoint4();
	double* getNormalVector();
	void computeNormal();
//	mafVMELineSeg     *m_LineNormal;
	double normal[3];
	vtkPlaneSource* surf;
	//vtkPlaneSource* parallelPlane;
//	void EnableGuiPlane();
//	void DisableGuiPlane();
	void push(double s);
	void SetUVector(double,int);
	double GetUVector(int);
	void SetLandmarkLink(const mafString&, mafNode *);
//	void Setm_p1LName(mafString);
//	void Setm_p2LName(mafString);
//	void Setm_p3LName(mafString);
	void Update();
protected:
	double tr=0;
	mafVMEPlane();
	virtual ~mafVMEPlane();

	enum PARAMETRIC_SURFACE_WIDGET_ID
	{
		CHANGE_PARAMETER = Superclass::ID_LAST,
		
		CHANGE_VALUE_PLANE,
		ID_P1_LINK,
//		ID_P2_LINK,
//		ID_P3_LINK,
		ID_PLAN1_LINK,
		//ID_P4_LINK,
		//CHANGE_VALUE_ELLIPSOID,
		//ID_GEOMETRY_TYPE,
		ID_LAST
	};

	bool pts_change;
	
	void SetPlaneLink(const mafString&, mafNode *);
	mafString m_p1LandmarkName;
//	mafString m_p2LandmarkName;
//	mafString m_p3LandmarkName;
	mafString m_plan1Name;
	mafString angleBPlanes;
	double angPlanes;
	//mafString m_p4LandmarkName;
	mafVMELandmarkCloud	*m_Cloud2;
	mafVME* p1;
	//mafVME* p2;
	//mafVME* p3;
	mafVME* plan1;
	//mafVME* p4;
	/** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
	
	mafVME *GetLandmarkVME(mafString str);
	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

	/** Internally used to create a new instance of the GUI.*/
	virtual mafGUI *CreateGui();

	//void CreateGuiPlane();
	





	//mafGUI *m_GuiPlane;

	
	mafTransform *m_Transform;
	vtkPolyData  *m_PolyData;

	//int m_GeometryType;

	double m_PlaneXRes;
	double m_PlaneYRes;
	double m_PlaneOrigin[3];
	double m_PlanePoint1[3];
	double m_PlanePoint2[3];
	//double m_ParallelPlaneOrigin[3];

private:
	mafVMEPlane(const mafVMEPlane&); // Not implemented
	void operator=(const mafVMEPlane&); // Not implemented
};
#endif
