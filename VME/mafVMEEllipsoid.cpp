//
//  Ellipsoid.cpp

#include "mafDefines.h" 
#include "mafVMEEllipsoid.h"
#include <vtkTransform.h>
#include <vtkSphereSource.h>
#include <vtkMAFSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafDataPipeCustom.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "wx/busyinfo.h"
#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafEventSender.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"

#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>
//#include "../DataElementInfoPanel.hpp"
//#include <wx/xml/xml.h>

// public methods

// constructor
mafCxxTypeMacro(mafVMEEllipsoid);
mafVMEEllipsoid::mafVMEEllipsoid()
{

	m_LandmarkName = _R("");
	center_vme = NULL;
	resPhi = 10;
	resTheta = 10;
	this->center = Vector3d(0, 0, 0);

	this->a = 10;
	this->b = 10;
	this->c = 10;

	this->rotationMatrix = Matrix3d::Identity();
	
	this->name = _R("ellipsoid");

	mafNEW(m_Transform);
	mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);
	GetMaterial();
	vtkNEW(m_PolyData);
	
	vtkMAFSmartPointer<vtkSphereSource> surf;
	surf->SetRadius(b);
	surf->SetPhiResolution(resPhi);
	surf->SetThetaResolution(resTheta);
	surf->Update();




	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(a / b, 1, c / b);
	t->Update();
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();

	DependsOnLinkedNodeOn();
	// attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}



mafVMEEllipsoid::~mafVMEEllipsoid()
{
	
	vtkDEL(m_PolyData);
	mafDEL(m_Transform);
	SetOutput(NULL);

}

int mafVMEEllipsoid::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{

	
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
			if(
			//parent->StoreObject("centervme", center_vme)&&
			parent->StoreText(_R("name"), name) == MAF_OK &&
			parent->StoreText(_R("landmarkName"),m_LandmarkName) == MAF_OK &&
			parent->StoreDouble(_R("Centerx"), center(0)) == MAF_OK &&
			parent->StoreDouble(_R("Centery"), center(1)) == MAF_OK &&
			parent->StoreDouble(_R("Centerz"), center(2)) == MAF_OK &&
			parent->StoreDouble(_R("a"), a) == MAF_OK &&
			parent->StoreDouble(_R("b"), b) == MAF_OK &&
			parent->StoreDouble(_R("c"), c) == MAF_OK &&
			parent->StoreDouble(_R("Theta"), resTheta) == MAF_OK &&
			parent->StoreDouble(_R("Phi"), resPhi) == MAF_OK &&
			parent->StoreMatrix(_R("Transform"), &m_Transform->GetMatrix()) == MAF_OK
			
		)
		return MAF_OK;
	}
	return MAF_ERROR;
}

