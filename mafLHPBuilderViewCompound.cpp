/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafLHPBuilderViewCompound.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-27 14:24:00 $
  Version:   $Revision: 1.1 $
  Authors:   Matteo Giacomoni
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

#include "mafViewCompound.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mmgViewWin.h"
#include "mmgGui.h"
#include "mafInteractor.h"
#include "mafAvatar.h"
#include "mafAvatar3D.h"
#include "mmdTracker.h"
#include "mmdMouse.h"
#include "mafLHPBuilderViewCompound.h"
#include "mmgLutSlider.h"
#include "mafVMEVolume.h"
#include "vtkDataSet.h"
#include "mafEventSource.h"
#include "mafViewSlice.h"
#include "mafViewSliceLHPBuilder.h"
#include "mafPipeVolumeSlice.h"
#include "vtkLookupTable.h"
#include "mmgLutPreset.h"


//----------------------------------------------------------------------------
mafCxxTypeMacro(mafLHPBuilderViewCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafLHPBuilderViewCompound::mafLHPBuilderViewCompound( wxString label, int num_row, int num_col)
: mafViewCompound(label, num_row,  num_col)
//----------------------------------------------------------------------------
{
  m_Luts = NULL;
  m_CurrentVolume = NULL;
  m_ColorLUT = NULL;
}
//----------------------------------------------------------------------------
mafLHPBuilderViewCompound::~mafLHPBuilderViewCompound()
//----------------------------------------------------------------------------
{
  cppDEL(m_Luts);
  m_CurrentVolume=NULL;
  vtkDEL(m_ColorLUT);
}
//----------------------------------------------------------------------------
mafView *mafLHPBuilderViewCompound::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafLHPBuilderViewCompound *v = new mafLHPBuilderViewCompound(m_Label, m_ViewRowNum, m_ViewColNum);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  for (int i=0;i<m_PluggedChildViewList.size();i++)
  {
    v->m_PluggedChildViewList.push_back(m_PluggedChildViewList[i]->Copy(this));
  }
  v->m_NumOfPluggedChildren = m_NumOfPluggedChildren;
  v->Create();
  return v;
}
//----------------------------------------------------------------------------
void mafLHPBuilderViewCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
  m_GuiView = new mmgGui(this);
  
  m_Luts = new mmgLutSlider(m_GuiView,-1,wxPoint(0,0),wxSize(10,24));
  //EnableWidgets(m_CurrentVolume != NULL);
  m_Luts->SetListener(this);
  m_GuiView->Add(m_Luts);
  m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void mafLHPBuilderViewCompound::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  m_Size = event.GetSize();
  OnLayoutLHPBuilder();
}
//----------------------------------------------------------------------------
void mafLHPBuilderViewCompound::OnLayoutLHPBuilder()
//----------------------------------------------------------------------------
{
  assert(m_NumOfPluggedChildren == m_PluggedChildViewList.size());
  if (m_NumOfPluggedChildren == 0)
  {
    return;
  }

  int sw = m_Size.GetWidth();
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

  if (m_SubViewMaximized == -1)
  {
    LayoutSubView(sw,sh);
  }
  else
  {
    int i = 0;
    for (int r=0; r<m_ViewRowNum; r++)
    {
      for (int c=0; c<m_ViewColNum; c++)
      {
        if (i == m_SubViewMaximized)
        {
          m_ChildViewList[m_SubViewMaximized]->GetWindow()->SetSize(0,0,sw,sh);
					#ifndef WIN32
						m_ChildViewList[m_SubViewMaximized]->SetWindowSize(sw,sh);
					#endif
        }
        else
        {
          m_ChildViewList[i]->GetWindow()->SetSize(0,0,0,0);
					#ifndef WIN32
						m_ChildViewList[i]->SetWindowSize(0,0);
					#endif
        }
        i++;
      }
    }
  }
}
//----------------------------------------------------------------------------
void mafLHPBuilderViewCompound::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  m_Luts->Enable(enable);
}
//----------------------------------------------------------------------------
void mafLHPBuilderViewCompound::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumOfChildView; i++)
    m_ChildViewList[i]->VmeShow(node, show);

  if (node->IsMAFType(mafVMEVolume))
  {
    if (show)
    {
      m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
      double sr[2],center[3];
      vtkDataSet *data = m_CurrentVolume->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetScalarRange(sr);
      m_Luts->SetRange((long)sr[0],(long)sr[1]);
      m_Luts->SetSubRange((long)sr[0],(long)sr[1]);
      vtkNEW(m_ColorLUT);
      m_ColorLUT->SetRange(sr);
      m_ColorLUT->Build();
      lutPreset(4,m_ColorLUT);
    }
    else
    {
      m_CurrentVolume->GetEventSource()->RemoveObserver(this);
      m_CurrentVolume = NULL;
	    for(int i=0; i<m_NumOfChildView; i++)
		    ((mafViewSliceLHPBuilder *)m_ChildViewList[i])->UpdateText(0);
    }
  }
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafLHPBuilderViewCompound::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId()) 
  {
    case ID_DEFAULT_CHILD_VIEW:
    {
      mafSceneGraph *sg = GetSceneGraph();
      if(sg)
      {
        mafNode *vme = sg->GetSelectedVme();
        if (vme)
        {
          mafEventMacro(mafEvent(this,VME_MODIFIED,vme));
        }
      }
    }
  	break;
    case ID_LAYOUT_CHOOSER:
      OnLayout();
    break;
    case ID_LINK_SUBVIEW:
      LinkView(m_LinkSubView != 0);
    break;
	  case ID_RANGE_MODIFIED:
      {
        if(((mafViewSliceLHPBuilder *)m_ChildViewList[0])->VolumeIsVisible())
        {
          int low, hi;
          m_Luts->GetSubRange(&low,&hi);
          for(int i=0; i<6; i++)
          {
            mafPipeVolumeSlice *p = NULL;
            p = mafPipeVolumeSlice::SafeDownCast(((mafViewSliceLHPBuilder *)m_ChildViewList[i])->GetNodePipe(m_CurrentVolume));
            m_ColorLUT->SetRange(low,hi);
            p->SetColorLookupTable(m_ColorLUT);
          }
          CameraUpdate();
        }
      }
      break;
    default:
      mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
void mafLHPBuilderViewCompound::SetLut(int low,int hight)
//----------------------------------------------------------------------------
{
  m_Luts->SetSubRange(low,hight);
}