/*=========================================================================

 Program: MAF2
 Module: mafVMEMuscleWrapping
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
#include "mafVMEMuscleWrapping.h"
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
#include "mafVMEEllipsoid.h"
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
#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPoints.h"
#include "vtkActor.h"
#include "mafVMEOutputMuscleWrapping.h"
//#include "mafdmLineSegment.hpp"

#include "mafPointCloud.hpp"
#include <vector>

#include "mafVMEMeter.h"


const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MAF_ID_IMP(mafVMESurfaceParametric::CHANGE_PARAMETER);   // Event rised by change parameter 

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEMuscleWrapping);
//------------------------------------------------------------------------------
int sgnwrapping(double x)
{
	if (x >= 0)
		return 1;
	else return -1;
}
//------------------------------------------------------------------------------
mafVMEMuscleWrapping::mafVMEMuscleWrapping()
//-------------------------------------------------------------------------
{

	
	centerTemp = new double[3];
	centerTemp[0] = 0; centerTemp[01] = 0; centerTemp[02] = 0;
	m_Q0LandmarkName="";
	m_P0LandmarkName = "";
	mafString gLength = "";
	globalError = 0;
	globalLength = 0;
	m_ComputeStateCheckbox = 01;
	rate = 100;
	intersectionInit = false;
  


  P0 = mafVMELandmark::New();
  P0->SetName("P0");
  P0 = NULL;
  Q0 = mafVMELandmark::New();
  Q0->SetName("Q0");
  Q0 = NULL;
  p2 = mafVMELandmark::New();
  p2->SetName("p2");
  
  q2 = mafVMELandmark::New();
  q2->SetName("q2");
  p = mafVMELandmark::New();
  p->SetName("p");
  q = mafVMELandmark::New();
  q->SetName("q");


  n1 = 0;
  n2 = 0;

 /* mafNEW(m_Cloud2);

  m_Cloud2->Open();
  m_Cloud2->SetName(_("Quadric_Muscle_Intersection"));
  m_Cloud2->SetRadius(1.5);
  m_Cloud2->ReparentTo(this);
  m_Cloud2->AppendLandmark(0, 0, 0, "p0", false);
  m_Cloud2->AppendLandmark(1, 1, 1, "q0", false);
  //m_Cloud2->AppendLandmark(0.5, 0.5, 0.5, "c0", false);
  m_Cloud2->AppendLandmark(1, 0, 0, "p", false);
  m_Cloud2->AppendLandmark(1, 1, 0, "q", false);
  m_Cloud2->AppendLandmark(1, 0, 0, "p2", false);
  m_Cloud2->AppendLandmark(1, 1, 0, "q2", false);
  m_Cloud2->AppendLandmark(0, 0, 0, "pIntersec", false);
  m_Cloud2->AppendLandmark(0, 0, 0, "qIntersec", false);*/
  //mafNEW(m_CloudPath1);

  //m_CloudPath1->Open();
  //m_CloudPath1->SetName(_("path1"));
  //m_CloudPath1->SetRadius(1.5);
  //m_CloudPath1->ReparentTo(this);
  //m_CloudPath1->AppendLandmark(0, 0, 0, "first", false);
  //m_CloudPath1->AppendLandmark(0, 0, 0, "last", false);
 // m_CloudPath1->AppendLandmark(0, 0, 0, "mm", false);
  
  //mafNEW(m_CloudPath2);

  //m_CloudPath2->Open();
  //m_CloudPath2->SetName(_("path2"));
  //m_CloudPath2->SetRadius(1.5);
  //m_CloudPath2->ReparentTo(this);
  //m_CloudPath2->AppendLandmark(0, 0, 0, "first", false);
  //m_CloudPath2->AppendLandmark(0, 0, 0, "last", false);
 // m_CloudPath2->AppendLandmark(0, 0, 0, "mm", false);


  vtkNEW(m_Goniometer);


  m_Goniometer->Update();


//  m_PointsR1 = 30.0;
//  m_PointsR2 = 30.0;
//  m_PointsR3 = 30.0;
//  m_PointsPhiRes = 10.0;
//  m_PointsThetaRes = 10.0;
  length = 80;

  ellip = NULL;
  ellip2 = NULL;

  rotationMat = Matrix3d::Identity();


  mafNEW(m_Transform);
  mafVMEOutputPolyline* output = mafVMEOutputPolyline::New();

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

	rate = 10000;

}

//-------------------------------------------------------------------------
mafVMEMuscleWrapping::~mafVMEMuscleWrapping()
//-------------------------------------------------------------------------
{
	vtkDEL(m_PolyData);
    mafDEL(m_Transform);
	SetOutput(NULL);
	
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEMuscleWrapping::GetMaterial()
//-------------------------------------------------------------------------
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapping::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 

  if (Superclass::DeepCopy(a)==MAF_OK)
  {
	  mafVMEMuscleWrapping *vmeQuadricSurface = mafVMEMuscleWrapping::SafeDownCast(a);
	  m_Transform->SetMatrix(vmeQuadricSurface->m_Transform->GetMatrix());

	 
	  this->globalLength = vmeQuadricSurface->globalLength;
	  this->rotationMat = vmeQuadricSurface->rotationMat;
	  this->m_P0LandmarkName = vmeQuadricSurface->m_P0LandmarkName;
	  this->m_Q0LandmarkName = vmeQuadricSurface->m_Q0LandmarkName;
	  this->m_ComputeStateCheckbox = vmeQuadricSurface->m_ComputeStateCheckbox;
	  this->ellip = vmeQuadricSurface->ellip;
	  this->ellip2 = vmeQuadricSurface->ellip2;


	 
	  

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_PolyData);
	  m_PolyData->Update();
    }
  
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEMuscleWrapping::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{

  bool ret = false;
  if (Superclass::Equals(vme))
  {
    if (     
		m_Transform->GetMatrix() == ((mafVMEMuscleWrapping *)vme)->m_Transform->GetMatrix() &&


		this->rotationMat == ((mafVMEMuscleWrapping *)vme)->rotationMat


      )
    {
      ret = true;
    }
  }
 return ret;
}

mafVMEOutputPolyline *mafVMEMuscleWrapping::GetPolylineOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapping::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMEMuscleWrapping::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafGUI* mafVMEMuscleWrapping::CreateGui()
//-------------------------------------------------------------------------
{

	
	m_Gui=mafVME::CreateGui();

  if(m_Gui)
  {

	m_Gui->SetListener(this);

	m_ComputeStateCheckbox = 1;
	
	m_Gui->Button(ID_ELLIPSOID_LINK, &m_EllipsoidVmeName, _("Quadric1"), _("Select Quadric1"));
	m_Gui->Button(ID_ELLIPSOID2_LINK, &m_EllipsoidVmeName2, _("Quadric2"), _("Select Quadric2"));
	m_Gui->Button(ID_P0_LINK, &m_P0LandmarkName, _("Starting Point"), _("Select Starting Point"));
	m_Gui->Button(ID_Q0_LINK, &m_Q0LandmarkName, _("Ending Point"), _("Select Ending Point"));
	m_Gui->Integer(ID_DRate, _("Discretization rate"), &rate);

	
	gLength = ss.Append( std::to_string(globalLength).c_str());
	gError = std::to_string(globalError).c_str();
	m_Gui->Label(_("distance: "), &gLength, true);
	m_Gui->Label(_("error: "), &gError, true);
    m_Gui->FitGui();
    m_Gui->Update();

	
  }



  //wxBusyInfo wait("GUI created...");
  return m_Gui;
}