int mafVMEEllipsoid::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{

	if (Superclass::InternalRestore(node) == MAF_OK)
	{
		
		mafMatrix matrix;
		if (
			
			node->RestoreText(_R("name"), name) == MAF_OK &&
			node->RestoreText(_R("landmarkName"), m_LandmarkName) == MAF_OK &&
			node->RestoreDouble(_R("Centerx"), center(0)) == MAF_OK &&
			node->RestoreDouble(_R("Centery"), center(1)) == MAF_OK &&
			node->RestoreDouble(_R("Centerz"), center(2)) == MAF_OK &&
			node->RestoreDouble(_R("a"), a) == MAF_OK &&
			node->RestoreDouble(_R("b"), b) == MAF_OK &&
			node->RestoreDouble(_R("c"), c) == MAF_OK) //&&

		{ 
			if(node->RestoreDouble(_R("Theta"), resTheta) == MAF_OK &&
				node->RestoreDouble(_R("Phi"), resPhi) == MAF_OK &&
				node->RestoreMatrix(_R("Transform"), &matrix) == MAF_OK 
     		 )
			{
			m_Transform->SetMatrix(matrix);
			return MAF_OK;
			}
		}
	}
	return MAF_ERROR;
}
void mafVMEEllipsoid::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{


	// events to be sent up or down in the tree are simply forwarded
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{


		case CHANGE_VALUE_Ellipsoid:
		{

			InternalUpdate();
			e->SetId(CAMERA_UPDATE);
			ForwardUpEvent(e);
									 
		}
			break;
		case ID_ELLIPSOIDCenter_LINK:
		{
				  mafID button_id = e->GetId();
				  mafString title = _L("Choose ellipsoid center vme link");
				  e->SetId(VME_CHOOSE);
								  
				  e->SetString(&title);
				  ForwardUpEvent(e);
				  mafNode *n = e->GetVme();
				  if (n != NULL)
					  {

					  mafTimeStamp currTs = GetTimeStamp();
					  mafTransform *m_TmpTransform;
					  mafNEW(m_TmpTransform);
					  mafMatrix tm;
					 
					
				    
					  SetCenterLink(_R("centerLandmark"), n);
					  m_LandmarkName = n->GetName();
								  
					  center_vme = GetCenterVME();
					   double r[3]; double centerLocal[3];
					   center_vme->GetOutput()->GetAbsPose(centerAbs, r, currTs);

					   center_vme->GetOutput()->GetAbsMatrix(tm, currTs);
					   m_TmpTransform->SetMatrix(tm);
					   m_TmpTransform->TransformPoint(centerAbs, centerAbs);
				
					  center(0) = centerAbs[0];
					  center(1) = centerAbs[1];
					  center(2) = centerAbs[2];
					  
				
					  


					  m_Gui->Update();
					  InternalUpdate();
					  }
								  
		}
			break;

		default:
			mafVME::OnEvent(maf_event);
		}
	}

	else
	{
		Superclass::OnEvent(maf_event);
	}
}
mafVME *mafVMEEllipsoid::GetCenterVME()
//-------------------------------------------------------------------------
{

	center_vme = mafVME::SafeDownCast(GetLink(_R("centerLandmark")));
	//string s = center_vme->GetName();
	//wxBusyInfo wait12(s.c_str());
	//Sleep(1500);
	return center_vme;
}
mafGUI* mafVMEEllipsoid::CreateGui()
//-------------------------------------------------------------------------
{
	
	m_Gui = mafVME::CreateGui();
	m_Gui->Label(_R("Ellipsoid Gui"));
	//m_Gui->Double(CHANGE_VALUE_Ellipsoid, _("CenterX"), &center(0));
//	m_Gui->Double(CHANGE_VALUE_Ellipsoid, _("CenterY"), &center(1));
//	m_Gui->Double(CHANGE_VALUE_Ellipsoid, _("CenterZ"), &center(2));
	m_Gui->Double(CHANGE_VALUE_Ellipsoid, _L("RX"), &a);
	m_Gui->Double(CHANGE_VALUE_Ellipsoid, _L("RY"), &b);
	m_Gui->Double(CHANGE_VALUE_Ellipsoid, _L("RZ"), &c);

	m_Gui->Double(CHANGE_VALUE_Ellipsoid, _L("Theta"), &resTheta);
	m_Gui->Double(CHANGE_VALUE_Ellipsoid, _L("Phi"), &resPhi);
	m_Gui->Divider();
	m_Gui->Button(ID_ELLIPSOIDCenter_LINK, &m_LandmarkName, _L("centerLandmark"), _L("select the center"));
	m_Gui->FitGui();
	m_Gui->Update();
	return m_Gui;
}


