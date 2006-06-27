/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewRXCTLHPBuilder.cpp,v $
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

#include "mafViewRXCTLHPBuilder.h"
#include "mafViewVTK.h"
#include "mafViewRXLHPBuilder.h"
#include "mafViewSliceLHPBuilder.h"
#include "mafPipeVolumeSlice.h"
#include "mmgLutPreset.h"
#include "mmgGui.h"
#include "mafVMEVolume.h"
#include "mafVMESurface.h"
#include "mafGizmoSlice.h"
#include "mafNodeIterator.h"

#include "vtkDataSet.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "mafLHPBuilderViewCompound.h"
#include "mafViewCompound.h"
#include "mafEventSource.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafPipe.h"
#include "mafPipeSurfaceSlice.h"
#include "vtkMAFToLinearTransform.h"
#include "mafAbsMatrixPipe.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLinearTransform.h"
#include "vtkPlane.h"
#include "vtkTransformFilter.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewRXCTLHPBuilder);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewRXCTLHPBuilder::mafViewRXCTLHPBuilder(wxString label)
: mafViewCompound(label,1,3)
//----------------------------------------------------------------------------
{
  for (int v=0;v<2;v++)
  {
    m_ViewsRX[v] = NULL;
  }
  m_BorderColor[0][0] = 1; m_BorderColor[0][1] = 0; m_BorderColor[0][2] = 0;
  m_BorderColor[1][0] = 0; m_BorderColor[1][1] = 1; m_BorderColor[1][2] = 0;
  m_BorderColor[2][0] = 0; m_BorderColor[2][1] = 0; m_BorderColor[2][2] = 1;
  m_BorderColor[3][0] = 1; m_BorderColor[3][1] = 1; m_BorderColor[3][2] = 0;
  m_BorderColor[4][0] = 0; m_BorderColor[4][1] = 1; m_BorderColor[4][2] = 1;
  m_BorderColor[5][0] = 1; m_BorderColor[5][1] = 0; m_BorderColor[5][2] = 1;

	for(int j=0; j<6; j++) 
	{
		m_Gizmo[j] = NULL;
		m_Pos[j]=0;
		m_Sort[j]=j;
	}

  m_ViewCT    = NULL;
  m_ColorLUT  = NULL;
  m_LutSwatch = NULL;
  m_CurrentVolume = NULL;
  m_LayoutConfiguration = LAYOUT_CUSTOM;

  m_MoveAllSlices = 0;
  m_RightOrLeft=1;
  m_MoveAllSlices = 0; 
  m_Snap=1;
  m_CurrentSurface.clear();
  m_AllSurface=0;
  m_Border=1;

	m_NumSlice=6;
}
//----------------------------------------------------------------------------
mafViewRXCTLHPBuilder::~mafViewRXCTLHPBuilder()
//----------------------------------------------------------------------------
{
  m_ViewsRX[0] = NULL;
  m_ViewsRX[1] = NULL;
  m_ViewCT = NULL;

	m_CurrentSurface.clear();
}
//-------------------------------------------------------------------------
mmgGui* mafViewRXCTLHPBuilder::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  wxString m_Choices[2];
  m_Choices[0]="Right";
  m_Choices[1]="Left";
  m_Gui->Radio(ID_RIGHT_OR_LEFT,"Side",&m_RightOrLeft,2,m_Choices);

  m_Gui->Bool(ID_SNAP,"Snap on grid",&m_Snap);

  m_Gui->Bool(ID_MOVE_ALL_SLICES,"Move all",&m_MoveAllSlices);

  m_Gui->Button(ID_ADJUST_SLICES,"Adjust Slices");
  
  if (m_ColorLUT == NULL)
  {
    vtkNEW(m_ColorLUT);
  }
  lutPreset(4,m_ColorLUT);
  //m_Gui->Lut(ID_LUT_CHOOSER,"lut",m_ColorLUT);

  //EnableWidgets(m_CurrentVolume != NULL);