double mafVMEMuscleWrapping::GetDistance()
{
	return globalLength;

}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapping::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{


  // events to be sent up or down in the tree are simply forwarded
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{


		string ee;
		if (e->GetVme() == NULL)
		{
			ee = "VME NOT found";
			
		}
	  else
	  {
		  ;
		  ee = e->GetVme()->GetName();
		  
		
	  }
	  
	  
	 
	 
    switch(e->GetId())
    {
//		EnableQuadricSurfaceGui(m_GeometryType);
		m_Gui->Update();
		m_Gui->FitGui();
		//////////////////



     /* case ID_GEOMETRY_TYPE:
      {  
        EnableQuadricSurfaceGui(m_GeometryType);
        m_Gui->Update();
        m_Gui->FitGui();
		//////////////////

		mafTimeStamp currTs = GetTimeStamp();

		wxBusyInfo wait040("ID GEometry...");
		Sleep(1500);

		//mafVME *P0 = GetP0VME();
		//mafVME *Q0 = GetQ0VME();
		//mafVME *Ellipse = GetEllipsoidVME();
		//mafVME *Ellipse2 = GetEllipsoidVME2();

		double xyzr[3];


	//  switch (m_GeometryType)
	//	{
	//	case PARAMETRIC_SPHERE://6
	//	{
	//							   ;
	//	}
	//		break;
		

	//	case POINTS_SELECTION_Ellipsoid1://points
	//	{
	//										 ;

	//	}
	//		break;

	//	}

      }
		  break;*/

	case ID_DRate:

	{
							   InternalUpdate();
							   e->SetId(CAMERA_UPDATE);
							   ForwardUpEvent(e);
	}
		break;
	  case ID_ELLIPSOID_LINK:
	  {
								
								 mafID button_id = e->GetId();
								 mafString title = _("Choose quadric1 vme link");
								 e->SetId(VME_CHOOSE);
								 e->SetArg((long)&mafQuadraticSurface::VMEAccept);
								 e->SetString(&title);
								 ForwardUpEvent(e);
								 mafNode *n = e->GetVme();
								 if (n != NULL)
								 {
									
										 SetEllipsoidLink("EllipVME", n);
										 m_EllipsoidVmeName = n->GetName();
										 
										 ellip = (mafQuadraticSurface*)n;
									 	 length = std::max(length,std::max(std::max(ellip->getSzZ(), ellip->getSzY()), ellip->getSzX()));
									 m_Gui->Update();
									
								 }

								 
								
	  }
		  break;
	  case ID_ELLIPSOID2_LINK:
	  {
								
								mafID button_id = e->GetId();
								mafString title = _("Choose quadric2 vme link");
								e->SetId(VME_CHOOSE);
								e->SetArg((long)&mafQuadraticSurface::VMEAccept);
								e->SetString(&title);
								ForwardUpEvent(e);
								mafNode *n = e->GetVme();
								if (n != NULL)
								{

									SetEllipsoidLink("EllipVME2", n);
									m_EllipsoidVmeName2 = n->GetName();
									ellip2 = (mafQuadraticSurface*)n;
									length = std::max(length,std::max(std::max(ellip2->getSzZ(), ellip2->getSzY()), ellip2->getSzX()));
									m_Gui->Update();
									
								}


								
	  }
		  break;
	  case ID_Q0_LINK:
	  {
		 
			mafID button_id = e->GetId();
			mafString title = _("Choose Q0 Landmark link");
			e->SetId(VME_CHOOSE);

			e->SetString(&title);
			ForwardUpEvent(e);
			mafNode *n = e->GetVme();
			if (n != NULL)
			{							 
				//q0 = (mafVMELandmark*)n;
				SetLandmarkLink("Q0Landmark", n);
				m_Q0LandmarkName = n->GetName();
				m_Gui->Update();							 
			}
			
	  
	  }
	  break;
	  case ID_P0_LINK:
	  {

		
	  
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
	  


      default:
        mafVME::OnEvent(maf_event);
    }
  }
  
  else
  {
    Superclass::OnEvent(maf_event);
  }

 
 
}
void mafVMEMuscleWrapping::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{

	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
//-----------------------------------------------------------------------
void mafVMEMuscleWrapping::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------


void mafVMEMuscleWrapping::InternalUpdate()
//-----------------------------------------------------------------------
{
	
	UpdateLinks();
	
	
	mafTimeStamp currTs = GetTimeStamp();
	double xyzr[3];
	n1 = 0;
	n2 = 0;
	globalError = 0;
	std::vector<mafQuadraticSurface*> surfaces;
	if (P0 != NULL && Q0 != NULL)
	{
		points->Reset();
		points->Initialize();
		mafMatrix tm;
		mafTransform *m_TmpTransform1;
		mafNEW(m_TmpTransform1);
		mafTransform *m_TmpTransform2;
		mafNEW(m_TmpTransform2);
		((mafVMELandmark *)P0)->GetPoint(m_StartPoint, currTs);
		((mafVMELandmarkCloud *)((mafVMELandmark *)P0)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

		m_TmpTransform1->SetMatrix(tm);
		m_TmpTransform1->TransformPoint(m_StartPoint, m_StartPoint);

		m_TmpTransform2->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
		m_TmpTransform2->Invert();
		m_TmpTransform2->TransformPoint(m_StartPoint, local_start);


		//
		//local_start[0]=m_StartPoint[0];
		//local_start[1] = m_StartPoint[1];
		//local_start[2] = m_StartPoint[2];
		//			
		((mafVMELandmark *)Q0)->GetPoint(m_EndPoint, currTs);
		((mafVMELandmarkCloud *)((mafVMELandmark *)Q0)->GetParent())->GetOutput()->GetAbsMatrix(tm, currTs);

		m_TmpTransform1->SetMatrix(tm);
		m_TmpTransform1->TransformPoint(m_EndPoint, m_EndPoint);

		m_TmpTransform2->TransformPoint(m_EndPoint, local_end);

		//local_end[0] = m_EndPoint[0];
		//local_end[1] = m_EndPoint[1];
		//local_end[2] = m_EndPoint[2];
		//m_Cloud2->SetLandmark("q0", local_end[0], local_end[1], local_end[2], currTs);
		//m_Cloud2->Update();
		LMFunctor functor;
		functor.p0 << m_StartPoint[0], m_StartPoint[1], m_StartPoint[2];
		functor.q0 << m_EndPoint[0], m_EndPoint[1], m_EndPoint[2];
		
		int iteration = 0;
		
		
		
		if (ellip != NULL  && ellip2 != NULL)
		{

			

			globalError = 21;
			double** intersectionpts;
			double** intersectionpts2;
			double dist1 = sqrt(pow((ellip->GetCenter()[0] - m_StartPoint[0]), 2) + pow((ellip->GetCenter()[1] - m_StartPoint[1]), 2) + pow((ellip->GetCenter()[2] - m_StartPoint[2]), 2));
			double dist2 = sqrt(pow((ellip2->GetCenter()[0] - m_StartPoint[0]), 2) + pow((ellip2->GetCenter()[1] - m_StartPoint[1]), 2) + pow((ellip2->GetCenter()[2] - m_StartPoint[2]), 2));
			mafQuadraticSurface* ellip1;
			mafQuadraticSurface* ellip10;
			if (dist1 < dist2)
			{
				ellip1 = ellip;
				ellip10 = ellip2;
			}
			else
			{
				ellip1 = ellip2;
				ellip10 = ellip;
			}
			intersectionpts = computeLineIntersection(m_StartPoint, m_EndPoint, &n1, ellip1);


			if (n1 == 0)
			{

				intersectionpts2 = computeLineIntersection(m_StartPoint, m_EndPoint, &n2, ellip10);
				if (n2 == 0)
				{

					//double pos1[3];
					//double pos2[3];

					vtkPoints* pts1 = vtkPoints::New();
					vtkPoints* pts2 = vtkPoints::New();
					vtkPoints* pts3 = vtkPoints::New();

					//local points already computed
					pts1->InsertNextPoint(local_start);
					pts1->InsertNextPoint(local_end);

					mafVMELineSeg* line13 = mafVMELineSeg::New();
					line13->SetName("line13");

					line13->setPoints(pts1);
					line13->GetOutput()->Update();
					line13->Update();
					globalLength = sqrt((local_end[0] - local_start[0])*(local_end[0] - local_start[0]) + (local_end[1] - local_start[1])*(local_end[1] - local_start[1]) + (local_end[2] - local_start[2])*(local_end[2] - local_start[2]));
					//gLength = globalLength;
					mafString ss = " ";
					gLength = ss.Append(std::to_string(globalLength).c_str());
					gError = std::to_string(globalError).c_str();
					//m_Gui->Update();


					m_Goniometer->RemoveAllInputs();
					m_Goniometer->AddInput(line13->getVTKPolydata());
					m_Goniometer->Update();
					m_PolyData->DeepCopy(m_Goniometer->GetOutput());

					pts1->Delete();
					line13->Delete();
				}
				else //n2==1
				{


					vtkPoints* pts1 = vtkPoints::New();
					vtkPoints* pts2 = vtkPoints::New();
					vtkPoints* pts5 = vtkPoints::New();
					VectorXd X(6);

					surfaces.push_back(ellip10);
					X << intersectionpts2[0][0], intersectionpts2[0][1], intersectionpts2[0][2], intersectionpts2[1][0], intersectionpts2[1][1], intersectionpts2[1][2];
					//m_Cloud2->SetLandmark("pIntersec", intersectionpts2[0][0], intersectionpts2[0][1], intersectionpts2[0][2], currTs);
					//m_Cloud2->Update();
					//m_Cloud2->SetLandmark("qIntersec", intersectionpts2[1][0], intersectionpts2[1][1], intersectionpts2[1][2], currTs);
					//m_Cloud2->Update();					

					//LMFunctor functor;
					functor.surfaces = &surfaces;

					functor.m = 6;
					functor.n = 6;

			

				
					Eigen::LevenbergMarquardt<LMFunctor> lm(functor);
					lm.resetParameters();
					int r = lm.minimize(X);

					double d1, d2;
					double positionP[3];
					double positionQ[3];
					d1 = sqrt((X[0] - m_StartPoint[0])*(X[0] - m_StartPoint[0]) + (X[1] - m_StartPoint[1])*(X[1] - m_StartPoint[1]) + (X[2] - m_StartPoint[2])*(X[2] - m_StartPoint[2]));
					d2 = sqrt((X[3] - m_StartPoint[0])*(X[3] - m_StartPoint[0]) + (X[4] - m_StartPoint[1])*(X[4] - m_StartPoint[1]) + (X[5] - m_StartPoint[2])*(X[5] - m_StartPoint[2]));


					if (d1 < d2)
					{
						p->SetPoint(X(0), X(1), X(2), currTs);
						p->Update();
						positionP[0] = X(0);
						positionP[01] = X(01);
						positionP[02] = X(02);

						q->SetPoint(X(3), X(4), X(5), currTs);
						q->Update();
						positionQ[0] = X(3);
						positionQ[01] = X(4);
						positionQ[02] = X(5);
					}
					else
					{
						p->SetPoint(X(3), X(4), X(5), currTs);
						p->Update();
						positionP[0] = X(3);
						positionP[01] = X(4);
						positionP[02] = X(5);

						q->SetPoint(X(0), X(1), X(2), currTs);
						q->Update();
						positionQ[0] = X(0);
						positionQ[01] = X(1);
						positionQ[02] = X(2);

					}

				
					double local_posPt[3];
					double posPt[3];
					vector<Vector3d> path;
					path = ellip10->computeGeodesicPath((mafVMELandmark*)P0, (mafVMELandmark*)Q0, p, q, 2 * length,rate ,&globalError);

					double wrappingDistance = 0;

					double pointP[3] = { positionP[0], positionP[1], positionP[2] };
					double localP[3];


					double tempdist;
					if (path.size() > 1)
					if (m_ComputeStateCheckbox)
					{
						posPt[0] = positionP[0];
						posPt[1] = positionP[1];
						posPt[2] = positionP[2];

						m_TmpTransform2->TransformPoint(posPt, local_posPt);
						pts5->InsertNextPoint(local_posPt);
						//pts5->InsertNextPoint(posPt);

						for (int i = 0; i < path.size(); i++)
						{
							//wxBusyInfo wait20("checkbox ok...");
							//Sleep(1500);
							tempdist = sqrt((path[i][0] - posPt[0])*(path[i][0] - posPt[0]) + (path[i][1] - posPt[1])*(path[i][1] - posPt[1]) + (path[i][2] - posPt[2])*(path[i][2] - posPt[2]));
							if (tempdist < 1000)
							{
								wrappingDistance = wrappingDistance + tempdist;

								posPt[0] = path[i][0];
								posPt[1] = path[i][1];
								posPt[2] = path[i][2];

								m_TmpTransform2->TransformPoint(posPt, local_posPt);
								pts5->InsertNextPoint(local_posPt);
							}

						}
					}

					//int idx1 = m_Cloud2->FindLandmarkIndex("p0");
					//int idx3 = m_Cloud2->FindLandmarkIndex("q0");
					//int idx2 = m_Cloud2->FindLandmarkIndex("p");
					//int idx4 = m_Cloud2->FindLandmarkIndex("q");;
					//m_Cloud2->GetLandmarkPosition(idx1, pos1, currTs);
					//m_Cloud2->GetLandmarkPosition(idx2, pos2, currTs);

					pts1->InsertNextPoint(local_start[0], local_start[1], local_start[2]);
					//local P


					m_TmpTransform2->TransformPoint(pointP, localP);

					pts1->InsertNextPoint(localP);
					mafVMELineSeg* line13 = mafVMELineSeg::New();
					//line13->SetName("line13");
					line13->resetPoints();
					line13->setPoints(pts1);
					line13->GetOutput()->Update();
					line13->Update();

					globalLength = sqrt((positionP[0] - m_StartPoint[0])*(positionP[0] - m_StartPoint[0]) + (positionP[1] - m_StartPoint[1])*(positionP[1] - m_StartPoint[1]) + (positionP[2] - m_StartPoint[2])*(positionP[2] - m_StartPoint[2]));

					//m_Cloud2->GetLandmarkPosition(idx3, pos3, currTs);
					//m_Cloud2->GetLandmarkPosition(idx4, pos4, currTs);

					double pointQ[3] = { positionQ[0], positionQ[1], positionQ[2] };
					double localQ[3];
					m_TmpTransform2->TransformPoint(pointQ, localQ);
					pts2->InsertNextPoint(localQ);
					pts2->InsertNextPoint(local_end[0], local_end[1], local_end[2]);

					globalLength = globalLength + sqrt((m_EndPoint[0] - positionQ[0])*(m_EndPoint[0] - positionQ[0]) + (m_EndPoint[1] - positionQ[1])*(m_EndPoint[1] - positionQ[1]) + (m_EndPoint[2] - positionQ[2])*(m_EndPoint[2] - positionQ[2]));
					mafVMELineSeg* line14 = mafVMELineSeg::New();
					//line14->SetName("line14");
					line14->resetPoints();
					line14->setPoints(pts2);
					line14->GetOutput()->Update();
					line14->Update();
					mafVMELineSeg* line15 = mafVMELineSeg::New();
					//line15->SetName("line15");
					if (path.size() > 1)
					{

						line15->resetPoints();
						line15->setPoints(pts5);
						line15->GetOutput()->Update();
						line15->Update();
					}

					globalLength = globalLength + wrappingDistance;
					//gLength = globalLength;
					mafString ss = " ";
					gLength = ss.Append(std::to_string(globalLength).c_str());
					gError = std::to_string(globalError).c_str();

					m_Goniometer->RemoveAllInputs();
					m_Goniometer->AddInput(line13->getVTKPolydata());
					//if (path.size()>1)
					m_Goniometer->AddInput(line15->getVTKPolydata());
					m_Goniometer->AddInput(line14->getVTKPolydata());
					m_Goniometer->Update();
					m_PolyData->DeepCopy(m_Goniometer->GetOutput());


					//functor.m = 0;
					//functor.n = 0;
					functor.surfaces = NULL;
					//functor.p0 << 0, 0, 0;
					//functor.q0 << 0, 0, 0;


					path.clear();
					pts1->Delete();
					pts2->Delete();
					pts5->Delete();
					line14->Delete();
					line15->Delete();
					line13->Delete();

				}
			}
			else //n1==1
			{


				vtkPoints* pts1 = vtkPoints::New();
				vtkPoints* pts2 = vtkPoints::New();
				vtkPoints* pts5 = vtkPoints::New();
				VectorXd X(6);

				surfaces.push_back(ellip1);
				X << intersectionpts[0][0], intersectionpts[0][1], intersectionpts[0][2], intersectionpts[1][0], intersectionpts[1][1], intersectionpts[1][2];
		


				//LMFunctor functor;
				functor.surfaces = &surfaces;
				functor.m = 6;
				functor.n = 6;
		

				Eigen::LevenbergMarquardt<LMFunctor> lm(functor);
				int r = lm.minimize(X);
				double d1, d2;
				double positionP[3];
				double positionQ[3];
				d1 = sqrt((X[0] - m_StartPoint[0])*(X[0] - m_StartPoint[0]) + (X[1] - m_StartPoint[1])*(X[1] - m_StartPoint[1]) + (X[2] - m_StartPoint[2])*(X[2] - m_StartPoint[2]));
				d2 = sqrt((X[3] - m_StartPoint[0])*(X[3] - m_StartPoint[0]) + (X[4] - m_StartPoint[1])*(X[4] - m_StartPoint[1]) + (X[5] - m_StartPoint[2])*(X[5] - m_StartPoint[2]));


				if (d1 < d2)
				{
					p->SetPoint(X(0), X(1), X(2), currTs);
					p->Update();
					positionP[0] = X(0);
					positionP[01] = X(01);
					positionP[02] = X(02);

					q->SetPoint(X(3), X(4), X(5), currTs);
					q->Update();
					positionQ[0] = X(3);
					positionQ[01] = X(4);
					positionQ[02] = X(5);
				}
				else
				{
					p->SetPoint(X(3), X(4), X(5), currTs);
					p->Update();
					positionP[0] = X(3);
					positionP[01] = X(4);
					positionP[02] = X(5);

					q->SetPoint(X(0), X(1), X(2), currTs);
					q->Update();
					positionQ[0] = X(0);
					positionQ[01] = X(1);
					positionQ[02] = X(2);

				}

				
				double local_posPt[3];
				double starting2[3];
				q->GetOutput()->GetAbsPose(starting2, xyzr, currTs);
				intersectionpts2 = computeLineIntersection(starting2, m_EndPoint, &n2, ellip10);
				if (n2 == 0)
				{

					double posPt[3];
					vector<Vector3d> path;
					path = ellip1->computeGeodesicPath((mafVMELandmark*)P0, (mafVMELandmark*)Q0, p, q, 2 * length,rate ,&globalError);
					double wrappingDistance = 0;


					double tempdist;
					if (path.size() > 5)
					if (m_ComputeStateCheckbox)
					{
						posPt[0] = positionP[0];
						posPt[1] = positionP[1];
						posPt[2] = positionP[2];
					
						m_TmpTransform2->TransformPoint(posPt, local_posPt);
						pts5->InsertNextPoint(local_posPt);


						for (int i = 0; i < path.size(); i++)
						{
						
							tempdist = sqrt((path[i][0] - posPt[0])*(path[i][0] - posPt[0]) + (path[i][1] - posPt[1])*(path[i][1] - posPt[1]) + (path[i][2] - posPt[2])*(path[i][2] - posPt[2]));
							if (tempdist < 1000)
							{
								wrappingDistance = wrappingDistance + tempdist;

								posPt[0] = path[i][0];
								posPt[1] = path[i][1];
								posPt[2] = path[i][2];
								

								m_TmpTransform2->TransformPoint(posPt, local_posPt);
								pts5->InsertNextPoint(local_posPt);
							}

						}
					}


		

					pts1->InsertNextPoint(local_start[0], local_start[1], local_start[2]);

					posPt[0] = positionQ[0];
					posPt[1] = positionQ[1];
					posPt[2] = positionQ[2];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts1->InsertNextPoint(local_posPt);
					
					mafVMELineSeg* line13 = mafVMELineSeg::New();
					
					line13->resetPoints();
					line13->setPoints(pts1);
					line13->GetOutput()->Update();
					line13->Update();

					globalLength = sqrt((positionP[0] - m_StartPoint[0])*(positionP[0] - m_StartPoint[0]) + (positionP[1] - m_StartPoint[1])*(positionP[1] - m_StartPoint[1]) + (positionP[2] - m_StartPoint[2])*(positionP[2] - m_StartPoint[2]));

				


					posPt[0] = positionQ[0];
					posPt[1] = positionQ[1];
					posPt[2] = positionQ[2];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts2->InsertNextPoint(local_posPt);
					pts2->InsertNextPoint(local_end[0], local_end[1], local_end[2]);

					globalLength = globalLength + sqrt((m_EndPoint[0] - positionQ[0])*(m_EndPoint[0] - positionQ[0]) + (m_EndPoint[1] - positionQ[1])*(m_EndPoint[1] - positionQ[1]) + (m_EndPoint[2] - positionQ[2])*(m_EndPoint[2] - positionQ[2]));

					mafVMELineSeg* line14 = mafVMELineSeg::New();
					//line14->SetName("line14");
					line14->resetPoints();
					line14->setPoints(pts2);
					line14->GetOutput()->Update();
					line14->Update();

					mafVMELineSeg* line15 = mafVMELineSeg::New();
					//line15->SetName("line15");
					if (path.size() > 1)
					{
						line15->resetPoints();
						line15->setPoints(pts5);
						line15->GetOutput()->Update();
						line15->Update();
					}

					globalLength = globalLength + wrappingDistance;
					//gLength = globalLength;

					mafString ss = " ";
					gLength = ss.Append(std::to_string(globalLength).c_str());
					gError = std::to_string(globalError).c_str();

					m_Goniometer->RemoveAllInputs();
					m_Goniometer->AddInput(line13->getVTKPolydata());
					if (path.size() > 1)
						m_Goniometer->AddInput(line15->getVTKPolydata());
					m_Goniometer->AddInput(line14->getVTKPolydata());

					m_Goniometer->Update();
					m_PolyData->DeepCopy(m_Goniometer->GetOutput());


					//functor.m = 0;
					//functor.n = 0;
					functor.surfaces = NULL;
					//functor.p0 << 0, 0, 0;
					//functor.q0 << 0, 0, 0;


					path.clear();
					pts1->Delete();
					pts2->Delete();
					pts5->Delete();
					line14->Delete();
					line15->Delete();
					line13->Delete();


				}//end one intersecion


				else // (((n1 == 1) && (n2 == 1)) //2 intersections

				{

					//LMFunctor functor;
					functor.m = 12;
					functor.n = 12;
					VectorXd X(12);
					X << intersectionpts[0][0], intersectionpts[0][1], intersectionpts[0][2], intersectionpts[1][0], intersectionpts[1][1], intersectionpts[1][2], intersectionpts2[0][0], intersectionpts2[0][1], intersectionpts2[0][2], intersectionpts2[1][0], intersectionpts2[1][1], intersectionpts2[1][2];

				

					surfaces.push_back(ellip10);
					functor.surfaces = &surfaces;
					Eigen::LevenbergMarquardt<LMFunctor> lm(functor);
					int r = lm.minimize(X);



					double d1, d2,d3,d4;
					double positionP1[3];
					double positionQ1[3];
					double positionP2[3];
					double positionQ2[3];
					d1 = sqrt((X[0] - m_StartPoint[0])*(X[0] - m_StartPoint[0]) + (X[1] - m_StartPoint[1])*(X[1] - m_StartPoint[1]) + (X[2] - m_StartPoint[2])*(X[2] - m_StartPoint[2]));
					d2 = sqrt((X[3] - m_StartPoint[0])*(X[3] - m_StartPoint[0]) + (X[4] - m_StartPoint[1])*(X[4] - m_StartPoint[1]) + (X[5] - m_StartPoint[2])*(X[5] - m_StartPoint[2]));
					d3 = sqrt((X[6] - m_EndPoint[0])*(X[6] - m_EndPoint[0]) + (X[7] - m_EndPoint[1])*(X[7] - m_EndPoint[1]) + (X[8] - m_EndPoint[2])*(X[8] - m_EndPoint[2]));
					d4 = sqrt((X[9] - m_EndPoint[0])*(X[9] - m_EndPoint[0]) + (X[10] - m_EndPoint[1])*(X[10] - m_EndPoint[1]) + (X[11] - m_EndPoint[2])*(X[11] - m_EndPoint[2]));

					if (d1 < d2)
					{
						p->SetPoint(X(0), X(1), X(2), currTs);
						p->Update();
						positionP1[0] = X(0);
						positionP1[01] = X(01);
						positionP1[02] = X(02);

						q->SetPoint(X(3), X(4), X(5), currTs);
						q->Update();
						positionQ1[0] = X(3);
						positionQ1[01] = X(4);
						positionQ1[02] = X(5);
					}
					else
					{
						p->SetPoint(X(3), X(4), X(5), currTs);
						p->Update();
						positionP1[0] = X(3);
						positionP1[01] = X(4);
						positionP1[02] = X(5);

						q->SetPoint(X(0), X(1), X(2), currTs);
						q->Update();
						positionQ1[0] = X(0);
						positionQ1[01] = X(1);
						positionQ1[02] = X(2);

					}
					if (d3 > d4)
					{
						p2->SetPoint(X(6), X(7), X(8), currTs);
						p2->Update();
						positionP2[0] = X(6);
						positionP2[01] = X(7);
						positionP2[02] = X(8);

						q2->SetPoint(X(9), X(10), X(11), currTs);
						q2->Update();
						positionQ2[0] = X(9);
						positionQ2[01] = X(10);
						positionQ2[02] = X(11);
					}
					else
					{
						p2->SetPoint(X(9), X(10), X(11), currTs);
						p2->Update();
						positionP2[0] = X(9);
						positionP2[01] = X(10);
						positionP2[02] = X(11);

						q2->SetPoint(X(6), X(7), X(8), currTs);
						q2->Update();
						positionQ2[0] = X(6);
						positionQ2[01] = X(7);
						positionQ2[02] = X(8);

					}

					vtkPoints* pts1 = vtkPoints::New();
					vtkPoints* pts2 = vtkPoints::New();
					vtkPoints* pts3 = vtkPoints::New();
					vtkPoints* pts4 = vtkPoints::New();
					vtkPoints* pts5 = vtkPoints::New();

				

					//p->SetPoint(X(0), X(1), X(2), currTs);
					//q->SetPoint(X(3), X(4), X(5), currTs);
					//p2->SetPoint(X(6), X(7), X(8), currTs);
					//q2->SetPoint(X(9), X(10), X(11), currTs);
					//p->Update();
					//q->Update();
					//p2->Update();
					//q2->Update();

				

					double posPt[3];
					double local_posPt[3];
					vector<Vector3d> path = ellip1->computeGeodesicPath((mafVMELandmark*)P0, p2, p, q, 2 * length,rate, &globalError);
				

					vector<Vector3d> path2 = ellip10->computeGeodesicPath(q, (mafVMELandmark*)Q0, p2, q2, 2 * length,rate, &globalError);
			

					double wrappingDistance = 0;
					double tempdist = 0;
					if (path.size() > 1)
					if (m_ComputeStateCheckbox)
					{
						posPt[0] = positionP1[0];
						posPt[1] = positionP1[1];
						posPt[2] = positionP1[2];
						//pts5->InsertNextPoint(posPt);

						m_TmpTransform2->TransformPoint(posPt, local_posPt);
						pts5->InsertNextPoint(local_posPt);

						//m_CloudPath1->SetLandmark("first",posPt[0], posPt[01], posPt[02], currTs);
						for (int i = 0; i < path.size(); i++)
						{
							tempdist = sqrt((path[i][0] - posPt[0])*(path[i][0] - posPt[0]) + (path[i][1] - posPt[1])*(path[i][1] - posPt[1]) + (path[i][2] - posPt[2])*(path[i][2] - posPt[2]));
							posPt[0] = path[i][0];
							posPt[1] = path[i][1];
							posPt[2] = path[i][2];

							m_TmpTransform2->TransformPoint(posPt, local_posPt);
							pts5->InsertNextPoint(local_posPt);
							//pts5->InsertNextPoint(posPt);
							wrappingDistance = wrappingDistance + tempdist;
							// m_CloudPath1->AppendLandmark(posPt[0], posPt[1], posPt[2], "mm", false);

						}

						posPt[0] = positionQ1[0];
						posPt[1] = positionQ1[1];
						posPt[2] = positionQ1[2];
						m_TmpTransform2->TransformPoint(posPt, local_posPt);
						pts5->InsertNextPoint(local_posPt);
						//pts5->InsertNextPoint(X(3), X(4), X(5));
						// m_CloudPath1->SetLandmark("last", X(3), X(4), X(5), currTs);
					}


					//m_CloudPath1->Update();

					if (path2.size() > 1)
					if (m_ComputeStateCheckbox)
					{
						posPt[0] = positionP2[0];
						posPt[1] = positionP2[1];
						posPt[2] = positionP2[2];
						//pts1->InsertNextPoint(posPt);

						m_TmpTransform2->TransformPoint(posPt, local_posPt);
						pts1->InsertNextPoint(local_posPt);


						//m_CloudPath2->SetLandmark("first",posPt[0], posPt[01], posPt[02],currTs);
						for (int i = 0; i < path2.size(); i++)
						{
							tempdist = sqrt((path2[i][0] - posPt[0])*(path2[i][0] - posPt[0]) + (path2[i][1] - posPt[1])*(path2[i][1] - posPt[1]) + (path2[i][2] - posPt[2])*(path2[i][2] - posPt[2]));
							posPt[0] = path2[i][0];
							posPt[1] = path2[i][1];
							posPt[2] = path2[i][2];

							m_TmpTransform2->TransformPoint(posPt, local_posPt);
							pts1->InsertNextPoint(local_posPt);

							// pts1->InsertNextPoint(posPt);
							wrappingDistance = wrappingDistance + tempdist;
							//m_CloudPath2->AppendPoint(posPt[0], posPt[01], posPt[02],-1);
							//m_CloudPath2->SetPoint("mm", posPt[0], posPt[01], posPt[02], -1);
						}
						//pts1->InsertNextPoint(X(9), X(10), X(11));

						posPt[0] = positionQ2[0];
						posPt[1] = positionQ2[1];
						posPt[2] = positionQ2[2];
						m_TmpTransform2->TransformPoint(posPt, local_posPt);
						pts1->InsertNextPoint(local_posPt);

						//m_CloudPath2->SetLandmark("last", X(9), X(10), X(11),currTs);
					}//end checkbox
					//m_CloudPath2->Update();


				
					//P0-P
					pts3->InsertNextPoint(local_start[0], local_start[1], local_start[2]);
					posPt[0] = positionP1[0];
					posPt[1] = positionP1[1];
					posPt[2] = positionP1[2];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts3->InsertNextPoint(local_posPt);
					globalLength = sqrt((positionP1[0] - m_StartPoint[0])*(positionP1[0] - m_StartPoint[0]) + (positionP1[1] - m_StartPoint[1])*(positionP1[1] - m_StartPoint[1]) + (positionP1[2] - m_StartPoint[2])*(positionP1[2] - m_StartPoint[2]));
					//q0 q2
					pts2->InsertNextPoint(local_end[0], local_end[1], local_end[2]);

					posPt[0] = positionQ2[0];
					posPt[1] = positionQ2[1];
					posPt[2] = positionQ2[2];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					//pts2->InsertNextPoint(X(9),X(10),X(11));
					pts2->InsertNextPoint(local_posPt);
					globalLength = globalLength + sqrt((positionQ2[0] - m_EndPoint[0])*(positionQ2[0] - m_EndPoint[0]) + (positionQ2[1] - m_EndPoint[1])*(positionQ2[1] - m_EndPoint[1]) + (positionQ2[2] - m_EndPoint[2])*(positionQ2[2] - m_EndPoint[2]));
					//q p2

					posPt[0] = positionQ1[0];
					posPt[1] = positionQ1[1];
					posPt[2] = positionQ1[2];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts4->InsertNextPoint(local_posPt);
					posPt[0] = positionP2[0];
					posPt[1] = positionP2[1];
					posPt[2] = positionP2[2];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts4->InsertNextPoint(local_posPt);
					globalLength = globalLength + sqrt((positionP2[0] - positionQ1[0])*(positionP2[0] - positionQ1[0]) + (positionP2[1] - positionQ1[1])*(positionP2[1] - positionQ1[1]) + (positionP2[2] - positionQ1[2])*(positionP2[2] - positionQ1[2]));

					mafVMELineSeg* line11 = mafVMELineSeg::New();
					//line11->SetName("line11");
					line11->resetPoints();
					line11->setPoints(pts1);
					line11->GetOutput()->Update();
					line11->Update();

					mafVMELineSeg* line12 = mafVMELineSeg::New();
					//line12->SetName("line12");
					line12->resetPoints();
					line12->setPoints(pts2);
					line12->GetOutput()->Update();
					line12->Update();

					mafVMELineSeg* line13 = mafVMELineSeg::New();
					//line13->SetName("line13");
					line13->resetPoints();
					line13->setPoints(pts3);
					line13->GetOutput()->Update();
					line13->Update();

					mafVMELineSeg* line14 = mafVMELineSeg::New();
					//line14->SetName("line14");
					line14->resetPoints();
					line14->setPoints(pts4);
					line14->GetOutput()->Update();
					line14->Update();


					mafVMELineSeg* line15 = mafVMELineSeg::New();
					//line15->SetName("line15");
					line15->resetPoints();
					line15->setPoints(pts5);
					line15->GetOutput()->Update();
					line15->Update();


					globalLength = globalLength + wrappingDistance;
					//gLength = globalLength;
					mafString ss = " ";
					gLength = ss.Append(std::to_string(globalLength).c_str());
					gError = std::to_string(globalError).c_str();


					m_Goniometer->RemoveAllInputs();
					m_Goniometer->AddInput(line13->getVTKPolydata());

					if (path.size() > 1)
						m_Goniometer->AddInput(line15->getVTKPolydata());

					m_Goniometer->AddInput(line14->getVTKPolydata());

					//if (path2.size()>1)
					m_Goniometer->AddInput(line11->getVTKPolydata());

					m_Goniometer->AddInput(line12->getVTKPolydata());

					m_Goniometer->Update();
					m_PolyData->DeepCopy(m_Goniometer->GetOutput());


					//functor.m = 0;
					//functor.n = 0;
					functor.surfaces = NULL;
					//functor.p0 << 0, 0, 0;
					//functor.q0 << 0, 0, 0;

					path2.clear();
					path.clear();
					pts1->Delete();
					pts2->Delete();
					pts3->Delete();
					pts4->Delete();
					pts5->Delete();
					line12->Delete();
					line11->Delete();
					line14->Delete();
					line15->Delete();
					line13->Delete();


				}//Else 2 intersections end

			}
		}//2 ellipses
		else
		{
			if (ellip != NULL || ellip2 != NULL)

			{//1 seule ellipse

			
				
			

				mafQuadraticSurface* ellip1;
				if (ellip == NULL)
					ellip1 = ellip2;
				if (ellip2 == NULL)
					ellip1 = ellip;
				double localEllip_start[3];
			
				double** intersectionpts = computeLineIntersection(m_StartPoint, m_EndPoint, &n1, ellip1);

		


				if (n1 == 0)
				{

					globalError = 0;
				
					vtkPoints* pts1 = vtkPoints::New();
					pts1->InsertNextPoint(local_start);
					pts1->InsertNextPoint(local_end);

					mafVMELineSeg* line11 = mafVMELineSeg::New();
					line11->SetName("line11");
					line11->resetPoints();
					line11->setPoints(pts1);
					line11->GetOutput()->Update();
					line11->Update();

					globalLength = sqrt((m_EndPoint[0] - m_StartPoint[0])*(m_EndPoint[0] - m_StartPoint[0]) + (m_EndPoint[1] - m_StartPoint[1])*(m_EndPoint[1] - m_StartPoint[1]) + (m_EndPoint[2] - m_StartPoint[2])*(m_EndPoint[2] - m_StartPoint[2]));
					//gLength = globalLength;
					mafString ss = " ";
					gLength = ss.Append(std::to_string(globalLength).c_str());
					gError = std::to_string(globalError).c_str();

					m_Goniometer->RemoveAllInputs();
					m_Goniometer->AddInput(line11->getVTKPolydata());

					m_Goniometer->Update();
					m_PolyData->DeepCopy(m_Goniometer->GetOutput());


					line11->Delete();
					pts1->Delete();
				}
				else //n1==1

				{

					globalError = 21;
					double posPt[3];
					double local_posPt[3];
					mafVMELineSeg* line13 = mafVMELineSeg::New();
					mafVMELineSeg* line14 = mafVMELineSeg::New();
					mafVMELineSeg* line15 = mafVMELineSeg::New();
					
					vtkPoints* pts1 = vtkPoints::New();
					vtkPoints* pts2 = vtkPoints::New();
					vtkPoints* pts5 = vtkPoints::New();
					
					
					surfaces.push_back(ellip1);
					functor.surfaces = &surfaces;

					functor.m = 6;
					functor.n = 6;

					VectorXd X(6);
			
					X << intersectionpts[0][0], intersectionpts[0][1], intersectionpts[0][2], intersectionpts[1][0], intersectionpts[1][1], intersectionpts[1][2];

					Eigen::LevenbergMarquardt<LMFunctor> lm(functor);
					lm.resetParameters();
					int r = lm.minimize(X);
					double d1, d2;
					double positionP[3];
					double positionQ[3];
					d1 = sqrt((X[0] - m_StartPoint[0])*(X[0] - m_StartPoint[0]) + (X[1] - m_StartPoint[1])*(X[1] - m_StartPoint[1]) + (X[2] - m_StartPoint[2])*(X[2] - m_StartPoint[2]));
					d2 = sqrt((X[3] - m_StartPoint[0])*(X[3] - m_StartPoint[0]) + (X[4] - m_StartPoint[1])*(X[4] - m_StartPoint[1]) + (X[5] - m_StartPoint[2])*(X[5] - m_StartPoint[2]));


					if (d1 < d2)
					{	
						p->SetPoint(X(0), X(1), X(2), currTs);
						p->Update();
						positionP[0] = X(0);
						positionP[01] = X(01);
						positionP[02] = X(02);

						q->SetPoint(X(3), X(4), X(5), currTs);
						q->Update();
						positionQ[0] = X(3);
						positionQ[01] = X(4);
						positionQ[02] = X(5);
					}
					else
					{
						p->SetPoint(X(3), X(4), X(5), currTs);
						p->Update();
						positionP[0] = X(3);
						positionP[01] = X(4);
						positionP[02] = X(5);

						q->SetPoint(X(0), X(1), X(2), currTs);
						q->Update();
						positionQ[0] = X(0);
						positionQ[01] = X(1);
						positionQ[02] = X(2);

					}
				

					
					vector<Vector3d> path = ellip1->computeGeodesicPath((mafVMELandmark*)P0, (mafVMELandmark*)Q0, p, q, 2 * length,rate, &globalError);

					

					double wrappingDistance = 0;
					double tempdist = sqrt((X(0) - X(3))*(X(0) - X(3)) + (X(1) - X(4))*(X(1) - X(4)) + (X(2) - X(5))*(X(2) - X(5)));

					
					posPt[0] = positionP[0];
					posPt[1] = positionP[1];
					posPt[2] = positionP[2];
					
					
					


					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts5->InsertNextPoint(local_posPt);
					
					if ((path.size() > 1) && (tempdist > 10))
					if (m_ComputeStateCheckbox)
					{
						for (int i = 0; i < path.size(); i++)
						{
							tempdist = sqrt((path[i][0] - posPt[0])*(path[i][0] - posPt[0]) + (path[i][1] - posPt[1])*(path[i][1] - posPt[1]) + (path[i][2] - posPt[2])*(path[i][2] - posPt[2]));
							if (tempdist < 10)
							{
								wrappingDistance = wrappingDistance + tempdist;

								posPt[0] = path[i][0];
								posPt[1] = path[i][1];
								posPt[2] = path[i][2];

								m_TmpTransform2->TransformPoint(posPt, local_posPt);
								pts5->InsertNextPoint(local_posPt);
							
							}
						}
					}

					
					
					posPt[0] = positionQ[0];
					posPt[1] = positionQ[1];
					posPt[2] = positionQ[2];

					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts5->InsertNextPoint(local_posPt);
				
					pts1->InsertNextPoint(local_start);

					posPt[0] = positionP[0];
					posPt[1] = positionP[1];
					posPt[2] = positionP[2];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts1->InsertNextPoint(local_posPt);
					

					line13->resetPoints();
					line13->setPoints(pts1);
					line13->GetOutput()->Update();
					

					globalLength = sqrt((positionP[0] - m_StartPoint[0])*(positionP[0] - m_StartPoint[0]) + (positionP[1] - m_StartPoint[1])*(positionP[1] - m_StartPoint[1]) + (positionP[2] - m_StartPoint[2])*(positionP[2] - m_StartPoint[2]));
				
					
					posPt[0] = positionQ[0];
					posPt[1] = positionQ[01];
					posPt[2] = positionQ[02];
					m_TmpTransform2->TransformPoint(posPt, local_posPt);
					pts2->InsertNextPoint(local_posPt);
					pts2->InsertNextPoint(local_end);
					
					globalLength = globalLength + sqrt((m_EndPoint[0] - positionQ[0])*(m_EndPoint[0] - positionQ[0]) + (m_EndPoint[1] - positionQ[1])*(m_EndPoint[1] - positionQ[1]) + (m_EndPoint[2] - positionQ[02])*(m_EndPoint[2] - positionQ[02]));
					line14->resetPoints();
					line14->setPoints(pts2);
					line14->GetOutput()->Update();
					

					line15->resetPoints();
					if (pts5->GetNumberOfPoints() > 1)
					{

						line15->setPoints(pts5);
						line15->GetOutput()->Update();
						
					}

					
					
					globalLength = globalLength + wrappingDistance;
					//gLength = globalLength;
					mafString ss = " ";
					gLength = ss.Append(std::to_string(globalLength).c_str());
					gError = std::to_string(globalError).c_str();


					m_Goniometer->RemoveAllInputs();
					m_Goniometer->AddInput(line13->getVTKPolydata());

					if (pts5->GetNumberOfPoints() > 1)
						m_Goniometer->AddInput(line15->getVTKPolydata());

					m_Goniometer->AddInput(line14->getVTKPolydata());
					m_Goniometer->Update();
					m_PolyData->DeepCopy(m_Goniometer->GetOutput());

					
					functor.surfaces = NULL;
					
					line15->Delete();
					line14->Delete();
					line13->Delete();
					path.clear();
					pts5->Delete();
					pts2->Delete();
					pts1->Delete();

				}
			}

			else //No Ellipse found
			{


				vtkPoints* pts1 = vtkPoints::New();

				pts1->InsertNextPoint(local_start);
				pts1->InsertNextPoint(local_end);






				mafVMELineSeg* line11 = mafVMELineSeg::New();
				line11->SetName("line11");

				line11->resetPoints();
				line11->setPoints(pts1);
				line11->GetOutput()->Update();
				line11->Update();

				globalLength = sqrt((m_EndPoint[0] - m_StartPoint[0])*(m_EndPoint[0] - m_StartPoint[0]) + (m_EndPoint[1] - m_StartPoint[1])*(m_EndPoint[1] - m_StartPoint[1]) + (m_EndPoint[2] - m_StartPoint[2])*(m_EndPoint[2] - m_StartPoint[2]));
				//gLength = globalLength;
				mafString ss = " ";
				gLength = ss.Append(std::to_string(globalLength).c_str());
				globalError = 0;
				gError = std::to_string(globalError).c_str();


				m_Goniometer->RemoveAllInputs();
				m_Goniometer->AddInput(line11->getVTKPolydata());

				m_Goniometer->Update();
				m_PolyData->DeepCopy(m_Goniometer->GetOutput());
				line11->Delete();
				pts1->Delete();


				}//No ellipse found
			  }//
			
		}//(P0 != NULL && Q0 != NULL)
	
	
	
		if (m_Gui)
			m_Gui->Update();
	
  mafEvent ev(this,CAMERA_UPDATE);
  this->ForwardUpEvent(&ev);

}
//-----------------------------------------------------------------------
int mafVMEMuscleWrapping::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalStore(parent) == MAF_OK)
	{
		parent->StoreInteger("computeState", m_ComputeStateCheckbox);
		parent->StoreMatrix("Transform", &m_Transform->GetMatrix());
		return MAF_OK;
	}
	return MAF_ERROR;

	//wxBusyInfo wait040("internal store");
	//Sleep(1500);
}

//-----------------------------------------------------------------------
int mafVMEMuscleWrapping::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalRestore(node) == MAF_OK)
	{
		mafMatrix matrix;
		if (node->RestoreMatrix("Transform", &matrix) == MAF_OK
			&&
			node->RestoreInteger("computeState", m_ComputeStateCheckbox) == MAF_OK 
			)
		{
			m_Transform->SetMatrix(matrix);
			
			
			return MAF_OK;
		}
	}

	return MAF_ERROR;
	
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//char** mafVMEMuscleWrapping::GetIcon()
//-------------------------------------------------------------------------
//{
  //#include "mafVMEProcedural.xpm"
  //return mafVMEProcedural_xpm;