mafVMEOutputSurface *mafVMEEllipsoid::GetSurfaceOutput()
//-------------------------------------------------------------------------
{

	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEEllipsoid::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{

	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMEEllipsoid::IsAnimated()
//-------------------------------------------------------------------------
{

	return false;
}
//-------------------------------------------------------------------------
void mafVMEEllipsoid::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{

	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
void mafVMEEllipsoid::describe()
{

	cout << "mafVMEEllipsoid" << endl;
}
void mafVMEEllipsoid::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{

	Superclass::SetTimeStamp(t);

	this->InternalUpdate();
	if (m_Gui)
		m_Gui->Update();
}


// private methods

double mafVMEEllipsoid::surf(const Vector3d &point) const
{
	double x = point(0);
	double y = point(1);
	double z = point(2);

	double x0 = center(0);
	double y0 = center(1);
	double z0 = center(2);

	/*double dist = (((x - x0) * (x - x0)) / (a * a)) +
		(((y - y0) * (y - y0)) / (b * b)) +
		(((z - z0) * (z - z0)) / (c * c))
		- 1;*/
	//introducing rotation
	RowVector3d xyz_loc;
	xyz_loc << x - x0, y - y0, z - z0;
	Vector3d aa=xyz_loc*rotationMatrix;
	


	double dist = ((aa[0] * aa[0]) / (a * a)) +
		((aa[1] * aa[1]) / (b * b)) +
		((aa[2] * aa[2]) / (c * c))
		- 1;

	return dist;
}

RowVector3d mafVMEEllipsoid::grad(const Vector3d &point) const
{
	//RowVector3d gradient;
	RowVector3d gradient2;
	RowVector3d gradientLoc;
	double x = point(0);
	double y = point(1);
	double z = point(2);

	double x0 = center(0);
	double y0 = center(1);
	double z0 = center(2);

//	gradient(0) = 2 * (x - x0) / (a*a);
//	gradient(1) = 2 * (y - y0) / (b*b);
//	gradient(2) = 2 * (z - z0) / (c*c);

	//introducing Rotation
	RowVector3d xyz_loc;
	xyz_loc << x - x0, y - y0, z - z0;
	Vector3d aa = xyz_loc*rotationMatrix;

	gradientLoc(0) = 2 * aa(0) / (a*a);
	gradientLoc(1) = 2 * aa(1) / (b*b);
	gradientLoc(2) = 2 * aa(2) / (c*c);

	gradient2 = gradientLoc*rotationMatrix.transpose();
	return gradient2;
}




Matrix3d mafVMEEllipsoid::hess(const Vector3d &point) const
{
	Matrix3d hess;
	hess.setZero();
	hess(0, 0) = 2 / (a*a);
	hess(1, 1) = 2 / (b*b);
	hess(2, 2) = 2 / (c*c);

	//introducing rotation
	Matrix3d hess2;
	

	hess2 = rotationMatrix*hess*rotationMatrix.transpose();
	return hess2;
}

double mafVMEEllipsoid::getSzX()
{

	return this->a;
}

double mafVMEEllipsoid::getSzY()
{

	return this->b;
}

double mafVMEEllipsoid::getSzZ()
{

	return this->c;
}


void mafVMEEllipsoid::UpdateLinks()
//-------------------------------------------------------------------------
{

	mafID sub_id = -1;
	center_vme = GetCenterVME();
	

	if (center_vme && center_vme->IsMAFType(mafVMELandmarkCloud))
	{
		sub_id = GetLinkSubId(_R("centerLandmark"));
		m_LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)center_vme)->GetLandmarkName(sub_id) : _L("none");
	}
	else
	{
		m_LandmarkName = center_vme ? center_vme->GetName() : _L("none");
	
	}

}
void mafVMEEllipsoid::Update()
{
	InternalUpdate();

}
void mafVMEEllipsoid::InternalUpdate()
{



	UpdateLinks();
	mafTimeStamp currTs = GetTimeStamp();

	double r[3] = { 1, 1, 1 };
	double centerLocal[3];
	center_vme = GetCenterVME();
	mafTransform *m_TmpTransform;
	mafNEW(m_TmpTransform);
	if (center_vme)
	{
		if (center_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("centerLandmark")) != -1)
		{
			((mafVMELandmarkCloud *)center_vme)->GetLandmark(GetLinkSubId(_R("centerLandmark")), centerAbs, currTs);

			//
			mafMatrix tm;
			center_vme->GetOutput()->GetAbsMatrix(tm, currTs);
			m_TmpTransform->SetMatrix(tm);
			m_TmpTransform->TransformPoint(centerAbs, centerAbs);

			//
			center(0) = centerAbs[0];
			center(1) = centerAbs[1];
			center(2) = centerAbs[2];


		}
		else
		{
			center_vme->GetOutput()->Update();
			center_vme->GetOutput()->GetAbsPose(centerAbs, r, currTs);
			center(0) = centerAbs[0];
			center(1) = centerAbs[1];
			center(2) = centerAbs[2];


		}

	}

	vtkMAFSmartPointer<vtkSphereSource> surf;


	m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	m_TmpTransform->Invert();
	m_TmpTransform->TransformPoint(centerAbs, centerLocal);
	surf->SetCenter(centerLocal[0] * (b / a), centerLocal[1] * 1, centerLocal[2] * (b / c));
	//surf->SetCenter(centerLocal[0] , centerLocal[1] , centerLocal[2] );
	//surf->SetCenter(center(0)*(b/a),center(1)*1,center(2)*(b/c));
	surf->SetRadius(b);
	surf->SetPhiResolution(resPhi);
	surf->SetThetaResolution(resTheta);
	surf->Update();

	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(a / b, 1, c / b);
	t->Update();
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();


	if (currTs == 0)
	{
		double xyz[3];
		this->GetOutput()->GetAbsPose(xyz, r, currTs);
		Eigen::Matrix3d r1, r2, r3;
		double rRad[3];
		rRad[0] = r[0] * (3.14159 / 180);
		rRad[1] = r[1] * (3.14159 / 180);
		rRad[2] = r[2] * (3.14159 / 180);
		r1 << 1, 0, 0, 0, cos(rRad[0]), sin(rRad[0]), 0, -sin(rRad[0]), cos(rRad[0]);
		r2 << cos(rRad[1]), 0, -sin(rRad[1]), 0, 1, 0, sin(rRad[1]), 0, cos(rRad[1]);
		r3 << cos(rRad[2]), sin(rRad[2]), 0, -sin(rRad[2]), cos(rRad[2]), 0, 0, 0, 1;
		rotationMatrix = r3*r2*r1;
	}
	
	
}
int mafVMEEllipsoid::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{

	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		mafVMEEllipsoid *vmeEllipsoid = mafVMEEllipsoid::SafeDownCast(a);
		m_Transform->SetMatrix(vmeEllipsoid->m_Transform->GetMatrix());
		this->a = vmeEllipsoid->a;
		this->b = vmeEllipsoid->b;
		this->c = vmeEllipsoid->c;
		

		this->center(0) = vmeEllipsoid->center(0);
		this->center(1) = vmeEllipsoid->center(1);
		this->center(2) = vmeEllipsoid->center(2);

		this->m_LandmarkName = vmeEllipsoid->m_LandmarkName;

		this->center_vme = vmeEllipsoid->center_vme;

		mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
		if (dpipe)
		{
			dpipe->SetInput(m_PolyData);
		}
		InternalUpdate();
		return MAF_OK;
	}
	return MAF_ERROR;

}

