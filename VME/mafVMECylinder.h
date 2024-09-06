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
#include "vtkSmartPointer.h"
#include "mafVMELandmarkCloud.h"
class mmaMaterial;
class mafNode;

class mafVMEOutputSurface;
class vtkPolyData;

class MAF_EXPORT mafVMECylinder : public mafQuadraticSurface
{
public:

	mafTypeMacro(mafVMECylinder, mafQuadraticSurface);

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
	void InternalUpdate() override;
	void UpdateLinks();
	
	mafVME *center_vme;
	void SetTimeStamp(mafTimeStamp t) override;
	//double centerTemp[3];
	mafString m_LandmarkName;
	void SetCenterLink(const mafString&, mafNode *);
	bool Equals(mafVME*) override;
	double a,b, h;
	
	void OnEvent(mafEventBase *maf_event) override;
	int m_CylinderOrientationAxis;
	RowVector3d grad(const Vector3d &point) const override;
	Matrix3d hess(const Vector3d &point) const override;
	mafTransform *m_Transform;
	vtkPolyData  *m_PolyData;
	
	double res;

	mafGUI *CreateGui() override;
	double centerAbs[3];
	
	mafVMEOutputSurface *GetSurfaceOutput();


	void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;
	void SetMatrix(const mafMatrix &mat) override;
	bool IsAnimated() override;
	mafString GetVisualPipe() override { return mafString(_R("mafPipeSurface")); };
	
public:
	Matrix3d rotationMatrix;
	double surf(const Vector3d &point) const override;
	void Update() override;
	double* GetCenter();
	double* GetCenterAbs();
	static char ** GetIcon();
	int DeepCopy(mafNode *a) override;
	mafVME *GetCenterVME();
	mmaMaterial *GetMaterial();
	mafVMECylinder();
	~mafVMECylinder() override;
	void InternalStore(mafStorageElementBuilder& parent) override;
	void InternalRestore(const mafStorageElement& node) override;
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