//}

void mafVMEMuscleWrapping::SetEllipsoidLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{

	if (n->IsMAFType(mafQuadraticSurface))
			SetLink(link_name, n);
	else 
		SetLink(link_name, NULL);
}

double** mafVMEMuscleWrapping::computeLineIntersection(double localpt1[3], double localpt2[3], int *nbr, mafQuadraticSurface* ellipsoid)

{	
	mafTimeStamp currTs = GetTimeStamp();
	double xyzr[3];
	double direction[3];
	double** result = new double*[2];
	result[0] = new double[3];
	result[1] = new double[3];
	
	direction[0] = localpt2[0] - localpt1[0];
	direction[1] = localpt2[1] - localpt1[1];
	direction[2] = localpt2[2] - localpt1[2];

	double val=0;
	double valprecedent=10;
	double point[3];
	double pointLocal[3];
	double comp;
	Vector3d pt;




	/*double a, b, c;
	if (ellipsoid != NULL)
	{
		a=ellipsoid->getSzX();
		b=ellipsoid->getSzY();
		c=ellipsoid->getSzZ();
	}
	*/
	for (int i = 0; i < 10000; i++)
	{	
		point[0] = localpt1[0] + 0.0001*direction[0] * i;
		point[1] = localpt1[1] + 0.0001*direction[1] * i;
		point[2] = localpt1[2] + 0.0001*direction[2] * i;
	
		//distance point-Quadric
		/*if (ellipsoid->IsMAFType(mafVMEEllipsoid))
		{
			centerTemp = ((mafVMEEllipsoid *)ellipsoid)->GetCenter();

		}
		else
		{
			ellipsoid->GetOutput()->Update();
			ellipsoid->GetOutput()->GetAbsPose(centerTemp, xyzr, currTs);
		}*/
		if (ellipsoid != NULL)
		{
			pt << point[0], point[1], point[2];
			/*val = ((point[0] - centerTemp[0]) * (point[0] - centerTemp[0])) / (a*a) +
				((point[1] - centerTemp[1]) * (point[1] - centerTemp[1])) / (b*b) +
				((point[2] - centerTemp[2]) * (point[2] - centerTemp[2])) / (c*c);*/			
			val = ellipsoid->surf(pt);
			
			if (i == 0)
			{
				valprecedent = valprecedent*sgnwrapping(val);
			}
			//comp = (val - 1)*(valprecedent - 1);
			comp = (val)*(valprecedent );
			if (comp<0)
			{
				
				//wxBusyInfo wait("intersection detection...");
				//Sleep(1500);
				if ((*nbr) == 0)
				{
					result[0][0] = point[0];
					result[0][1] = point[1];
					result[0][2] = point[2];
					(*nbr) = 1;
					result[1][0] = point[0];
					result[1][1] = point[1];
					result[1][2] = point[2];
				}
				else
				{
					result[1][0] = point[0];
					result[1][1] = point[1];;
					result[1][2] = point[2];
				}		
				
			}
			else
			{
				;
			}
			valprecedent = val;
		}
		
	}
	intersectionInit = true;
	
	return result;
}

