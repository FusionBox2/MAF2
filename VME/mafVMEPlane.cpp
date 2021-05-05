

#include "mafDefines.h" 
#include <string>
#include "wx/busyinfo.h"
#include "mafVMEPlane.h"
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


#include "mafVMELandmarkCloud.h"

#include "vtkPlaneSource.h"

#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"


#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>
using namespace std;
//const bool DEBUG_MODE = true;
//#include "../DataElementInfoPanel.hpp"
//#include <wx/xml/xml.h>

// public methods

// constructor

mafCxxTypeMacro(mafVMEPlane);

mafVMEPlane::mafVMEPlane()
//-------------------------------------------------------------------------
{
	

	
	m_PlaneXRes = 1.0;
	m_PlaneYRes = 5.0;
	m_PlaneOrigin[0] = m_PlaneOrigin[1] = m_PlaneOrigin[2] = 0;
	m_PlanePoint1[0] = 1.0;	m_PlanePoint1[1] = m_PlanePoint1[2] = 1.0;
	m_PlanePoint2[1] = 3.0;	m_PlanePoint2[0] = m_PlanePoint2[2] = 1.0;
	

	pts_change = true;
	double rxyz[3];
	p1 = mafVMELandmark::New();
	p1->SetName(_R("p1"));
	p1->SetAbsPose(m_PlaneOrigin,rxyz,-1);
//	p2 = mafVMELandmark::New();
//	p2->SetName("p2");
//	p2->SetAbsPose(m_PlanePoint1, rxyz, -1);
//	p3 = mafVMELandmark::New();
//	p3->SetName("p3");
//	p3->SetAbsPose(m_PlanePoint2, rxyz, -1);
	 mafNEW(m_Cloud2);

	m_Cloud2->Open();
	m_Cloud2->SetName(_L("Plane points"));
	m_Cloud2->SetRadius(1.5);
	m_Cloud2->ReparentTo(this);
	m_Cloud2->AppendLandmark(m_PlaneOrigin[0], m_PlaneOrigin[1], m_PlaneOrigin[2], _R("p1"), false);
	m_Cloud2->AppendLandmark(m_PlanePoint1[0], m_PlanePoint1[1], m_PlanePoint1[2], _R("p2"), false);
	m_Cloud2->AppendLandmark(m_PlanePoint2[0], m_PlanePoint2[1], m_PlanePoint2[2], _R("p3"), false);
	
	normal[0] = normal[01] = normal[02] = 0;

	m_p1LandmarkName = _R("");
//	m_p2LandmarkName = _R("");
//	m_p3LandmarkName = _R("");
	m_plan1Name = _R("");
	//m_p4LandmarkName = _R("");
	surf = vtkPlaneSource::New();
	//parallelPlane= vtkPlaneSource::New();

	mafNEW(m_Transform);
	mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();
	

	vtkNEW(m_PolyData);

	// attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
mafVMEPlane::~mafVMEPlane()
//-------------------------------------------------------------------------
{
//	vtkDEL(m_LineNormal);
	vtkDEL(m_PolyData);
	mafDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEPlane::GetMaterial()
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

//-------------------------------------------------------------------------
int mafVMEPlane::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		mafVMEPlane *vmePlane = mafVMEPlane::SafeDownCast(a);
		m_Transform->SetMatrix(vmePlane->m_Transform->GetMatrix());
		//this->m_GeometryType = vmePlane->m_GeometryType;

		this->m_PlaneXRes = vmePlane->m_PlaneXRes;
		this->m_PlaneYRes = vmePlane->m_PlaneYRes;

		



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

//-------------------------------------------------------------------------
bool mafVMEPlane::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (
			m_Transform->GetMatrix() == ((mafVMEPlane *)vme)->m_Transform->GetMatrix() &&
			//this->m_GeometryType == ((mafVMEPlane *)vme)->m_GeometryType &&
		
			this->m_PlaneXRes == ((mafVMEPlane *)vme)->m_PlaneXRes  &&
			this->m_PlaneYRes == ((mafVMEPlane *)vme)->m_PlaneYRes  

			
			)
		{
			ret = true;
		}
	}
	return ret;
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMEPlane::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEPlane::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMEPlane::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
void mafVMEPlane::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}

