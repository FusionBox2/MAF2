//
//  Ellipsoid.hpp


#ifndef __mafVMEEllipsoid_h
#define __mafVMEEllipsoid_h

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
#include "vtkSmartPointer.h"
#include "mafVMELandmarkCloud.h"
class mmaMaterial;
class mafNode;
class mafVMEOutputSurface;
class vtkPolyData;

class MAF_EXPORT mafVMEEllipsoid : public mafQuadraticSurface
{
public:

	mafTypeMacro(mafVMEEllipsoid, mafQuadraticSurface);

protected:

	enum ELLIPSOID_ID
	{
		CHANGE_VALUE_Ellipsoid,
		ID_ELLIPSOIDCenter_LINK
	};

	
private:
	//mafVMELandmarkCloud	*m_Cloud2;
	void InternalUpdate() override;
	void UpdateLinks();
	//int initvme = 0;
	mafVME *center_vme;
	void SetTimeStamp(mafTimeStamp t) override;
	//double centerTemp[3];
	mafString m_LandmarkName;
	void SetCenterLink(const mafString&, mafNode *);
	bool Equals(mafVME*) override;
	double a, b, c;
	
	void OnEvent(mafEventBase *maf_event) override;
	
	RowVector3d grad(const Vector3d &point) const override;
	Matrix3d hess(const Vector3d &point) const override;
	mafTransform *m_Transform;
	vtkPolyData  *m_PolyData;
	//vtkPolyData* polyData;
	double resPhi;
	double resTheta;
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
	std::shared_ptr<mmaMaterial> GetMaterial();
	mafVMEEllipsoid();
	~mafVMEEllipsoid() override;
	void InternalStore(mafStorageElementBuilder& parent) override;
	void InternalRestore(const mafStorageElement& node) override;
	//mafVMEEllipsoid(double x0, double y0, double z0, double a, double b, double c);
//	vtkSmartPointer<vtkContentActor> makeRenderer();
	void describe();
	static bool VMEAccept(mafNode *node) { return(node != NULL && node->IsMAFType(mafVME)); };
	//vtkPolyData* getOutput();
	vtkTransformPolyDataFilter* getTransformPDF();
	//wxXmlNode* getNode();
	void setResolution(double p, double t);
	void setCenter(double, double, double);
	void setSize(double, double,double);
	double getSzX();
	double getSzY();
	double getSzZ();
	
};


#endif /* Ellipsoid_hpp */
