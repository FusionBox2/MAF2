#include "mafViewManager.h"
#include <wx/config.h>
#include "mafDecl.h"
#include "mafView.h"
#include "mafNode.h"
#include "mafVMERoot.h"

#include "ftk/Gui/wx/MainFrame.h"
#include "ftk/Gui/wx/ViewFrame.h"
#include "mafViewVTK.h"

#include "mafEvent.h"
#include "mafDeviceButtonsPadMouse.h"

mafViewManager::mafViewManager() = default;

mafViewManager::~mafViewManager()
{
	ViewDeleteAll();
}

void mafViewManager::SetMouse(mafDeviceButtonsPadMouse* mouse)
{
	m_Mouse = mouse;
	if (m_SelectedView && m_Mouse)
	{
		mafEvent evUnq(this, VIEW_SELECT); evUnq.SetView(m_SelectedView); m_Mouse->OnEvent(&evUnq);
	} // Update the mouse for the selected view
}

void mafViewManager::OnEvent(mafEventBase* maf_event)
{
	if (mafEvent* e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case VIEW_CREATE:
			ViewCreate(e->GetId());
			break;
		case VIEW_DELETE:
			ViewDelete(e->GetView());
			break;
		case VIEW_SELECT:
		{
			auto view = e->GetView();
			bool notifylogic = (view != m_SelectedView);

			ViewSelected(view/*, rwi*/);

			if (notifylogic)
			{
				mafEvent evUnq(this, VIEW_SELECT); evUnq.SetArg((intptr_t)e->GetSender()); InvokeEvent(evUnq);
			} // forward the view selection event to logic

			if (m_CollaborateStatus && m_RemoteListener && !m_FromRemote)
			{
				// Send the event to synchronize the remote application in case of collaboration modality
				mafEvent ev(this, VIEW_SELECTED);
				ev.SetView(view);
				ev.SetChannel(REMOTE_COMMAND_CHANNEL);
				m_RemoteListener->OnEvent(&ev);
			}
		}
		break;
		default:
			InvokeEvent(*e); // forward up the event
			break;
		}
	}
}

long mafViewManager::ViewAdd(std::unique_ptr<mafView> view)
{
	// Update the view ID (starting from VIEW_START)
	view->m_Id = m_ViewTemplate.size() + VIEW_START;
	m_ViewTemplate.emplace_back(std::move(view));
	m_ViewTemplate.back()->m_Mult = 0; // template views multiplicity is always 0
	m_ViewTemplate.back()->SetListener(this);
	m_ViewMatrixID.push_back({});
	return m_ViewTemplate.size() - 1;
}

void mafViewManager::ViewSelected(mafView* view/*, wxVTKWindow *rwi*/)
{
	m_SelectedView = view;

	//  if(m_SelectedRWI)
	//    m_SelectedRWI->SetInteractorStyle(NULL);

	m_SelectedRWI = view->GetRWI();
	if (m_Mouse)
	{
		mafEvent e(this, VIEW_SELECT);
		e.SetView(m_SelectedView);
		e.SetChannel(MCH_OUTPUT);
		m_Mouse->OnEvent(&e); // update the mouse
	}
}

void mafViewManager::VmeAdd(std::shared_ptr<mafNode> node)
{
	for (auto& view : m_ViewList)
	{
		view->VmeAdd(node); // Add the VME in all the views
	}

	wxString s = node->GetTypeName();
	if (s == "mafVMERoot") // Add a root means add a new tree
	{
		m_RootVme = mafVMERoot::StaticDownCast(node);
		m_SelectedVme = node.get(); // Adding new tree, selected vme must be initialized at the root.
	}
}

void mafViewManager::VmeRemove(mafNode* node)
{
	for (auto& view : m_ViewList)
	{
		view->VmeRemove(node); // Remove the VME in all the views
	}

	wxString s = node->GetTypeName();
	if (s == "mafVMERoot") // Remove the root means remove the tree
	{
		m_RootVme = nullptr;
		m_SelectedVme = nullptr; // Removing the tree, selected vme must be set to NULL.
	}
}

void mafViewManager::VmeModified(mafNode* node)
{
	//@@@ for(auto& view : m_ViewList) 
	//@@@   view->VmeModified(node);  -- view::vmeModified not exist now -- is this required ?
}

void mafViewManager::VmeSelect(mafNode* node)
{
	if (node != m_SelectedVme)
	{
		if (m_SelectedVme)
		{
			for (auto& view : m_ViewList)
			{
				view->VmeSelect(m_SelectedVme, false); //deselect the previous selected vme
			}
		}
		m_SelectedVme = node; // set the new selected vme
	}
	for (auto& view : m_ViewList)
	{
		view->VmeSelect(node, true); // select the new one in the views
	}
	CameraUpdate();
}

void mafViewManager::VmeShow(mafNode* node, bool show)
{
	if (m_ViewBeingCreated) // Important - test m_ViewBeingCreated first
	{
		// show the vme in the view that has been created but
		// is not already inside the views' list (if exists)
		m_ViewBeingCreated->VmeShow(node, show);
	}
	else
	{
		if (m_SelectedView)
		{
			m_SelectedView->VmeShow(node, show); // show the vme in the selected view
		}
	}
}

void mafViewManager::PropertyUpdate(bool fromTag)
{
	for (auto& view : m_ViewList)
	{
		view->VmeUpdateProperty(this->m_SelectedVme, fromTag); // update the vme properties in all views
	}
}

void mafViewManager::CameraReset(bool sel)
{
	if (m_SelectedView)
	{
		m_SelectedView->CameraReset(sel ? m_SelectedVme : nullptr); // reset the camera in the selected view
	}
}