double* mafVMEPlane::getPoint1()
//-------------------------------------------------------------------------
{
	double d[3];
	mafTimeStamp currTs = GetTimeStamp();
	double xyzr[3];
	mafTransform *m_TmpTransform = mafTransform::New();
	mafMatrix tm;
	//((mafVMELandmark *)p1)->GetPoint(d, currTs);

	//	((mafVMELandmarkCloud *)m_Cloud2)->GetOutput()->GetAbsMatrix(tm, currTs);
	
	//p1->GetOutput()->GetAbsPose(d, xyzr, currTs);
	//int idx1 = m_Cloud2->FindLandmarkIndex("p1");;
	//m_Cloud2->GetLandmarkPosition(idx1, d, currTs);
	
	//((mafVMELandmark *)p1)->GetPoint(d, currTs);

	//((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId("p1"), d, currTs);
	//m_TmpTransform->SetMatrix(tm);
	//m_TmpTransform->TransformPoint(m_PlaneOrigin, d);
	//Sleep(1500);
	


	
	mafNEW(m_TmpTransform);
	
	m_Cloud2->GetOutput()->GetAbsMatrix(tm, currTs);
	m_TmpTransform->SetMatrix(tm);

	((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId(_R("p1")), m_PlaneOrigin, currTs);
	m_TmpTransform->TransformPoint(m_PlaneOrigin, d);

	return d;// m_PlaneOrigin;//d;
	
}

void mafVMEPlane::setPoint1(double d[3])
//-------------------------------------------------------------------------
{
	
	m_PlaneOrigin[0] = d[0];
	m_PlaneOrigin[1] = d[1];
	m_PlaneOrigin[2] = d[2];


	double rxyz[3];
	p1->SetAbsPose(m_PlaneOrigin, rxyz, -1);
	p1->Update();
	m_Cloud2->SetLandmark(_R("p1"), m_PlaneOrigin[0], m_PlaneOrigin[1], m_PlaneOrigin[2], -1);
	//pts_change = true;

}
double* mafVMEPlane::getPoint2()
//-------------------------------------------------------------------------
{

	double d[3];
	mafTimeStamp currTs = GetTimeStamp();
	double xyzr[3];

	mafTransform *m_TmpTransform = mafTransform::New();
	mafMatrix tm;


	//p2->GetOutput()->GetAbsPose(m_PlanePoint1, xyzr, currTs);
	//int idx1 = m_Cloud2->FindLandmarkIndex("p2");;
	//m_Cloud2->GetLandmarkPosition(idx1, d, currTs);
//	((mafVMELandmarkCloud *)m_Cloud2)->GetOutput()->GetAbsMatrix(tm, currTs);
	//((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId("p2"), d, currTs);

	//((mafVMELandmark *)p2)->GetPoint(d, currTs);
	//m_TmpTransform->SetMatrix(tm);
	//m_TmpTransform->TransformPoint(m_PlanePoint1, d);

	


	mafNEW(m_TmpTransform);

	m_Cloud2->GetOutput()->GetAbsMatrix(tm, currTs);
	m_TmpTransform->SetMatrix(tm);

	((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId(_R("p2")), m_PlanePoint1, currTs);
	m_TmpTransform->TransformPoint(m_PlanePoint1, d);

	return d;// m_PlanePoint1;// d;
}
void mafVMEPlane::setPoint2(double d[3])
//-------------------------------------------------------------------------
{
	
	m_PlanePoint1[0] = d[0];
	m_PlanePoint1[01] = d[01];
	m_PlanePoint1[02] = d[02];


	double rxyz[3];
	//p2->SetAbsPose(m_PlanePoint1, rxyz, -1);

	m_Cloud2->SetLandmark(_R("p2"), m_PlanePoint1[0], m_PlanePoint1[1], m_PlanePoint1[2], -1);
	//pts_change = true;
}
double* mafVMEPlane::getNormalVector()
//-------------------------------------------------------------------------
{
	computeNormal();
//	surf->GetNormal(normal);
	return normal;
}
double* mafVMEPlane::getPoint3()
//-------------------------------------------------------------------------
{

	double d[3];
	mafTimeStamp currTs = GetTimeStamp();
	double xyzr[3];
	mafTransform *m_TmpTransform = mafTransform::New();

	mafMatrix tm;

	/////
	//p3->GetOutput()->GetAbsPose(m_PlanePoint2, xyzr, currTs);

	//((mafVMELandmark *)p3)->GetPoint(m_PlanePoint2, currTs);
	////
	//p3->GetOutput()->GetAbsPose(d, xyzr, currTs);
	//int idx1 = m_Cloud2->FindLandmarkIndex("p3");;
	//m_Cloud2->GetLandmarkPosition(idx1, d, currTs);


//	((mafVMELandmarkCloud *)m_Cloud2)->GetOutput()->GetAbsMatrix(tm, currTs);
	//((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId("p3"), d, currTs);

	//((mafVMELandmark *)p3)->GetPoint(d, currTs);
	//m_TmpTransform->SetMatrix(tm);
	//m_TmpTransform->TransformPoint(m_PlanePoint2, d);

//	string sss40 = " get P3" + std::to_string(d[0]) + " " + std::to_string(d[1]) + " " + std::to_string(d[2]);
//	wxBusyInfo wait12040s(sss40.c_str());
//	Sleep(2500);
	
	mafNEW(m_TmpTransform);

	m_Cloud2->GetOutput()->GetAbsMatrix(tm, currTs);
	m_TmpTransform->SetMatrix(tm);

	((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId(_R("p3")), m_PlanePoint2, currTs);
	m_TmpTransform->TransformPoint(m_PlanePoint2, d);
	//return m_PlanePoint2;
	return d;
}
void mafVMEPlane::setPoint3(double d[3])
//-------------------------------------------------------------------------
{

	
	m_PlanePoint2[0] = d[0];
	m_PlanePoint2[1] = d[1];
	m_PlanePoint2[2] = d[2];


	double rxyz[3];
//	p3->SetAbsPose(m_PlanePoint2, rxyz, -1);
	//p3->SetPose(m_PlanePoint2, rxyz, -1);
	//p3->Update();
	m_Cloud2->SetLandmark(_R("p3"), m_PlanePoint2[0], m_PlanePoint2[1], m_PlanePoint2[2], -1);
	
	
	//string sss40 =  std::to_string(m_PlanePoint2[0]) + " " + std::to_string(m_PlanePoint2[1]) + " " + std::to_string(m_PlanePoint2[2]);
	
	//wxBusyInfo wait12040s(this->GetName().GetCStr());
	//Sleep(1000);

	//wxBusyInfo wait1204js(sss40.c_str());
	//Sleep(1500);
	
	//pts_change = true;
}
void mafVMEPlane::push(double t)
{

	tr = t;

}
//-------------------------------------------------------------------------
mafGUI* mafVMEPlane::CreateGui()
//-------------------------------------------------------------------------
{
	m_Gui = mafVME::CreateGui();
	m_Gui->Label(_R("Plane Gui"));
	

	m_Gui->Double(CHANGE_VALUE_PLANE, _L("Ux"), &m_PlaneXRes);
	m_Gui->Double(CHANGE_VALUE_PLANE, _L("Uy"), &m_PlaneYRes);
	m_Gui->Button(ID_P1_LINK, &m_p1LandmarkName, _L("first_Point"), _L("Select 1st Point"));
//	m_Gui->Button(ID_P2_LINK, &m_p2LandmarkName, _L("second_Point"), _L("Select 2nd Point"));
//	m_Gui->Button(ID_P3_LINK, &m_p3LandmarkName, _L("third_Point"), _L("Select 3rd Point"));

	m_Gui->Label(_R("Plane intersection"));
	m_Gui->Button(ID_PLAN1_LINK, &m_plan1Name, _L("Plan_intersect"), _L("Select plan to intersect"));


	m_Gui->Label(_L("angle between planes: "), &angleBPlanes, true);


	return m_Gui;
}

void mafVMEPlane::Update()
{
	if (m_Gui)
	{
		m_Gui->Update();
	
	}

	this->InternalUpdate();
	
	

}
//-------------------------------------------------------------------------
void mafVMEPlane::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
	// events to be sent up or down in the tree are simply forwarded
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
				
		
		case CHANGE_VALUE_PLANE:
		
		{
									   InternalUpdate();
									   e->SetId(CAMERA_UPDATE);
									   ForwardUpEvent(e);
		}
			break;
		case ID_PLAN1_LINK:
		{



						   mafID button_id = e->GetId();
						   mafString title = _L("Choose PLAN1 intersection link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {

							   SetPlaneLink(_R("Plan_intersect"), n);
							   m_plan1Name = n->GetName();
							   m_Gui->Update();

							   InternalUpdate();

						   }
		}
			break;
		case ID_P1_LINK:
		{

						  

						   mafID button_id = e->GetId();
						   mafString title = _L("Choose P1 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {

							   SetLandmarkLink(_R("first_Point"), n);
							   m_p1LandmarkName = n->GetName();
							   m_Gui->Update();
							   pts_change = true;
							   InternalUpdate();

						   }
		}
			break;
		
			/*case ID_P2_LINK:
		{

						   

						   mafID button_id = e->GetId();
						   mafString title = _("Choose P2 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {

							   
							   SetLandmarkLink("second_Point", n);
							   m_p2LandmarkName = n->GetName();

							   m_Gui->Update();
							   pts_change = true;
							   InternalUpdate();

						   }
		}
			break;
		case ID_P3_LINK:
		{

						   

						   mafID button_id = e->GetId();
						   mafString title = _("Choose P3 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {

							   
							   SetLandmarkLink("third_Point", n);
							   m_p3LandmarkName = n->GetName();

							   m_Gui->Update();
							   pts_change = true;
							   InternalUpdate();

						   }
		}
			break;
			
			*/
		default:
			mafVME::OnEvent(maf_event);
		}
	}

	else
	{
		Superclass::OnEvent(maf_event);
	}
}

