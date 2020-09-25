/*=========================================================================

 Program: MAF2
 Module: mafVMEGravityLine
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "wx/busyinfo.h"
#include "mafVMEGravityLine.h"
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
#include "mafRWI.h"
#include "mafGUIDialogPreview.h"
#include <math.h> 
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafOpAddLandmark.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPoints.h"
#include "vtkActor.h"


#include "vtkXYPlotActor.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"


#include "mafVMESurface.h"

#include "mafPointCloud.hpp"
#include <vector>

#include "mafVMEMeter.h"
#include "mafVMEPlane.h"

//const bool DEBUG_MODE = true;


mafCxxTypeMacro(mafVMEGravityLine);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMEGravityLine::mafVMEGravityLine()
//-------------------------------------------------------------------------
{

	// wxBusyInfo wait001("constructor");
	// Sleep(1500);
	centerTemp = new double[3];
	centerTemp[0] = 0; centerTemp[01] = 0; centerTemp[02] = 0;
	m_SurfaceName = "";
	m_P0LandmarkName = "";
//	m_PlaneVmeName = "";
	mafString gLength = "";

	mafNEW(parallelPlane);
	parallelPlane->SetName("parallelPlane");
	
	parallelPlane->ReparentTo(this);
	//parallelPlane->DisableGuiPlane();
    P0 = mafVMELandmark::New();
    P0->SetName("P0");



  mafNEW(m_Cloud2);

  m_Cloud2->Open();
  m_Cloud2->SetName(_("computed_points"));
  m_Cloud2->SetRadius(1.5);
  m_Cloud2->ReparentTo(this);
  m_Cloud2->AppendLandmark(1, 0, 0, "projectedPt", false);
//  m_Cloud2->AppendLandmark(1, 0, 0, "parallelPlanePt1", false);
 // m_Cloud2->AppendLandmark(1, 0, 0, "parallelPlanePt2", false);
  //m_Cloud2->AppendLandmark(1, 0, 0, "parallelPlanePt3", false);

  vtkNEW(m_Goniometer);

  m_Goniometer->Update();



  //plan = NULL;
  mafNEW(plan);
  plan->SetName("FPlane");
  plan->ReparentTo(this);
  double* pt1; pt1 = new double[3]; pt1[0] = 0; pt1[01] = 0; pt1[02] = 0;
  plan->setPoint1(pt1);
  double* pt2; pt2 = new double[3]; pt2[0] = 10; pt2[01] = 0; pt2[02] = 0;
  plan->setPoint2(pt2);
  double* pt3; pt3 = new double[3]; pt3[0] = 0; pt3[01] = 10; pt3[02] = 0;
  plan->setPoint3(pt3);

  rotationMat = Eigen::Matrix3d::Identity();

  //////


  mafString plot_title = _("Density vs. Length (mm)");
  mafString plot_titleX = "mm";
  mafString plot_titleY = _("Dens.");
  vtkNEW(m_PlotActor);
  m_PlotActor->GetProperty()->SetColor(0.02, 0.06, 0.62);
  m_PlotActor->GetProperty()->SetLineWidth(2);
  m_PlotActor->SetPosition(0.03, 0.03);
  m_PlotActor->SetPosition2(0.9, 0.9);
  m_PlotActor->SetLabelFormat("%g");
  m_PlotActor->SetXRange(0, 300);
  m_PlotActor->SetPlotCoordinate(0, 300);
  m_PlotActor->SetNumberOfXLabels(10);
  m_PlotActor->SetXValuesToIndex();
  m_PlotActor->SetTitle(plot_title);
  m_PlotActor->SetXTitle(plot_titleX);
  m_PlotActor->SetYTitle(plot_titleY);
  vtkTextProperty* tprop = m_PlotActor->GetTitleTextProperty();
  tprop->SetColor(0.02, 0.06, 0.62);
  tprop->SetFontFamilyToArial();
  tprop->ItalicOff();
  tprop->BoldOff();
  tprop->SetFontSize(12);
  m_PlotActor->SetPlotColor(0, .8, .3, .3);

  m_PlotActor->SetXRange(0, 100);
  double srY[2];
 
  m_PlotActor->SetYRange(20,50);
  m_PlotActor->SetPlotCoordinate(0, 100);














  //////

	mafNEW(m_Transform);
	mafVMEOutputPolyline* output = mafVMEOutputPolyline::New();
	//mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
	//mafVMEOutputMuscleWrapping* output = mafVMEOutputMuscleWrapping::New();
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);
	DependsOnLinkedNodeOn();
  // attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	m_PolyData->DeepCopy(m_Goniometer->GetOutput());
	m_PolyData->Update();
	dpipe->SetInput(m_PolyData);
	
	SetDataPipe(dpipe);



}

//-------------------------------------------------------------------------
mafVMEGravityLine::~mafVMEGravityLine()
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait001("desturctor");
	//Sleep(1500);
	vtkDEL(m_PolyData);
    mafDEL(m_Transform);
	SetOutput(NULL);
	
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEGravityLine::GetMaterial()
//-------------------------------------------------------------------------
{

	//wxBusyInfo wait001("material");
	//Sleep(1500);
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}
//-------------------------------------------------------------------------
int mafVMEGravityLine::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
	//wxBusyInfo wait001("deepcopy");
	//Sleep(1500);

  if (Superclass::DeepCopy(a)==MAF_OK)
  {
	  mafVMEGravityLine *vmeGravityLine = mafVMEGravityLine::SafeDownCast(a);
	  m_Transform->SetMatrix(vmeGravityLine->m_Transform->GetMatrix());
	
	 
	  
	  this->rotationMat = vmeGravityLine->rotationMat;
	  this->m_P0LandmarkName = vmeGravityLine->m_P0LandmarkName;
	  this->m_SurfaceName = vmeGravityLine->m_SurfaceName;
	 
	  //this->plan = vmeGravityLine->plan;


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
bool mafVMEGravityLine::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{

	//wxBusyInfo wait001("equals");
	//Sleep(1500);
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    if (     
		m_Transform->GetMatrix() == ((mafVMEGravityLine *)vme)->m_Transform->GetMatrix() &&
		

		this->rotationMat == ((mafVMEGravityLine *)vme)->rotationMat


      )
    {
      ret = true;
    }
  }
 return ret;
}

mafVMEOutputPolyline *mafVMEGravityLine::GetPolylineOutput()
//-------------------------------------------------------------------------
{

	//wxBusyInfo wait001("getpolymine");
	//Sleep(1500);
	return (mafVMEOutputPolyline *)GetOutput();
	//return (mafVMEOutputPolyline *)m_Goniometer->GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEGravityLine::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait001("setmatrix");
	//Sleep(1500);
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMEGravityLine::IsAnimated()
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait001("isanimated");
	//Sleep(1500);
	return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafGUI* mafVMEGravityLine::CreateGui()
//-------------------------------------------------------------------------
{
//	wxBusyInfo wait001("createGUi");
//	Sleep(1500);
	m_Gui=mafNode::CreateGui();
  //if(m_Gui)
  //{
   // mafString geometryType[2] =// {"Points_selection1", "Points_selection2", "Points_selection3", "Points_selection4", "Points_selection5", "sphere", "Points_selection7"};
	//{ "Points_selection1", "sphere" };
   // m_Gui->Combo(ID_GEOMETRY_TYPE, "", &m_GeometryType, 2, geometryType);
   // m_Gui->Divider(2);
	
	m_Gui->Label("Points Selection1");

//	m_Gui->Button(ID_PLANE_LINK, &m_PlaneVmeName, _("Plane"), _("Select Plane"));
	m_Gui->Button(ID_P0_LINK, &m_P0LandmarkName, _("Starting Point"), _("Select Starting Point"));
	m_Gui->Button(ID_Surface_LINK, &m_SurfaceName, _("Surface"), _("Select Surace"));

	//CreateGuiResult();
	//m_Gui->Divider(2);
	mafString ss = " ";
	gLength = ss.Append(std::to_string(distToPlan).c_str());
	m_Gui->Label(_("distance: "), &gLength, true);
    m_Gui->Divider(2);
   // CreateGuiSphere();
 
	


    
  //  EnableQuadricSurfaceGui(m_GeometryType);
    m_Gui->Update();

	
  //}

  m_Gui->Divider();

 
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEGravityLine::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait001("onevent");
	//Sleep(1500);

  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {


	  string ee;
	  if (e->GetVme() == NULL)
		 ee = "VME NOT found";
	  else
	  {
		  ee = e->GetVme()->GetName();
		  InternalUpdate();
	  }
	  
	  
	//  wxBusyInfo wait001(ee.c_str());
	//  Sleep(1500);
	 
    switch(e->GetId())
    {
		m_Gui->Update();
		m_Gui->FitGui();
		//////////////////



/*	case ID_PLANE_LINK:
	{

						  mafID button_id = e->GetId();
						  mafString title = _("Choose plane vme link");
						  e->SetId(VME_CHOOSE);
						  e->SetArg((long)&mafVMEPlane::VMEAccept);
						  e->SetString(&title);
						  ForwardUpEvent(e);
						  mafNode *n = e->GetVme();
						  if (n != NULL)
						  {

							  SetPlaneLink("PlaneVME", n);
							  m_PlaneVmeName = n->GetName();
							  
							  if (n->IsMAFType(mafVMEPlane))
								  plan = (mafVMEPlane*)n;

							  m_Gui->Update();

						  }
	}
		break;
	
	*/
	case ID_Surface_LINK:
	  {
						
								 mafID button_id = e->GetId();
								 mafString title = _("Choose surface vme link");
								 e->SetId(VME_CHOOSE);
								// e->SetArg((long)&mafVMESurface::VMEAccept);
								 e->SetString(&title);
								 ForwardUpEvent(e);
								 mafNode *n = e->GetVme();
								 if (n != NULL)
								 {
									
										 SetSurfaceLink("SurfaceVME", n);
										 m_SurfaceName = n->GetName();
										 if (n->IsMAFType(mafVMESurface))
											 surface = (mafVMESurface*)n;
										 
					
									 	 
									 m_Gui->Update();
									
								 }
	  }
		  break;
	  
	  
	  case ID_P0_LINK:
	  {

	//  wxBusyInfo wait040("ID P0...");
	//  Sleep(1500);
	  
	  mafID button_id = e->GetId();
	  mafString title = _("Choose P0 Landmark link");
	  e->SetId(VME_CHOOSE);

	  e->SetString(&title);
	 ForwardUpEvent(e);
	  mafNode *n = e->GetVme();
	  if (n != NULL)
	  {

		  //p0 = (mafVMELandmark*)n;
		  SetLandmarkLink("P0Landmark", n);
		  m_P0LandmarkName = n->GetName();

		  m_Gui->Update();
		  
	  }
	  }
		  break;
	  //case CHANGE_VALUE_SPHERE:
	  case CHANGE_VALUE_POINTS1:
	
      {
		m_Gui->Update();
        e->SetId(CAMERA_UPDATE);
        ForwardUpEvent(e);
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

 
  //InternalUpdate();
}
void mafVMEGravityLine::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
//-----------------------------------------------------------------------
void mafVMEGravityLine::InternalPreUpdate()
//-----------------------------------------------------------------------
{

	//wxBusyInfo wait001("internalpreupdate");
	//Sleep(1500);
	InternalInitialize();
}
//-----------------------------------------------------------------------

