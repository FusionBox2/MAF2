/*=========================================================================

 Program: MAF2
 Module: mafPipeMeter
 Authors: Paolo Quadrani
 
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

#include "mafPipeMuscleWrapper.h"
#include "mafGUI.h"
#include "mafSceneNode.h"
#include "mafDataPipe.h"
#include "mafVMEMuscleWrapper.h"
#include "mafEventSender.h"
#include "mmaMuscleWrapperAQ.h"
#include "mmaMaterial.h"
#include "mafGUIMaterialButton.h"
#include "mafVMELandmarkCloud.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"

#include "wx/busyinfo.h"
#include "vtkMAFAssembly.h"

//@@@ #include "mafDecl.h"
//@@@ #include "mafAxes.h"  //SIL. 20-5-2003 added line - 
//@@@ #include "mafMaterial.h"  //SIL. 30-5-2003 added line -
//@@@ #include "mafGUIMaterialButton.h"

#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkTubeFilter.h"
#include "vtkLookupTable.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkCaptionActor2D.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeMuscleWrapperAQ);
//----------------------------------------------------------------------------
//#include "mafMemDbg.h"

//----------------------------------------------------------------------------
mafPipeMuscleWrapperAQ::mafPipeMuscleWrapperAQ()
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("pipe muscle wrapperAQ construction...");
	Sleep(5000);
  m_DataMapper        = NULL;
  m_DataActor         = NULL;
  m_SelectionBox      = NULL;
  m_SelectionMapper   = NULL;
  m_SelectionProperty = NULL;
  m_SelectionActor    = NULL;
  m_Tube              = NULL;
  m_MuscleWrapperVME = NULL;
  m_Lut               = NULL;
  m_Caption           = NULL;
  m_MaterialButton    = NULL;
  wxBusyInfo wait2("pipe muscle wrapperAQ construction done");
  Sleep(3000);
}
//----------------------------------------------------------------------------
void mafPipeMuscleWrapperAQ::Create(mafNode *node, mafView *view/*, bool use_axes*/)
//----------------------------------------------------------------------------
{

	wxBusyInfo wait("Pipecreate...");
	Sleep(1000);
  Superclass::Create(node, view);
  
  m_Selected          = false;
  m_DataMapper        = NULL;
  m_DataActor         = NULL;
  m_SelectionBox      = NULL;
  m_SelectionMapper   = NULL;
  m_SelectionProperty = NULL;
  m_SelectionActor    = NULL;
  m_Tube              = NULL;
  m_MuscleWrapperVME = NULL;
  m_Lut               = NULL;
  m_Caption           = NULL;

  assert(m_Vme->IsA("mafVMEMuscleWrapperAQ"));
  m_MuscleWrapperVME = mafVMEMuscleWrapperAQ::SafeDownCast(m_Vme);
  m_MuscleWrapperVME->AddObserver(this);
  assert(m_MuscleWrapperVME->GetPolylineOutput());
  m_MuscleWrapperVME->GetPolylineOutput()->Update();
  vtkPolyData *data = m_MuscleWrapperVME->GetPolylineOutput()->GetPolylineData();
 
 // vtkPolyData *data = m_MuscleWrapperVME->e->getOutput();

  assert(data);

  vtkNEW(m_Tube);
  m_Tube->UseDefaultNormalOff();
  m_Tube->SetInput(data);
  m_Tube->SetRadius(m_MuscleWrapperVME->GetMeterRadius());
  m_Tube->SetCapping(m_MuscleWrapperVME->GetMeterCapping());
  m_Tube->SetNumberOfSides(20);
  m_Tube->UseDefaultNormalOff();

  double *range;
  range = m_MuscleWrapperVME->GetDistanceRange();

  vtkNEW(m_Lut);
  m_Lut->SetTableRange(range[0],range[1]);
  m_Lut->SetHueRange(0.7,0);
  m_Lut->SetNumberOfTableValues(16);
  m_Lut->Build();

  vtkNEW(m_DataMapper);
  if (m_MuscleWrapperVME->GetMeterRepresentation() == mafVMEMuscleWrapperAQ::LINE_REPRESENTATION)
    m_DataMapper->SetInput(data);
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }
    
	m_DataMapper->ImmediateModeRenderingOff();
	if (m_MuscleWrapperVME->GetMeterColorMode() == mafVMEMuscleWrapperAQ::RANGE_COLOR)
    m_DataMapper->SetLookupTable(m_Lut);

  vtkNEW(m_DataActor);
  m_DataActor->SetProperty(m_MuscleWrapperVME->GetMaterial()->m_Prop);
	m_DataActor->SetMapper(m_DataMapper);

  m_AssemblyFront->AddPart(m_DataActor);

  // selection hilight
	vtkNEW(m_SelectionBox);
	m_SelectionBox->SetInput(data);  

	vtkNEW(m_SelectionMapper);
	m_SelectionMapper->SetInput(m_SelectionBox->GetOutput());

	vtkNEW(m_SelectionProperty);
	m_SelectionProperty->SetColor(1,1,1);
	m_SelectionProperty->SetAmbient(1);
	m_SelectionProperty->SetRepresentationToWireframe();
	m_SelectionProperty->SetInterpolationToFlat();

	vtkNEW(m_SelectionActor);
	m_SelectionActor->SetMapper(m_SelectionMapper);
	m_SelectionActor->VisibilityOff();
	m_SelectionActor->PickableOff();
	m_SelectionActor->SetProperty(m_SelectionProperty);

  m_AssemblyFront->AddPart(m_SelectionActor);

  vtkNEW(m_Caption);
  m_Caption->SetPosition(25,10);
  m_Caption->ThreeDimensionalLeaderOff();
  if (m_MuscleWrapperVME->GetMeterColorMode() != mafVMEMuscleWrapperAQ::RANGE_COLOR)
  {
    double c[3];
    m_DataActor->GetProperty()->GetColor(c);
    m_Caption->GetProperty()->SetColor(c);
  }
  else
  {
	  m_DataActor->GetProperty()->SetColor(m_MuscleWrapperVME->GetMaterial()->m_Diffuse);
	  m_Caption->GetProperty()->SetColor(m_MuscleWrapperVME->GetMaterial()->m_Diffuse);
  }