void mafVMEPlane::SetLandmarkLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{
	pts_change = true;
	if (n->IsMAFType(mafVMELandmark))
	{
		SetLink(link_name, n, ((mafVMELandmarkCloud *)(n->GetParent()))->FindLandmarkIndex(n->GetName()));
	
	}
	else
	{
		SetLink(link_name, NULL);
	
	

	}
}
void mafVMEPlane::SetPlaneLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{

	if (n->IsMAFType(mafVMEPlane))
	{
		SetLink(link_name, n, ((mafVMEPlane*)n)->GetId());
		
	}
	else
		SetLink(link_name, NULL);
}
//-----------------------------------------------------------------------
void mafVMEPlane::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMEPlane::InternalUpdate()
//-----------------------------------------------------------------------
{
	//std::string s24 = "m_PlanePoint2 init" + std::to_string(m_PlanePoint2[0]) + " " + std::to_string(m_PlanePoint2[1]) + " " + std::to_string(m_PlanePoint2[2]);
	//wxBusyInfo wait52(s24.c_str());
	//Sleep(1500);


	UpdateLinks();

	surf->SetXResolution(20);
	surf->SetYResolution(20);
	
	
	mafTimeStamp currTs = GetTimeStamp();
	mafTransform *m_TmpTransform = mafTransform::New();
	mafMatrix tm;
	double xyzr[3];
	{


			

		//	m_Cloud2->SetLandmark("p1", m_PlaneOrigin[0], m_PlaneOrigin[1], m_PlaneOrigin[2], currTs);
		//	m_Cloud2->SetLandmark("p2", m_PlanePoint1[0], m_PlanePoint1[1], m_PlanePoint1[2], currTs);
		//	m_Cloud2->SetLandmark("p3", m_PlanePoint2[0], m_PlanePoint2[1], m_PlanePoint2[2], currTs);
		//	m_Cloud2->Update();
			//
		
	/*	if (pts_change)
			if ((p1 != NULL) )
				{


					((mafVMELandmark *)p1)->GetPoint(m_PlaneOrigin, currTs);
					((mafVMELandmarkCloud *)((mafVMELandmark *)p1)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->TransformPoint(m_PlaneOrigin, m_PlaneOrigin);


					this->GetOutput()->GetAbsMatrix(tm, currTs);
					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->Invert();
					m_TmpTransform->TransformPoint(m_PlaneOrigin, m_PlaneOrigin);



				//	m_Cloud2->SetLandmark("p1", m_PlaneOrigin[0], m_PlaneOrigin[1], m_PlaneOrigin[2], currTs);
				//	m_Cloud2->Update();
				}
				else
				{

				}
				*/
			/*if (pts_change)
			if ((p2 != NULL) )
				{
					//p2->GetOutput()->GetAbsPose(m_PlanePoint1, xyzr, currTs);
					((mafVMELandmark *)p2)->GetPoint(m_PlanePoint1, currTs);
					//this->GetOutput()->GetAbsMatrix(tm, currTs);
					((mafVMELandmarkCloud *)((mafVMELandmark *)p2)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->TransformPoint(m_PlanePoint1, m_PlanePoint1);


					this->GetOutput()->GetAbsMatrix(tm, currTs);
					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->Invert();

					m_TmpTransform->TransformPoint(m_PlanePoint1, m_PlanePoint1);

					//m_Cloud2->SetLandmark("p2", m_PlanePoint1[0], m_PlanePoint1[1], m_PlanePoint1[2], currTs);
					//m_Cloud2->Update();
				}
				else
				{
					
					//m_PlanePoint1[0] = 1.0; m_PlanePoint1[1] = m_PlanePoint1[2] = 1.0;
					//std::string s24 = "plane displaying1 " + std::to_string(m_PlanePoint1[0]) + " " + std::to_string(m_PlanePoint1[1]) + " " + std::to_string(m_PlanePoint1[2]);
					//wxBusyInfo wait52(s24.c_str());
					//Sleep(1500);
				}
				*/
			/*if (pts_change)
			if ((p3 != NULL) )
				{

			//	wxBusyInfo wait52("p3 not null");
			//	Sleep(1500);
					//p3->GetOutput()->GetAbsPose(m_PlanePoint2, xyzr, currTs);
					((mafVMELandmark *)p3)->GetPoint(m_PlanePoint2, currTs);
					((mafVMELandmarkCloud *)((mafVMELandmark *)p3)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->TransformPoint(m_PlanePoint2, m_PlanePoint2);


					this->GetOutput()->GetAbsMatrix(tm, currTs);
					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->Invert();

					m_TmpTransform->TransformPoint(m_PlanePoint2, m_PlanePoint2);
					
					//m_Cloud2->SetLandmark("p3", m_PlanePoint2[0], m_PlanePoint2[1], m_PlanePoint2[2], currTs);
					//m_Cloud2->Update();
				}
				else
				{

					//m_PlanePoint2[1] = 3.0; m_PlanePoint2[0] = m_PlanePoint2[2] = 1.0;
				//	std::string s24 = "plane displaying2 " + std::to_string(m_PlanePoint2[0]) + " " + std::to_string(m_PlanePoint2[1]) + " " + std::to_string(m_PlanePoint2[2]);
				//	wxBusyInfo wait52(s24.c_str());
				//	Sleep(1500);
				}
				*/
				
		}
			
			
			double Ux[3];
			double Uy[3];
			double m_PlaneOriginP[3];
			double m_PlanePoint1P[3];
			double m_PlanePoint2P[3];

			Ux[0] = m_PlanePoint1[0] - m_PlaneOrigin[0];
			Ux[1] = m_PlanePoint1[1] - m_PlaneOrigin[1];
			Ux[2] = m_PlanePoint1[2] - m_PlaneOrigin[2];

			Uy[0] = m_PlanePoint2[0] - m_PlaneOrigin[0];
			Uy[1] = m_PlanePoint2[1] - m_PlaneOrigin[1];
			Uy[2] = m_PlanePoint2[2] - m_PlaneOrigin[2];
			

			m_PlaneOriginP[0] = m_PlaneOrigin[0] - (Ux[0] + Uy[0]) * m_PlaneXRes;
			m_PlaneOriginP[1] = m_PlaneOrigin[1] - (Ux[1] + Uy[1]) * m_PlaneXRes;
			m_PlaneOriginP[2] = m_PlaneOrigin[2] - (Ux[2] + Uy[2]) * m_PlaneXRes;

			m_PlanePoint1P[0] = m_PlanePoint1[0] - (Ux[0] + Uy[0]) * m_PlaneXRes + Ux[0] * m_PlaneYRes;
			m_PlanePoint1P[1] = m_PlanePoint1[1] - (Ux[1] + Uy[1]) * m_PlaneXRes + Ux[1] * m_PlaneYRes;
			m_PlanePoint1P[2] = m_PlanePoint1[2] - (Ux[2] + Uy[2]) * m_PlaneXRes + Ux[2] * m_PlaneYRes;

			m_PlanePoint2P[0] = m_PlanePoint2[0] - (Ux[0] + Uy[0]) * m_PlaneXRes + Uy[0] * m_PlaneYRes;
			m_PlanePoint2P[1] = m_PlanePoint2[1] - (Ux[1] + Uy[1]) * m_PlaneXRes + Uy[1] * m_PlaneYRes;
			m_PlanePoint2P[2] = m_PlanePoint2[2] - (Ux[2] + Uy[2]) * m_PlaneXRes + Uy[2] * m_PlaneYRes;

			surf->RemoveAllInputs();
			surf->SetOrigin(m_PlaneOriginP);
			surf->SetPoint1(m_PlanePoint1P);
			surf->SetPoint2(m_PlanePoint2P);

		//	string sss40 = "Plane internal update final ..." + std::to_string(m_PlanePoint2[0]) + " " + std::to_string(m_PlanePoint2[1]) + " " + std::to_string(m_PlanePoint2[2]);
		//	wxBusyInfo wait12040s(sss40.c_str());
		//	Sleep(1500);
			
			surf->Push(tr);
							 
			//surf->GetNormal(normal);
			computeNormal();
		
							 
			vtkMAFSmartPointer<vtkTriangleFilter> triangle;
			triangle->SetInput(surf->GetOutput());
			triangle->Update();


			
			m_PolyData->DeepCopy(triangle->GetOutput());
			m_PolyData->Update();

			if (plan1 != NULL)
			{
				double* v1=this->getNormalVector();
				double* v2=((mafVMEPlane*)plan1)->getNormalVector();
				angPlanes = 180 * std::acos((v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]) / ((v1[0] * v1[0] + v1[1] * v1[1] + v1[2] * v1[2]) + (v2[0] * v2[0] + v2[1] * v2[1] + v2[2] * v2[2]))) / 3.14159;
				mafString ss = _R(" ");
				angleBPlanes = ss.Append(mafToString(angPlanes)); 


			
				if (m_Gui)
					m_Gui->Update();
			}
			
	
	
	pts_change = false;
	

	mafEvent ev(this,CAMERA_UPDATE);
	this->ForwardUpEvent(&ev);
}