double* mafVMEEllipsoid::GetCenterAbs()
{

	return centerAbs;

}
double* mafVMEEllipsoid::GetCenter()
{

	double* centerT=new double[3];

	centerT[0] = center(0); centerT[01] = center(01); centerT[02] = center(02);
	return centerT;

}
bool mafVMEEllipsoid::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{

	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (
			m_Transform->GetMatrix() == ((mafVMEEllipsoid *)vme)->m_Transform->GetMatrix() &&
			this->a == ((mafVMEEllipsoid *)vme)->a &&
			this->b == ((mafVMEEllipsoid *)vme)->b &&
			this->c == ((mafVMEEllipsoid *)vme)->c &&
			this->center(0) == ((mafVMEEllipsoid *)vme)->center(0) &&
			this->center(1) == ((mafVMEEllipsoid *)vme)->center(1) &&
			this->center(2) == ((mafVMEEllipsoid *)vme)->center(2)
			)
		{
			ret = true;
		}
	}
	return ret;
}
vtkTransformPolyDataFilter* mafVMEEllipsoid::getTransformPDF()
{

	vtkMAFSmartPointer<vtkSphereSource> surf;
	surf->SetCenter(center(0), center(1), center(2));
	surf->SetRadius(a);
	surf->SetPhiResolution(resPhi);
	surf->SetThetaResolution(resTheta);
	surf->Update();

	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(a / b, 1, c / b);
	t->Update();
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();


	return ptf;
}
void mafVMEEllipsoid::setResolution(double p, double t)
{
	resPhi = p;
	resTheta = t;
}
void mafVMEEllipsoid::setCenter(double x, double y, double z)
{

	this->center = Vector3d(x, y, z);
}

void mafVMEEllipsoid::setSize(double a, double b, double c)
{

	this->a = a;
	this->b = b;
	this->c = c;
}
mmaMaterial *mafVMEEllipsoid::GetMaterial()
//-------------------------------------------------------------------------
{

	mmaMaterial *material = (mmaMaterial *)GetAttribute(_R("MaterialAttributes"));
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute(_R("MaterialAttributes"), material);
	}
	return material;
}

char** mafVMEEllipsoid::GetIcon()
//-------------------------------------------------------------------------
{

#include "mafVMEProcedural.xpm"
	return mafVMEProcedural_xpm;
}

void mafVMEEllipsoid::SetCenterLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{

	if (n->IsMAFType(mafVMELandmark))
	{
		SetLink(link_name, n->GetParent(), ((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
	
	}
	else
		SetLink(link_name, n);
}

