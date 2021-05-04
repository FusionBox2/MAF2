//
//  Cylinder.cpp

#include "mafDefines.h" 
#include "mafVMECylinder.h"
#include <vtkTransform.h>
#include <vtkCylinderSource.h>
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
mafCxxTypeMacro(mafVMECylinder);
mafVMECylinder::mafVMECylinder()
{

	m_LandmarkName = _R("");
	center_vme = NULL;
	res = 10;
	m_CylinderOrientationAxis = 1;
	this->center = Vector3d(0, 0, 0);

	this->a = 10;
	this->b = 10;
	this->h = 10;

	this->rotationMatrix = Matrix3d::Identity();
	
	this->name = _R("cylinder");

	mafNEW(m_Transform);
	mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);
	GetMaterial();
	vtkNEW(m_PolyData);
	
	vtkMAFSmartPointer<vtkCylinderSource> surf;
	surf->SetRadius(a);
	surf->SetResolution(res);
	
	surf->Update();



	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(1, 1, 1);
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



mafVMECylinder::~mafVMECylinder()
{
	
	vtkDEL(m_PolyData);
	mafDEL(m_Transform);
	SetOutput(NULL);

}

int mafVMECylinder::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{	
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
		if (
		//	parent->StoreObject("centervme", center_vme)&&
			parent->StoreText(_R("name"), name) == MAF_OK &&
			parent->StoreText(_R("landmarkName"), m_LandmarkName) == MAF_OK &&
			parent->StoreDouble(_R("Centerx"), center(0)) == MAF_OK &&
			parent->StoreDouble(_R("Centery"), center(1)) == MAF_OK &&
			parent->StoreDouble(_R("Centerz"), center(2)) == MAF_OK &&
			parent->StoreDouble(_R("a"), a) == MAF_OK &&
			parent->StoreDouble(_R("b"), b) == MAF_OK &&
			parent->StoreDouble(_R("h"), h) == MAF_OK &&
			parent->StoreInteger(_R("CylinderOrientationAxis"), m_CylinderOrientationAxis) == MAF_OK &&
			parent->StoreDouble(_R("res"), res) == MAF_OK &&
			parent->StoreMatrix(_R("Transform"), &m_Transform->GetMatrix()) == MAF_OK

			)
		{
		
			return MAF_OK;
		}
	}
	return MAF_ERROR;
}