double mafVMEPlane::GetUVector(int a)
{
	if (a == 0)
	{
		return m_PlaneXRes;
	}
	if (a==1)
	
	{
		return m_PlaneYRes;
	}

}

void mafVMEPlane::SetUVector(double aa, int a)
{
	if (a == 0)
	{
		 m_PlaneXRes=aa;
	}
	if (a == 1)

	{
		m_PlaneYRes = aa;
	}

}
//-----------------------------------------------------------------------
int mafVMEPlane::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
		if (

		//	parent->StoreText("name", m_Name) == MAF_OK &&
			
		//	parent->StoreInteger("Geometry", m_GeometryType) == MAF_OK &&
	
			parent->StoreDouble(_R("PlaneUx"), m_PlaneXRes) == MAF_OK &&
			parent->StoreDouble(_R("PlaneUy"), m_PlaneYRes) == MAF_OK &&

			/*parent->StoreDouble("m_PlaneOrigin0", m_PlaneOrigin[0]) == MAF_OK &&
			parent->StoreDouble("m_PlaneOrigin1", m_PlaneOrigin[1]) == MAF_OK &&
			parent->StoreDouble("m_PlaneOrigin2", m_PlaneOrigin[2]) == MAF_OK &&

			parent->StoreDouble("m_PlanePoint1_0", m_PlanePoint1[0]) == MAF_OK &&
			parent->StoreDouble("m_PlanePoint1_1", m_PlanePoint1[1]) == MAF_OK &&
			parent->StoreDouble("m_PlanePoint1_2", m_PlanePoint1[2]) == MAF_OK &&


			parent->StoreDouble("m_PlanePoint2_0", m_PlanePoint2[0]) == MAF_OK &&
			parent->StoreDouble("m_PlanePoint2_1", m_PlanePoint2[1]) == MAF_OK &&
			parent->StoreDouble("m_PlanePoint2_2", m_PlanePoint2[2]) == MAF_OK &&
			*/
			parent->StoreMatrix(_R("Transform"), &m_Transform->GetMatrix()) == MAF_OK 
			

			)
			return MAF_OK;
	}
	return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEPlane::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalRestore(node) == MAF_OK)
	{
		mafMatrix matrix;
		if (
	//	node->RestoreText("name", m_Name) &&
		node->RestoreDouble(_R("PlaneUx"), m_PlaneXRes) &&
		node->RestoreDouble(_R("PlaneUy"), m_PlaneYRes) &&
		
		/*node->RestoreDouble("m_PlaneOrigin0", m_PlaneOrigin[0]) == MAF_OK &&
		node->RestoreDouble("m_PlaneOrigin1", m_PlaneOrigin[1]) == MAF_OK &&
		node->RestoreDouble("m_PlaneOrigin2", m_PlaneOrigin[2]) == MAF_OK &&

		node->RestoreDouble("m_PlanePoint1_0", m_PlanePoint1[0]) == MAF_OK &&
		node->RestoreDouble("m_PlanePoint1_1", m_PlanePoint1[1]) == MAF_OK &&
		node->RestoreDouble("m_PlanePoint1_2", m_PlanePoint1[2]) == MAF_OK &&


		node->RestoreDouble("m_PlanePoint2_0", m_PlanePoint2[0]) == MAF_OK &&
		node->RestoreDouble("m_PlanePoint2_1", m_PlanePoint2[1]) == MAF_OK &&
		node->RestoreDouble("m_PlanePoint2_2", m_PlanePoint2[2]) == MAF_OK &&
*/
		node->RestoreMatrix(_R("Transform"), &matrix) == MAF_OK)
		{
			m_Transform->SetMatrix(matrix);
		}
		
			
			return MAF_OK;
	}
	
	return MAF_ERROR;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
