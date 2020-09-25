//
//  Hyperboloid.hpp


#ifndef __mafVMEHyperboloid2S_h
#define __mafVMEHyperboloid2S_h

#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyData.h>
#include "vtkQuadric.h"
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
class mmaMaterial;
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class vtkPolyData;

class MAF_EXPORT mafVMEHyperboloid2S : public mafQuadraticSurface
{
public:

	mafTypeMacro(mafVMEHyperboloid2S, mafQuadraticSurface, mafVME);

protected:

	enum HYPERBOLOID2S_ID
	{
		CHANGE_VALUE_Hyperboloid,
		ID_HyperboloidCenter_LINK
	};

	
private:
	virtual void InternalUpdate();
	void UpdateLinks();
	int initvme = 0;
	mafVME *center_vme;
	void SetTimeStamp(mafTimeStamp t);
	double centerTemp[3];
	mafString m_LandmarkName;
	void SetCenterLink(const char *, mafNode *);
	bool Equals(mafVME*);
	double a, b, c;
	Matrix3d rotationMatrix;
	void OnEvent(mafEventBase *maf_event);
	double surf(const Vector3d &point) const;
	RowVector3d grad(const Vector3d &point) const;
	Matrix3d hess(const Vector3d &point) const;
	mafTransform *m_Transform;
	vtkPolyData  *m_PolyData;
	//vtkPolyData* polyData;
	double resPhi;
	double resTheta;
	virtual mafGUI *CreateGui();
	double centerAbs[3];
	mafVMEOutputSurface *GetSurfaceOutput();

	double centerLocal[3];
	virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);
	virtual void SetMatrix(const mafMatrix &mat);
	virtual bool IsAnimated();
	virtual mafString GetVisualPipe() { return mafString("mafPipeSurface"); };
	
public:
	vtkPolyData* plotFunction(vtkQuadric* quadric, double value);;
	virtual void Update();
	double* GetCenter();
	double* GetCenterAbs();
	static char ** GetIcon();
	int DeepCopy(mafNode *a);
	mafVME *GetCenterVME();
	mmaMaterial *GetMaterial();
	mafVMEHyperboloid2S();
	virtual ~mafVMEHyperboloid2S();
	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);
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
	vtkQuadric* quadric;
	
};


#endif /* Hyperboloid_hpp */