void mafVMEGravityLine::Update()
{

	//wxBusyInfo wait001("update");
	//Sleep(1500);
	if (m_Gui)
	{
		m_Gui->Update();

	}
	InternalUpdate();

}
/*void mafVMEGravityLine::SetPlane(mafVMEPlane* p, mafString a)
{
	//wxBusyInfo wait001("setplane");
	//Sleep(1500);
	//wxBusyInfo wait52(a.GetCStr());
	//Sleep(1500);
	SetPlaneLink("PlaneVME", (mafNode*)p);
	this->plan = p;
	this->m_PlaneVmeName = a;
	//wxBusyInfo wait5("end");
	//Sleep(1500);
	
}*/
void mafVMEGravityLine::SetSurfaceName(mafVMESurface* s,mafString a)
{
	//wxBusyInfo wait001("setsurfacename");
	//Sleep(1500);
	SetSurfaceLink("SurfaceVME", s);
	this->surface = s;
	this->m_SurfaceName = a;
	
	//assert(m_Gui);
	//m_Gui->Update();
}
void mafVMEGravityLine::InternalUpdate()
//-----------------------------------------------------------------------
{
	//wxBusyInfo wait001("internal update");
	//Sleep(1500);
	//wxBusyInfo wait40("gline update..");
	//Sleep(1500);
	UpdateLinks();
	mafTimeStamp currTs = GetTimeStamp();
	double xyzr[3];
	double m_Point1[3];
	double pPoint[3];

	if (plan != NULL)
	{
		//wxBusyInfo wait40("plan not null");
		//Sleep(1500);
	}
	bool test=true;
	if ((P0 != NULL) && (plan != NULL))
	{
		//wxBusyInfo wait40("P0 and Plan not NULL");
		//Sleep(1500);

		

		mafMatrix tm;
		mafTransform *m_TmpTransform;
		mafNEW(m_TmpTransform);
		((mafVMELandmark*) P0)->GetPoint(m_Point1, currTs);
		((mafVMELandmarkCloud *)P0->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);
		m_TmpTransform->SetMatrix(tm);
		m_TmpTransform->TransformPoint(m_Point1, m_Point1);
		
		
		//double* NewplanPt;
		//Newposition of the plan point
		//double pt3[3];
		double NewplanPt[3];

		//mafVMELandmark* Pt3vme = (mafVMELandmark*)plan->GetVMEPt1();
		//Pt3vme->GetPoint(pt3,  currTs);
		//((mafVMELandmarkCloud *)Pt3vme->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);
		//((mafVMELandmarkCloud *)Pt3vme->GetParent())->GetOutput()->GetMatrix(tm, currTs);
		//pt3[0] = plan->getPoint1()[0];
		//pt3[1] = plan->getPoint1()[1];
		//pt3[2] = plan->getPoint1()[2];
		//plan->GetOutput()->GetAbsMatrix(tm, currTs);
		//m_TmpTransform->SetMatrix(tm);		
		//m_TmpTransform->TransformPoint(pt3, NewplanPt);

	//	m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
	//	m_TmpTransform->TransformPoint(pt3, NewplanPt);

		//NewplanPt[0] = pt3[0];
		//NewplanPt[01] = pt3[01];
		//[02] = pt3[02];
		NewplanPt[0] = plan->getPoint1()[0];
		NewplanPt[01] = plan->getPoint1()[01];
		NewplanPt[02] = plan->getPoint1()[02];
		//string s = std::to_string(pt3[0]) + " " + std::to_string(pt3[1]) + " " + std::to_string(pt3[2]) + " " + std::to_string(NewplanPt[0]) + " " + std::to_string(NewplanPt[1]) + " " + std::to_string(NewplanPt[2]);
		//wxBusyInfo wait12(s.c_str());
		//Sleep(1500);
		PointToPlanPt1[0] = m_Point1[0] - NewplanPt[0];
		PointToPlanPt1[1] = m_Point1[1] - NewplanPt[1];
		PointToPlanPt1[2] = m_Point1[2] - NewplanPt[2];

		double* normal;
		normal = plan->getNormalVector();
		//string s1 = std::to_string(plan->getNormalVector()[0]) + " " + std::to_string(plan->getNormalVector()[1]) + " " + std::to_string(plan->getNormalVector()[2]);
		//wxBusyInfo wait121(s1.c_str());
		//Sleep(1500);

		distToPlan = (PointToPlanPt1[0] * normal[0] + PointToPlanPt1[1] * normal[1] + PointToPlanPt1[2] * normal[2]);
		//distToPlan = std::sqrt(std::pow((PointToPlanPt1[0] * plan->getNormalVector()[0]), 2.0) + std::pow((PointToPlanPt1[1] * plan->getNormalVector()[1]), 2) + std::pow((PointToPlanPt1[2] * plan->getNormalVector()[2]), 2));
		//if (PointToPlanPt1[0] * plan->getNormalVector()[0] + PointToPlanPt1[1] * plan->getNormalVector()[1] + PointToPlanPt1[2] * plan->getNormalVector()[2] < 0)
		//	distToPlan = -distToPlan;

		pPoint[0] = m_Point1[0] - normal[0] * (distToPlan);
		pPoint[1] = m_Point1[1] - normal[1] * (distToPlan);
		pPoint[2] = m_Point1[2] - normal[2] * (distToPlan);
		//local coordinates
		double local_start[3];
		double local_end[3];
		plan->GetOutput()->Update();
		plan->GetOutput()->GetAbsMatrix(tm, currTs);
		m_TmpTransform->SetMatrix(tm);
		m_TmpTransform->Invert();
		m_TmpTransform->TransformPoint(pPoint, local_start);
		m_TmpTransform->TransformPoint(m_Point1, local_end);
		
		//
		vtkLineSource     *m_LineSource = vtkLineSource::New();
		m_LineSource->SetPoint1(local_start);
		m_LineSource->SetPoint2(local_end);
		m_LineSource->Update();
		//if (m_Goniometer == NULL)
		//{
		//	wxBusyInfo wait400("m_Goniometer is NULL");
		//	Sleep(1500);
		//}
		//else
		//{
		//	wxBusyInfo wait400("m_Goniometer not null");
		//	Sleep(1500);
		//}

		
		m_Goniometer->SetInput(m_LineSource->GetOutput());
		m_Goniometer->Update();
		/////
		vtkPolyData *polydata = m_Goniometer->GetOutput();
		int num = m_Goniometer->GetOutput()->GetNumberOfPoints();
		int pointId[2];
		vtkMAFSmartPointer<vtkCellArray> cellArray;
		for (int i = 0; i< num; i++)
		{
			if (i > 0)
			{
				pointId[0] = i - 1;
				pointId[1] = i;
				cellArray->InsertNextCell(2, pointId);
			}
		}

		m_PolyData->SetPoints(m_Goniometer->GetOutput()->GetPoints());
		m_PolyData->SetLines(cellArray);
		m_PolyData->Update();
		////

		
		m_Cloud2->SetLandmark("projectedPt", local_start[0], local_start[01], local_start[02], currTs);


		m_Cloud2->Update();
		mafString ss = " ";
		gLength = ss.Append(std::to_string(std::abs(distToPlan)).c_str());
		//m_Gui->Update();

		double vectAdded[3];

		vectAdded[0] = normal[0] * distToPlan;
		vectAdded[1] = normal[1] * distToPlan;
		vectAdded[2] = normal[2] * distToPlan;

		//ParallelPlane
		double point[3];
		point[0] = plan->getPoint1()[0];
		point[1] = plan->getPoint1()[1];
		point[2] = plan->getPoint1()[2];
		m_TmpTransform->TransformPoint(point, point);
		
	//	m_Cloud2->SetLandmark("parallelPlanePt1", point[0] + vectAdded[0], point[01] + vectAdded[01], point[02] + vectAdded[02], currTs);
		parallelPlane->setPoint1(point);
		//parallelPlane->SetLandmarkLink("first_Point", m_Cloud2->GetLandmark("parallelPlanePt1"));
		//parallelPlane->Setm_p1LName("parallelPlanePt1");

		point[0] = plan->getPoint2()[0];
		point[1] = plan->getPoint2()[1];
		point[2] = plan->getPoint2()[2];
		m_TmpTransform->TransformPoint(point, point);
				
		//m_Cloud2->SetLandmark("parallelPlanePt2", point[0] + vectAdded[0], point[01] + vectAdded[01], point[02] + vectAdded[02], currTs);
		parallelPlane->setPoint2(point);
		//parallelPlane->SetLandmarkLink("second_Point", m_Cloud2->GetLandmark("parallelPlanePt2"));
		//parallelPlane->Setm_p2LName("parallelPlanePt2");


		point[0] = plan->getPoint3()[0];
		point[1] = plan->getPoint3()[1];
		point[2] = plan->getPoint3()[2];
		m_TmpTransform->TransformPoint(point, point);
		
		//m_Cloud2->SetLandmark("parallelPlanePt3", point[0] + vectAdded[0], point[01] + vectAdded[1], point[02] + vectAdded[02], currTs);
		parallelPlane->setPoint3(point);
		//parallelPlane->SetLandmarkLink("third_Point", m_Cloud2->GetLandmark("parallelPlanePt3"));
		//parallelPlane->Setm_p3LName("parallelPlanePt3");

		//parallelPlane->UpdateLinks();
		//parallelPlane->Update();
		//m_Cloud2->Update();

		parallelPlane->SetUVector(plan->GetUVector(0), 0);
		parallelPlane->SetUVector(plan->GetUVector(1), 1);
		parallelPlane->push(distToPlan);
		parallelPlane->Update();

		
		
		

	}
	else
	{
		
		if ((plan != NULL) && (surface != NULL))
		{
			//wxBusyInfo wait40("surface and plane ..");
			//Sleep(1500);
			
			int nbr = surface->GetSurfaceOutput()->GetVTKData()->GetNumberOfPoints();
			double* coord;
			double dist = 0;
			distToPlan = 0;
			coord = new double[3];
			int pt=0;

			double* normal;
			normal = plan->getNormalVector();


			mafMatrix tm;
			mafTransform *m_TmpTransform1;
			mafNEW(m_TmpTransform1);			
			//surface->GetSurfaceOutput()->GetAbsMatrix(tm, currTs);
			tm=surface->GetOutput()->GetAbsTransform()->GetMatrix();
			m_TmpTransform1->SetMatrix(tm);
			if (nbr > 0)
			{
				for (int i = 0; i < nbr; i++)

				{
					coord = surface->GetSurfaceOutput()->GetVTKData()->GetPoint(i);
					m_TmpTransform1->TransformPoint(coord, coord);
					PointToPlanPt1[0] = coord[0] - plan->getPoint1()[0];
					PointToPlanPt1[1] = coord[1] - plan->getPoint1()[1];
					PointToPlanPt1[2] = coord[2] - plan->getPoint1()[2];


					dist = (PointToPlanPt1[0] * normal[0] + PointToPlanPt1[1] * normal[1] + PointToPlanPt1[2] * normal[2]);
					if (std::abs(dist) > std::abs(distToPlan))
					{
						distToPlan = dist;
						pt = i;
					}
				}
			}
			else
			{
				;
				
				wxBusyInfo wait12("vertices extraction problem");
				Sleep(2500);
				test = false;
			}
		
			
			//wxBusyInfo wait120("vertices extraction ok");
			//Sleep(2500);
			if (test)

			{
				coord = surface->GetSurfaceOutput()->GetVTKData()->GetPoint(pt);
				m_TmpTransform1->TransformPoint(coord, coord);
				pPoint[0] = coord[0] - normal[0] * (distToPlan);
				pPoint[1] = coord[1] - normal[1] * (distToPlan);
				pPoint[2] = coord[2] - normal[2] * (distToPlan);



				//compute coordinates in local system
				double pPointLocal[3];
				double coordLocal[3];

				m_TmpTransform1->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
				m_TmpTransform1->Invert();
				m_TmpTransform1->TransformPoint(pPoint, pPointLocal);
				m_TmpTransform1->TransformPoint(coord, coordLocal);


				//	string sss = "visualization...";
				//	wxBusyInfo wait120s(sss.c_str());
				//	Sleep(2500);

				//

				vtkLineSource     *m_LineSource = vtkLineSource::New();
				m_LineSource->SetPoint1(pPointLocal);
				m_LineSource->SetPoint2(coordLocal);
				m_LineSource->Update();

				m_Goniometer->SetInput(m_LineSource->GetOutput());
				m_Goniometer->Update();
				/////
				vtkPolyData *polydata = m_Goniometer->GetOutput();
				int num = m_Goniometer->GetOutput()->GetNumberOfPoints();
				int pointId[2];
				vtkMAFSmartPointer<vtkCellArray> cellArray;
				for (int i = 0; i< num; i++)
				{
					if (i > 0)
					{
						pointId[0] = i - 1;
						pointId[1] = i;
						cellArray->InsertNextCell(2, pointId);
					}
				}

				m_PolyData->SetPoints(m_Goniometer->GetOutput()->GetPoints());
				m_PolyData->SetLines(cellArray);
				m_PolyData->Update();
				////

				//string sss4 = "visualization ok";
				//wxBusyInfo wait1204s(sss4.c_str());
				//Sleep(2500);

				m_Cloud2->SetLandmark("projectedPt", pPointLocal[0], pPointLocal[01], pPointLocal[02], currTs);
				m_Cloud2->Update();
				mafString ss = " ";
				gLength = ss.Append(std::to_string(std::abs(distToPlan)).c_str());
				//m_Gui->Update();

				//string sss40 = "computing parallel plane ...";
				//wxBusyInfo wait12040s(sss40.c_str());
				//Sleep(2500);

				//ParallelPlane
				double point[3];
				point[0] = plan->getPoint1()[0];
				point[1] = plan->getPoint1()[1];
				point[2] = plan->getPoint1()[2];
				
				double vectAdded[3];

				vectAdded[0] = normal[0] * distToPlan;
				vectAdded[1] = normal[1] * distToPlan;
				vectAdded[2] = normal[2] * distToPlan;
			//	m_Cloud2->SetLandmark("parallelPlanePt1", point[0] + vectAdded[0], point[01] + vectAdded[1], point[02] + vectAdded[2], currTs);
			parallelPlane->setPoint1(point);
			//	parallelPlane->SetLandmarkLink("first_Point", m_Cloud2->GetLandmark("parallelPlanePt1"));
			//	parallelPlane->Setm_p1LName("parallelPlanePt1");

				

				point[0] = plan->getPoint2()[0];
				point[1] = plan->getPoint2()[1];
				point[2] = plan->getPoint2()[2];
				
			//	m_Cloud2->SetLandmark("parallelPlanePt2", point[0] + vectAdded[0], point[01] + vectAdded[1], point[02] + vectAdded[2], currTs);
			parallelPlane->setPoint2(point);
			//	parallelPlane->SetLandmarkLink("second_Point", m_Cloud2->GetLandmark("parallelPlanePt2"));
			//	parallelPlane->Setm_p2LName("parallelPlanePt2");
				

				point[0] = plan->getPoint3()[0];
				point[1] = plan->getPoint3()[1];
				point[2] = plan->getPoint3()[2];
			parallelPlane->setPoint3(point);
				
			//	m_Cloud2->SetLandmark("parallelPlanePt3", point[0] + vectAdded[0], point[01] + vectAdded[1], point[02] + vectAdded[2], currTs);
			//	parallelPlane->SetLandmarkLink("third_Point", m_Cloud2->GetLandmark("parallelPlanePt3"));
			//	parallelPlane->Setm_p3LName("parallelPlanePt3");
			//	m_Cloud2->Update();

				parallelPlane->Update();


				parallelPlane->SetUVector(plan->GetUVector(0), 0);
				parallelPlane->SetUVector(plan->GetUVector(1), 1);
				parallelPlane->push(distToPlan);
				parallelPlane->Update();

				
				
				
				//string sss40 = "computing parallel plane ..." + std::to_string(pt[0]) + " " + std::to_string(pt[1]) + " " + std::to_string(pt[2]);
				//wxBusyInfo wait12040s(sss40.c_str());
				//Sleep(2500);

			}
		}

	}

	//wxBusyInfo wait405("INTERNAL UPDATE OK");
	//Sleep(1500);
  mafEvent ev(this,CAMERA_UPDATE);
  this->ForwardUpEvent(&ev);

}
//-----------------------------------------------------------------------
int mafVMEGravityLine::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{

	//wxBusyInfo wait001("internal store");
	//Sleep(1500);
//	Superclass::InternalStore(parent);
	if (Superclass::InternalStore(parent)==MAF_OK)
	{
		if (
					
 
			parent->StoreMatrix("Transform", &m_Transform->GetMatrix()) == MAF_OK 
				)
			{
			return MAF_OK;
		}
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEGravityLine::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{

	//wxBusyInfo wait001("internal restore");
	//Sleep(1500);
	if (Superclass::InternalRestore(node)==MAF_OK)
	{
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform", &matrix) == MAF_OK
		)
	  {
		  m_Transform->SetMatrix(matrix);
		 return MAF_OK;
	  }
	else{
		;
		//	wxBusyInfo wait400("pb restoring ...");
			//Sleep(2500);
	}
	}
  return MAF_ERROR;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//char** mafVMEGravityLine::GetIcon()
//-------------------------------------------------------------------------
//{
//  #include "mafVMEProcedural.xpm"
//  return mafVMEProcedural_xpm;
//}

/*void mafVMEGravityLine::CreateGuiSelectPoints()
{

	wxBusyInfo wait001("createGuiselectPoints");
	Sleep(1500);
	m_Gui = mafNode::CreateGui();
	m_Gui->Label("Points Selection1");
	
	m_Gui->Button(ID_PLANE_LINK, &m_PlaneVmeName, _("Plane"), _("Select Plane"));
	m_Gui->Button(ID_P0_LINK, &m_P0LandmarkName, _("Starting Point"), _("Select Starting Point"));
	m_Gui->Button(ID_Surface_LINK, &m_SurfaceName, _("Surface"), _("Select Surace"));
	
	
//	assert(m_Gui);
//	m_Gui->AddGui(m_GuiSelectPoints);
	m_Gui->Update();
}

*/







/*void mafVMEGravityLine::SetPlaneLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{

	//wxBusyInfo wait001("setplanelink");
	//Sleep(1500);
	if (n->IsMAFType(mafVMEPlane))
			SetLink(link_name, n);
	else 
		SetLink(link_name, NULL);

	
}*/
void mafVMEGravityLine::SetSurfaceLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{


	if (n->IsMAFType(mafVMESurface))
	{	
		SetLink(link_name, n);
		//wxBusyInfo wait001(" surfacelink ok");
		//Sleep(1500);

	}
	else
	{
		SetLink(link_name, NULL);
		//wxBusyInfo wait001("Not surfacelink");
		//Sleep(1500);
	}

	
}

void mafVMEGravityLine::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait001("settimestamp");
	//Sleep(1500);
	Superclass::SetTimeStamp(t);
	
	this->InternalUpdate();
	
}


void mafVMEGravityLine::SetLandmarkLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait001("setlandmarklink");
	//Sleep(1500);
	if (n->IsMAFType(mafVMELandmark))
	{
		SetLink(link_name, n, ((mafVMELandmarkCloud *)(n->GetParent()))->FindLandmarkIndex(n->GetName()));
	}
	else
		SetLink(link_name, NULL);
}


