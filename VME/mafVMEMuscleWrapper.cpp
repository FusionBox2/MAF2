/*=========================================================================

 Program: MAF2
 Module: mafVMEMuscleWrapper
 Authors: Marco Petrone, Paolo Quadrani
 
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

#include "mafVMEMuscleWrapper.h"
#include "mafVMEOutputMuscleWrapper.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mmaMuscleWrapperAQ.h"
#include "mmaMaterial.h"
#include "mafEventSender.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mmuIdFactory.h"
#include "mafGUI.h"
#include "mafAbsMatrixPipe.h"

#include "mafRWI.h"
#include "mafGUIDialogPreview.h"
#include "mafVectors.h"

#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkAppendPolyData.h"
#include "vtkProbeFilter.h"
#include "vtkXYPlotActor.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkCellArray.h"

//dictionary
#include "mafGUIHolder.h"
#include "mafGUISplittedPanel.h"
#include "mafGUINamedPanel.h"
#include "mafGUIDictionaryWidget.h"
//#include "mafOpExplodeCollapse.h"
//#include "mafSmartPointer.h"
#include "mafdmLineSegment.hpp"

#include "mafPointCloud.hpp"
#include <assert.h>
#include <wx/busyinfo.h>
MAF_ID_IMP(mafVMEMuscleWrapperAQ::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEMuscleWrapperAQ)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEMuscleWrapperAQ::mafVMEMuscleWrapperAQ()
//-------------------------------------------------------------------------
{

  wxBusyInfo wait("muscle wrapperAQ construction...");
  Sleep(3000);
  m_Distance      = -1.0;
  m_Angle         = 0.0;

  m_InfiniteLine  = 0;
  m_LineAngle2    = 0;
  
  m_StartVmeName  = _R("");
  m_StartVme2Name  = _R("");
  m_EndVme1Name   = _R("");
  m_EndVme2Name   = _R("");
  m_ProbeVmeName   = _R("");
  
  mafNEW(m_Transform);
  mafVMEOutputMuscleWrapperAQ *output = mafVMEOutputMuscleWrapperAQ::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  vtkNEW(m_LineSource1);
  vtkNEW(m_LineSource2);
  vtkNEW(m_LineSource3);
  vtkNEW(m_Goniometer);
  vtkNEW(m_PolyData);
  

  m_Goniometer->AddInput(m_LineSource1->GetOutput());
  m_Goniometer->AddInput(m_LineSource2->GetOutput());
  m_Goniometer->AddInput(m_LineSource3->GetOutput());

  m_PolyData->DeepCopy(m_Goniometer->GetOutput());

  mafNEW(m_TmpTransform);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_PolyData);

  // histogram
  // Probing tool
  vtkNEW(m_ProbingLine);
  m_ProbingLine->SetResolution(512);

  m_ProbedVME = NULL;

  mafString plot_title = _L("Density vs. Length (mm)");
  mafString plot_titleX = _R("mm");
  mafString plot_titleY = _L("Dens.");
  vtkNEW(m_PlotActor);
  m_PlotActor->GetProperty()->SetColor(0.02,0.06,0.62);	
  m_PlotActor->GetProperty()->SetLineWidth(2);
  m_PlotActor->SetPosition(0.03,0.03);
  m_PlotActor->SetPosition2(0.9,0.9);
  m_PlotActor->SetLabelFormat("%g");
  m_PlotActor->SetXRange(0,300);
  m_PlotActor->SetPlotCoordinate(0,300);
  m_PlotActor->SetNumberOfXLabels(10);
  m_PlotActor->SetXValuesToIndex();
  m_PlotActor->SetTitle(plot_title.GetCStr());
  m_PlotActor->SetXTitle(plot_titleX.GetCStr());
  m_PlotActor->SetYTitle(plot_titleY.GetCStr());
  vtkTextProperty* tprop = m_PlotActor->GetTitleTextProperty();
  tprop->SetColor(0.02,0.06,0.62);
  tprop->SetFontFamilyToArial();
  tprop->ItalicOff();
  tprop->BoldOff();
  tprop->SetFontSize(12);
  m_PlotActor->SetPlotColor(0,.8,.3,.3);

  m_HistogramDialog = NULL;
  m_HistogramRWI    = NULL;
  
  m_GenerateHistogram = 0;

  wxBusyInfo wait2("muscle wrapperAQ construction done");
  Sleep(3000);
}
//-------------------------------------------------------------------------
mafVMEMuscleWrapperAQ::~mafVMEMuscleWrapperAQ()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  vtkDEL(m_LineSource1);
  vtkDEL(m_LineSource2);
  vtkDEL(m_LineSource3);
  vtkDEL(m_Goniometer);
  mafDEL(m_TmpTransform);
  vtkDEL(m_PolyData);
  SetOutput(NULL);

  if(m_HistogramRWI)
    m_HistogramRWI->m_RenFront->RemoveActor(m_PlotActor);
  vtkDEL(m_PlotActor);
  vtkDEL(m_ProbingLine);
  cppDEL(m_HistogramDialog);
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a)==MAF_OK)
	{
		mafVMEMuscleWrapperAQ *meter = mafVMEMuscleWrapperAQ::SafeDownCast(a);
		m_Transform->SetMatrix(meter->m_Transform->GetMatrix());
    m_InfiniteLine = meter->m_InfiniteLine;
    m_LineAngle2   = meter->m_LineAngle2;

		mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
		if (dpipe)
		{
			dpipe->SetInput(m_Goniometer->GetOutput());
			m_Goniometer->Update();
		}
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEMuscleWrapperAQ::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
	  ret = m_Transform->GetMatrix() == ((mafVMEMuscleWrapperAQ *)vme)->m_Transform->GetMatrix();
  }
  return ret;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();

    return MAF_OK;
  }

  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEMuscleWrapperAQ::GetMaterial()
//-------------------------------------------------------------------------
{
	wxBusyInfo wait("muscle wrapperAQ getting material...");
	Sleep(3000);
  mmaMaterial *material = (mmaMaterial *)GetAttribute(_R("MaterialAttributes"));
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute(_R("MaterialAttributes"), material);
  }

  wxBusyInfo wait2("muscle wrapperAQ getting material done");
  Sleep(3000);
  return material;
}
//-------------------------------------------------------------------------
mafVMEOutputPolyline *mafVMEMuscleWrapperAQ::GetPolylineOutput()
//-------------------------------------------------------------------------
{
	wxBusyInfo wait("muscle wrapperAQ getting output");
	Sleep(3000);
  return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	wxBusyInfo wait("muscle wrapperAQ SetMatrix...");
	Sleep(3000);
  m_Transform->SetMatrix(mat);
  Modified();
}
//-------------------------------------------------------------------------
bool mafVMEMuscleWrapperAQ::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	wxBusyInfo wait("muscle wrapperAQ getting GetLocalTimeStamps...");
	Sleep(3000);
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::InternalPreUpdate()
//-----------------------------------------------------------------------
{

  wxBusyInfo wait("muscle wrapperAQ getting preupdate...");
  Sleep(3000);
  GetMeterAttributes();
  wxBusyInfo wait2("muscle wrapperAQ getting preupdate done");
  Sleep(3000);
}
//-----------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::InternalUpdate()
//-----------------------------------------------------------------------
{

	wxBusyInfo wait("muscle wrapperAQ getting update...");
	Sleep(3000);
  GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
  GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
  GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

  double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

  UpdateLinks();
  mafTimeStamp currTs = GetTimeStamp();

  if (GetMeterMode() == mafVMEMuscleWrapperAQ::POINT_DISTANCE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end_vme   = GetEnd1VME();

    bool start_ok = true, end_ok = true;

    double orientation[3];

    if (start_vme && end_vme)
    {
      // start is a landmark, consider also visibility
      /*if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
        start_ok = start_landmark->GetLandmarkVisibility();*/

      if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("StartVME")) != -1)
      {
        ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId(_R("StartVME")),m_StartPoint,currTs);
        mafMatrix tm;
        start_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
      }
      else
      {
        start_vme->GetOutput()->Update();  
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation, currTs);
      }

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end_landmark = mflVMELandmark::SafeDownCast(end_vme))
        end_ok = end_landmark->GetLandmarkVisibility();*/

      if(end_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("EndVME1")) != -1)
      {
        ((mafVMELandmarkCloud *)end_vme)->GetLandmark(GetLinkSubId(_R("EndVME1")),m_EndPoint,currTs);
        mafMatrix tm;
        end_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
      }
      else
      {
        end_vme->GetOutput()->Update();  
        end_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation, currTs);
      }
    }
    else
    {
      start_ok = false;
      end_ok   = false;
    }

    if (start_ok && end_ok)
    {
      // compute distance between points
      m_Distance = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint, m_EndPoint));

	  if (GetMeterMeasureType() == mafVMEMuscleWrapperAQ::RELATIVE_MEASURE)
        m_Distance -= GetMeterAttributes()->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(m_StartPoint, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
      //GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

      // compute end point in local coordinate system
      double local_end[3];
      m_TmpTransform->TransformPoint(m_EndPoint,local_end);

      m_LineSource3->SetPoint1(local_end[0], local_end[1], local_end[2]);
      m_LineSource3->SetPoint2(local_end[0], local_end[1], local_end[2]);
      m_LineSource3->Update();
      m_LineSource2->SetPoint1(local_end[0], local_end[1], local_end[2]);
      m_LineSource2->SetPoint2(local_end[0],local_end[1],local_end[2]);
      m_LineSource2->Update();
      m_LineSource1->SetPoint1(local_start[0],local_start[1],local_start[2]);
      m_LineSource1->SetPoint2(local_end[0],local_end[1],local_end[2]);
      m_LineSource1->Update();
      m_Goniometer->Modified();

      GenerateHistogram(m_GenerateHistogram);
    }
    else
      m_Distance = -1;

    GetOutput()->Update();
    InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

	if (GetMeterMeasureType() == mafVMEMuscleWrapperAQ::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }
  else if (GetMeterMode() == mafVMEMuscleWrapperAQ::LINE_DISTANCE)
  {
    mafVME *start_vme = GetStartVME();
    mafVME *end1_vme  = GetEnd1VME();
    mafVME *start2_vme = GetStart2VME();
    mafVME *end2_vme = GetEnd2VME();

    bool start_ok = true, start2_ok = true, end1_ok = true, end2_ok = true;
    double orientation[3];

    if(start_vme && (start2_vme || !m_LineAngle2) && end1_vme && end2_vme)
    {
      // start is a landmark, consider also visibility
      /*if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
        start_ok = start_landmark->GetLandmarkVisibility();*/

      if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("StartVME")) != -1)
      {
        ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId(_R("StartVME")),m_StartPoint,currTs);
        mafMatrix tm;
        start_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
      }
      else
      {
        start_vme->GetOutput()->Update();  
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation, currTs);
      }
      if(m_LineAngle2)
      {
        if(start2_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("StartVME2")) != -1)
        {
          ((mafVMELandmarkCloud *)start2_vme)->GetLandmark(GetLinkSubId(_R("StartVME2")), m_StartPoint2, currTs);
          mafMatrix tm;
          start2_vme->GetOutput()->GetAbsMatrix(tm, currTs);
          m_TmpTransform->SetMatrix(tm);
          m_TmpTransform->TransformPoint(m_StartPoint2, m_StartPoint2);
        }
        else
        {
          start2_vme->GetOutput()->Update();
          start2_vme->GetOutput()->GetAbsPose(m_StartPoint2, orientation, currTs);
        }
      }

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
        end1_ok = end1_landmark->GetLandmarkVisibility();*/

      if(end1_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("EndVME1")) != -1)
      {
        ((mafVMELandmarkCloud *)end1_vme)->GetLandmark(GetLinkSubId(_R("EndVME1")),m_EndPoint,currTs);
        mafMatrix tm;
        end1_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
      }
      else
      {
        end1_vme->GetOutput()->Update();  
        end1_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation, currTs);
      }

      // end is a landmark, consider also visibility
      /*if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
        end2_ok = end2_landmark->GetLandmarkVisibility();*/

      if(end2_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("EndVME2")) != -1)
      {
        ((mafVMELandmarkCloud *)end2_vme)->GetLandmark(GetLinkSubId(_R("EndVME2")),m_EndPoint2,currTs);
        mafMatrix tm;
        end2_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_EndPoint2,m_EndPoint2);
      }
      else
      {
        end2_vme->GetOutput()->Update();  
        end2_vme->GetOutput()->GetAbsPose(m_EndPoint2, orientation, currTs);
      }
    }
    else
    {
      start_ok = false;
      start2_ok = false;
      end1_ok = false;
      end2_ok  = false;
    }

    if(start_ok && start2_ok && end1_ok && end2_ok)
    {
      double start[3], start2[3], p1[3], p2[3], p3[3], p4[3], t;

      start[0] = m_StartPoint[0];
      start[1] = m_StartPoint[1];
      start[2] = m_StartPoint[2];

      if(m_LineAngle2)
      {
        start2[0] = m_StartPoint2[0];
        start2[1] = m_StartPoint2[1];
        start2[2] = m_StartPoint2[2];
      }
      else
      {
        p4[0] = start2[0] = m_StartPoint[0];
        p4[1] = start2[1] = m_StartPoint[1];
        p4[2] = start2[2] = m_StartPoint[2];
      }

      p1[0] = m_EndPoint[0];
      p1[1] = m_EndPoint[1];
      p1[2] = m_EndPoint[2];

      p2[0] = m_EndPoint2[0];
      p2[1] = m_EndPoint2[1];
      p2[2] = m_EndPoint2[2];

      if(m_LineAngle2)
      {
        V3d<double> origin(p1);
        V3d<double> direct = V3d<double>(p2) - origin;
        V3d<double> pivot(start);
        V3d<double> haxis = V3d<double>(start2) -V3d<double>(start);


        double alph = 0.0;
        double beta = 0.0;

        double hal2 = haxis.length2();
        double dil2 = direct.length2();
        if(hal2 > 1e-6 && dil2 > 1e-6)
        {
        haxis /= sqrt(hal2);
        direct /= sqrt(dil2);

        double dircos = haxis * direct;
        if(fabs(dircos) < 1.0 - 1e-6)
        {
        beta = (pivot - origin) * direct;
        beta += ((origin - pivot) * haxis) * dircos;
        beta /= 1 - dircos * dircos;
        }
        alph = (origin - pivot) * haxis + beta * dircos;
        }

        V3d<double> SP = pivot + alph * haxis;
        V3d<double> EP = origin + beta * direct;

        for(int i = 0; i < 3; i++)
        {
          p3[i] = EP[i];
          p4[i] = SP[i];
        }
      }
      else
      {
        if(!m_InfiniteLine)
          vtkLine::DistanceToLine(start, p1, p2, t, p3);
        else
        {
          double np1[3], p1p2[3], proj, den;

          for(int i = 0; i < 3; i++)
          {
            np1[i] = start[i] - p1[i];
            p1p2[i] = p1[i] - p2[i];
          }

          proj = 0;
          if((den = vtkMath::Norm(p1p2)) != 0.0)
          {
            for(int i = 0; i < 3; i++)
            {
              p1p2[i] /= den;
            }
            proj = vtkMath::Dot(np1, p1p2);
          }
          for(int i = 0; i < 3; i++)
          {
            p3[i] = p1[i] + proj * p1p2[i];
            p4[i] = start[i];
          }
        }
      }

      // compute distance between start and closest point
      m_Distance = sqrt(vtkMath::Distance2BetweenPoints(p4, p3));

	  if (GetMeterMeasureType() == mafVMEMuscleWrapperAQ::RELATIVE_MEASURE)
        m_Distance -= GetMeterAttributes()->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(start,local_start);

      double local_start2[3];
      m_TmpTransform->TransformPoint(start2, local_start2);

      // compute end point in local coordinate system
      double local_closest[3];
      m_TmpTransform->TransformPoint(p3,local_closest);

      double local_closest2[3];
      m_TmpTransform->TransformPoint(p4, local_closest2);

      double local_p1[3];
      m_TmpTransform->TransformPoint(p1,local_p1);

      double local_p2[3];
      m_TmpTransform->TransformPoint(p2,local_p2);

      m_LineSource1->SetPoint1(local_p1);
      m_LineSource1->SetPoint2(local_p2);
      m_LineSource1->Update();

      m_LineSource2->SetPoint1(local_closest);
      m_LineSource2->SetPoint2(local_closest2);
      m_LineSource2->Update();

      m_LineSource3->SetPoint1(local_start);
      m_LineSource3->SetPoint2(local_start2);
      m_LineSource3->Update();
      m_Goniometer->Modified();
    }
    else
      m_Distance = -1;

    GetOutput()->Update();
    InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

	if (GetMeterMeasureType() == mafVMEMuscleWrapperAQ::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      InvokeEvent(this, LENGTH_THRESHOLD_EVENT);
  }
  else if (GetMeterMode() == mafVMEMuscleWrapperAQ::LINE_ANGLE)
  {

	

    mafVME *start_vme = GetStartVME();
    mafVME *start2_vme = GetStart2VME();
    mafVME *end1_vme  = GetEnd1VME();
    mafVME *end2_vme  = GetEnd2VME();

    double orientation[3];

    bool start_ok = true, start2_ok = true, end1_ok = true, end2_ok = true;
    if (start_vme && (start2_vme || !m_LineAngle2) && end1_vme && end2_vme)
    {
      // start is a landmark, consider also visibility
      /*if (mflVMELandmark *start_landmark = mflVMELandmark::SafeDownCast(start_vme))
        start_ok = start_landmark->GetLandmarkVisibility();*/

      if(start_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("StartVME")) != -1)
      {
        ((mafVMELandmarkCloud *)start_vme)->GetLandmark(GetLinkSubId(_R("StartVME")),m_StartPoint,currTs);
        mafMatrix tm;
        start_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_StartPoint,m_StartPoint);
      }
      else
      {
        start_vme->GetOutput()->Update();  
        start_vme->GetOutput()->GetAbsPose(m_StartPoint, orientation, currTs);
      }

      if(m_LineAngle2)
      {
        if(start2_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("StartVME2")) != -1)
        {
          ((mafVMELandmarkCloud *)start2_vme)->GetLandmark(GetLinkSubId(_R("StartVME2")),m_StartPoint2,currTs);
          mafMatrix tm;
          start2_vme->GetOutput()->GetAbsMatrix(tm, currTs);
          m_TmpTransform->SetMatrix(tm);
          m_TmpTransform->TransformPoint(m_StartPoint2,m_StartPoint2);
        }
        else
        {
          start2_vme->GetOutput()->Update();  
          start2_vme->GetOutput()->GetAbsPose(m_StartPoint2, orientation, currTs);
        }
      }

      /*if(mflVMELandmark *end1_landmark = mflVMELandmark::SafeDownCast(end1_vme))
        end1_ok = end1_landmark->GetLandmarkVisibility();*/

      if(end1_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("EndVME1")) != -1)
      {
        ((mafVMELandmarkCloud *)end1_vme)->GetLandmark(GetLinkSubId(_R("EndVME1")),m_EndPoint,currTs);
        mafMatrix tm;
        end1_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_EndPoint,m_EndPoint);
      }
      else
      {
        end1_vme->GetOutput()->Update();  
        end1_vme->GetOutput()->GetAbsPose(m_EndPoint, orientation, currTs);
      }

      /*if (mflVMELandmark *end2_landmark = mflVMELandmark::SafeDownCast(end2_vme))
        end2_ok = end2_landmark->GetLandmarkVisibility();*/

      if(end2_vme->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId(_R("EndVME2")) != -1)
      {
        ((mafVMELandmarkCloud *)end2_vme)->GetLandmark(GetLinkSubId(_R("EndVME2")),m_EndPoint2,currTs);
        mafMatrix tm;
        end2_vme->GetOutput()->GetAbsMatrix(tm, currTs);
        m_TmpTransform->SetMatrix(tm);
        m_TmpTransform->TransformPoint(m_EndPoint2,m_EndPoint2);
      }
      else
      {
        end2_vme->GetOutput()->Update();  
        end2_vme->GetOutput()->GetAbsPose(m_EndPoint2, orientation, currTs);
      }
    }
    else
    {
      start_ok  = false;
      start2_ok = false;
      end1_ok   = false;
      end2_ok   = false;
    }
    if (start_ok && start2_ok && end1_ok && end2_ok)
    {
      double start[3],start2[3], p1[3],p2[3], v1[3], v2[3], vn1, vn2, s;

      start[0] = m_StartPoint[0];
      start[1] = m_StartPoint[1];
      start[2] = m_StartPoint[2];
      if(m_LineAngle2)
      {
        start2[0] = m_StartPoint2[0];
        start2[1] = m_StartPoint2[1];
        start2[2] = m_StartPoint2[2];
      }
      else
      {
        start2[0] = m_StartPoint[0];
        start2[1] = m_StartPoint[1];
        start2[2] = m_StartPoint[2];
      }
      p1[0] = m_EndPoint[0];
      p1[1] = m_EndPoint[1];
      p1[2] = m_EndPoint[2];
      p2[0] = m_EndPoint2[0];
      p2[1] = m_EndPoint2[1];
      p2[2] = m_EndPoint2[2];
      v1[0] = p1[0] - start[0];
      v1[1] = p1[1] - start[1];
      v1[2] = p1[2] - start[2];
      v2[0] = p2[0] - start2[0];
      v2[1] = p2[1] - start2[1];
      v2[2] = p2[2] - start2[2];
      vn1 = vtkMath::Norm(v1);
      vn2 = vtkMath::Norm(v2);
      s = vtkMath::Dot(v1,v2);
      if(vn1 != 0 && vn2 != 0)
      {
        m_Angle = acos(s / (vn1 * vn2)) * vtkMath::RadiansToDegrees();
		if (GetMeterMeasureType() == mafVMEMuscleWrapperAQ::RELATIVE_MEASURE)
          m_Angle -= GetMeterAttributes()->m_InitMeasure;
      }
      else
        m_Angle = 0;

      // compute points in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(start,local_start);

      double local_start2[3];
      m_TmpTransform->TransformPoint(start2,local_start2);
      double local_end1[3];
      m_TmpTransform->TransformPoint(p1,local_end1);
      double local_end2[3];
      m_TmpTransform->TransformPoint(p2,local_end2);

      m_LineSource1->SetPoint1(local_start);
      m_LineSource1->SetPoint2(local_end1);
      m_LineSource1->Update();

      m_LineSource2->SetPoint1(local_start2);
      m_LineSource2->SetPoint2(local_end2);
      m_LineSource2->Update();
      m_LineSource3->SetPoint1(local_end2);
      m_LineSource3->SetPoint2(local_end2);
      m_LineSource3->Update();

      m_Goniometer->Modified();
    }
    else
      m_Angle = 0;

    GetOutput()->Update();
    InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

	if (GetMeterMeasureType() == mafVMEMuscleWrapperAQ::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Angle > 0 && m_Angle >= threshold)
      InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }

  m_Goniometer->Update();
  vtkPolyData *polydata = m_Goniometer->GetOutput();
  int num = m_Goniometer->GetOutput()->GetNumberOfPoints();
  int pointId[2];
  vtkMAFSmartPointer<vtkCellArray> cellArray;
  for(int i = 0; i< num;i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cellArray->InsertNextCell(2 , pointId);  
    }
  }

  m_PolyData->SetPoints(m_Goniometer->GetOutput()->GetPoints());
  m_PolyData->SetLines(cellArray);
  m_PolyData->Update();

  wxBusyInfo wait2("muscle wrapperAQ getting update done");
  Sleep(3000);

}
//-----------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
	wxBusyInfo wait2("muscle wrapperAQ internal store");
	Sleep(3000);
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger(_R("Infinite"), m_InfiniteLine);
    parent->StoreInteger(_R("LineAngle2"), m_LineAngle2);
    parent->StoreMatrix(_R("Transform"),&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	wxBusyInfo wait2("muscle wrapperAQ internal restore");
	Sleep(3000);
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    node->RestoreInteger(_R("Infinite"), m_InfiniteLine);
    node->RestoreInteger(_R("LineAngle2"), m_LineAngle2);
    if (node->RestoreMatrix(_R("Transform"),&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
	wxBusyInfo wait2("muscle wrapperAQ internal print");
	Sleep(3000);
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
//char** mafVMEMeter::GetIcon() 
//-------------------------------------------------------------------------
//{
//  #include "mafVMEMeter.xpm"
//  return mafVMEMeter_xpm;
//}
//-------------------------------------------------------------------------
mmaMuscleWrapperAQ *mafVMEMuscleWrapperAQ::GetMeterAttributes()
//-------------------------------------------------------------------------
{
	wxBusyInfo wait("muscle wrapperAQ getting attributes...");
	Sleep(3000);
	mmaMuscleWrapperAQ *meter_attributes = (mmaMuscleWrapperAQ *)GetAttribute(_R("MeterAttributes"));
  if (meter_attributes == NULL)
  {
    meter_attributes = mmaMuscleWrapperAQ::New();
    SetAttribute(_R("MeterAttributes"), meter_attributes);
  }

  wxBusyInfo wait2("muscle wrapperAQ getting attributes done");
  Sleep(3000);
  return meter_attributes;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMeterMode(int mode)
//-------------------------------------------------------------------------
{
	wxBusyInfo wait2("muscle wrapperAQ setting mode");
	Sleep(3000);
  GetMeterAttributes()->m_MeterMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::GetMeterMode()
//-------------------------------------------------------------------------
{

	wxBusyInfo wait2("muscle wrapperAQ getting metermode");
	Sleep(3000);
  return GetMeterAttributes()->m_MeterMode;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DistanceRange[0] = min;
  GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *mafVMEMuscleWrapperAQ::GetDistanceRange()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::GetMeterCapping()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double mafVMEMuscleWrapperAQ::GetInitMeasure()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double mafVMEMuscleWrapperAQ::GetMeterRadius()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int mafVMEMuscleWrapperAQ::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
double mafVMEMuscleWrapperAQ::GetDistance()
//-------------------------------------------------------------------------
{
  return m_Distance;
}
//-------------------------------------------------------------------------
double mafVMEMuscleWrapperAQ::GetAngle()
//-------------------------------------------------------------------------
{
  return m_Angle;
}
//-------------------------------------------------------------------------
mafGUI* mafVMEMuscleWrapperAQ::CreateGui()
//-------------------------------------------------------------------------
{

	wxBusyInfo wait2("muscle wrapperAQ creating gui...");
	Sleep(3000);
  int num_mode = 3;
  const mafString mode_choices_string[] = {_L("point distance"), _L("line distance"), _L("line angle")};
  /*
  // setup gui_panel
  mafGUINamedPanel *m_GuiPanel = new mafGUINamedPanel(mafGetFrame(), -1);
  m_GuiPanel->SetTitle(_("Add Meter:"));

  // setup splitter
  mafGUISplittedPanel *sp = new mafGUISplittedPanel(m_GuiPanel, -1);
  m_GuiPanel->Add(sp, 1, wxEXPAND);

  //setup dictionary
  m_Dict = new mafGUIDictionaryWidget(sp, -1);
  m_Dict->SetListener(this);
  //m_Dict->SetCloud(m_Cloud);
  sp->PutOnTop(m_Dict->GetWidget());

  // setup GuiHolder
  mafGUIHolder *m_Guih = new mafGUIHolder(sp, -1, false, true);

  */

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  /*
  // setup dictionary
  m_Dict = new mafGUIDictionaryWidget(sp,-1);
  m_Dict->SetListener(this);
  m_Dict->SetCloud(m_Cloud);
  sp->PutOnTop(m_Dict->GetWidget());

  // setup GuiHolder
  m_Guih = new mafGUIHolder(sp,-1,false,true);


  // setup Gui
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);
  m_Gui->Button(ID_LOAD,_("load dictionary"));
  m_Gui->Divider();

  */
  /*m_Gui->SetListener(this);
  int ID_LOAD = 12845;
  m_Gui->Button(ID_LOAD, _("load dictionary"));
  m_Gui->Divider();*/
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Combo(ID_METER_MODE,_L("mode"),&(GetMeterAttributes()->m_MeterMode),num_mode,mode_choices_string,_L("Choose the meter mode"));
  m_Gui->Divider();

  UpdateLinks();
  
  m_Gui->Button(ID_START_METER_LINK,&m_StartVmeName,_L("Start"), _L("Select the start vme for the meter"));
  m_Gui->Button(ID_END1_METER_LINK,&m_EndVme1Name,_L("End 1"), _L("Select the end vme for point distance"));
  m_Gui->Button(ID_START2_METER_LINK,&m_StartVme2Name,_L("Start2"), _L("Select the start vme for the second line"));
  m_Gui->Button(ID_END2_METER_LINK,&m_EndVme2Name,_L("End 2"), _L("Select the vme representing \nthe point for line distance"));

  m_Gui->Bool(ID_INFINITE_LINE, _L("Infinite"), &m_InfiniteLine);
  m_Gui->Bool(ID_LINE_ANGLE2, _L("2 lines angle"), &m_LineAngle2);

  if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
    m_Gui->Enable(ID_END2_METER_LINK,false);
  m_Gui->Enable(ID_INFINITE_LINE, GetMeterAttributes()->m_MeterMode == LINE_DISTANCE);
  m_Gui->Enable(ID_LINE_ANGLE2, GetMeterAttributes()->m_MeterMode == LINE_ANGLE || GetMeterAttributes()->m_MeterMode == LINE_DISTANCE);
  m_Gui->Enable(ID_START2_METER_LINK, GetMeterAttributes()->m_MeterMode == LINE_ANGLE && m_LineAngle2);

  m_Gui->Bool(ID_PLOT_PROFILE,_L("plot profile"),&m_GenerateHistogram);
  m_Gui->Enable(ID_PLOT_PROFILE,GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);

  m_Gui->Button(ID_PLOTTED_VME_LINK,&m_ProbeVmeName,_L("Probed"), _L("Select the vme that will be plotted"));
  m_Gui->Enable(ID_PLOTTED_VME_LINK, GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);

	m_Gui->Divider();
	wxBusyInfo wait("muscle wrapperAQ creating gui done");
	Sleep(3000);
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::UpdateLinks()
//-------------------------------------------------------------------------
{
  mafID sub_id = -1;
  mafVME *start_vme = GetStartVME();
  mafVME *start_vme2 = GetStart2VME();
  mafVME *end_vme1 = GetEnd1VME();
  mafVME *end_vme2 = GetEnd2VME();
  mafVME *probedVme = GetPlottedVME();

  if (start_vme && start_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId(_R("StartVME"));
    m_StartVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)start_vme)->GetLandmarkName(sub_id) : _L("none");
  }
  else
    m_StartVmeName = start_vme ? start_vme->GetName() : _L("none");

  if (start_vme2 && start_vme2->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId(_R("Start2VME"));
    m_StartVme2Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)start_vme2)->GetLandmarkName(sub_id) : _L("none");
  }
  else
    m_StartVme2Name = start_vme2 ? start_vme2->GetName() : _L("none");

  if (end_vme1 && end_vme1->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId(_R("EndVME1"));
    m_EndVme1Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)end_vme1)->GetLandmarkName(sub_id) : _L("none");
  }
  else
    m_EndVme1Name = end_vme1 ? end_vme1->GetName() : _L("none");

  if (end_vme2 && end_vme2->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId(_R("EndVME2"));
    m_EndVme2Name = (sub_id != -1) ? ((mafVMELandmarkCloud *)end_vme2)->GetLandmarkName(sub_id) : _L("none");
  }
  else
    m_EndVme2Name = end_vme2 ? end_vme2->GetName() : _L("none");

  m_ProbedVME = mafVMEVolumeGray::SafeDownCast(probedVme);
  m_ProbeVmeName = probedVme ? probedVme->GetName() : _L("none");
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
	wxBusyInfo wait2("muscle wrapperAQ on event..");
	Sleep(3000);
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *ev = mafEvent::SafeDownCast(maf_event))
  {
    switch(ev->GetId())
    {
      case ID_LINE_ANGLE2:
        m_Gui->Enable(ID_START2_METER_LINK, m_LineAngle2 != 0);
        this->Modified();
        ev->SetId(CAMERA_UPDATE);
        ForwardUpEvent(ev);
        break;
      case ID_INFINITE_LINE:
        this->Modified();
        ev->SetId(CAMERA_UPDATE);
        ForwardUpEvent(ev);
        break;
      case ID_START_METER_LINK:
      case ID_START2_METER_LINK:
      case ID_END1_METER_LINK:
      case ID_END2_METER_LINK:
      {
        mafID button_id = ev->GetId();
        mafString title = _L("Choose meter vme link");
        ev->SetId(VME_CHOOSE);
		ev->SetArg((long)&mafVMEMuscleWrapperAQ::VMEAccept);
        ev->SetString(&title);
        ForwardUpEvent(ev);
        mafNode *n = ev->GetVme();
        if (n != NULL)
        {
          if (button_id == ID_START_METER_LINK)
          {
            SetMeterLink(_R("StartVME"), n);
            m_StartVmeName = n->GetName();
          }
          else if (button_id == ID_START2_METER_LINK)
          {
            SetMeterLink(_R("StartVME2"), n);
            m_StartVme2Name = n->GetName();
          }
          else if (button_id == ID_END1_METER_LINK)
          {
            SetMeterLink(_R("EndVME1"), n);
            m_EndVme1Name = n->GetName();
          }
          else
          {
            SetMeterLink(_R("EndVME2"), n);
            m_EndVme2Name = n->GetName();
          }
          m_Gui->Update();
          //InternalUpdate();
        }
      }
      break;
      case ID_PLOTTED_VME_LINK:
        {
          mafID button_id = ev->GetId();
          mafString title = _L("Choose meter vme link");
          ev->SetId(VME_CHOOSE);
		  ev->SetArg((long)&mafVMEMuscleWrapperAQ::VolumeAccept);
          ev->SetString(&title);
          ForwardUpEvent(ev);
          mafNode *n = ev->GetVme();
          if (n != NULL)
          {
            SetMeterLink(_R("PlottedVME"),n);
            m_ProbedVME = mafVMEVolumeGray::SafeDownCast(n);
            m_ProbeVmeName = n->GetName();
            CreateHistogram();
          }
          m_Gui->Update();
          
        }
        break;
	  case ID_METER_MODE:
	  {
		  if(GetMeterAttributes()->m_MeterMode == POINT_DISTANCE)
		  {  
        m_Gui->Enable(ID_END2_METER_LINK,false);
        m_Gui->Enable(ID_LINE_ANGLE2, false);
        m_Gui->Enable(ID_START2_METER_LINK, false);
		  }
		  else if(GetMeterAttributes()->m_MeterMode ==  LINE_DISTANCE)
		  { 
			  m_Gui->Enable(ID_END2_METER_LINK,true);
        m_Gui->Enable(ID_LINE_ANGLE2, true);
        m_Gui->Enable(ID_START2_METER_LINK, false);
		  }
		  else if(GetMeterAttributes()->m_MeterMode ==  LINE_ANGLE)
		  {       
			  m_Gui->Enable(ID_END2_METER_LINK,true);
        m_Gui->Enable(ID_LINE_ANGLE2, true);
        m_Gui->Enable(ID_START2_METER_LINK, m_LineAngle2 != 0);
		  }
      m_Gui->Enable(ID_INFINITE_LINE,GetMeterAttributes()->m_MeterMode == LINE_DISTANCE);
      m_Gui->Enable(ID_PLOT_PROFILE,GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);
      m_Gui->Enable(ID_PLOTTED_VME_LINK, GetMeterAttributes()->m_MeterMode == POINT_DISTANCE);
      this->Modified();
      mafID button_id = ev->GetId();
      ev->SetId(CAMERA_UPDATE);
      ForwardUpEvent(ev);
     
	  }
	  break;
    case ID_PLOT_PROFILE:
      {
        // Histogram dialog
        if(m_HistogramDialog == NULL)
        {
          int width = 400;
          int height = 300;
          int x_init,y_init;
          x_init = mafGetFrame()->GetPosition().x;
          y_init = mafGetFrame()->GetPosition().y;
          m_HistogramDialog = new mafGUIDialogPreview(_L("Histogram Dialog"), mafCLOSEWINDOW | mafUSERWI);
          m_HistogramRWI = m_HistogramDialog->GetRWI();
          m_HistogramRWI->SetListener(this);
          m_HistogramRWI->m_RenFront->AddActor2D(m_PlotActor);
          m_HistogramRWI->m_RenFront->SetBackground(1,1,1);
          m_HistogramRWI->SetSize(0,0,width,height);

          m_HistogramDialog->SetSize(x_init,y_init,width,height);
          m_HistogramDialog->Show(FALSE);
        }
        
        GenerateHistogram(m_GenerateHistogram);
      }
      break;
      default:
        mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }


  wxBusyInfo wait("muscle wrapperAQ on event done");
  Sleep(3000);
}
//-------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::SetMeterLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{
  if (n->IsMAFType(mafVMELandmark))
  {
    SetLink(link_name,n->GetParent(),((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
  }
  else
    SetLink(link_name, n);
}
//-------------------------------------------------------------------------
mafVME *mafVMEMuscleWrapperAQ::GetStartVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(_R("StartVME")));
}
//-------------------------------------------------------------------------
mafVME *mafVMEMuscleWrapperAQ::GetStart2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(_R("StartVME2")));
}
//-------------------------------------------------------------------------
mafVME *mafVMEMuscleWrapperAQ::GetEnd1VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(_R("EndVME1")));
}
//-------------------------------------------------------------------------
mafVME *mafVMEMuscleWrapperAQ::GetEnd2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(_R("EndVME2")));
}
//-------------------------------------------------------------------------
mafVME *mafVMEMuscleWrapperAQ::GetPlottedVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(_R("PlottedVME")));
}
//----------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::GenerateHistogram(int generate)
//----------------------------------------------------------------------------
{
  if(m_HistogramDialog)
  {
    m_GenerateHistogram = generate;
    if (m_GenerateHistogram)
    {
      CreateHistogram();
      m_HistogramRWI->m_RwiBase->Render();
    }
    m_HistogramDialog->Show(m_GenerateHistogram != 0);
  }
}
//----------------------------------------------------------------------------
void mafVMEMuscleWrapperAQ::CreateHistogram()
//----------------------------------------------------------------------------
{
  mafTimeStamp currTs = GetTimeStamp();
  if (m_ProbedVME != NULL)
  {
    mafTimeStamp tsPrb = m_ProbedVME->GetTimeStamp();
    m_ProbedVME->SetTimeStamp(currTs);
    m_ProbedVME->Update();
    vtkDataSet *probed_data = m_ProbedVME->GetOutput()->GetVTKData();
    probed_data->Update();

    vtkMAFSmartPointer<vtkTransform> transformStart;
    mafVME *start_vme = GetStartVME();
    mafMatrix matrixStart;
    start_vme->GetOutput()->GetMatrix(matrixStart, currTs);
    double point1[3], rotStart[3];

    if (start_vme && start_vme->IsMAFType(mafVMELandmarkCloud))
    {
      int sub_id = GetLinkSubId(_R("StartVME"));
      ((mafVMELandmark *)((mafVMELandmarkCloud *)start_vme)->GetChild(sub_id))->GetOutput()->GetAbsPose(point1, rotStart, currTs);
      
    }
    else
    {
      point1[0] = m_StartPoint[0];
      point1[1] = m_StartPoint[1];
      point1[2] = m_StartPoint[2];
    }

    vtkMAFSmartPointer<vtkTransform> transformEnd;
   
    mafVME *end_vme1 = GetEnd1VME();
    mafMatrix matrixEnd1;
    end_vme1->GetOutput()->GetMatrix(matrixEnd1, currTs);

    double point2[3], rotEnd[3];
    if (end_vme1 && end_vme1->IsMAFType(mafVMELandmarkCloud))
    {
      int sub_id = GetLinkSubId(_R("EndVME1"));
      ((mafVMELandmark *)((mafVMELandmarkCloud *)end_vme1)->GetChild(sub_id))->GetOutput()->GetAbsPose(point2, rotEnd, currTs);

    }
    else
    {   
      
      point2[0] = m_EndPoint[0];
      point2[1] = m_EndPoint[1];
      point2[2] = m_EndPoint[2];
    }

    m_PlotActor->SetXRange(0,m_Distance);
    double srY[2];
    m_ProbedVME->GetOutput()->GetVTKData()->GetScalarRange(srY);
    m_PlotActor->SetYRange(srY);
    m_PlotActor->SetPlotCoordinate(0,m_Distance);

    double b[6];
    m_ProbedVME->GetOutput()->GetBounds(b);

   

    m_ProbingLine->SetPoint1(point1);
    m_ProbingLine->SetPoint2(point2);
    m_ProbingLine->SetResolution((int)m_Distance);
    m_ProbingLine->Update();

    vtkMAFSmartPointer<vtkProbeFilter> prober;
    prober->SetInput(m_ProbingLine->GetOutput());
    prober->SetSource(probed_data);
    prober->Update();

    m_PlotActor->RemoveAllInputs();

    vtkPolyData *probimg_result = prober->GetPolyDataOutput();
    m_PlotActor->AddInput(probimg_result);
    if(m_HistogramRWI) m_HistogramRWI->m_RwiBase->Render();

    m_ProbedVME->SetTimeStamp(tsPrb);
    m_ProbedVME->Update();

  }
}
void mafVMEMuscleWrapperAQ::SetLineAngle2(int la2)
{
	m_LineAngle2=la2;
}
int mafVMEMuscleWrapperAQ::GetLineAngle2()
{
	return m_LineAngle2;
}