char** mafVMEPlane::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMEProcedural.xpm"
	return mafVMEProcedural_xpm;


}

/*void mafVMEPlane::CreateGuiPlane()
{
	m_GuiPlane = new mafGUI(this);

	m_GuiPlane->Double(CHANGE_VALUE_PLANE, _("Ux"), &m_PlaneXRes);
	m_GuiPlane->Double(CHANGE_VALUE_PLANE, _("Uy"), &m_PlaneYRes);
	m_Gui->Button(ID_P1_LINK, &m_p1LandmarkName, _("first_Point"), _("Select 1st Point"));
	m_Gui->Button(ID_P2_LINK, &m_p2LandmarkName, _("second_Point"), _("Select 2nd Point"));
	m_Gui->Button(ID_P3_LINK, &m_p3LandmarkName, _("third_Point"), _("Select 3rd Point"));
	
	m_GuiPlane->Label("Plane intersection");
	m_Gui->Button(ID_PLAN1_LINK, &m_plan1Name, _("Plan_intersect"), _("Select plan to intersect"));

	
	
	
	
	m_Gui->Label(_("angle between planes: "), &angleBPlanes, true);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiPlane);
}
void mafVMEPlane::EnableGuiPlane()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, true);

}

void mafVMEPlane::DisableGuiPlane()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);

}
*/



