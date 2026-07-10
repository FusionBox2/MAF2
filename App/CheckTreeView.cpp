#include "CheckTreeView.h"

#include "mafPics.h"

BEGIN_FTK_NAMESPACE

CheckTreeView::CheckTreeView(wxWindow* parent)
	: TreeView(parent)
{
	//initializeImageList();

	//m_tree->Bind(wxEVT_TREE_ITEM_ACTIVATED, &CheckTreeView::onItemClicked, this);
	//m_tree->Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {OnMouseDown(event); });
	//m_tree->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& event) {OnMouseUp(event); });
	//m_tree->Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent& event) {});
	//m_tree->Bind(wxEVT_RIGHT_UP, [this](wxMouseEvent& event) {ShowContextualMenu(event); });

	//m_context->m_events.onVisibilityChanged.push_back(
		//[this](const NodeVisibilityChanged& e)
		//{
			//onVisibilityChanged(e));
		//});
}

CheckTreeView::~CheckTreeView()
{
	
}

void CheckTreeView::setImages(std::unique_ptr<wxImageList> images)
{
	//m_images = std::move(images);
	//m_tree->SetImageList(m_images.get());
}

void CheckTreeView::OnMouseDown(wxMouseEvent& event)
{
	//purpose: intercept and notify if the icon was clicked,
	//prevent node selection if the icon was clicked,
	//prevent node selection anyway, if the selection is disabled,
	int flag;
	//wxTreeItemId i = m_tree->HitTest(wxPoint(event.GetX(), event.GetY()), flag);
	//if (i.IsOk() && flag & wxTREE_HITTEST_ONITEMICON)
	{
		//OnIconClick(i);
		return;//eat message
	}
	//if (!this->m_CanSelect)
		//return; //also eat message if selection is disabled
	event.Skip();//process event as usual
}

void CheckTreeView::OnMouseUp(wxMouseEvent& event)
{
	//purpose: prevent selection if I clicked on the icon.
	//to select you must click the node name
	int flag;
	//wxTreeItemId i = m_tree->HitTest(wxPoint(event.GetX(), event.GetY()), flag);
	//if (flag & wxTREE_HITTEST_ONITEMICON)
		return;//eat message
	event.Skip();//process event as usual
}

//void CheckTreeView::OnIconClick(wxTreeItemId item)
//{
	/*auto vme = static_cast<CheckTreeItemData*>(m_NodeTree->GetItemData(item))->GetSharedNode();
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
	}*/
//}

//void CheckTreeView::ShowContextualMenu(wxMouseEvent& event)
//{}

void CheckTreeView::setupAdapter()
{
	/*m_adapter->onItemActivated = [this](std::shared_ptr<model::data::Node> node)
		{
			bool current = m_vm->isChecked(node);
			m_vm->setChecked(node, !current);
		};

	m_adapter->onItemBuilt = [this](std::shared_ptr<model::data::Node> node, wxTreeItemId id)
		{
			m_tree->SetItemImage(id, 1);
			m_tree->SetItemImage(id, 1, wxTreeItemIcon_Selected);
			//updateItem(node, id);
		};

	m_vm->onVisibilityChanged = [this](const NodeVisibilityChanged& e)
		{

			//if (e.viewId != viewId_)
				//return;

			auto id = m_adapter->item(e.node);
			if (!id.IsOk())
				return;
			updateItem(e.node, id);
		};*/
}

//void CheckTreeView::updateItem(gui::ITreeViewModel::NodeId node, wxTreeItemId id)
//{
	//bool checked = m_vm->isChecked(node);

	//m_tree->SetItemImage(id, checked ? 0 : 1);
//}

//void CheckTreeView::onItemClicked(wxTreeEvent& e)
//{
	//auto node = m_getNode(e.GetItem());
	//if (!node)
		//return;

	//bool current = m_context->isNodeVisible(node, viewId_);

	//m_context->execute(std::make_shared<SetVisibilityCommand>(node, viewId_, !current));
//}

END_FTK_NAMESPACE
