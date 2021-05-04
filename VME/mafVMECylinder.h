//
// Cylinder.hpp


#ifndef __mafVMECylinder_h
#define __mafVMECylinder_h

#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyData.h>
#include "mafQuadraticSurface.hpp"
#include "mafGUIHolder.h"
#include "mafGUISplittedPanel.h"
#include "mafGUINamedPanel.h"
#include "mafGUIDictionaryWidget.h"
#include "mafVME.h"
#include "mafEvent.h"
#include "mafTransform.h"
#include "mafVMEOutputSurface.h"
#include "mafObjectFactory.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMELandmarkCloud.h"
class mmaMaterial;
class mafNode;

class mafVMEOutputSurface;
class vtkPolyData;

class MAF_EXPORT mafVMECylinder : public mafQuadraticSurface
{
public:

	mafTypeMacro(mafVMECylinder, mafQuadraticSurface, mafVME);

protected:

	enum CYLINDER_ID
	{
		CHANGE_VALUE_Cylinder,
		ID_CYLINDERCenter_LINK
	};

	//enum ID_ORIENTATIONCYLINDER_AXIS
	//{
	//	ID_X_AXIS = 0,
	//	ID_Y_AXIS,
	//	ID_Z_AXIS,
	//};

	
private:
	//mafVMELandmarkCloud	*m_Cloud2;
	virtual void InternalUpdate();
	void UpdateLinks();
	
	mafVME *center_vme;
	void SetTimeStamp(mafTimeStamp t);
	//double centerTemp[3];
	mafString m_LandmarkName;
	void SetCenterLink(const mafString&, mafNode *);
	bool Equals(mafVME*);
	double a,b, h;
	
	void OnEvent(mafEventBase *maf_event);
	int m_CylinderOrientationAxis;
	RowVector3d grad(const Vector3d &point) const;
	Matrix3d hess(const Vector3d &point) const;
	mafTransform *m_Transform;
	vtkPolyData  *m_PolyData;
	
	double res;
	
	virtual mafGUI *CreateGui();
	double centerAbs[3];
	
	mafVMEOutputSurface *GetSurfaceOutput();

	
	virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);
	virtual void SetMatrix(const mafMatrix &mat);
	virtual bool IsAnimated();
	virtual mafString GetVisualPipe() { return mafString(_R("mafPipeSurface")); };
	
public:
	Matrix3d rotationMatrix;
	double surf(const Vector3d &point) const;
	virtual void Update();
	double* GetCenter();
	double* GetCenterAbs();
	static char ** GetIcon();
	int DeepCopy(mafNode *a);
	mafVME *GetCenterVME();
	mmaMaterial *GetMaterial();
	mafVMECylinder();
	virtual ~mafVMECylinder();
	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);
	//mafVMECylinder(double x0, double y0, double z0, double a, double b, double c);
//	vtkSmartPointer<vtkContentActor> makeRenderer();
	void describe();
	static bool VMEAccept(mafNode *node) { return(node != NULL && node->IsMAFType(mafVME)); };
	//vtkPolyData* getOutput();
	vtkTransformPolyDataFilter* getTransformPDF();
	//wxXmlNode* getNode();
	void setResolution(double p);
	void setCenter(double, double, double);
	void setSize(double,double,double);
	double getSzX();
	double getSzY();
	double getSzZ();
	int GetCylinderAxis() { return m_CylinderOrientationAxis; };
};


#endif /* Cylinder_hpp */