//  else
//    Caption->GetProperty()->SetColor(data->m_mat_gui->GetMaterial()->m_prop->GetColor());
  m_Caption->SetHeight(0.05);
  m_Caption->SetWidth(0.35);
  m_Caption->BorderOff();
  wxString dis;
  dis = wxString::Format("%.2f", m_MuscleWrapperVME->GetDistance());
  m_Caption->SetCaption(dis.c_str());

  if (m_MuscleWrapperVME->GetMeterMode() == mafVMEMuscleWrapperAQ::LINE_ANGLE)
	  m_Caption->SetVisibility((m_MuscleWrapperVME->GetAngle() != 0) && m_MuscleWrapperVME->GetMeterAttributes()->m_LabelVisibility);
  else
	  m_Caption->SetVisibility((m_MuscleWrapperVME->GetDistance() >= 0 || m_MuscleWrapperVME->GetMeterMeasureType() == mafVMEMuscleWrapperAQ::RELATIVE_MEASURE) && m_MuscleWrapperVME->GetMeterAttributes()->m_LabelVisibility);
  
  if (m_MuscleWrapperVME->GetStartVME())
  {
    double pos[3], rot[3];
	mafVME *linked_vme = m_MuscleWrapperVME->GetStartVME();
    mafSmartPointer<mafTransform> TmpTransform;
	if (linked_vme && linked_vme->IsMAFType(mafVMELandmarkCloud) && m_MuscleWrapperVME->GetLinkSubId(_R("StartVME")) != -1)
    {
		((mafVMELandmarkCloud *)linked_vme)->GetLandmark(m_MuscleWrapperVME->GetLinkSubId(_R("StartVME")), pos, -1);
      TmpTransform->SetMatrix(*linked_vme->GetOutput()->GetAbsMatrix());
      TmpTransform->TransformPoint(pos,pos);
    }
    else
		m_MuscleWrapperVME->GetStartVME()->GetOutput()->GetAbsPose(pos, rot);
    m_Caption->SetAttachmentPoint(pos[0],pos[1],pos[2]);
  } 

  m_RenFront->AddActor2D(m_Caption);

  wxBusyInfo wait2("Pipe  create ok");
  Sleep(1000);

  /*
  m_axes = NULL;
	if(m_use_axes) m_axes = new mafAxes(m_ren1,m_Vme);
	if(m_use_axes) m_axes->SetVisibility(0);
	*/
}
//----------------------------------------------------------------------------
mafPipeMuscleWrapperAQ::~mafPipeMuscleWrapperAQ()
//----------------------------------------------------------------------------
{
	m_MuscleWrapperVME->RemoveObserver(this);

  m_AssemblyFront->RemovePart(m_DataActor);
  m_AssemblyFront->RemovePart(m_SelectionActor);
  m_RenFront->RemoveActor2D(m_Caption);

  vtkDEL(m_Tube);
  vtkDEL(m_Lut);
  vtkDEL(m_Caption);
  vtkDEL(m_DataMapper);
  vtkDEL(m_DataActor);
  vtkDEL(m_SelectionBox);
  vtkDEL(m_SelectionMapper);
  vtkDEL(m_SelectionProperty);
  vtkDEL(m_SelectionActor);

  cppDEL(m_MaterialButton);
	//@@@ if(m_use_axes) wxDEL(m_axes);  
}
//----------------------------------------------------------------------------
mafGUI *mafPipeMuscleWrapperAQ::CreateGui()
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("Pipe create gui ...");
	Sleep(1000);
  const mafString type_measure_string[] = {_R("absolute"), _R("relative")};
  const mafString representation_string[] = {_R("line"), _R("tube")};
  const mafString color_string[] = {_R("one"), _R("range")};
  int num_choices = 2;

  mmaMuscleWrapperAQ *meter_attrib = m_MuscleWrapperVME->GetMeterAttributes();

  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->Bool(ID_SHOW_LABEL,_R("label"), &meter_attrib->m_LabelVisibility);
  m_Gui->Combo(ID_COLOR_MODE,_R("color"), &meter_attrib->m_ColorMode,num_choices,color_string);
  m_MaterialButton = new mafGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_Gui->VectorN(ID_DISTANCE_RANGE,_R("range"),meter_attrib->m_DistanceRange,2,0);
  m_Gui->Combo(ID_METER_REPRESENTATION,_R(""),&meter_attrib->m_Representation,num_choices,representation_string);
  m_Gui->Double(ID_TUBE_RADIUS,_R("radius"),&meter_attrib->m_TubeRadius,0);
  m_Gui->Bool(ID_TUBE_CAPPING,_R("capping"),&meter_attrib->m_Capping);
  m_Gui->Combo(ID_METER_MEASURE_TYPE,_R(""),&meter_attrib->m_MeasureType,num_choices,type_measure_string);
  m_Gui->Double(ID_INIT_MEASURE,_R("abs. init"),&meter_attrib->m_InitMeasure,0); 
  //Kewei Duan: 
  //This is the solution to bug raised by Danielle Ascani.
  //"init" is updated by "absolute init" to avoid misunderstanding from users from GUI design perspective.
  //because there are two measure modes, users will get confused what to fill here under different modes
  //It is nonsense to fill in a 0 in relative mode as initial value. 
  //13/12/2013
  m_Gui->Bool(ID_GENERATE_EVENT,_R("gen. event"),&meter_attrib->m_GenerateEvent);
  m_Gui->Double(ID_DELTA_PERCENT,_R("delta %"),&meter_attrib->m_DeltaPercent,0);

  m_MaterialButton->Enable(meter_attrib->m_ColorMode == mafVMEMuscleWrapperAQ::ONE_COLOR);
  m_Gui->Enable(ID_DISTANCE_RANGE, meter_attrib->m_ColorMode == mafVMEMuscleWrapperAQ::RANGE_COLOR);
  m_Gui->Enable(ID_TUBE_RADIUS, meter_attrib->m_Representation == mafVMEMuscleWrapperAQ::TUBE_REPRESENTATION);
  m_Gui->Enable(ID_TUBE_CAPPING, meter_attrib->m_Representation == mafVMEMuscleWrapperAQ::TUBE_REPRESENTATION);
	m_Gui->Divider();
	wxBusyInfo wait2("Pipe create gui ok");
	Sleep(1000);
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeMuscleWrapperAQ::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("Pipe onevent ...");
	Sleep(1000);
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
	  mmaMuscleWrapperAQ *meter_attrib = m_MuscleWrapperVME->GetMeterAttributes();
    switch(e->GetId()) 
    {
      case ID_SHOW_LABEL:
        UpdateProperty();
      break;
      case ID_COLOR_MODE:
		  m_Gui->Enable(ID_DISTANCE_RANGE, meter_attrib->m_ColorMode == mafVMEMuscleWrapperAQ::RANGE_COLOR);
		  m_MaterialButton->Enable(meter_attrib->m_ColorMode == mafVMEMuscleWrapperAQ::ONE_COLOR);
        UpdateProperty();
      break;
      case ID_METER_REPRESENTATION:
		  m_Gui->Enable(ID_TUBE_RADIUS, meter_attrib->m_Representation == mafVMEMuscleWrapperAQ::TUBE_REPRESENTATION);
		  m_Gui->Enable(ID_TUBE_CAPPING, meter_attrib->m_Representation == mafVMEMuscleWrapperAQ::TUBE_REPRESENTATION);
        UpdateProperty();
      break;
      case ID_TUBE_RADIUS:
        m_Tube->SetRadius(meter_attrib->m_TubeRadius);
      break;
      case ID_TUBE_CAPPING:
        m_Tube->SetCapping(meter_attrib->m_Capping);
      break;
      case ID_DISTANCE_RANGE:
		  m_MuscleWrapperVME->Modified();
		  m_MuscleWrapperVME->Update();
      break;
      case ID_METER_MEASURE_TYPE:
		  m_MuscleWrapperVME->GetDataPipe()->Update();
      case ID_INIT_MEASURE:
      {
							  if (meter_attrib->m_MeasureType == mafVMEMuscleWrapperAQ::ABSOLUTE_MEASURE){
              meter_attrib->m_DistanceRange[0] = meter_attrib->m_InitMeasure;
		      meter_attrib->m_DistanceRange[1] = meter_attrib->m_InitMeasure * (1.0 + meter_attrib->m_DeltaPercent / 100.0);
							  }
							  else if (meter_attrib->m_MeasureType == mafVMEMuscleWrapperAQ::RELATIVE_MEASURE){
		      meter_attrib->m_DistanceRange[0] = 0;
		      meter_attrib->m_DistanceRange[1] = meter_attrib->m_InitMeasure * (1.0 + meter_attrib->m_DeltaPercent / 100.0)-meter_attrib->m_InitMeasure;
		  } 
		  //Kewei Duan: Updated. 
		  //This is the solution to bug raised by Danielle Ascani.
		  //Absolute and relative modes should be considered seperately 
		  //in this event.
		  //13/12/2013 
        m_Gui->Update();
		m_MuscleWrapperVME->Modified();
		m_MuscleWrapperVME->GetDataPipe()->Update();
      }
      break;
      case ID_DELTA_PERCENT:
      {
							   if (meter_attrib->m_MeasureType == mafVMEMuscleWrapperAQ::ABSOLUTE_MEASURE){
              meter_attrib->m_DistanceRange[0] = meter_attrib->m_InitMeasure;
		      meter_attrib->m_DistanceRange[1] = meter_attrib->m_InitMeasure * (1.0 + meter_attrib->m_DeltaPercent / 100.0);
							   }
							   else if (meter_attrib->m_MeasureType == mafVMEMuscleWrapperAQ::RELATIVE_MEASURE){
		      meter_attrib->m_DistanceRange[0] = 0;
		      meter_attrib->m_DistanceRange[1] = meter_attrib->m_InitMeasure * (1.0 + meter_attrib->m_DeltaPercent / 100.0)-meter_attrib->m_InitMeasure;
		  } 
		  //Kewei Duan: Updated. 
		  //This is the solution to bug raised by Danielle Ascani.
		  //Absolute and relative modes should be considered seperately 
		  //in this event.
		  //13/12/2013
        m_Gui->Update();
		m_MuscleWrapperVME->Modified();
		m_MuscleWrapperVME->GetDataPipe()->Update();
      }
      break;
      case ID_GENERATE_EVENT:
		  m_MuscleWrapperVME->GetDataPipe()->Update();
      break;
      default:
		  m_MuscleWrapperVME->ForwardUpEvent(*e);
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else if (maf_event->GetSender() == m_MuscleWrapperVME)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
      UpdateProperty();
    }
	else if (maf_event->GetId() == mafVMEMuscleWrapperAQ::LENGTH_THRESHOLD_EVENT)
    {
    }
  }

  wxBusyInfo wait2("Pipe onevent ok");
  Sleep(1000);
}
//----------------------------------------------------------------------------
void mafPipeMuscleWrapperAQ::Select(bool sel)
//----------------------------------------------------------------------------
{

	wxBusyInfo wait("Pipe select...");
	Sleep(1000);
	m_Selected = sel;
	if(m_DataActor->GetVisibility()) 
	{
			m_SelectionActor->SetVisibility(sel);
			//@@@ if(m_use_axes) m_axes->SetVisibility(sel);
	}
	wxBusyInfo wait2("Pipe select ok");
	Sleep(1000);
}
//----------------------------------------------------------------------------
void mafPipeMuscleWrapperAQ::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
	wxBusyInfo wait("Pipe update property ...");
	Sleep(1000);
	if (NULL == m_DataMapper || NULL == m_DataActor || NULL == m_Caption)
	{
		wxBusyInfo wait("Pipe NULL ...");
		Sleep(1000);
		return;
	}
  
  vtkPolyData *data = m_MuscleWrapperVME->GetPolylineOutput()->GetPolylineData();
  if (m_MuscleWrapperVME->GetMeterRepresentation() == mafVMEMuscleWrapperAQ::LINE_REPRESENTATION)
  {
    m_DataMapper->SetInput(data);
  }
  else
  {
    m_Tube->Update();
    m_DataMapper->SetInput(m_Tube->GetOutput());
  }

  double distance_value = m_MuscleWrapperVME->GetDistance();
  if (m_MuscleWrapperVME->GetMeterMode() == mafVMEMuscleWrapperAQ::LINE_ANGLE)
  {
	  distance_value = m_MuscleWrapperVME->GetAngle();
  }
  distance_value = RoundValue(distance_value);
  wxString dis;
  dis = wxString::Format("%.2f",distance_value);
  m_Caption->SetCaption(dis.c_str());
  m_Caption->SetVisibility(m_MuscleWrapperVME->GetMeterAttributes()->m_LabelVisibility);

  double rgb[3];
  double v = m_MuscleWrapperVME->GetDistance();
  int color_mode = m_MuscleWrapperVME->GetMeterColorMode();
  if (color_mode == mafVMEMuscleWrapperAQ::RANGE_COLOR)
  {
    double *range;
    m_DataMapper->SetLookupTable(m_Lut);
	range = m_MuscleWrapperVME->GetDistanceRange();
    m_Lut->SetTableRange(range[0],range[1]);
    m_Lut->Build();
    m_Lut->GetColor(v,rgb);
    m_DataActor->GetProperty()->SetColor(rgb);
    m_Caption->GetProperty()->SetColor(rgb);
  }
  else
  {
    m_DataMapper->SetColorModeToDefault();
	m_DataActor->GetProperty()->SetColor(m_MuscleWrapperVME->GetMaterial()->m_Diffuse);
	m_Caption->GetProperty()->SetColor(m_MuscleWrapperVME->GetMaterial()->m_Diffuse);
  }

  double pos[3] = {0,0,0};
  double rot[3] = {0,0,0};
  mafVME *linked_vme = m_MuscleWrapperVME->GetStartVME();
  mafSmartPointer<mafTransform> TmpTransform;
  if (linked_vme && linked_vme->IsMAFType(mafVMELandmarkCloud) && m_MuscleWrapperVME->GetLinkSubId(_R("StartVME")) != -1)
  {
	  ((mafVMELandmarkCloud *)linked_vme)->GetLandmark(m_MuscleWrapperVME->GetLinkSubId(_R("StartVME")), pos, -1);
    TmpTransform->SetMatrix(*linked_vme->GetOutput()->GetAbsMatrix());
    TmpTransform->TransformPoint(pos,pos);
  }
  else if(linked_vme)
  {
    linked_vme->GetOutput()->GetAbsPose(pos,rot);
  }
  m_Caption->SetAttachmentPoint(pos[0],pos[1],pos[2]);

  GetGui()->Update();
  
  /*
	if(fromTag)
  {
		((mafVmeData *)m_Vme->GetClientData())->UpdateFromTag();
    int idx = m_Vme->GetTagArray()->FindTag("VME_CENTER_ROTATION_POSE");
    vtkTagItem *item = NULL;
    double vec[16];
    if (idx != -1)
    {
      item = m_Vme->GetTagArray()->GetTag(idx);
      mflSmartPointer<vtkMatrix4x4> pose;
      for (int el=0;el<16;el++)
      {
        vec[el] = item->GetValueAsDouble(el);
      }
      pose->DeepCopy(vec);
      m_axes->SetPose(pose);
    }
    else
      m_axes->SetPose();
  }
  else
	  m_DataMapper->SetScalarVisibility(((mafVmeData *)m_Vme->GetClientData())->GetColorByScalar());
	*/

  wxBusyInfo wait2("Pipe update property ok");
  Sleep(1000);
}