void mafViewManager::CameraReset(mafNode* node)
{
	//@@@ if(m_SelectedView) m_SelectedView->CameraReset(vme);
	if (m_SelectedView)
	{
		m_SelectedView->CameraReset(); // reset the camera in the selected view
	}
}

void mafViewManager::CameraUpdate(bool only_selected)
{
	if (only_selected && m_SelectedView != nullptr)
	{
		m_SelectedView->CameraUpdate(); // Update only the selected view
		return;
	}
	for (auto& view : m_ViewList) // Update all views
	{
		view->CameraUpdate();
	}
}

void mafViewManager::CameraFlyToMode()
{
	//  if(m_is) m_is->FlyToMode();
}

void mafViewManager::OnQuit()
{
}

mafView* mafViewManager::ViewCreate(int id)
{
	int index = id - VIEW_START;

	if (index <0 || index > m_ViewTemplate.size()) // the specified template id is out of bound
	{
		assert(false);
		return nullptr;
	}

	auto view = m_ViewTemplate[index].get();
	if (!view)
	{
		return nullptr;
	}

	auto new_view = view->Copy(this); // the created view is a copy of the specified template view

	int view_mult = 0;
	for (; view_mult < m_ViewMatrixID[index].size(); view_mult++) // iterate over view matrix to calculate the multiplicity
	{
		if (m_ViewMatrixID[index][view_mult] == nullptr)
		{
			break;
		}
	}
	if (view_mult == m_ViewMatrixID[index].size())
	{
		m_ViewMatrixID[index].push_back(nullptr);
	}
	new_view->m_Mult = view_mult;

	//update the matrix containing all created views
	m_ViewMatrixID[index][view_mult] = new_view;

	// during ViewInsert the View may send Events that will not be forwarded because 
	// the view isn't already selected - 
	m_ViewBeingCreated = new_view;
	ViewInsert(new_view);
	m_ViewBeingCreated = nullptr;

	{ mafEvent evUnq(this, VIEW_CREATED); evUnq.SetView(new_view); InvokeEvent(evUnq); } // ask Logic to create the frame

	new_view->GetFrame()->Show(true); // show the view's frame

	return new_view;
}

mafView* mafViewManager::ViewCreate(const mafString& label)
{
	for (auto& view : m_ViewTemplate)
	{
		if (view->GetLabel() == label)
		{
			return ViewCreate(view->m_Id);
		}
	}
	return nullptr;
}

void mafViewManager::ViewInsert(mafView* view)
{
	view->SetListener(this);
	if (m_RootVme)
	{
		std::vector<std::shared_ptr<mafNode> > stack(1, m_RootVme);
		while (!stack.empty())
		{
			auto vme = stack.back();
			view->VmeAdd(vme); // Add them in the specified view
			stack.pop_back();
			for (size_t i = 0; i < vme->GetNumberOfChildren(); i++)
			{
				stack.push_back(vme->GetChild(i));
			}
		}
	}

	if (m_SelectedVme)
	{
		view->VmeSelect(m_SelectedVme, true); // select the vme in the inserted view
	}

	m_ViewList.push_back(view);
}

void mafViewManager::ViewDelete(mafView* view)
{
	if (m_CollaborateStatus && m_RemoteListener && !m_FromRemote)
	{
		// Send the event to synchronize the remote application in case of collaboration modality
		mafEvent e(this, VIEW_DELETE);
		e.SetView(view);
		e.SetChannel(REMOTE_COMMAND_CHANNEL);
		m_RemoteListener->OnEvent(&e);
	}

	if (m_SelectedView == view)
	{
		m_SelectedRWI = nullptr;
		m_SelectedView = nullptr;
	}

	{ mafEvent evUnq(this, VIEW_DELETE); evUnq.SetView(view); InvokeEvent(evUnq); } // inform the sidebar

	int index = view->m_Id - VIEW_START;
	m_ViewMatrixID[index][view->m_Mult] = nullptr;

	// Remove the specified view from the views' list
	erase(m_ViewList, view);
	delete view;
}

void mafViewManager::ViewDeleteAll()
{
	if (m_SelectedView)
	{
		m_SelectedRWI = nullptr;
		m_SelectedView = nullptr;
	}

	while (!m_ViewList.empty()) // Close all views' frames
	{
		auto view = m_ViewList.front();
		view->GetFrame()->Show(false);
		view->GetFrame()->Close();
	}
}

mafView* mafViewManager::GetSelectedView() const
{
	return m_SelectedView;
}

void mafViewManager::Activate(mafView* view)
{
	bool externalViewFlag;
	// Determine if is an external view
	auto config = std::make_unique<wxConfig>(wxEmptyString);
	config->Read("ExternalViewFlag", &externalViewFlag, false);

	// Set the focus to the frame of the specified view
	if (externalViewFlag)
		((mafGUIViewFrame*)view->GetFrame())->SetFocus();
	else
		//((mafGUIMDIChild *)view->GetFrame())->Activate();
		((mafGUIMDIChild*)view->GetFrame())->SetFocus();
}

mafView* mafViewManager::GetFromList(const mafString& label) const
{
	for (auto& view : m_ViewList)
	{
		if (view->GetLabel() == label)
		{
			return view;
		}
	}
	return nullptr;
}

mafView* mafViewManager::GetView(int id, int mult) const
{
	if (id >= 0 && id < m_ViewMatrixID.size())
	{
		if (mult >= 0 && mult < m_ViewMatrixID[id].size())
		{
			return m_ViewMatrixID[id][mult];
		}
	}
	return nullptr;
}
