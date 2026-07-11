#include "mafViewCompound.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafInteractor.h"
#include "mafAvatar.h"
#include "mafAvatar3D.h"
#include "mafDeviceButtonsPadTracker.h"
#include "mafDeviceButtonsPadMouse.h"

//----------------------------------------------------------------------------
mafViewCompound::mafViewCompound(const mafString& label, int num_row, int num_col)
	: mafView(label)
	//----------------------------------------------------------------------------
{
	m_ViewRowNum = num_row;
	m_ViewColNum = num_col;
	m_DefauldChildView = 0;
	m_LinkSubView = 0;
	m_ChildViewList.clear();
	m_PluggedChildViewList.clear();
	m_GuiView = NULL;
	m_GuiViewWindow = NULL;
	m_SubViewMaximized = -1;
	m_LayoutConfiguration = GRID_LAYOUT;
}

mafViewCompound::~mafViewCompound() = default;

mafView* mafViewCompound::Copy(mafBaseEventHandler* Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
	m_LightCopyEnabled = lightCopyEnabled;
	auto v = new mafViewCompound(GetLabel(), m_ViewRowNum, m_ViewColNum);
	v->SetListener(Listener);
	v->m_Id = m_Id;
	for (auto& pluggedChild : m_PluggedChildViewList)
	{
		v->m_PluggedChildViewList.emplace_back(pluggedChild->Copy(this, m_LightCopyEnabled));
	}
	v->m_LightCopyEnabled = lightCopyEnabled;
	v->Create();
	return v;
}
//----------------------------------------------------------------------------
void mafViewCompound::PlugChildView(std::unique_ptr<mafView>  child)
//----------------------------------------------------------------------------
{
	if (child)
	{
		m_PluggedChildViewList.push_back(std::move(child));
	}
}
//----------------------------------------------------------------------------
void mafViewCompound::Create()
//----------------------------------------------------------------------------
{
	auto w = new mafGUIViewWin(mafGetFrame(), -1);
	w->SetBackgroundColour(wxColour(102, 102, 102));
	w->m_Owner = this;
	w->Show(false);
	m_Win = w;

	for (auto& pluggedChild : m_PluggedChildViewList)
	{
		m_ChildViewList.emplace_back(pluggedChild->Copy(this, m_LightCopyEnabled));
		if (m_ChildViewList.back()->GetWindow()) {//MOD_DAN_LIGHT
			m_ChildViewList.back()->GetWindow()->Reparent(m_Win);
			m_ChildViewList.back()->GetWindow()->Show(true);
		}
	}
	for (int f = m_PluggedChildViewList.size(); f < m_ViewColNum * m_ViewRowNum; f++)
	{
		m_ChildViewList.emplace_back(m_PluggedChildViewList.back()->Copy(this, m_LightCopyEnabled));
		if (m_ChildViewList[f]->GetWindow()) {//MOD_DAN_LIGHT
			m_ChildViewList[f]->GetWindow()->Reparent(m_Win);
			m_ChildViewList[f]->GetWindow()->Show(true);
		}
	}

	CreateGuiView();
}
//----------------------------------------------------------------------------
void mafViewCompound::CreateGuiView()
//----------------------------------------------------------------------------
{
	/*m_GuiView = new mafGUI(this);
	m_GuiView->Label("Compound View's GUI",true);
	m_GuiView->Reparent(m_Win);*/
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeAdd(std::shared_ptr<mafNode> node)
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
		childView->VmeAdd(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeRemove(mafNode* node)
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
		childView->VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeSelect(mafNode* node, bool select)
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
		childView->VmeSelect(node, select);
}
//----------------------------------------------------------------------------
void mafViewCompound::VmeShow(mafNode* node, bool show)
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
		childView->VmeShow(node, show);
}
//----------------------------------------------------------------------------
int mafViewCompound::GetNodeStatusI(mafNode* node)
//----------------------------------------------------------------------------
{
	// should be redefined for compounded views
	return m_ChildViewList[m_DefauldChildView]->GetNodeStatus(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::CameraReset(mafNode* node)
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
		childView->CameraReset(node);
}
//----------------------------------------------------------------------------
void mafViewCompound::CameraUpdate()
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
		childView->CameraUpdate();
}
//----------------------------------------------------------------------------
mafSceneGraph* mafViewCompound::GetSceneGraph()
//----------------------------------------------------------------------------
{
	//return ((mafViewVTK *)m_ChildViewList[m_DefauldChildView])->GetSceneGraph();
	return GetSubView()->GetSceneGraph();
}
//----------------------------------------------------------------------------
vtkRenderWindow* mafViewCompound::GetRWI()
//----------------------------------------------------------------------------
{
	//return ((mafViewVTK *)m_ChildViewList[m_DefauldChildView])->GetRWI();
	return GetSubView()->GetRWI();
}
//----------------------------------------------------------------------------
bool mafViewCompound::FindPokedVme(mafDevice* device, mafMatrix& point_pose, vtkProp3D*& picked_prop, mafVME*& picked_vme, mafInteractor*& picked_behavior)
//----------------------------------------------------------------------------
{
	if (auto v = mafViewVTK::SafeDownCast(GetSubView()))
		return v->FindPokedVme(device, point_pose, picked_prop, picked_vme, picked_behavior);
	return false;
}
/*//----------------------------------------------------------------------------
mafPipe *mafViewCompound::GetNodePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafSceneGraph *sg = ((mafViewVTK *)m_ChildViewList[m_DefauldChildView])->GetSceneGraph();
  assert(sg);
  mafSceneNode *n = sg->Vme2Node(vme);
  if(!n)
	return NULL;
  return n->m_Pipe;
}
//----------------------------------------------------------------------------
mafGUI *mafViewCompound::GetNodePipeGUI(mafNode *vme)
//----------------------------------------------------------------------------
{
  return GetNodePipeGUI(vme, m_DefauldChildView);
}
//----------------------------------------------------------------------------
mafGUI *mafViewCompound::GetNodePipeGUI(mafNode *vme, int view_idx)
//----------------------------------------------------------------------------
{
  mafSceneGraph *sg = ((mafViewVTK *)m_ChildViewList[view_idx])->GetSceneGraph();
  assert(sg);
  mafSceneNode *n = sg->Vme2Node(vme);
  if(!n)
	return NULL;
  if(n->m_Pipe)
  {
	return n->m_Pipe->GetGui();
  }
  return NULL;
}*/
//----------------------------------------------------------------------------
void mafViewCompound::OnEvent(mafEventBase* maf_event)
//----------------------------------------------------------------------------
{
	switch (maf_event->GetId())
	{
	case ID_DEFAULT_CHILD_VIEW:
	{
		mafSceneGraph* sg = GetSceneGraph();
		if (sg)
		{
			mafNode* vme = sg->GetSelectedVme();
			if (vme)
			{
				{ mafEvent evUnq(this, VME_MODIFIED); evUnq.SetVme(sg->Vme2Node(vme)->m_Vme.get()); InvokeEvent(evUnq); }
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
	default:
		mafView::OnEvent(maf_event);
	}
}
//-------------------------------------------------------------------------
mafGUI* mafViewCompound::CreateGui()
//-------------------------------------------------------------------------
{
	mafString childview_tooltip;
	childview_tooltip = _R("set the default child view");

	mafString layout_choices[4] = { _R("default"),_R("layout 1"),_R("layout 2"), _R("custom") };

	assert(!AccessGUI());
	auto gui = new mafGUI(this);
	gui->Integer(ID_DEFAULT_CHILD_VIEW, _R("default child"), &m_DefauldChildView, 0, m_ChildViewList.size(), childview_tooltip);
	gui->Combo(ID_LAYOUT_CHOOSER, _R("layout"), &m_LayoutConfiguration, 4, layout_choices);
	gui->Bool(ID_LINK_SUBVIEW, _R("link camera"), &m_LinkSubView);
	return gui;
}
//----------------------------------------------------------------------------
void mafViewCompound::OnSize(wxSizeEvent& event)
//----------------------------------------------------------------------------
{
	m_Size = event.GetSize();
	OnLayout();
}
//----------------------------------------------------------------------------
void mafViewCompound::OnLayout()
//----------------------------------------------------------------------------
{
	if (m_PluggedChildViewList.empty())
	{
		return;
	}

	int sw = m_Size.GetWidth();
	int sh = m_Size.GetHeight();
	int gh = 0;

	if (m_GuiViewWindow)
	{
		wxSize gui_size = m_GuiViewWindow->GetBestSize();
		gh = gui_size.GetHeight();
		if (sw < gh || sh < gh) return;
		sh -= gh;
		m_GuiViewWindow->SetMinSize(wxSize(sw, gh));
		m_GuiViewWindow->SetSize(0, sh, sw, gh);
	}

	if (m_GuiView)
	{
		wxSize gui_size = m_GuiView->GetBestSize();
		gh = gui_size.GetHeight();
		if (sw < gh || sh < gh) return;
		sh -= gh;
		m_GuiView->SetMinSize(wxSize(sw, gh));
		m_GuiView->SetSize(0, sh, sw, gh);
	}

	if (m_SubViewMaximized == -1)
	{
		LayoutSubView(sw, sh);
	}
	else
	{
		int i = 0;
		for (int r = 0; r < m_ViewRowNum; r++)
		{
			for (int c = 0; c < m_ViewColNum; c++)
			{
				if (i == m_SubViewMaximized)
				{
					m_ChildViewList[m_SubViewMaximized]->GetWindow()->SetSize(0, 0, sw, sh);
#ifndef WIN32
					m_ChildViewList[m_SubViewMaximized]->SetWindowSize(sw, sh);
#endif
				}
				else
				{
					m_ChildViewList[i]->GetWindow()->SetSize(0, 0, 0, 0);
#ifndef WIN32
					m_ChildViewList[i]->SetWindowSize(0, 0);
#endif
				}
				i++;
			}
		}
	}
}
//----------------------------------------------------------------------------
void mafViewCompound::LayoutSubView(int width, int height)
//----------------------------------------------------------------------------
{
	// this implement the Fixed SubViews Layout
	int border = 2;
	int x_pos, y_pos, r, c, i;

	if (m_LayoutConfiguration == GRID_LAYOUT)
	{
		int step_width = (width - border) / m_ViewColNum;
		int step_height = (height - 2 * border) / m_ViewRowNum;

		i = 0;
		for (r = 0; r < m_ViewRowNum; r++)
		{
			for (c = 0; c < m_ViewColNum; c++)
			{
				x_pos = c * (step_width + border);
				y_pos = r * (step_height + border);
				m_ChildViewList[i]->GetWindow()->SetSize(x_pos, y_pos, step_width, step_height);
#ifndef WIN32
				m_ChildViewList[i]->SetWindowSize(step_width, step_height);
#endif
				i++;
			}
		}
	}
	else if (m_LayoutConfiguration == LAYOUT_1)
	{
		int step_width = (width - border) / (m_ChildViewList.size() - 1);
		int step_height = (height - 2 * border) / 3 * 2;
		m_ChildViewList[0]->GetWindow()->SetSize(0, 0, width, step_height);
#ifndef WIN32
		m_ChildViewList[0]->SetWindowSize(width, step_height);
#endif
		i = 1;
		for (c = 0; c < m_ChildViewList.size() - 1; c++)
		{
			x_pos = c * (step_width + border);
			y_pos = step_height;
			m_ChildViewList[i]->GetWindow()->SetSize(x_pos, y_pos, step_width, height - step_height);
#ifndef WIN32
			m_ChildViewList[i]->SetWindowSize(step_width, height - step_height);
#endif
			i++;
		}
	}
	else if (m_LayoutConfiguration == LAYOUT_2)
	{
		int step_width = (width - border) / 3 * 2;
		int step_height = (height - 2 * border) / (m_ChildViewList.size() - 1);
		m_ChildViewList[0]->GetWindow()->SetSize(0, 0, step_width, height);
#ifndef WIN32
		m_ChildViewList[0]->SetWindowSize(step_width, height);
#endif
		i = 1;
		for (r = 0; r < m_ChildViewList.size() - 1; r++)
		{
			x_pos = step_width;
			y_pos = r * (step_height + border);
			m_ChildViewList[i]->GetWindow()->SetSize(x_pos, y_pos, width - step_width, step_height);
#ifndef WIN32
			m_ChildViewList[i]->SetWindowSize(width - step_width, step_height);
#endif
			i++;
		}
	}
	else
	{
		LayoutSubViewCustom(width, height);
	}
}
//----------------------------------------------------------------------------
mafView* mafViewCompound::GetSubView()
//----------------------------------------------------------------------------
{
	if (auto rwi = GetGlobalMouse()->GetRenderWindow())
	{
		for (auto& childView : m_ChildViewList)
		{
			if (childView->IsMAFType(mafViewCompound))
			{
				if (mafViewCompound::StaticDownCast(childView.get())->GetSubView()->GetRWI() == rwi)
				{
					return mafViewCompound::StaticDownCast(childView.get())->GetSubView();
				}
			}
			else if (mafViewVTK::StaticDownCast(childView.get())->GetRWI() == rwi)
			{
				return childView.get();
			}
		}
	}
	return m_ChildViewList[m_DefauldChildView].get();
}
//----------------------------------------------------------------------------
mafView* mafViewCompound::GetSubView(int idx)
//----------------------------------------------------------------------------
{
	if (idx >= 0 && idx < m_ChildViewList.size())
	{
		return m_ChildViewList[idx].get();
	}
	return m_ChildViewList[m_DefauldChildView].get();
}
//----------------------------------------------------------------------------
int mafViewCompound::GetSubViewIndex()
//----------------------------------------------------------------------------
{
	if (auto rwi = GetGlobalMouse()->GetRenderWindow())
	{
		for (int i = 0; i < m_ChildViewList.size(); i++)
		{
			if (mafViewVTK::StaticDownCast(m_ChildViewList[i].get())->GetRWI() == rwi)
			{
				return i;
			}
		}
	}
	return m_DefauldChildView;
}
//----------------------------------------------------------------------------
void mafViewCompound::MaximizeSubView(int subview_id, bool maximize)
//----------------------------------------------------------------------------
{
	if (m_SubViewMaximized != -1 && maximize)
	{
		return;
	}
	if (subview_id < 0 || subview_id >= m_ChildViewList.size())
	{
		mafMessage(_M("Wrong sub-view id !!"));
		return;
	}
	m_SubViewMaximized = maximize ? subview_id : -1;
	OnLayout();
}
//----------------------------------------------------------------------------
bool mafViewCompound::Pick(int x, int y)
//----------------------------------------------------------------------------
{
	if (auto sub_view = GetSubView())
	{
		return sub_view->Pick(x, y);
	}
	return false;
}
//----------------------------------------------------------------------------
bool mafViewCompound::Pick(mafMatrix& m)
//----------------------------------------------------------------------------
{
	if (auto sub_view = GetSubView())
	{
		return sub_view->Pick(m);
	}
	return false;
}
//----------------------------------------------------------------------------
void mafViewCompound::GetPickedPosition(double pos[3])
//----------------------------------------------------------------------------
{
	if (auto sub_view = GetSubView())
	{
		sub_view->GetPickedPosition(pos);
	}
}
//----------------------------------------------------------------------------
mafVME* mafViewCompound::GetPickedVme()
//----------------------------------------------------------------------------
{
	if (auto sub_view = GetSubView())
	{
		return sub_view->GetPickedVme();
	}
	return nullptr;
}
//----------------------------------------------------------------------------
void mafViewCompound::Print(wxDC* dc, wxRect margins)
//----------------------------------------------------------------------------
{
	if (m_PluggedChildViewList.empty())
	{
		return;
	}

	wxBitmap image;
	GetImage(image/*, 2*/);
	PrintBitmap(dc, margins, &image);
}
//----------------------------------------------------------------------------
void mafViewCompound::GetImage(wxBitmap& bmp, int magnification)
//----------------------------------------------------------------------------
{
	wxSize vsz = this->GetWindow()->GetSize();
	wxBitmap compoundImage = wxBitmap(magnification * vsz.GetWidth(), magnification * vsz.GetHeight(), 24);
	wxMemoryDC compoundDC;
	compoundDC.SelectObject(compoundImage);
	compoundDC.SetBackground(*wxWHITE_BRUSH);
	compoundDC.Clear();
	// this implement the Fixed SubViews Print

	int x_pos, y_pos;
	wxMemoryDC subViewDC;
	for (auto& childView : m_ChildViewList)
	{
		wxSize win_size = childView->GetWindow()->GetSize();
		if (win_size.GetWidth() == 0 || win_size.GetHeight() == 0) continue;
		wxBitmap image;
		childView->GetImage(image, magnification);
		float iw = image.GetWidth();
		float ih = image.GetHeight();
		childView->GetWindow()->GetPosition(&x_pos, &y_pos);
		subViewDC.SelectObject(image);
		compoundDC.Blit(magnification * x_pos, magnification * y_pos, iw, ih, &subViewDC, 0, 0);
	}

	compoundDC.SelectObject(wxNullBitmap);
	bmp = compoundImage;
}
//----------------------------------------------------------------------------
void mafViewCompound::LinkView(bool link_camera)
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
	{
		if (auto vv = mafViewVTK::SafeDownCast(childView.get()))
		{
			vv->LinkView(link_camera);
		}
		else if (auto cv = mafViewCompound::SafeDownCast(childView.get()))
		{
			cv->LinkView(link_camera);
		}
	}
}
//----------------------------------------------------------------------------
void mafViewCompound::OptionsUpdate()
//----------------------------------------------------------------------------
{
	for (auto& childView : m_ChildViewList)
		childView->OptionsUpdate();
}