void mafVMEPlane::UpdateLinks()
//-------------------------------------------------------------------------
{


	mafID sub_id = -1;
	mafNode* nd = NULL;
	

	plan1 = GetLandmarkVME(_R("Plan_intersect"));

	if (plan1 && plan1->IsMAFType(mafVMEPlane))
	{
		nd = GetLink(_R("Plan_intersect"));
		m_plan1Name = (nd != NULL) ? ((mafVMEPlane *)plan1)->GetName() : _L("none");
	}
	else
		m_plan1Name = plan1 ? plan1->GetName() : _L("none");

	
	//if (pts_change)
	//{

		mafTimeStamp currTs = GetTimeStamp();
		double xyzr[3];
		p1 = GetLandmarkVME(_R("first_Point"));
		
		if (p1 && p1->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId(_R("first_Point"));
			m_p1LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p1->GetParent())->GetLandmarkName(sub_id) : _L("none");
			p1->GetOutput()->GetAbsPose(m_PlaneOrigin, xyzr, currTs);
		}
		else
			m_p1LandmarkName = p1 ? p1->GetName() : _L("none");


		/*

		p2 = GetLandmarkVME("second_Point");
		if (p2 && p2->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId("second_Point");
			m_p2LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p2->GetParent())->GetLandmarkName(sub_id) : _("none");
		}
		else
			m_p2LandmarkName = p2 ? p2->GetName() : _("none");




		p3 = GetLandmarkVME("third_Point");
		if (p3 && p3->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId("third_Point");
			m_p3LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p3->GetParent())->GetLandmarkName(sub_id) : _("none");
		}
		else
		{
			m_p3LandmarkName = p3 ? p3->GetName() : _("none");

			
		}
			
	}
	*/
	

}
void mafVMEPlane::computeNormal()