void mafVMEMuscleWrapping::SetLandmarkLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{

	if (n->IsMAFType(mafVMELandmark))
	{
		SetLink(link_name, n, ((mafVMELandmarkCloud *)(n->GetParent()))->FindLandmarkIndex(n->GetName()));
	}
	else
		SetLink(link_name, NULL);
}
mafVME *mafVMEMuscleWrapping::GetP0VME()
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink("P0Landmark"));
}
mafVME *mafVMEMuscleWrapping::GetEllipsoidVME()
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink("EllipVME"));
}
mafVME *mafVMEMuscleWrapping::GetEllipsoidVME2()
//-------------------------------------------------------------------------
{
	return mafVME::SafeDownCast(GetLink("EllipVME2"));
}
mafVME *mafVMEMuscleWrapping::GetQ0VME()
//-------------------------------------------------------------------------
{	
	return mafVME::SafeDownCast(GetLink("Q0Landmark"));
}
void mafVMEMuscleWrapping::UpdateLinks()
//-------------------------------------------------------------------------
{

	mafID sub_id = -1;
	mafNode* nd = NULL;

	ellip = (mafQuadraticSurface*)GetEllipsoidVME();
	if (ellip && ellip->IsMAFType(mafQuadraticSurface))
	{
		nd = GetLink("EllipVME");
		m_EllipsoidVmeName = (nd != NULL) ? ((mafQuadraticSurface *)ellip)->GetName() : _("none");
		
	}
	else
	{
		m_EllipsoidVmeName = ellip ? ellip->GetName() : _("none");
		
	}
	

	ellip2 = (mafQuadraticSurface*)GetEllipsoidVME2();
	if (ellip2 && ellip2->IsMAFType(mafQuadraticSurface))
	{
		nd = GetLink("EllipVME2");
		m_EllipsoidVmeName2 = (nd != NULL) ? ((mafQuadraticSurface *)ellip2)->GetName() : _("none");
	}
	else
		m_EllipsoidVmeName2 = ellip2 ? ellip2->GetName() : _("none");

	
	Q0 = GetQ0VME();
	if (Q0 && Q0->IsMAFType(mafVMELandmark))
	{
		sub_id = GetLinkSubId("Q0Landmark");
		m_Q0LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)Q0->GetParent())->GetLandmarkName(sub_id) : _("none");
	}
	else
		m_Q0LandmarkName = Q0 ? Q0->GetName() : _("none");


	P0 = GetP0VME();
	if (P0 && P0->IsMAFType(mafVMELandmark))
	{
		sub_id = GetLinkSubId("P0Landmark");
		m_P0LandmarkName = (sub_id != -1) ? ((mafVMELandmarkCloud *)P0->GetParent())->GetLandmarkName(sub_id) : _("none");
	}
	else
		m_P0LandmarkName = P0 ? P0->GetName() : _("none");
		

}