/*  for(int i=1; i<m_NumOfChildView; i++)
  {
    m_ChildViewList[i]->GetGui();
  }*/
  int sub_gui;
  for (sub_gui=0; sub_gui<2; sub_gui++) 
  {
    m_ChildViewList[sub_gui]->GetGui();
  }
  for (sub_gui=0; sub_gui<6; sub_gui++) 
  {
    ((mafViewSlice *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(sub_gui))->GetGui();
  }
  m_Gui->Divider(1);

  m_Gui->Bool(ID_ALL_SURFACE,"All Surface",&m_AllSurface);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,"Border",&m_Border,1.0,5.0);
  
  mafNode* node=this->GetSceneGraph()->GetSelectedVme();
  if (node->IsA("mafVMESurface"))
  {
	  m_Gui->Enable(ID_ALL_SURFACE,true);
	  m_Gui->Enable(ID_BORDER_CHANGE,true);
	  m_Gui->Enable(ID_ADJUST_SLICES,true);
  }
  else
  {
	  m_Gui->Enable(ID_ALL_SURFACE,false);
	  m_Gui->Enable(ID_BORDER_CHANGE,false);
	  m_Gui->Enable(ID_ADJUST_SLICES,false);
  }
	((mafViewRXLHPBuilder *)m_ChildViewList[0])->EnableWidgets(m_CurrentVolume != NULL);
	((mafViewRXLHPBuilder *)m_ChildViewList[1])->EnableWidgets(m_CurrentVolume != NULL);
	((mafLHPBuilderViewCompound *)m_ChildViewList[2])->EnableWidgets(m_CurrentVolume != NULL);
  return m_Gui;
}
//----------------------------------------------------------------------------
mafView *mafViewRXCTLHPBuilder::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewRXCTLHPBuilder *v = new mafViewRXCTLHPBuilder(m_Label);
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
void mafViewRXCTLHPBuilder::VmeSelect(mafNode *node, bool select)
//----------------------------------------------------------------------------
{
  mafViewCompound::VmeSelect(node,select);
  // code to manage widget.
  SetGuiForBorder(node,select);
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::SetGuiForBorder(mafNode *node,bool select)
//----------------------------------------------------------------------------
{
	if(m_Gui)
	{

		if(node->IsA("mafVMESurface"))
		{
			mafSceneNode *SN = this->GetSceneGraph()->Vme2Node(node);
			if (select && SN->IsVisible())
			{
				
				mafPipe *p=((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(0))->GetNodePipe(node);
				m_Gui->Enable(ID_ALL_SURFACE,true);
				m_Gui->Enable(ID_BORDER_CHANGE,true);
				m_Gui->Enable(ID_ADJUST_SLICES,true);
				if (p)
				{
					double old_thickness=((mafPipeSurfaceSlice *)p)->GetThickness();
					m_Border=old_thickness;
					m_Gui->Update();
				}
				else
					m_Border=1;
			}
			else
			{
				m_Gui->Enable(ID_ALL_SURFACE,false);
				m_Gui->Enable(ID_BORDER_CHANGE,false);
				m_Gui->Enable(ID_ADJUST_SLICES,false);
			}
		}
   }
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(maf_event->GetId()) 
    {
      /*case ID_LUT_CHOOSER:
      {
        for(int i=0; i<6; i++)
        {
          mafPipeVolumeSlice *p = NULL;
          p = mafPipeVolumeSlice::SafeDownCast(((mafViewSlice *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
          p->SetColorLookupTable(m_ColorLUT);
        }
        double* range=m_ColorLUT->GetRange();
        ((mafLHPBuilderViewCompound*)m_ChildViewList[2])->SetLut(range[0],range[1]);
        CameraUpdate();
      }
      break;*/
      case MOUSE_UP:
      case MOUSE_MOVE:
      {
        long slice = e->GetArg();
        //float pos = e->GetDouble();        //modified by Paolo 26-5-2003
        double pos[3];
        vtkPoints *p = (vtkPoints *)e->GetVtkObj();
        if(p == NULL) return;
        p->GetPoint(0,pos);
        if (m_MoveAllSlices)
        {
          double old_slice[3], delta[3], b[6];
          ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(slice))->GetSlice(old_slice);
          delta[0] = pos[0] - old_slice[0];
          delta[1] = pos[1] - old_slice[1];
          delta[2] = pos[2] - old_slice[2];
          for (int sv=0; sv<((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetNumberOfSubView(); sv++)
          {
            if (m_Gizmo[sv])
            {
              m_CurrentVolume->GetOutput()->GetVMEBounds(b);
							int i=0;
							while (sv!=m_Sort[i]) i++;
              
							((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->GetSlice(old_slice);
              pos[0] = old_slice[0] + delta[0];
              pos[1] = old_slice[1] + delta[1];
              pos[2] = old_slice[2] + delta[2];
              pos[2] = pos[2] > b[5] ? b[5] : pos[2];
              pos[2] = pos[2] < b[4] ? b[4] : pos[2];
              m_Gizmo[sv]->SetSlice(sv,mafGizmoSlice::GIZMO_SLICE_Z,pos[2]);
							
							m_Pos[sv]=pos[2];

              ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->SetSlice(pos);
			        ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->UpdateText();
			        ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->CameraUpdate();
            }
           }
        }
        else
        {
          
					m_Pos[slice]=pos[2];
					SortSlices();
					int i=0;
					while (slice!=m_Sort[i]) i++;
					((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->SetSlice(pos);
					((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->UpdateText();
          ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->CameraUpdate();
				}
        m_ChildViewList[0]->CameraUpdate();
        m_ChildViewList[1]->CameraUpdate();

      }
      break;
	  case ID_RIGHT_OR_LEFT:
		  {
			  if (m_RightOrLeft==0)
			  {
				((mafViewRXLHPBuilder *)m_ChildViewList[1])->CameraSet(CAMERA_RX_RIGHT);
			  }
			  else
				((mafViewRXLHPBuilder *)m_ChildViewList[1])->CameraSet(CAMERA_RX_LEFT);
		  }
	  break;
	  case ID_SNAP:
		  {
			  if(this->m_CurrentVolume==NULL && m_Snap)
			  {
				  wxMessageBox("You can't switch to snap modality!");
				  m_Snap=0;
				  m_Gui->Update();
			  }
			  else
			  {
				for(int i=0; i<6; i++)
				{
					if(m_Snap==1)
						m_Gizmo[i]->SetGizmoModalityToSnap();
					else
						m_Gizmo[i]->SetGizmoModalityToBound();
				}
			  }
		  }
	  break;
	  case ID_ADJUST_SLICES:
		  {
			  mafNode* node=GetSceneGraph()->GetSelectedVme();
			  mafPipe *p=((mafViewRXLHPBuilder *)m_ChildViewList[0])->GetNodePipe(node);
			  if (node->IsMAFType(mafVMEVolume))
				  mafLogMessage("SURFACE NOT SELECTED");
			  else  if (node->IsMAFType(mafVMESurface))
			  {
				  double center[3],b[6],step;
				  mafVMESurface *surface=(mafVMESurface*)node;
				  surface->GetOutput()->GetBounds(b);
				  step = (b[5]-b[4])/7.0;
				  center[0]=0;
				  center[1]=0;
				  for (int sv=0; sv<((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetNumberOfSubView(); sv++)
				  {
            if(m_Gizmo[sv])
            {
					    center[2] = b[5]-step*(sv+1);
						  center[2] = center[2] > b[5] ? b[5] : center[2];
						  center[2] = center[2] < b[4] ? b[4] : center[2];
						  m_Gizmo[sv]->SetSlice(sv,mafGizmoSlice::GIZMO_SLICE_Z,center[2]);
							m_Pos[sv]=center[2];
							m_Sort[sv]=sv;
					    ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(sv))->SetSlice(center);
						  ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(sv))->SetTextColor(m_BorderColor[sv]);
							((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(sv))->UpdateText();
							((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(sv))->BorderCreate(m_BorderColor[sv]);
						  ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(sv))->CameraUpdate();
            }
				  }
				  m_ChildViewList[0]->CameraUpdate();
				  m_ChildViewList[1]->CameraUpdate();
			  }
		  }
	  break;
	  case ID_BORDER_CHANGE:
		  {

			  if(m_AllSurface)
			  {
					mafNode* node=this->GetSceneGraph()->GetSelectedVme();
					mafVME* vme=(mafVME*)node;
					mafNode* root=vme->GetRoot();
		      NavigateTree(root);
			  }
			  else
			  {
				mafNode *node=this->GetSceneGraph()->GetSelectedVme();
				mafSceneNode *SN = this->GetSceneGraph()->Vme2Node(node);
				mafPipe *p=((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(0))->GetNodePipe(node);
				((mafPipeSurfaceSlice *)p)->SetThickness(m_Border);
			  }

		  }
	  break;
	  case ID_ALL_SURFACE:
		  {
			  if(m_AllSurface)
			  {
				mafNode* node=GetSceneGraph()->GetSelectedVme();
				mafVME* vme=(mafVME*)node;
				mafNode* root=vme->GetRoot();
				NavigateTree(root);
			  }
		  }
      default:
        mafViewCompound::OnEvent(maf_event);
    }
  }
  else
  {
    mafViewCompound::OnEvent(maf_event);
  }
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::PackageView()
//----------------------------------------------------------------------------
{
  int cam_pos[2] = {CAMERA_RX_FRONT, CAMERA_RX_LEFT};
  for(int v=0; v<2; v++)
  {
    m_ViewsRX[v] = new mafViewRXLHPBuilder("RX view", cam_pos[v]);
    m_ViewsRX[v]->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeProjected",MUTEX);
	  PlugChildView(m_ViewsRX[v]);
  }
  m_ViewCT = new mafLHPBuilderViewCompound("CT view",3);
  mafViewSliceLHPBuilder *vs = new mafViewSliceLHPBuilder("Slice view", CAMERA_CT);
  vs->PlugVisualPipe("mafVMEVolumeGray", "mafPipeVolumeSlice",MUTEX);
  vs->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice",MUTEX);
  m_ViewCT->PlugChildView(vs);
  PlugChildView(m_ViewCT);
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  Superclass::OnSize(event);
  for (int i=0;i<2;i++)
  {
	  ((mafViewRXLHPBuilder *)m_ChildViewList[i])->OnSize(event);
  }
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::VmeShow(mafNode *node, bool show)
//----------------------------------------------------------------------------
{
  for(int i=0; i<2; i++)
    m_ChildViewList[i]->VmeShow(node, show);

  if (node->IsMAFType(mafVMEVolume))
  {
    if (show)
    {
      double sr[2],center[3],b[6],step;
      m_CurrentVolume = mafVMEVolume::SafeDownCast(node);
      //mmaVolumeMaterial *material = m_CurrentVolume->GetMaterial();
      vtkDataSet *data = ((mafVME *)node)->GetOutput()->GetVTKData();
      data->Update();
      data->GetCenter(center);
      data->GetBounds(b);
      step = (b[5]-b[4])/7.0;
      data->GetScalarRange(sr);
      //m_ColorLUT = material->m_ColorLut;
      m_ColorLUT->SetRange(sr);
      m_ColorLUT->Build();
      lutPreset(4,m_ColorLUT);
      //m_LutSwatch->SetLut(m_ColorLUT);
      for(int i=0; i<6; i++)
      {
        center[2] = b[5]-step*(i+1);
        ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->InitializeSlice(center);
        ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->VmeShow(node,show);
        ((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->SetTextColor(m_BorderColor[i]);
		    mafPipeVolumeSlice *p = NULL;
        p = mafPipeVolumeSlice::SafeDownCast(((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->GetNodePipe(node));
        p->SetColorLookupTable(m_ColorLUT);
				//m_Sort[i] = m_NumSlice - i -1;
				m_Pos[i] = b[5]-step*(i+1);
      }
	    ((mafLHPBuilderViewCompound *)m_ChildViewList[2])->VmeShow(node,show);
	    m_CurrentVolume->Update();
      GizmoCreate(node);

    }//if (show)
    else
    {
      lutPreset(4,m_ColorLUT);
      m_ChildViewList[2]->VmeShow(node, show);
	    m_CurrentVolume->GetEventSource()->RemoveObserver(this);
      m_CurrentVolume = NULL;
	    ((mafViewRXLHPBuilder *)m_ChildViewList[0])->EnableWidgets(m_CurrentVolume != NULL);
	    ((mafViewRXLHPBuilder *)m_ChildViewList[1])->EnableWidgets(m_CurrentVolume != NULL);
      GizmoDelete();
	  }//if (show) else
  }//if mafVMEVolume
  else if (node->IsMAFType(mafVMESurface))
  {
		if (show && m_CurrentVolume)
			{
				mafNode *node_selected = this->GetSceneGraph()->GetSelectedVme();
				((mafLHPBuilderViewCompound *)m_ChildViewList[2])->VmeShow(node,show);
				for(int j=0;j<m_NumSlice;j++)
				{
					int i=0;
					while (j!=m_Sort[i]) i++;
					double pos[3]={0.0,0.0,m_Pos[m_Sort[i]]};
					//((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->SetSlice(pos);
					//((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->UpdateText();
					((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->CameraUpdate();
				}
				mafPipe *p=((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(0))->GetNodePipe(node);
				if (node_selected==node)
				{
					m_Gui->Enable(ID_ALL_SURFACE,true);
					m_Gui->Enable(ID_BORDER_CHANGE,true);
					m_Gui->Enable(ID_ADJUST_SLICES,true);
					if (p)
					{
						double old_thickness=((mafPipeSurfaceSlice *)p)->GetThickness();
						m_Border=old_thickness;
						m_Gui->Update();
					}
					else
						m_Border=1;
				}
			}//if (show)
	else if (!show)
	{
			m_ChildViewList[2]->VmeShow(node, show);
      mafNode *node_selected = this->GetSceneGraph()->GetSelectedVme();
      if (node_selected==node)
			{
			m_Gui->Enable(ID_ALL_SURFACE,false);
			m_Gui->Enable(ID_BORDER_CHANGE,false);
			m_Gui->Enable(ID_ADJUST_SLICES,false);
			}
			for(int i=0; i<6; i++)
				{
					((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(i))->UpdateListSurface(node);
				}
		}//else if(show)
  }
  else
  {
    m_ChildViewList[2]->VmeShow(node, show);
  }
  EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::GizmoDelete()
//----------------------------------------------------------------------------
{
  for(int i=0; i<6; i++) 
  {
    if(m_Gizmo[i])
    {
      ((mafViewSlice *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->BorderDelete();
      m_ChildViewList[0]->VmeShow(m_Gizmo[i]->GetOutput(),false);
      m_ChildViewList[1]->VmeShow(m_Gizmo[i]->GetOutput(),false);
      cppDEL(m_Gizmo[i]);
    }
  }
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::GizmoCreate(mafNode *node)
//----------------------------------------------------------------------------
{
		for(int i=0; i<6; i++) 
		{
			double slice[3];
			mafPipeVolumeSlice *p = NULL;
			p = mafPipeVolumeSlice::SafeDownCast(((mafViewSliceLHPBuilder *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->GetNodePipe(m_CurrentVolume));
			p->GetSliceOrigin(slice);
			m_Gizmo[i] = new mafGizmoSlice(m_CurrentVolume, this);
			m_Gizmo[i]->SetSlice(i,mafGizmoSlice::GIZMO_SLICE_Z,slice[2]);
			m_Gizmo[i]->SetColor(m_BorderColor[i]);
			((mafViewSliceLHPBuilder *)((mafViewCompound *)m_ChildViewList[2])->GetSubView(i))->BorderCreate(m_BorderColor[i]);

			m_ChildViewList[0]->VmeShow(m_Gizmo[i]->GetOutput(), true);
			m_ChildViewList[1]->VmeShow(m_Gizmo[i]->GetOutput(), true);
		}
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::NavigateTree(mafNode *root)
//----------------------------------------------------------------------------
{
  mafNodeIterator *iter = root->NewIterator();
  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
	{
		if(node->IsA("mafVMESurface"))
		{
			mafPipe *p=((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(0))->GetNodePipe(node);
			if(p)
				((mafPipeSurfaceSlice *)p)->SetThickness(m_Border);
		}
	}
  iter->Delete();
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::VmeRemove(mafNode *node)
//----------------------------------------------------------------------------
{
  if (m_CurrentVolume && node == m_CurrentVolume) 
  {
    m_CurrentVolume = NULL;
    GizmoDelete();
  }
  Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  if (m_Gui)
  {
    m_Gui->Enable(ID_LUT_CHOOSER,enable);
  }
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::LayoutSubViewCustom(int width, int height)
//----------------------------------------------------------------------------
{
  // this implement the Fixed SubViews Layout
  int border = 2;
  int x_pos, c, i;

  int step_width  = (width-border) / 3;
  i = 0;
  for (c = 0; c < m_NumOfChildView; c++)
  {
    x_pos = c*(step_width + border);
    m_ChildViewList[i]->GetWindow()->SetSize(x_pos, 0, step_width, height);
    i++;
  }
  ((mafViewCompound *)m_ChildViewList[i-1])->OnLayout();
}
//----------------------------------------------------------------------------
void mafViewRXCTLHPBuilder::SortSlices()
//----------------------------------------------------------------------------
{
   bool modified = false;
	 int i,j,t;
	 //check if a ct view should change posistion
   for(j=0; j<m_NumSlice; j++)
	 {
      for(i=j; i<m_NumSlice; i++)
			{
	       if( m_Pos[m_Sort[j]] < m_Pos[m_Sort[i]])
				 {
            t = m_Sort[j];
						m_Sort[j] = m_Sort[i];
            m_Sort[i] = t;
						modified=true; 
				 }
			}
	 }	

	 if (modified)
	 {
		double *OldPos;
		for(j=0;j<m_NumSlice; j++)
		{
				OldPos=((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(j))->GetSlice();
				OldPos[2]=m_Pos[m_Sort[j]];
				((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(j))->SetSlice(OldPos);
				((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(j))->SetTextColor(m_BorderColor[m_Sort[j]]);
				((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(j))->UpdateText();
				((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(j))->BorderCreate(m_BorderColor[m_Sort[j]]);
				((mafViewSliceLHPBuilder *)((mafLHPBuilderViewCompound *)m_ChildViewList[2])->GetSubView(j))->CameraUpdate();
		}
	 }

}