int mafVMECylinder::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	
	if (Superclass::InternalRestore(node) == MAF_OK)
	{
	
		mafMatrix matrix;
		if (	
			//node->RestoreObject("centervme", center_vme) == MAF_OK &&
			node->RestoreText(_R("name"), name) == MAF_OK &&
			node->RestoreText(_R("landmarkName"), m_LandmarkName) == MAF_OK &&
			node->RestoreDouble(_R("Centerx"), center(0)) == MAF_OK &&
			node->RestoreDouble(_R("Centery"), center(1)) == MAF_OK &&
			node->RestoreDouble(_R("Centerz"), center(2)) == MAF_OK &&
			node->RestoreDouble(_R("a"), a) == MAF_OK &&
			node->RestoreDouble(_R("b"), b) == MAF_OK &&
			node->RestoreDouble(_R("h"), h) == MAF_OK &&
			node->RestoreInteger(_R("CylinderOrientationAxis"), m_CylinderOrientationAxis) == MAF_OK
			)

		{ 
		
		if(
			node->RestoreDouble(_R("res"), res) == MAF_OK &&
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
void mafVMECylinder::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{


	// events to be sent up or down in the tree are simply forwarded
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{			
		case CHANGE_VALUE_Cylinder:
		{

			InternalUpdate();
			e->SetId(CAMERA_UPDATE);
			ForwardUpEvent(e);
									 
		}
			break;
		case ID_CYLINDERCenter_LINK:
		{
				  mafID button_id = e->GetId();
				  mafString title = _L("Choose cylinder center vme link");
				  e->SetId(VME_CHOOSE);
								  
				  e->SetString(&title);
				  ForwardUpEvent(e);
				  mafNode *n = e->GetVme();
				  if (n != NULL)
					  {

					  mafTimeStamp currTs = GetTimeStamp();
					  mafTransform *m_TmpTransform;
					  mafNEW(m_TmpTransform);
					  
					  SetCenterLink(_R("centerLandmark"), n);
					  m_LandmarkName = n->GetName();
								  
					  center_vme = GetCenterVME();
					   double r[3]; double centerLocal[3];
					   center_vme->GetOutput()->GetAbsPose(centerAbs, r, currTs);
				
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
mafVME *mafVMECylinder::GetCenterVME()
//-------------------------------------------------------------------------
{

	center_vme = mafVME::SafeDownCast(GetLink(_R("centerLandmark")));
	//string s = center_vme->GetName();
	//wxBusyInfo wait12(s.c_str());
	//Sleep(1500);
	return center_vme;
}
mafGUI* mafVMECylinder::CreateGui()
//-------------------------------------------------------------------------
{
	
	m_Gui = mafVME::CreateGui();
	m_Gui->Label(_R("Cylinder Gui"));
//	m_Gui->Double(CHANGE_VALUE_Cylinder, _("CenterX"), &center(0));
//	m_Gui->Double(CHANGE_VALUE_Cylinder, _("CenterY"), &center(1));
//	m_Gui->Double(CHANGE_VALUE_Cylinder, _("CenterZ"), &center(2));
	m_Gui->Double(CHANGE_VALUE_Cylinder, _L("RX"), &a);
//	m_Gui->Double(CHANGE_VALUE_Cylinder, _("RY"), &b);
	m_Gui->Double(CHANGE_VALUE_Cylinder, _L("H"), &h);
	//mafString orientationArray[3] = { _("X axis"), _("Y axis"), _("Z axis") };
	//m_Gui->Radio(CHANGE_VALUE_Cylinder, "Orientation", &m_CylinderOrientationAxis, 3, orientationArray);
	m_Gui->Divider();
	m_Gui->Button(ID_CYLINDERCenter_LINK, &m_LandmarkName, _L("centerLandmark"), _L("select the center"));
	m_Gui->FitGui();
	m_Gui->Update();
	return m_Gui;
}


mafVMEOutputSurface *mafVMECylinder::GetSurfaceOutput()
//-------------------------------------------------------------------------
{

	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMECylinder::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{

	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMECylinder::IsAnimated()
//-------------------------------------------------------------------------
{

	return false;
}
//-------------------------------------------------------------------------
void mafVMECylinder::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{

	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
void mafVMECylinder::describe()
{

	cout << "mafVMECylinder" << endl;
}
void mafVMECylinder::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{

	Superclass::SetTimeStamp(t);

	this->InternalUpdate();
	if (m_Gui)
		m_Gui->Update();
}


// private methods

double mafVMECylinder::surf(const Vector3d &point) const
{
	//wxBusyInfo wait12("surf");
	//Sleep(1);


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
		((aa[1] * aa[1]) / (b * b)) 
		- 1;

	return dist;
}

RowVector3d mafVMECylinder::grad(const Vector3d &point) const
{
	//wxBusyInfo wait12("grad");
	//Sleep(1);

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
	gradientLoc(2) = 0;

	gradient2 = gradientLoc*rotationMatrix.transpose();
	return gradient2;
}




Matrix3d mafVMECylinder::hess(const Vector3d &point) const
{
	//wxBusyInfo wait12("hess");
	//Sleep(1);
	double x = point(0);
	double y = point(1);
	double z = point(2);
	double x0 = center(0);
	double y0 = center(1);
	double z0 = center(2);
	RowVector3d xyz_loc;
	xyz_loc << x - x0, y - y0, z - z0;

	Matrix3d hess;
	hess.setZero();
	hess(0, 0) = 2 / (a*a);
	hess(1, 1) = 2 / (b*b);
	hess(2, 2) = 0;

	//introducing rotation
	Matrix3d hess2;
	

	hess2 = rotationMatrix*hess*rotationMatrix.transpose();
	return hess2;
}

double mafVMECylinder::getSzX()
{

	return this->a;
}

double mafVMECylinder::getSzY()
{

	return this->b;
}

double mafVMECylinder::getSzZ()
{

	return this->h;
}


void mafVMECylinder::UpdateLinks()
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
void mafVMECylinder::Update()
{
	InternalUpdate();

}
void mafVMECylinder::InternalUpdate()
{
	UpdateLinks();
	b = a;


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

	vtkMAFSmartPointer<vtkCylinderSource> surf;


	m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	m_TmpTransform->Invert();
	m_TmpTransform->TransformPoint(centerAbs, centerLocal);


	surf->SetCenter(centerLocal[0], centerLocal[1], centerLocal[2]);



	surf->SetRadius(a);
	surf->SetResolution(res);
	surf->SetHeight(h);
	surf->SetCapping(false);
	surf->Update();
	

	/*vtkMAFSmartPointer<vtkTransform> t;
	t->Translate(-centerLocal[0], -centerLocal[1], -centerLocal[2]);
	t->PostMultiply();

	switch (m_CylinderOrientationAxis)
	{
	case 0:
		t->RotateZ(90);
		break;
	case 1:
		//do nothing
		break;
	case 2:
		t->RotateX(-90);
		break;
	default:
		break;
	}


	t->Translate(centerLocal[0], centerLocal[1], centerLocal[2]);
	t->Update();

	

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();*/


	//m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->DeepCopy(surf->GetOutput());
	m_PolyData->Update();

	if (currTs==0)
	{	double xyz[3];
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


//	string s = std::to_string(r[0]) + " "+std::to_string(r[1]) + " "+std::to_string(r[2]);
//		wxBusyInfo wait12(s.c_str());
//	Sleep(1500);
	}	
	
}
int mafVMECylinder::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{


	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		mafVMECylinder *vmeCylinder = mafVMECylinder::SafeDownCast(a);
		m_Transform->SetMatrix(vmeCylinder->m_Transform->GetMatrix());
		this->a = vmeCylinder->a;
		this->b = vmeCylinder->b;
		this->h = vmeCylinder->h;
		

		this->center(0) = vmeCylinder->center(0);
		this->center(1) = vmeCylinder->center(1);
		this->center(2) = vmeCylinder->center(2);

		this->m_LandmarkName = vmeCylinder->m_LandmarkName;

		this->center_vme = vmeCylinder->center_vme;
		this->m_CylinderOrientationAxis = vmeCylinder->m_CylinderOrientationAxis;
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

double* mafVMECylinder::GetCenterAbs()
{

	return centerAbs;

}
double* mafVMECylinder::GetCenter()
{

	double* centerT=new double[3];

	centerT[0] = center(0); centerT[01] = center(01); centerT[02] = center(02);
	return centerT;

}
bool mafVMECylinder::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{

	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (
			m_Transform->GetMatrix() == ((mafVMECylinder *)vme)->m_Transform->GetMatrix() &&
			this->a == ((mafVMECylinder *)vme)->a &&
			this->b == ((mafVMECylinder *)vme)->b &&
			this->h == ((mafVMECylinder *)vme)->h &&
			this->m_CylinderOrientationAxis == ((mafVMECylinder *)vme)->m_CylinderOrientationAxis &&
			this->center(0) == ((mafVMECylinder *)vme)->center(0) &&
			this->center(1) == ((mafVMECylinder *)vme)->center(1) &&
			this->center(2) == ((mafVMECylinder *)vme)->center(2)
			)
		{
			ret = true;
		}
	}
	return ret;
}
vtkTransformPolyDataFilter* mafVMECylinder::getTransformPDF()
{

	vtkMAFSmartPointer<vtkCylinderSource> surf;
	surf->SetCenter(center(0), center(1), center(2));
	surf->SetRadius(a);
	surf->SetResolution(res);
	surf->SetHeight(h);
	surf->Update();

	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(1, 1,1);
	t->Update();
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf->GetOutput());
	ptf->Update();


	return ptf;
}
void mafVMECylinder::setResolution(double p)
{
	res = p;
	
}
void mafVMECylinder::setCenter(double x, double y, double z)
{

	this->center = Vector3d(x, y, z);
}

void mafVMECylinder::setSize(double a, double b ,double h)
{

	this->a = a;
	this->b = b;
	this->h = h;
}
mmaMaterial *mafVMECylinder::GetMaterial()
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

char** mafVMECylinder::GetIcon()
//-------------------------------------------------------------------------
{

#include "mafVMEProcedural.xpm"
	return mafVMEProcedural_xpm;
}

void mafVMECylinder::SetCenterLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{

	if (n->IsMAFType(mafVMELandmark))
	{
		SetLink(link_name, n->GetParent(), ((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
	
	}
	else
		SetLink(link_name, n);
}

