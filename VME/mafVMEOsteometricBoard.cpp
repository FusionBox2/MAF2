

#include "mafDefines.h" 
#include <string>
#include "wx/busyinfo.h"
#include "mafVMEOsteometricBoard.h"
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
#include "vtkAppendPolydata.h"

#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>
#include "mafVMEGravityLine.h"
#include "../../Eigen/Eigen/Dense"
using namespace std;
//const bool DEBUG_MODE = true;
//#include "../DataElementInfoPanel.hpp"
//#include <wx/xml/xml.h>

// public methods

// constructor

mafCxxTypeMacro(mafVMEOsteometricBoard);

mafVMEOsteometricBoard::mafVMEOsteometricBoard()
//-------------------------------------------------------------------------
{



	m_PlaneXRes = 1.0;
	m_PlaneYRes = 1.0;
	m_PlaneOrigin[0] = m_PlaneOrigin[1] = m_PlaneOrigin[2] = 0;
	
	m_PlanePoint1[0] = 10.0;	m_PlanePoint1[1] = m_PlanePoint1[2] = 0.0;
	m_PlanePoint2[1] = 0.0;	m_PlanePoint2[0] = 10; m_PlanePoint2[2] = 0.0;
	m_PlanePoint3[1] = 10.0;	m_PlanePoint3[0] = 0; m_PlanePoint3[2] = 10.0;
	m_PlanePoint4[1] = 10.0;	m_PlanePoint4[0] = m_PlanePoint4[2] = 0.0;
	m_PlanePoint5[1] = 10.0;	m_PlanePoint5[0] = m_PlanePoint5[2] = 0.0;
	pos[0] = 0; pos[1] = 0;  pos[2] = 0;
	pos2[0] = 0; pos2[1] = 0;  pos2[2] = 0;
	pos3[0] = 0; pos3[1] = 0;  pos3[2] = 0;

	plan1 = mafVMEPlane::New();
	plan1->SetName(_R("prinicipal_plane"));
	plan1->ReparentTo(this);
	
	plan2 = mafVMEPlane::New();
	plan2->SetName(_R("2nd_plane"));
	plan2->ReparentTo(this);

	plan3 = mafVMEPlane::New();
	plan3->SetName(_R("3rd_plane"));
	plan3->ReparentTo(this);


	pts_change = true;
	double rxyz[3];
	/*p1 = mafVMELandmark::New();
	p1->SetName("p1");
	p1->SetAbsPose(m_PlaneOrigin,rxyz,-1);
	p2 = mafVMELandmark::New();
	p2->SetName("p2");
	p2->SetAbsPose(m_PlanePoint1, rxyz, -1);
	p3 = mafVMELandmark::New();
	p3->SetName("p3");
	p3->SetAbsPose(m_PlanePoint2, rxyz, -1);
	p4 = mafVMELandmark::New();
	p4->SetName("p4");
	p4->SetAbsPose(m_PlanePoint3, rxyz, -1);
	p5 = mafVMELandmark::New();
	p5->SetName("p5");
	p5->SetAbsPose(m_PlanePoint4, rxyz, -1);
	p6 = mafVMELandmark::New();
	p6->SetName("p6");
	p6->SetAbsPose(m_PlanePoint5, rxyz, -1);
	*/

	mafNEW(m_Cloud1);

	m_Cloud1->Open();
	m_Cloud1->SetName(_L("computed OB points"));
	m_Cloud1->SetRadius(1.5);
	m_Cloud1->ReparentTo(this);
	m_Cloud1->AppendLandmark(m_PlaneOrigin[0], m_PlaneOrigin[1], m_PlaneOrigin[2], _R("Pl2Pt3"), false);
	m_Cloud1->AppendLandmark(m_PlanePoint1[0], m_PlanePoint1[1], m_PlanePoint1[2], _R("Pl3Pt2"), false);
	m_Cloud1->AppendLandmark(m_PlanePoint2[0], m_PlanePoint2[1], m_PlanePoint2[2], _R("Pl3Pt3"), false);
	m_p1LandmarkName = _R("");
	m_p2LandmarkName = _R("");
	m_p3LandmarkName = _R("");
	m_p4LandmarkName = _R("");
	m_p5LandmarkName = _R("");
	m_p6LandmarkName = _R("");
	m_SurfaceName = _R("");

	//gline1 = NULL;
	//surface = NULL;
	mafNEW(gline1);
	//gline1->ReparentTo(plan1);
	gline1->ReparentTo(this);
	gline1->SetName(_R("gline1"));
	
	mafNEW(gline2);
	//gline2->ReparentTo(plan2);
	gline2->ReparentTo(this);
	gline2->SetName(_R("gline2"));
	;
	mafNEW(gline3);
	gline3->ReparentTo(this);
	//gline3->ReparentTo(plan3);
	gline3->SetName(_R("gline3"));
	
	m_plan1Name = _R("");
	//m_p4LandmarkName = "";
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
mafVMEOsteometricBoard::~mafVMEOsteometricBoard()
//-------------------------------------------------------------------------
{

//	vtkDEL(m_LineNormal);
	vtkDEL(m_PolyData);
	mafDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEOsteometricBoard::GetMaterial()
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
int mafVMEOsteometricBoard::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{


	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		mafVMEOsteometricBoard *vmeOsBoard = mafVMEOsteometricBoard::SafeDownCast(a);
		m_Transform->SetMatrix(vmeOsBoard->m_Transform->GetMatrix());
		//this->m_GeometryType = vmePlane->m_GeometryType;

		this->m_PlaneXRes = vmeOsBoard->m_PlaneXRes;
		this->m_PlaneYRes = vmeOsBoard->m_PlaneYRes;

		



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
bool mafVMEOsteometricBoard::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{


	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (
			m_Transform->GetMatrix() == ((mafVMEOsteometricBoard *)vme)->m_Transform->GetMatrix() &&
			
		
			this->m_PlaneXRes == ((mafVMEOsteometricBoard *)vme)->m_PlaneXRes  &&
			this->m_PlaneYRes == ((mafVMEOsteometricBoard *)vme)->m_PlaneYRes

			
			)
		{
			ret = true;
		}
	}
	return ret;
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMEOsteometricBoard::GetSurfaceOutput()
//-------------------------------------------------------------------------
{


	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEOsteometricBoard::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{


	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMEOsteometricBoard::IsAnimated()
//-------------------------------------------------------------------------
{


	return false;
}
//-------------------------------------------------------------------------
void mafVMEOsteometricBoard::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{


	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}


void mafVMEOsteometricBoard::push(double t)
{

	
	tr = t;

}
//-------------------------------------------------------------------------
mafGUI* mafVMEOsteometricBoard::CreateGui()
//-------------------------------------------------------------------------
{



	m_Gui=mafNode::CreateGui();
	//if (m_Gui)
	//{
	

	m_Gui->Double(CHANGE_VALUE_PLANE, _L("alphaPlan1"), &m_PlaneXRes);
	m_Gui->Double(CHANGE_VALUE_PLANE, _L("alphaPlan2"), &m_PlaneYRes);
	m_Gui->Button(ID_Surface_LINK, &m_SurfaceName, _L("Surface"), _L("Select Surace"));
	m_Gui->Button(ID_P1_LINK, &m_p1LandmarkName, _L("1st plane 1st_Point"), _L("Select Pl1 1st Point"));
	m_Gui->Button(ID_P2_LINK, &m_p2LandmarkName, _L("1st plane 2nd_Point"), _L("Select Pl1 2nd Point"));
	m_Gui->Button(ID_P3_LINK, &m_p3LandmarkName, _L("1st plane 3rd_Point"), _L("Select Pl1 3rd Point"));

	m_Gui->Button(ID_P4_LINK, &m_p4LandmarkName, _L("2nd plane 1st_Point"), _L("Select Pl2 1st Point"));
	m_Gui->Button(ID_P5_LINK, &m_p5LandmarkName, _L("2nd plane 2nd_Point"), _L("Select Pl2 2nd Point"));
	m_Gui->Button(ID_P6_LINK, &m_p6LandmarkName, _L("3rd plane 1st_Point"), _L("Select Pl3 1st Point"));

	m_Gui->Update();
	//}

	
	return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOsteometricBoard::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{


	//wxBusyInfo wait120("OB onevent");
	//Sleep(2500);
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

		case ID_Surface_LINK:
		{

								mafID button_id = e->GetId();
								mafString title = _L("Choose surface vme link");
								e->SetId(VME_CHOOSE);
								// e->SetArg((long)&mafVMESurface::VMEAccept);
								e->SetString(&title);
								ForwardUpEvent(e);
								mafNode *n = e->GetVme();
								if (n != NULL)
								{

									SetSurfaceLink(_R("SurfaceVME"), n);
									m_SurfaceName = n->GetName();
									surface = (mafVMESurface*)n;

									m_Gui->Update();

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
		case ID_P2_LINK:
		{

						   

						   mafID button_id = e->GetId();
						   mafString title = _L("Choose P2 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {

							   
							   SetLandmarkLink(_R("second_Point"), n);
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
						   mafString title = _L("Choose P3 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {

							   
							   SetLandmarkLink(_R("third_Point"), n);
							   m_p3LandmarkName = n->GetName();

							   m_Gui->Update();
							   pts_change = true;
							   InternalUpdate();

						   }
		}
			break;
		case ID_P4_LINK:
		{



						   mafID button_id = e->GetId();
						   mafString title = _L("Choose P4 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {
							   SetLandmarkLink(_R("4th_Point"), n);
							   m_p4LandmarkName = n->GetName();

							   m_Gui->Update();
							   pts_change = true;
							   InternalUpdate();

						   }
		}
			break;

		case ID_P5_LINK:
		{



						   mafID button_id = e->GetId();
						   mafString title = _L("Choose P5 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {


							   SetLandmarkLink(_R("5th_Point"), n);
							   m_p5LandmarkName = n->GetName();

							   m_Gui->Update();
							   pts_change = true;
							   InternalUpdate();

						   }
		}
			break;

		case ID_P6_LINK:
		{



						   mafID button_id = e->GetId();
						   mafString title = _L("Choose P6 Landmark link");
						   e->SetId(VME_CHOOSE);

						   e->SetString(&title);
						   ForwardUpEvent(e);
						   mafNode *n = e->GetVme();
						   if (n != NULL)
						   {


							   SetLandmarkLink(_R("6th_Point"), n);
							   m_p6LandmarkName = n->GetName();

							   m_Gui->Update();
							   pts_change = true;
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

void mafVMEOsteometricBoard::SetLandmarkLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{
	

	if (n->IsMAFType(mafVMELandmark))
	{
		SetLink(link_name, n, ((mafVMELandmarkCloud *)(n->GetParent()))->FindLandmarkIndex(n->GetName()));
	}
	else
		SetLink(link_name, NULL);
}
/*void mafVMEOsteometricBoard::SetPlaneLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{

	if (n->IsMAFType(mafVMEPlane))
	{
		SetLink(link_name, n, ((mafVMEPlane*)n)->GetId());
	}
	else
		SetLink(link_name, NULL);
}*/
//-----------------------------------------------------------------------
void mafVMEOsteometricBoard::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMEOsteometricBoard::InternalUpdate()
//-----------------------------------------------------------------------
{

	

	
	UpdateLinks();
	
	mafTimeStamp currTs = GetTimeStamp();
	mafTransform *m_TmpTransform = mafTransform::New();
	mafMatrix tm;
	double xyzr[3];
	{

		
			surf->SetXResolution(20);
			surf->SetYResolution(20);

		
			if (pts_change)
			//if ((p1 != NULL) && (plan1 != NULL))
				{

					

					//((mafVMELandmark *)p1)->GetPoint(m_PlaneOrigin, currTs);
					//((mafVMELandmarkCloud *)((mafVMELandmark *)p1)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->TransformPoint(m_PlaneOrigin, m_PlaneOrigin);


					this->GetOutput()->GetAbsMatrix(tm, currTs);
					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->Invert();
					m_TmpTransform->TransformPoint(m_PlaneOrigin, m_PlaneOrigin);


				
			//		plan1->setPoint1(m_PlaneOrigin);
					

				
				}
			

			if (pts_change)
			//if ((p2 != NULL) && (plan1 != NULL))
				{
				//	((mafVMELandmark *)p2)->GetPoint(m_PlanePoint1, currTs);
				//	((mafVMELandmarkCloud *)((mafVMELandmark *)p2)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->TransformPoint(m_PlanePoint1, m_PlanePoint1);


					this->GetOutput()->GetAbsMatrix(tm, currTs);
					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->Invert();

					m_TmpTransform->TransformPoint(m_PlanePoint1, m_PlanePoint1);

			//		plan1->setPoint2(m_PlanePoint1);
				}
				else
				{

					;
				}

			if (pts_change)
			//if ((p3 != NULL) && (plan1 != NULL))
				{
					//((mafVMELandmark *)p3)->GetPoint(m_PlanePoint2, currTs);
					//((mafVMELandmarkCloud *)((mafVMELandmark *)p3)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->TransformPoint(m_PlanePoint2, m_PlanePoint2);


					this->GetOutput()->GetAbsMatrix(tm, currTs);
					m_TmpTransform->SetMatrix(tm);
					m_TmpTransform->Invert();

					m_TmpTransform->TransformPoint(m_PlanePoint2, m_PlanePoint2);
					
					//plan1->setPoint2(m_PlanePoint2);
				}
				else
				{

					//m_PlanePoint2[1] = 3.0; m_PlanePoint2[0] = m_PlanePoint2[2] = 1.0;
					//std::string s24 = "plane displaying2 " + std::to_string(m_PlanePoint2[0]) + " " + std::to_string(m_PlanePoint2[1]) + " " + std::to_string(m_PlanePoint2[2]);
					//wxBusyInfo wait52(s24.c_str());
					//Sleep(1500);
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
			
			//surf->RemoveAllInputs();
			//surf->SetOrigin(m_PlaneOriginP);
			//surf->SetPoint1(m_PlanePoint1P);
			//surf->SetPoint2(m_PlanePoint2P);
			
		//	surf->Push(tr);
		//	((mafVMEPlane*)plan1)->setPoint1(m_PlaneOrigin);
		//	((mafVMEPlane*)plan1)->setPoint2(m_PlanePoint1);
		//	((mafVMEPlane*)plan1)->setPoint3(m_PlanePoint2);
			((mafVMEPlane*)plan1)->Update();
			double* normal1 = ((mafVMEPlane*)plan1)->getNormalVector();
			
			
			
		
							 
			//vtkMAFSmartPointer<vtkTriangleFilter> triangle;
			//triangle->SetInput(surf->GetOutput());
			//triangle->Update();
			if (plan2 != NULL)
			{
				m_PlaneOrigin[0] = plan2->getPoint1()[0];
				m_PlaneOrigin[01] = plan2->getPoint1()[01];
				m_PlaneOrigin[02] = plan2->getPoint1()[02];
			}
			
			pos[0] = m_PlaneOrigin[0] + m_PlaneXRes*normal1[0];
			pos[1] = m_PlaneOrigin[1] + m_PlaneXRes*normal1[1];
			pos[2] = m_PlaneOrigin[2] + m_PlaneXRes*normal1[2];

			

			double* normal2;
			
			//plan2->setPoint1(m_PlaneOrigin);
			//plan2->setPoint2(pos);
			m_Cloud1->SetLandmark(_R("Pl2Pt3"), pos[0], pos[1], pos[2], -1);
			//plan2->setPoint3(m_PlanePoint1);

//			plan2->SetLandmarkLink("third_Point", m_Cloud1->GetLandmark("Pl2Pt3"));
//			plan2->Setm_p3LName("Pl2Pt3");
			plan2->Update();
			normal2 = plan2->getNormalVector();
			

		/*	if (plan3 != NULL)
			{
				m_PlanePoint2[0] = plan3->getPoint1()[0];
				m_PlanePoint2[1] = plan3->getPoint1()[1];
				m_PlanePoint2[2] = plan3->getPoint1()[2];
			}
			

			*/
			
			std::string s24 = "plane displaying2 " + std::to_string(m_PlanePoint2[0]) + " " + std::to_string(m_PlanePoint2[1]) + " " + std::to_string(m_PlanePoint2[2]);
			wxBusyInfo wait52(s24.c_str());
			Sleep(1500);
			
			
			pos2[0] = m_PlanePoint2[0] + m_PlaneYRes*normal1[0];
			pos2[1] = m_PlanePoint2[1] + m_PlaneYRes*normal1[1];
			pos2[2] = m_PlanePoint2[2] + m_PlaneYRes*normal1[2];
		
			pos3[0] = m_PlanePoint2[0] + m_PlaneYRes*normal2[0];
			pos3[1] = m_PlanePoint2[1] + m_PlaneYRes*normal2[1];
			pos3[2] = m_PlanePoint2[2] + m_PlaneYRes*normal2[2];
			
			
			//plan3->setPoint1(pos2);

			m_Cloud1->SetLandmark(_R("Pl3Pt2"), pos2[0], pos2[1], pos2[2], -1);
//			plan3->SetLandmarkLink("second_Point", m_Cloud1->GetLandmark("Pl3Pt2"));
//			plan3->Setm_p2LName("Pl3Pt2");
			plan3->Update();

			//plan3->setPoint2(pos3);
			m_Cloud1->SetLandmark(_R("Pl3Pt3"), pos3[0], pos3[1], pos3[2], -1);
//			plan3->SetLandmarkLink("third_Point", m_Cloud1->GetLandmark("Pl3Pt3"));
//			plan3->Setm_p3LName("Pl3Pt3");
			plan3->Update();
			
			//plan3->setPoint3(m_PlanePoint2);
			plan3->Update();
			
	}
	pts_change = false;
	
	
	mafEvent ev(this,CAMERA_UPDATE);
	this->ForwardUpEvent(&ev);


	
	
}

double mafVMEOsteometricBoard::GetUVector(int a)
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

void mafVMEOsteometricBoard::SetUVector(double aa, int a)
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
int mafVMEOsteometricBoard::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{

	
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
		if (
			parent->StoreMatrix(_R("Transform"), &m_Transform->GetMatrix()) == MAF_OK &&
		//	parent->StoreInteger("Geometry", m_GeometryType) == MAF_OK &&
	
			parent->StoreDouble(_R("PlaneUx"), m_PlaneXRes) == MAF_OK &&
			parent->StoreDouble(_R("PlaneUy"), m_PlaneYRes) == MAF_OK //&&
			
	
			
			)
			return MAF_OK;
		else
		{
			wxBusyInfo wait52("storing osteometricboard pb");
			Sleep(1500);
		}
	}
	return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEOsteometricBoard::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{


	
	if (Superclass::InternalRestore(node) == MAF_OK)
	{
		mafMatrix matrix;
		if (node->RestoreMatrix(_R("Transform"), &matrix) == MAF_OK)
		{
			m_Transform->SetMatrix(matrix);
		
			
			node->RestoreDouble(_R("PlaneUx"), m_PlaneXRes);
			node->RestoreDouble(_R("PlaneUy"), m_PlaneYRes);
			/*node->RestoreText("m_p1LandmarkName", m_p1LandmarkName);
			node->RestoreText("m_p2LandmarkName", m_p2LandmarkName);
			node->RestoreText("m_p3LandmarkName", m_p3LandmarkName);
			node->RestoreText("m_p4LandmarkName", m_p4LandmarkName);
			node->RestoreText("m_p5LandmarkName", m_p5LandmarkName);
			node->RestoreText("m_p6LandmarkName", m_p6LandmarkName);
			node->RestoreText("m_SurfaceName", m_SurfaceName);*/

			//node->RestoreVectorN("PlaneOrigin", m_PlaneOrigin, 3);
			//node->RestoreVectorN("PlanePoint1", m_PlanePoint1, 3);
			//node->RestoreVectorN("PlanePoint2", m_PlanePoint2, 3);
			//node->RestoreVectorN("PlanePoint3", m_PlanePoint3, 3);
			//node->RestoreVectorN("PlanePoint4", m_PlanePoint4, 3);
			//node->RestoreVectorN("PlanePoint5", m_PlanePoint5, 3);
			
			return MAF_OK;
		}
	}
	return MAF_ERROR;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
char** mafVMEOsteometricBoard::GetIcon()
//-------------------------------------------------------------------------
{


	
	#include "mafVMEProcedural.xpm"
	return mafVMEProcedural_xpm;


}













void mafVMEOsteometricBoard::UpdateLinks()
//-------------------------------------------------------------------------
{
	

	//wxBusyInfo wait120("OB updateliks");
	//Sleep(2500);
	mafID sub_id = -1;
	mafNode* nd = NULL;
	
	surface = GetSurfaceVME();
	if (surface && surface->IsMAFType(mafVMESurface))
	{
		nd = GetLink(_R("SurfaceVME"));
		m_SurfaceName = (nd != NULL) ? ((mafVMESurface *)surface)->GetName() : _L("none");

		//gline1->SetSurfaceLink("SurfaceVME", nd);
		
		if (gline1 != NULL)
		{

			//wxBusyInfo wait1210("set gline1 surface link");
			//Sleep(2500);
			gline1->SetSurfaceName((mafVMESurface*)nd, nd->GetName());
		}
		


		//gline2->SetSurfaceLink("SurfaceVME", nd);
		if (gline2 != NULL)
			gline2->SetSurfaceName((mafVMESurface*)nd, nd->GetName());
		

		//gline3->SetSurfaceLink("SurfaceVME", nd);
		if (gline3 != NULL)
			gline3->SetSurfaceName((mafVMESurface*)nd, nd->GetName());
		
	}
	else
		m_SurfaceName = surface ? surface->GetName() : _L("none");

	
	
	if (pts_change)
	{

		 
		mafVMELandmark* p1 = GetLandmarkVME(_R("first_Point"));
		if (p1 && p1->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId(_R("first_Point"));
			m_p1LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p1->GetParent())->GetLandmarkName(sub_id) : _L("none");

//			plan1->SetLandmarkLink("first_Point", GetLandmarkVME("first_Point"));
//			plan1->Setm_p1LName(m_p1LandmarkName);
		}
		else
			m_p1LandmarkName = p1 ? p1->GetName() : _L("none");

		mafVMELandmark* p2 = GetLandmarkVME(_R("second_Point"));
		if (p2 && p2->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId(_R("second_Point"));
			m_p2LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p2->GetParent())->GetLandmarkName(sub_id) : _L("none");

//			plan1->SetLandmarkLink("second_Point", GetLandmarkVME("second_Point"));
//			plan1->Setm_p2LName(m_p2LandmarkName);
		}
		else
			m_p2LandmarkName = p2 ? p2->GetName() : _L("none");

		mafVMELandmark* p3 = GetLandmarkVME(_R("third_Point"));
		if (p3 && p3->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId(_R("third_Point"));
			m_p3LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p3->GetParent())->GetLandmarkName(sub_id) : _L("none");

//			plan1->SetLandmarkLink("third_Point", ((mafVMELandmarkCloud *)p3->GetParent())->GetLandmark(m_p3LandmarkName));
//			plan1->Setm_p3LName(m_p3LandmarkName);
		}
		else
			m_p3LandmarkName = p3 ? p3->GetName() : _L("none");

		mafVMELandmark* p4 = GetLandmarkVME(_R("4th_Point"));
		if (p4 && p4->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId(_R("4th_Point"));
			m_p4LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p4->GetParent())->GetLandmarkName(sub_id) : _L("none");

//			plan2->SetLandmarkLink("first_Point", ((mafVMELandmarkCloud *)p4->GetParent())->GetLandmark(m_p4LandmarkName));
//			plan2->Setm_p1LName(m_p4LandmarkName);
		}
		else
			m_p4LandmarkName = p4 ? p4->GetName() : _L("none");

		mafVMELandmark* p5 = GetLandmarkVME(_R("5th_Point"));
		if (p5 && p5->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId(_R("5th_Point"));
			m_p5LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p5->GetParent())->GetLandmarkName(sub_id) : _L("none");

//			plan2->SetLandmarkLink("second_Point", ((mafVMELandmarkCloud *)p5->GetParent())->GetLandmark(m_p5LandmarkName));
//			plan2->Setm_p2LName(m_p5LandmarkName);
		}
		else
			m_p5LandmarkName = p5 ? p5->GetName() : _L("none");

		mafVMELandmark* p6 = GetLandmarkVME(_R("6th_Point"));
		if (p6 && p6->IsMAFType(mafVMELandmark))
		{
			sub_id = GetLinkSubId(_R("6th_Point"));
			m_p6LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)p6->GetParent())->GetLandmarkName(sub_id) : _L("none");

//			plan3->SetLandmarkLink("first_Point", ((mafVMELandmarkCloud *)p6->GetParent())->GetLandmark(m_p6LandmarkName));
//			plan3->Setm_p1LName(m_p6LandmarkName);

			 p6->GetPoint(m_PlanePoint2[0], m_PlanePoint2[1], m_PlanePoint2[2],-1);
		}
		else
			m_p6LandmarkName = p6 ? p6->GetName() : _L("none");
			
	}
	
/*	if (plan1 !=NULL)
		gline1->SetPlane(plan1, plan1->GetName());
	if (plan2 != NULL)
		gline2->SetPlane(plan2, plan2->GetName());
	if (plan3 != NULL)
		gline3->SetPlane(plan3, plan3->GetName());
	*/
		
}

mafVMELandmark *mafVMEOsteometricBoard::GetLandmarkVME(mafString str)
//-------------------------------------------------------------------------
{


	
	return mafVMELandmark::SafeDownCast(GetLink(str));
}



void mafVMEOsteometricBoard::SetSurfaceLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{


	
	if (n->IsMAFType(mafVMESurface))
		SetLink(link_name, n);
	else
		SetLink(link_name, NULL);
}

mafVMESurface *mafVMEOsteometricBoard::GetSurfaceVME()
//-------------------------------------------------------------------------
{


	
	return mafVMESurface::SafeDownCast(GetLink(_R("SurfaceVME")));
}