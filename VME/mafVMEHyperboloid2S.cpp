//
//  Hyperboloid2S.cpp

#include "mafDefines.h" 
#include "mafVMEHyperboloid2S.h"
#include <vtkSampleFunction.h>
#include <vtkContourFilter.h>
#include <vtkOutlineFilter.h>

#include <vtkTransform.h>
#include <vtkQuadric.h>
//#include <vtkSphereSource.h>
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
#include "vtkClipVolume.h"
#include "vtkClipPolyData.h"
#include "vtkCutter.h"
#include <vector>
//#include "../DataElementInfoPanel.hpp"
//#include <wx/xml/xml.h>

// public methods

// constructor
mafCxxTypeMacro(mafVMEHyperboloid2S);
mafVMEHyperboloid2S::mafVMEHyperboloid2S()
{

	m_LandmarkName = _R("");
	center_vme = NULL;
	resPhi = -1;
	resTheta = 10;
	this->center = Vector3d(0, 0, 0);

	this->a = 10;
	this->b = 10;
	this->c = 10;
	this->center = Vector3d(0, 0, 0);
	this->rotationMatrix = Matrix3d::Identity();

	this->name = _R("TwoSheetedHyperboloid");

	mafNEW(m_Transform);
	mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);
	GetMaterial();
	vtkNEW(m_PolyData);
	
	/*vtkMAFSmartPointer<vtkSphereSource> surf;
	surf->SetRadius(b);
	surf->SetPhiResolution(resPhi);
	surf->SetThetaResolution(resTheta);
	surf->Update();*/

	quadric = vtkQuadric::New();
	



	vtkPolyData* surf = plotFunction(quadric, resPhi);
	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(1, 1, 1);
	t->Update();

	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf);
	ptf->Update();
	
	


	//m_PolyData->DeepCopy(ptf->GetOutput());
	//m_PolyData->Update();

	DependsOnLinkedNodeOn();
	// attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}



mafVMEHyperboloid2S::~mafVMEHyperboloid2S()
{

	vtkDEL(m_PolyData);
	mafDEL(m_Transform);
	SetOutput(NULL);

}

