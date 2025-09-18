#include "ftk/Gui/wx/CheckTree.h" 
#include "mafDecl.h"
#include "mafView.h"
#include "mafPics.h" 

#include "mafNode.h"
#include "mafVME.h"
#include <vector>

namespace gui::wx
{
	CheckTree::CheckTree(wxWindow* parent, wxWindowID id, bool CloseButton, bool HideTitle)
		:Tree(parent, id, CloseButton, HideTitle)
		//----------------------------------------------------------------------------
	{
		m_NodeTree->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {OnMouseDown(event); });
		m_NodeTree->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {OnMouseUp(event); });
		m_NodeTree->Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent& event) {});
		m_NodeTree->Bind(wxEVT_RIGHT_UP, [this](wxMouseEvent& event) {ShowContextualMenu(event); });
		InitializeImageList();
	}

	CheckTree::~CheckTree() = default;

	void CheckTree::NodeAdd(std::shared_ptr<mafNode> node)
	{
		AddNode(reinterpret_cast<intptr_t>(node.get()), reinterpret_cast<intptr_t>(node->GetParent()), node->GetName(), 0, 0, new CheckTreeItemData(reinterpret_cast<intptr_t>(node.get()), node));
		NodeUpdateIcon(node.get());
	}

	void CheckTree::NodeRemove(mafNode* node)
	{
		DeleteNode(reinterpret_cast<intptr_t>(node));
	}

	mafNode* CheckTree::GetSelectedNode() const
	{
		return reinterpret_cast<mafNode*>(GetSelected());
	}

	void CheckTree::ShowContextualMenu(wxMouseEvent& event)
	{
		int flag;
		wxTreeItemId i = m_NodeTree->HitTest(wxPoint(event.GetX(), event.GetY()), flag);
		bool vmeMenu = false;
#ifdef WIN32
		vmeMenu = i.IsOk() && ((flag == wxTREE_HITTEST_ONITEMICON) || (flag == wxTREE_HITTEST_ONITEMLABEL));
#else
		vmeMenu = i.IsOk();
#endif

		mafEvent e;
		e.SetSender(this);
		e.SetId(SHOW_CONTEXTUAL_MENU);
		e.SetBool(vmeMenu);
		if (vmeMenu)
		{
			e.SetVme(static_cast<CheckTreeItemData*>(m_NodeTree->GetItemData(i))->GetSharedNode().get());
		}
		InvokeEvent(e);
	}

	void CheckTree::EnableSelect(bool enable)
	{
		m_CanSelect = enable;
	}

	void CheckTree::OnMouseDown(wxMouseEvent& event)
	{
		//purpose: intercept and notify if the icon was clicked,
		//prevent node selection if the icon was clicked,
		//prevent node selection anyway, if the selection is disabled,
		int flag;
		wxTreeItemId i = m_NodeTree->HitTest(wxPoint(event.GetX(), event.GetY()), flag);
		if (i.IsOk() && flag & wxTREE_HITTEST_ONITEMICON)
		{
			OnIconClick(i);
			return;//eat message
		}
		if (!this->m_CanSelect)
			return; //also eat message if selection is disabled
		event.Skip();//process event as usual
	}

	void CheckTree::OnMouseUp(wxMouseEvent& event)
	{
		//purpose: prevent selection if I clicked on the icon.
		//to select you must click the node name
		int flag;
		wxTreeItemId i = m_NodeTree->HitTest(wxPoint(event.GetX(), event.GetY()), flag);
		if (flag & wxTREE_HITTEST_ONITEMICON)
			return;//eat message
		event.Skip();//process event as usual
	}

	void CheckTree::OnIconClick(wxTreeItemId item)
	{
		auto vme = static_cast<CheckTreeItemData*>(m_NodeTree->GetItemData(item))->GetSharedNode();
		int status = GetVmeStatus(vme.get());

		if (status != NODE_NON_VISIBLE)
		{
			bool show = !(status == NODE_VISIBLE_ON || status == NODE_MUTEX_ON);
			if (!show && !this->m_CanSelect)
			{
				if (auto selectedNode = GetSelectedNode(); selectedNode && selectedNode == vme.get())
				{
					return;
				}
			}
			{ mafEvent evUnq(this, VME_SHOW); evUnq.SetVme(vme.get()); evUnq.SetBool(show); InvokeEvent(evUnq); }
			{ mafEvent evUnq(this, CAMERA_UPDATE); InvokeEvent(evUnq); }
		}
	}

	bool CheckTree::IsIconChecked(wxTreeItemId item)
	{
		NodeID node = NodeFromItem(item);
		bool checked = GetNodeIcon(node) == (ClassNameToIcon(_R(reinterpret_cast<mafNode*>(node)->GetTypeName())) + NODE_VISIBLE_ON * 2);
		return checked;
	}

	void CheckTree::NodeExpand(mafNode* node)
	{
		ExpandNode(reinterpret_cast<intptr_t>(node));
	}

	void CheckTree::NodeCollapse(mafNode* node)
	{
		CollapseNode(reinterpret_cast<intptr_t>(node));
	}

	void CheckTree::NodeExpandSubTree(mafNode* node)
	{
		ExpandNodeSubTree(reinterpret_cast<intptr_t>(node));
	}

	void CheckTree::NodeCollapseSubTree(mafNode* node)
	{
		CollapseNodeSubTree(reinterpret_cast<intptr_t>(node));
	}

	void CheckTree::NodeExpandVisible(mafNode* node)
	{
		ExpandNodeVisible(reinterpret_cast<intptr_t>(node));
	}

	void CheckTree::NodeSelected(mafNode* node)
	{
		SelectNode(reinterpret_cast<intptr_t>(node));
		NodeUpdateIcon(node);
	}

	void CheckTree::NodeModified(mafNode* node)
	{
		SetNodeLabel(reinterpret_cast<intptr_t>(node), node->GetName());
		NodeUpdateIcon(node);
	}

	void CheckTree::NodeShow(mafNode* node, bool show)
	{
		NodeUpdateIcon(node);
	}

	int CheckTree::GetVmeStatus(mafNode* node)
	{
		if (!m_View)
			return NODE_NON_VISIBLE;
		return m_View->GetNodeStatus(node);
	}

	void CheckTree::NodeUpdateIcon(mafNode* node)
	{
		//auto item = ItemFromNode((intptr_t)vme);
		//auto iter = std::make_unique<mafNodeIterator>(vme);// static_cast<mafGUICheckTreeItemData*>(m_NodeTree->GetItemData(item))->GetSharedNode().get());
		//auto iter = std::make_unique<mafNodeIterator>(static_cast<mafGUICheckTreeItemData*>(m_NodeTree->GetItemData(item))->GetSharedNode().get());
		for (auto& node : *node)
		{
			int dataStatus = mafVME::StaticDownCast(&node)->IsDataAvailable() ? 0 : 1;
			int icon_index = ClassNameToIcon(_R(node.GetTypeName())) + (GetVmeStatus(&node) * 2) + dataStatus;
			SetNodeIcon((intptr_t)&node, icon_index);

			if (node.GetNumberOfLinks() != 0)
			{
				for (auto& link : node.GetLinks())
				{
					if (link.second.GetNode())
					{
						if (auto linkedVME = mafVME::SafeDownCast(link.second.GetNode()))
						{
							dataStatus = linkedVME->IsDataAvailable() ? 0 : 1;
							icon_index = ClassNameToIcon(_R(linkedVME->GetTypeName())) + (GetVmeStatus(linkedVME.get()) * 2) + dataStatus;
							SetNodeIcon((intptr_t)linkedVME.get(), icon_index);
						}
					}
				}
			}
		}
		//  int icon_index = ClassNameToIcon(vme->GetTypeName()) + (GetVmeStatus(vme)*2) + dataStatus;
		//  SetNodeIcon( (long)vme, icon_index );
	}

	void CheckTree::ViewSelected(mafView* view)
	{
		m_View = view;
		TreeUpdateIcon();
	}

	void CheckTree::TreeUpdateIcon()
	{
		if (auto selectedNode = GetSelectedNode())
		{
			NodeUpdateIcon(selectedNode->GetRoot());
		}
	}

	void CheckTree::ViewDeleted(mafView* view)
	{
		if (view != m_View)
			return;
		ViewSelected(nullptr);
	}

	int CheckTree::ClassNameToIcon(const mafString& classname)
	{
		if (auto it = m_MapClassNameToIcon.find(classname); it != m_MapClassNameToIcon.end())
			return it->second;
		// search superclass's icon
		// if also this icon is not present, "Unknown" icon is used
		mafLogMessage(_M(_R("mafPictureFactory::ClassNameToIcon: cant find = ") + classname));
		return 0;
	}

	void CheckTree::InitializeImageList()
	{
		// purpose:
		// each vme-picture is combined with each state-picture,
		// all the combined picture are inserted in the image-list.
		// given a vme-class-name and a vme-state
		// the corresponding icon index can be retrieved as 
		// ClassNameToIcon(vme-class-name) + vme-state

		std::vector<mafString> v = mafPictureFactory::GetPictureFactory()->GetVmeNames();

		const size_t num_of_status = 5;
		size_t num_types = v.size();
		int num_icons = num_types * (num_of_status * 2); // Added the status "Data not available"

		if (num_types <= 0)
		{
			wxLogMessage("mafGUICheckTree:  Warning - no vme-icons defined");
			return;
		}
		//retrieve state icons
		//I assume all state-icon to have the same size
		wxBitmap state_ico[num_of_status];
		state_ico[NODE_NON_VISIBLE] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("DISABLED"));
		state_ico[NODE_VISIBLE_OFF] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_OFF"));
		state_ico[NODE_VISIBLE_ON] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("CHECK_ON"));
		state_ico[NODE_MUTEX_OFF] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_OFF"));
		state_ico[NODE_MUTEX_ON] = mafPictureFactory::GetPictureFactory()->GetBmp(_R("RADIO_ON"));
		int sw = state_ico[0].GetWidth();
		int sh = state_ico[0].GetHeight();

		//get icon size 
		//I assume all vme-icon to have the same size
		wxBitmap bmp = mafPictureFactory::GetPictureFactory()->GetVmePic(v[0]);
		int w = bmp.GetWidth();
		int h = bmp.GetHeight();
		assert(w > 0 && h > 0);

		// create the ImageList 
		int mw = sw + w;
		int mh = (sh > h) ? sh : h;
		auto imgs = std::make_unique<wxImageList>(mw, mh, false, num_icons);

		for (size_t i = 0; i < num_types; i++)
		{
			//m_MapClassNameToIcon[name]=i*num_of_status;
			m_MapClassNameToIcon[v[i]] = i * (num_of_status * 2); // Paolo 18/12/2006

			for (size_t s = 0; s < num_of_status; s++)
			{
				wxBitmap vmeico = mafPictureFactory::GetPictureFactory()->GetVmePic(v[i]);
				if (s == 0)
					vmeico = mafGrayScale(vmeico);
				wxBitmap merged = MergeIcons(state_ico[s], vmeico);
				imgs->Add(merged);

				// Icons for missing data
				if (s != 0)
					vmeico = mafGrayScale(vmeico);
				vmeico = mafRedScale(vmeico);
				wxBitmap missingData = MergeIcons(state_ico[s], vmeico); // Same icon as above, but represent a 
				imgs->Add(missingData);                                 // node with no data available.
			}
		}
		SetImageList(std::move(imgs));
	}

	wxBitmap CheckTree::MergeIcons(wxBitmap state, wxBitmap vme)
	{
		int sw = state.GetWidth();
		int sh = state.GetHeight();
		int vw = vme.GetWidth();
		int vh = vme.GetHeight();
		int w = sw + vw;
		int h = vh;
		int hpos = (vh - sh) / 2; // state icon should be center-aligned in vertical 

		wxMemoryDC statedc;
		statedc.SelectObject(state);

		wxMemoryDC vmedc;
		vmedc.SelectObject(vme);

		wxBitmap merge = wxBitmap(w, h);
		wxMemoryDC mergedc;
		mergedc.SelectObject(merge);

		mergedc.SetBackground(*wxWHITE_BRUSH);
		mergedc.Clear();
		mergedc.Blit(0, hpos, sw, sh, &statedc, 0, 0);
		mergedc.Blit(sw, 0, vw, vh, &vmedc, 0, 0);
		mergedc.SelectObject(wxNullBitmap); //merge must be removed from the DC at the end
		return merge;
	}

	void CheckTree::OnSelectionChanged(wxTreeEvent& event)
	{
		if (m_PreventNotify)
			return;

		wxTreeItemId i = event.GetItem();
		if (i.IsOk())
		{
			mafEvent evUnq(this, VME_SELECT); evUnq.SetVme(static_cast<CheckTreeItemData*>(m_NodeTree->GetItemData(i))->GetSharedNode().get()); InvokeEvent(evUnq);
		}
		event.Skip();
	}

}