mafVME *mafVMEGravityLine::GetP0VME()
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink("P0Landmark"));
}
/*mafVMEPlane *mafVMEGravityLine::GetPlaneVME()
//-------------------------------------------------------------------------
{

	//wxBusyInfo wait001("getplanevme");
	//Sleep(1500);
	return mafVMEPlane::SafeDownCast(GetLink("PlaneVME"));
	//return ((mafVMEPlane*)GetParent());
}*/

mafVMESurface *mafVMEGravityLine::GetSurfaceVME()
//-------------------------------------------------------------------------
{
	//wxBusyInfo wait001("getsurfacevme");
	//Sleep(1500);
//	return ((mafVMESurface*)this->GetParent()->GetParent());

	return mafVMESurface::SafeDownCast(GetLink("SurfaceVME"));
}
void mafVMEGravityLine::UpdateLinks()
//-------------------------------------------------------------------------
{

//	 wxBusyInfo wait001("updatelinks");
//	 Sleep(1500);

	mafID sub_id = -1;
	mafNode* nd = NULL;


	/*plan = GetPlaneVME();

	

	if (plan && plan->IsMAFType(mafVMEPlane))
	{
	nd = GetLink("PlaneVME");
	m_PlaneVmeName = (nd != NULL) ? ((mafVMEPlane *)plan)->GetName() : _("none");
	//wxBusyInfo wait021("plane link ok");
	//Sleep(1500);

	}
	else
	{
		m_PlaneVmeName = plan ? plan->GetName() : _("none");
		//wxBusyInfo wait0012("plane link  not ok");
		//Sleep(1500);
	}
	*/


	P0 = GetP0VME();
	if (P0 && P0->IsMAFType(mafVMELandmark))
	{
		sub_id = GetLinkSubId("P0Landmark");
		m_P0LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)P0->GetParent())->GetLandmarkName(sub_id) : _("none");
		//wxBusyInfo wait0012("landmark link  ok");
		//Sleep(1500);
	}
	else
	{
		m_P0LandmarkName = P0 ? P0->GetName() : _("none");
		//wxBusyInfo wait0012("landmark link not  ok");
		//Sleep(1500);
	}



	surface = GetSurfaceVME();
	
	if (surface && surface->IsMAFType(mafVMESurface))
	{
		nd = GetLink("SurfaceVME");
		m_SurfaceName = (nd != NULL) ? ((mafVMESurface *)surface)->GetName() : _("none");
	//	wxBusyInfo wait0012("surface link  ok");
	//	Sleep(1500);
	}
	else
	{
		m_SurfaceName = surface ? surface->GetName() : _("none");
	//	wxBusyInfo wait0012("surface link  not ok");
	//	Sleep(1500);
	}
		
	//wxBusyInfo wait01("updatelinks end");
	//Sleep(1500);

}