int mafVMEHyperboloid2S::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{

	
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
			if(
			//parent->StoreObject("centervme", center_vme)&&
			parent->StoreText(_R("name"), name) == MAF_OK &&
			parent->StoreText(_R("landmarkName"),m_LandmarkName) == MAF_OK &&
			parent->StoreDouble(_R("Centerx"), center[0]) == MAF_OK &&
			parent->StoreDouble(_R("Centery"), center[1]) == MAF_OK &&
			parent->StoreDouble(_R("Centerz"), center[2]) == MAF_OK &&
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

int mafVMEHyperboloid2S::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{

	if (Superclass::InternalRestore(node) == MAF_OK)
	{
		
		mafMatrix matrix;
		if (
			//node->RestoreObject("centervme", center_vme)&&
			node->RestoreText(_R("name"), name) == MAF_OK &&
			node->RestoreText(_R("landmarkName"), m_LandmarkName) == MAF_OK &&
			node->RestoreDouble(_R("Centerx"), center[0]) == MAF_OK &&
			node->RestoreDouble(_R("Centery"), center[1]) == MAF_OK &&
			node->RestoreDouble(_R("Centerz"), center[2]) == MAF_OK &&
			node->RestoreDouble(_R("a"), a) == MAF_OK &&
			node->RestoreDouble(_R("b"), b) == MAF_OK &&
			node->RestoreDouble(_R("c"), c) == MAF_OK) //&&

		{ 
		if( node->RestoreDouble(_R("Theta"), resTheta) == MAF_OK &&
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
void mafVMEHyperboloid2S::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{

	
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{


		case CHANGE_VALUE_Hyperboloid:
		{

			InternalUpdate();
			e->SetId(CAMERA_UPDATE);
			ForwardUpEvent(e);
									 
		}
			break;
		case ID_HyperboloidCenter_LINK:
		{
				  mafID button_id = e->GetId();
				  mafString title = _L("Choose Hyperboloid center vme link");
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

					  initvme = 1;

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
mafVME *mafVMEHyperboloid2S::GetCenterVME()
//-------------------------------------------------------------------------
{

	center_vme = mafVME::SafeDownCast(GetLink(_R("centerLandmark")));
	return center_vme;
}
mafGUI* mafVMEHyperboloid2S::CreateGui()
//-------------------------------------------------------------------------
{

	m_Gui = mafVME::CreateGui();
	m_Gui->Label(_R("Hyperboloid Gui"));
	//m_Gui->Double(CHANGE_VALUE_Hyperboloid, _("CenterX"), &center[0]);
	//m_Gui->Double(CHANGE_VALUE_Hyperboloid, _("CenterY"), &center[1]);
	//m_Gui->Double(CHANGE_VALUE_Hyperboloid, _("CenterZ"), &center[2]);
	m_Gui->Double(CHANGE_VALUE_Hyperboloid, _L("RX"), &a);
	m_Gui->Double(CHANGE_VALUE_Hyperboloid, _L("RY"), &b);
	m_Gui->Double(CHANGE_VALUE_Hyperboloid, _L("RZ"), &c);
	m_Gui->Double(CHANGE_VALUE_Hyperboloid, _L("resolution"), &resTheta);
	m_Gui->Divider();
	m_Gui->Button(ID_HyperboloidCenter_LINK, &m_LandmarkName, _L("centerLandmark"), _L("select the center"));
	m_Gui->FitGui();
	m_Gui->Update();
	return m_Gui;
}


mafVMEOutputSurface *mafVMEHyperboloid2S::GetSurfaceOutput()
//-------------------------------------------------------------------------
{

	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEHyperboloid2S::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{

	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMEHyperboloid2S::IsAnimated()
//-------------------------------------------------------------------------
{

	return false;
}
//-------------------------------------------------------------------------
void mafVMEHyperboloid2S::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{

	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
void mafVMEHyperboloid2S::describe()
{

	cout << "mafVMEHyperboloid2S" << endl;
}
void mafVMEHyperboloid2S::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{

	Superclass::SetTimeStamp(t);
	this->GetOutput()->Update();
	double xyz[3], rxyz[3];
	this->GetOutput()->GetAbsPose(xyz, rxyz);
	centerTemp[0] = std::atof(wxString::Format("x: %f", xyz[0]));
	centerTemp[1] = std::atof(wxString::Format("y: %f", xyz[1]));
	centerTemp[2] = std::atof(wxString::Format("z: %f", xyz[2]));
	

	this->InternalUpdate();
	if (m_Gui)
		m_Gui->Update();
}


// private methods

double mafVMEHyperboloid2S::surf(const Vector3d &point) const
{
	double x = point(0);
	double y = point(1);
	double z = point(2);

	double x0 = center(0);
	double y0 = center(1);
	double z0 = center(2);
	RowVector3d xyz_loc;
	xyz_loc << x - x0, y - y0, z - z0;
	Vector3d aa = xyz_loc*rotationMatrix;


	//double dist = (((x - x0) * (x - x0)) / (a * a)) +
	//	(((y - y0) * (y - y0)) / (b * b)) -
	//	(((z - z0) * (z - z0)) / (c * c))
	//	- 1;

	double dist = ((aa[0] * aa[0]) / (a * a)) +
		((aa[1] * aa[1]) / (b * b)) -
		((aa[2] * aa[2]) / (c * c))
		- 1;
	return dist;
}

RowVector3d mafVMEHyperboloid2S::grad(const Vector3d &point) const
{
	RowVector3d gradient;
	RowVector3d gradient2;
	RowVector3d gradientLoc;
	double x = point(0);
	double y = point(1);
	double z = point(2);

	double x0 = center(0);
	double y0 = center(1);
	double z0 = center(2);

	//gradient(0) = 2 * (x - x0) / (a*a);
	//gradient(1) = 2 * (y - y0) / (b*b);
	//gradient(2) = -2 * (z - z0) / (c*c);


	RowVector3d xyz_loc;
	xyz_loc << x - x0, y - y0, z - z0;
	Vector3d aa = xyz_loc*rotationMatrix;

	gradientLoc(0) = 2 * aa(0) / (a*a);
	gradientLoc(1) = 2 * aa(1) / (b*b);
	gradientLoc(2) = -2 * aa(2) / (c*c);

	gradient2 = gradientLoc*rotationMatrix.transpose();
	return gradient2;
	return gradient2;
}

Matrix3d mafVMEHyperboloid2S::hess(const Vector3d &point) const
{
	Matrix3d hess;
	hess.setZero();
	hess(0, 0) = 2 / (a*a);
	hess(1, 1) = 2 / (b*b);
	hess(2, 2) = -2 / (c*c);
	//return hess;

	Matrix3d hess2;


	hess2 = rotationMatrix*hess*rotationMatrix.transpose();
	return hess2;
}

double mafVMEHyperboloid2S::getSzX()
{

	return this->a;
}

double mafVMEHyperboloid2S::getSzY()
{

	return this->b;
}

double mafVMEHyperboloid2S::getSzZ()
{

	return this->c;
}


void mafVMEHyperboloid2S::UpdateLinks()
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
void mafVMEHyperboloid2S::Update()
{
	InternalUpdate();

}
void mafVMEHyperboloid2S::InternalUpdate()
{
	//wxBusyInfo wait12("internalupdate");
	//Sleep(1500);

	UpdateLinks();
	mafTimeStamp currTs = GetTimeStamp();

	double r[3] = {1,1,1};
	
	center_vme = GetCenterVME();
	mafTransform *m_TmpTransform;
	mafNEW(m_TmpTransform);
	if (center_vme)
	{
		if (center_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("centerLandmark")) != -1)
		{
			((mafVMELandmarkCloud *)center_vme)->GetLandmark(GetLinkSubId(_R("centerLandmark")), centerAbs, currTs);
			mafMatrix tm;
			center_vme->GetOutput()->GetAbsMatrix(tm, currTs);
			m_TmpTransform->SetMatrix(tm);
			m_TmpTransform->TransformPoint(centerAbs, centerAbs);

		

		}
		else
		{
			center_vme->GetOutput()->Update();
			center_vme->GetOutput()->GetAbsPose(centerAbs, r, currTs);
		}
	}
	
	m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	m_TmpTransform->Invert();
	m_TmpTransform->TransformPoint(centerAbs, centerLocal);
	center(0) = centerAbs[0];
	center(1) = centerAbs[01]; 
	center(02) = centerAbs[02];

	vtkPolyData* surf = plotFunction(quadric, resPhi);

	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(1, 1, 1);
	t->Update();
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf);
	ptf->Update();

	m_PolyData->DeepCopy(ptf->GetOutput());
	m_PolyData->Update();
	
	//string ss;
	//ss = "local position "+std::to_string(center[0]) + " " + std::to_string(center[1]) + " " + std::to_string(center[2]);
	//wxBusyInfo wait12(ss.c_str());
	//Sleep(1500);


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
int mafVMEHyperboloid2S::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait12("deepcopy");
	//Sleep(1500);
	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		mafVMEHyperboloid2S *vmeHyperboloid = mafVMEHyperboloid2S::SafeDownCast(a);
		m_Transform->SetMatrix(vmeHyperboloid->m_Transform->GetMatrix());
		this->a = vmeHyperboloid->a;
		this->b = vmeHyperboloid->b;
		this->c = vmeHyperboloid->c;
		

		this->center[0] = vmeHyperboloid->center[0];
		this->center[1] = vmeHyperboloid->center[1];
		this->center[2] = vmeHyperboloid->center[2];

		this->m_LandmarkName = vmeHyperboloid->m_LandmarkName;

		this->center_vme = vmeHyperboloid->center_vme;

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

double* mafVMEHyperboloid2S::GetCenterAbs()
{
	//wxBusyInfo wait12("getcenterAbs");
	//Sleep(1500);
	return centerAbs;

}
double* mafVMEHyperboloid2S::GetCenter()
{
	//wxBusyInfo wait12("getcenter");
	//Sleep(1500);
	double* centerT=new double[3];

	centerT[0] = center[0]; centerT[01] = center[01]; centerT[02] = center[02];
	return centerT;

}
bool mafVMEHyperboloid2S::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait12("equals");
	//Sleep(1500);
	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (
			m_Transform->GetMatrix() == ((mafVMEHyperboloid2S *)vme)->m_Transform->GetMatrix() &&
			this->a == ((mafVMEHyperboloid2S *)vme)->a &&
			this->b == ((mafVMEHyperboloid2S *)vme)->b &&
			this->c == ((mafVMEHyperboloid2S *)vme)->c &&
			this->center[0] == ((mafVMEHyperboloid2S *)vme)->center[0] &&
			this->center[1] == ((mafVMEHyperboloid2S *)vme)->center[1] &&
			this->center[2] == ((mafVMEHyperboloid2S *)vme)->center[2]
			)
		{
			ret = true;
		}
	}
	return ret;
}
vtkTransformPolyDataFilter* mafVMEHyperboloid2S::getTransformPDF()
{
	//wxBusyInfo wait12("gettransformpdf");
	//Sleep(1500);
	



	vtkPolyData* surf = plotFunction(quadric, resPhi);

	vtkMAFSmartPointer<vtkTransform> t;
	t->Scale(1, 1, 1);
	t->Update();
	vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
	ptf->SetTransform(t);
	ptf->SetInput(surf);
	ptf->Update();


	return ptf;
}
void mafVMEHyperboloid2S::setResolution(double p, double t)
{
	resPhi = p;
	resTheta = t;
}
void mafVMEHyperboloid2S::setCenter(double x, double y, double z)
{
	//wxBusyInfo wait12("setcenter");
	//Sleep(1500);
	this->center = Vector3d(x, y, z);
}

void mafVMEHyperboloid2S::setSize(double a, double b, double c)
{
	//wxBusyInfo wait12("setsize");
	//Sleep(1500);
	this->a = a;
	this->b = b;
	this->c = c;
}
mmaMaterial *mafVMEHyperboloid2S::GetMaterial()
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait12("getmaterial");
	//Sleep(1500);
	mmaMaterial *material = (mmaMaterial *)GetAttribute(_R("MaterialAttributes"));
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute(_R("MaterialAttributes"), material);
	}
	return material;
}

char** mafVMEHyperboloid2S::GetIcon()
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait12("geticon");
	//Sleep(1500);
#include "mafVMEProcedural.xpm"
	return mafVMEProcedural_xpm;
}

void mafVMEHyperboloid2S::SetCenterLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait12("setcenterLink");
	//Sleep(1500);
	if (n->IsMAFType(mafVMELandmark))
	{
		SetLink(link_name, n->GetParent(), ((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
	
	}
	else
		SetLink(link_name, n);
}

vtkPolyData* mafVMEHyperboloid2S::plotFunction(vtkQuadric* quadric, double value)
{

	
	quadric->SetCoefficients(1 / (a*a), 1 / (b*b), -1 / (c*c), 0, 0, 0, -(2 * centerLocal[0]) / (a*a), -(2 * centerLocal[1]) / (b*b), (2 * centerLocal[2]) / (c*c), (centerLocal[0] * centerLocal[0]) / (a*a) + (centerLocal[1] * centerLocal[1]) / (b*b) - (centerLocal[2] * centerLocal[2]) / (c*c));

	vtkSampleFunction* sample = vtkSampleFunction::New();
	sample->SetSampleDimensions(50, 50, 50);
	sample->SetImplicitFunction(quadric);
	//double xmin = 0, xmax=1, ymin=0, ymax=1, zmin=0, zmax=1;
	double xmin = -500 + centerLocal[0], xmax = 500 + centerLocal[0], ymin = -500 + centerLocal[1], ymax = 500 + centerLocal[1], zmin = -500 + centerLocal[2], zmax = 500 + centerLocal[2];
	sample->SetModelBounds(xmin, xmax, ymin, ymax, zmin, zmax);

	sample->ComputeNormalsOff();
	vtkContourFilter* contours = vtkContourFilter::New();
	contours->SetInput((vtkDataSet*)sample->GetOutput());
	contours->GenerateValues(1, value, value);



	vtkPolyData* polydata = contours->GetOutput();

	return polydata;


}