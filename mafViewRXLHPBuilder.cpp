/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXLHPBuilder.cpp,v $
  Language:  C++
  Date:      $Date: 2006-10-20 09:33:13 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewRXLHPBuilder.h"
#include "mafPipeFactory.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"
#include "mmgGui.h"
#include "mafPipe.h"
#include "mafPipeVolumeProjected.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESlicer.h"
#include "mafVMESurface.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
#include "vtkPropPicker.h"

#include "mafEventSource.h"
#include "mmgLutSlider.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewRXLHPBuilder);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewRXLHPBuilder::mafViewRXLHPBuilder(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewRX( label, camera_position,show_axes, show_grid, show_ruler,  stereo)
//----------------------------------------------------------------------------
{
  m_Luts=NULL;
  m_CurrentVolume = NULL;
}
//----------------------------------------------------------------------------
mafViewRXLHPBuilder::~mafViewRXLHPBuilder()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafView *mafViewRXLHPBuilder::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewRXLHPBuilder *v = new mafViewRXLHPBuilder(m_Label, m_CameraPosition, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::Create()
//----------------------------------------------------------------------------
{
  m_Rwi = new mafRWI(mafGetFrame(), TWO_LAYER, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->InitializePickList();

  CreateGuiView();
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);

  mafSceneNode *n = m_Sg->Vme2Node(vme);
  assert(n && !n->m_Pipe);

  if (pipe_name != "")
  {
    if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 1)
    {
      m_NumberOfVisibleVme = 1;
    }
    else
    {
      m_NumberOfVisibleVme++;
    }
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj= NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    mafPipe *pipe = (mafPipe*)obj;
    if (pipe)
    {
      pipe->SetListener(this);
      if (pipe_name.Equals("mafPipeVolumeProjected"))
      {
        ((mafPipeVolumeProjected *)pipe)->InitializeProjectParameters(m_CameraPosition);
        m_CurrentVolume = n;
        if (m_AttachCamera)
        {
          m_AttachCamera->SetVme(m_CurrentVolume->m_Vme);
          CameraUpdate();
        }
      }
      pipe->Create(n);
      n->m_Pipe = (mafPipe*)pipe;
      if (m_NumberOfVisibleVme == 1)
      {
        mafEventMacro(mafEvent(this,CAMERA_RESET));
      }
      else
      {
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
	  }
	  if (pipe_name.Equals("mafPipeVolumeProjected"))
	  {
	    double sr[2];
		  ((mafPipeVolumeProjected*)n->m_Pipe)->GetLutRange(sr);
		  m_Luts->SetRange((long)sr[0],(long)sr[1]);
		  m_Luts->SetSubRange((long)sr[0],(long)sr[1]);	
	  }
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  if((vme->IsMAFType(mafVMELandmarkCloud) && ((mafVMELandmarkCloud*)vme)->IsOpen()) || vme->IsMAFType(mafVMELandmark) && m_NumberOfVisibleVme == 0)
    m_NumberOfVisibleVme = 0;
  else
    m_NumberOfVisibleVme--;
  if (vme->IsMAFType(mafVMEVolume))
  {
    m_CurrentVolume = NULL;
    if (m_AttachCamera)
    {
      m_AttachCamera->SetVme(NULL);
    }
  }
  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//-------------------------------------------------------------------------
int mafViewRXLHPBuilder::GetNodeStatus(mafNode *vme)
//-------------------------------------------------------------------------
{
  mafSceneNode *n = NULL;
  if (m_Sg != NULL)
  {
    if (vme->IsMAFType(mafVMEVolume))
    {
      n = m_Sg->Vme2Node(vme);
      n->m_Mutex = true;
    }
    else if (vme->IsMAFType(mafVMESlicer))
    {
      n = m_Sg->Vme2Node(vme);
      n->m_PipeCreatable = false;
    }
  }
  return m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}
//-------------------------------------------------------------------------
mmgGui *mafViewRXLHPBuilder::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_AttachCamera = new mafAttachCamera(m_Gui, m_Rwi, this);
  m_Gui->AddGui(m_AttachCamera->GetGui());
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	switch(maf_event->GetId()) 
	{
	case ID_RANGE_MODIFIED:
      {
        if(this->VolumeIsVisible())
        {
          int low, hi;
          m_Luts->GetSubRange(&low,&hi);
		      this->SetLutRange(low,hi);
          CameraUpdate();
        }
      }
     break;
     default:
		mafEventMacro(*maf_event);
	}
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::SetLutRange(double low_val, double high_val)
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume) 
    return;
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeProjected"))
  {
    mafPipeVolumeProjected *pipe = (mafPipeVolumeProjected *)m_CurrentVolume->m_Pipe;
    pipe->SetLutRange(low_val, high_val); 
  }
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  m_Size = event.GetSize();
  OnLayout();
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mmgGui(this);
  m_Luts = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(500,24));
  //EnableWidgets(m_CurrentVolume != NULL);
  m_Luts->SetListener(this);
  m_GuiView->Add(m_Luts);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::OnLayout()
//----------------------------------------------------------------------------
{ 
  int sw = m_Size.GetWidth()/3;
  int sh = m_Size.GetHeight();
  int gh = 0;

  if (m_GuiView)
  {
    wxSize gui_size = m_GuiView->GetBestSize();
    gh = gui_size.GetHeight();
    if(sw<gh || sh<gh) return;
    sh -= gh;
    m_GuiView->SetSize(0,sh,sw,gh);
  }
}
//----------------------------------------------------------------------------
void mafViewRXLHPBuilder::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  /*if (m_Gui)
  {
    m_Gui->Enable(ID_ORTHO_SLICE_X,enable);
    m_Gui->Enable(ID_ORTHO_SLICE_Y,enable);
    m_Gui->Enable(ID_ORTHO_SLICE_Z,enable);
    m_Gui->Enable(ID_LUT_CHOOSER,enable);
  }*/
  m_Luts->Enable(enable);
  /*m_Histogram->Enable(enable);
  m_Histogram->Show(enable);*/
}