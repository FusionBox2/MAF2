/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSliceLHPBuilder.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-27 14:24:00 $
  Version:   $Revision: 1.1 $
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

#include "mafViewSliceLHPBuilder.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafPipeFactory.h"
#include "mafPipe.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafAttachCamera.h"

#include "vtkDataSet.h"
#include "vtkRayCast3DPicker.h"
#include "vtkCellPicker.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkCoordinate.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkProperty2D.h"
#include "vtkActor2D.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "mafPipeSurfaceSlice.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewSliceLHPBuilder);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewSliceLHPBuilder::mafViewSliceLHPBuilder(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewSlice(label,camera_position,show_axes,show_grid, show_ruler, stereo)
//----------------------------------------------------------------------------
{
  /*m_CurrentVolume = NULL;
  m_Border        = NULL;
  
  m_Slice[0] = m_Slice[1] = m_Slice[2] = 0.0;
  m_SliceInitialized = false;*/
	m_TextActor=NULL;
	m_TextMapper=NULL;
	m_TextColor[0]=1;
	m_TextColor[1]=0;
	m_TextColor[2]=0;

	m_CurrentSurface.clear();
}
//----------------------------------------------------------------------------
mafViewSliceLHPBuilder::~mafViewSliceLHPBuilder()
//----------------------------------------------------------------------------
{
	vtkDEL(m_TextMapper);
	vtkDEL(m_TextActor);
	m_CurrentSurface.clear();
}
//----------------------------------------------------------------------------
mafView *mafViewSliceLHPBuilder::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewSliceLHPBuilder *v = new mafViewSliceLHPBuilder(m_Label, m_CameraPosition, m_ShowAxes,m_ShowGrid, m_ShowRuler, m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafViewSliceLHPBuilder::Create()
//----------------------------------------------------------------------------
{
  RWI_LAYERS num_layers = m_CameraPosition != CAMERA_OS_P ? TWO_LAYER : ONE_LAYER;
  
  m_Rwi = new mafRWI(mafGetFrame(), num_layers, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(m_CameraPosition);
  m_Win = m_Rwi->m_RwiBase;

  m_Sg  = new mafSceneGraph(this,m_Rwi->m_RenFront,m_Rwi->m_RenBack);
  m_Sg->SetListener(this);
  m_Rwi->m_Sg = m_Sg;

  vtkNEW(m_Picker3D);
  vtkNEW(m_Picker2D);
  m_Picker2D->SetTolerance(0.01);
  m_Picker2D->InitializePickList();

  m_Text = "";
  m_TextMapper = vtkTextMapper::New();
  m_TextMapper->SetInput(m_Text.c_str());
  m_TextMapper->GetTextProperty()->AntiAliasingOff();
  
  m_TextActor = vtkActor2D::New();
  m_TextActor->SetMapper(m_TextMapper);
  m_TextActor->SetPosition(3,3);
  m_TextActor->GetProperty()->SetColor(m_TextColor);

  m_Rwi->m_RenFront->AddActor(m_TextActor);
}
//----------------------------------------------------------------------------
void mafViewSliceLHPBuilder::UpdateText(int ID)
//----------------------------------------------------------------------------
{
	if (ID==1)
	{
		int slice_mode;
		switch(m_CameraPosition)
		{
			case CAMERA_OS_X:
				slice_mode = SLICE_X;
			break;
			case CAMERA_OS_Y:
				slice_mode = SLICE_Y;
			break;
			case CAMERA_OS_P:
				slice_mode = SLICE_ORTHO;
			break;
			case CAMERA_PERSPECTIVE:
				slice_mode = SLICE_ARB;
			break;
			default:
				slice_mode = SLICE_Z;
		}
		//set the init coordinates value
		if(slice_mode == SLICE_X)
			m_Text = "X = ";
		else if(slice_mode == SLICE_Y)
			m_Text = "Y = ";
		else if(slice_mode == SLICE_Z)
			m_Text = "Z = ";

		if((slice_mode != SLICE_ORTHO) && (slice_mode != SLICE_ARB))
		m_Text += wxString::Format("%.1f",m_Slice[slice_mode]);

		m_TextMapper->SetInput(m_Text.c_str());
		m_TextMapper->Modified();
	}
	else
	{
		m_Text="";
		m_TextMapper->SetInput(m_Text.c_str());
		m_TextMapper->Modified();
	}
}
//----------------------------------------------------------------------------
void mafViewSliceLHPBuilder::VmeCreatePipe(mafNode *vme)
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
      if (pipe_name.Equals("mafPipeVolumeSlice"))
      {
        m_CurrentVolume = n;
        if (m_AttachCamera)
          m_AttachCamera->SetVme(m_CurrentVolume->m_Vme);
        int slice_mode;
        vtkDataSet *data = ((mafVME *)vme)->GetOutput()->GetVTKData();
        assert(data);
        data->Update();
        switch(m_CameraPosition)
        {
          case CAMERA_OS_X:
            slice_mode = SLICE_X;
        	break;
          case CAMERA_OS_Y:
            slice_mode = SLICE_Y;
          break;
          case CAMERA_OS_P:
            slice_mode = SLICE_ORTHO;
          break;
          case CAMERA_PERSPECTIVE:
            slice_mode = SLICE_ARB;
          break;
          default:
            slice_mode = SLICE_Z;
        }
        if (m_SliceInitialized)
        {
          ((mafPipeVolumeSlice *)pipe)->InitializeSliceParameters(slice_mode,m_Slice,false);
        }
        else
        {
          ((mafPipeVolumeSlice *)pipe)->InitializeSliceParameters(slice_mode,false);
        }
		    UpdateText();
      }
	    else if(pipe_name.Equals("mafPipeSurfaceSlice"))
	    {
		    m_CurrentSurface.push_back(n);
		    ((mafPipeSurfaceSlice *)pipe)->SetSlice(m_Slice);
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
    }
    else
      mafErrorMessage("Cannot create visual pipe object of type \"%s\"!",pipe_name.GetCStr());
  }

}
//----------------------------------------------------------------------------
void mafViewSliceLHPBuilder::SetTextColor(double color[3])
//----------------------------------------------------------------------------
{
	m_TextColor[0]=color[0];
	m_TextColor[1]=color[1];
	m_TextColor[2]=color[2];
	m_TextActor->GetProperty()->SetColor(m_TextColor);
	m_TextMapper->Modified();
}
//----------------------------------------------------------------------------
void mafViewSliceLHPBuilder::SetSlice(double origin[3])
//----------------------------------------------------------------------------
{
  if(!m_CurrentVolume)
    return;
  memcpy(m_Slice,origin,sizeof(m_Slice));
  mafString pipe_name = m_CurrentVolume->m_Pipe->GetTypeName();
  if (pipe_name.Equals("mafPipeVolumeSlice"))
  {
    mafPipeVolumeSlice *pipe = (mafPipeVolumeSlice *)m_CurrentVolume->m_Pipe;
    pipe->SetSlice(origin); 
  }
  if(m_CurrentSurface.empty())
    return;
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
	pipe_name = m_CurrentSurface.at(i)->m_Pipe->GetTypeName();
	if (pipe_name.Equals("mafPipeSurfaceSlice"))
	{
		mafPipeSurfaceSlice *pipe = (mafPipeSurfaceSlice *)m_CurrentSurface[i]->m_Pipe;
		pipe->SetSlice(origin); 
	}
  }
}
void mafViewSliceLHPBuilder::UpdateListSurface(mafNode *node)
{
  for(int i=0;i<m_CurrentSurface.size();i++)
  {
	if (m_CurrentSurface[i]==m_Sg->Vme2Node(node))
	{
		std::vector<mafSceneNode*>::iterator startIterator;
		m_CurrentSurface.erase(m_CurrentSurface.begin()+i);
	}
  }
}