double mafVMEMuscleWrapperAQ::GetValue()
{

	if (m_Distance > -1)

		return m_Distance;
	if (m_Angle > -1)
		return m_Angle;

		

}
/*void mafVMEMuscleWrapperAQ::ComputeWrap()
{
	surfaces.push_back(e);
	functor.surfaces = &surfaces;
	functor.p0 = p0->getCenter();
	functor.q0 = q0->getCenter();
	functor.m = 6;
	functor.n = 6;

	X << 0, 0, 30, 0, 0, -30;

	Eigen::LevenbergMarquardt<LMFunctor> lm (functor);
	int r = lm.minimize(X);


	p= new mafdmLandmark(X(0), X(1), X(2), "p");
	mafdmLandmark* q= new mafdmLandmark(X(3), X(4), X(5), "q");
	vector<Vector3d> path = e->computeGeodesicPath(*p0, *q0, *p, *q);
	vtkMAFSmartPointer<vtkPoints> points;
	for (int i = 0; i<path.size(); i++)
		points->InsertNextPoint(path[i][0], path[i][1], path[i][2]);

	mafdmPointCloud* trajectory = new mafdmPointCloud(points);

	mafdmLineSegment* s1 = new mafdmLineSegment(p0, p);
	mafdmLineSegment* s2 = new mafdmLineSegment(q0, q);
}*/