{
	double v1[3];
	double v2[3];
	double v1crossv2[3];
	
	//
	double origin[3];
	double Point1[3];
	double Point2[3];
	mafTransform* m_TmpTransform;
	mafTimeStamp currTs = GetTimeStamp();
	mafNEW(m_TmpTransform);
	mafMatrix tm;
	m_Cloud2->GetOutput()->GetAbsMatrix(tm, currTs);
	m_TmpTransform->SetMatrix(tm);

	((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId(_R("p1")), m_PlaneOrigin, currTs);
	m_TmpTransform->TransformPoint(m_PlaneOrigin, origin);


	((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId(_R("p2")), m_PlanePoint1, currTs);
	m_TmpTransform->TransformPoint(m_PlanePoint1, Point1);


	((mafVMELandmarkCloud *)m_Cloud2)->GetLandmark(GetLinkSubId(_R("p3")), m_PlanePoint2, currTs);
	m_TmpTransform->TransformPoint(m_PlanePoint2, Point2);
	
	v1[0] = Point1[0] - origin[0];
	v1[1] = Point1[01] - origin[01];
	v1[2] = Point1[02] - origin[02];

	//string sss1 = " v1 " + std::to_string(v1[0]) + " " + std::to_string(v1[1]) + " " + std::to_string(v1[2]);
	//wxBusyInfo wait12s(sss1.c_str());
	//Sleep(2500);

	v2[0] = Point2[0] - origin[0];
	v2[1] = Point2[01] - origin[01];
	v2[2] = Point2[02] - origin[02];
	
	//string sss2 = " v2 " + std::to_string(v2[0]) + " " + std::to_string(v2[1]) + " " + std::to_string(v2[2]);
	//wxBusyInfo wait122s(sss2.c_str());
	//Sleep(2500);

	//
	/*v1[0] = m_PlanePoint1[0] - m_PlaneOrigin[0];
	v1[1] = m_PlanePoint1[01] - m_PlaneOrigin[01];
	v1[2] = m_PlanePoint1[02] - m_PlaneOrigin[02];

	v2[0] = m_PlanePoint2[0] - m_PlaneOrigin[0];
	v2[1] = m_PlanePoint2[01] - m_PlaneOrigin[01];
	v2[2] = m_PlanePoint2[02] - m_PlaneOrigin[02];
	
	*/
	v1crossv2[0] = v1[1] * v2[2] - v1[2] * v2[1];
	v1crossv2[1] = -(v1[0]*v2[2]-v1[2]*v2[0]);
	v1crossv2[2] = v1[0] * v2[1] - v1[1] * v2[0];

	double norm = sqrt(v1crossv2[0] * v1crossv2[0] + v1crossv2[1] * v1crossv2[1] + v1crossv2[2] * v1crossv2[2]);

	normal[0] = v1crossv2[0]/ norm;
	normal[1] = v1crossv2[1] / norm;
	normal[2] = v1crossv2[2] / norm;
	//string sss40 = " normal " + std::to_string(normal[0]) + " " + std::to_string(normal[1]) + " " + std::to_string(normal[2]);
	//wxBusyInfo wait12040s(sss40.c_str());
	//Sleep(2500);

}
mafVME *mafVMEPlane::GetLandmarkVME(mafString str)
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink(str));
}

mafVME *mafVMEPlane::GetVMEPt1()
{


	return m_Cloud2->GetLandmark(0);
}

/*void mafVMEPlane::Setm_p1LName(mafString a)
{
	m_p1LandmarkName = a;
}

void mafVMEPlane::Setm_p2LName(mafString a)
{
	m_p2LandmarkName = a;
}
void mafVMEPlane::Setm_p3LName(mafString a)
{
	m_p3LandmarkName = a;
}*/