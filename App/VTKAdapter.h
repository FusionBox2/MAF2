#pragma once

#include "ftkConfigure.h"

#include "IVTKViewModel.h"

class mafRWI;

BEGIN_FTK_NAMESPACE

class VTKAdapter
{
public:
	VTKAdapter(mafRWI *rwi);

	~VTKAdapter();

	void setModel(IVTKViewModel* model);

	//void onNodeAdded(ITreeViewModel::NodeId n);

	//void onNodeRemoved(ITreeViewModel::NodeId n);

	//void onNodeMoved(ITreeViewModel::NodeId n);

	//void onNodeRenamed(ITreeViewModel::NodeId n);

	void onNodeChanged(IVTKViewModel::NodeId n);

	//wxTreeItemId item(ITreeViewModel::NodeId node) const;

	//std::function<void(ITreeViewModel::NodeId)> onItemActivated;

private:

	//wxTreeItemId createItem(wxTreeItemId parent, ITreeViewModel::NodeId node);

	//wxTreeItemId buildRecursive(wxTreeItemId parent, ITreeViewModel::NodeId node);

	//ITreeViewModel::NodeId getNode(wxTreeItemId id);

	// event handlers
	//void onBeginDrag(wxTreeEvent& e);

	//void onEndDrag(wxTreeEvent& e);

	//void onEndLabelEdit(wxTreeEvent& e);

	//void onTreeItemActivated(wxTreeEvent& e);

	//wxTreeCtrl* m_tree;
	IVTKViewModel* m_model = nullptr;
	mafRWI* m_rwi = nullptr;
	//std::vector<vtkRenderer*> m_renderers;
	std::vector<base::Connection> m_connections;
	//wxTreeItemId m_dragged;
	//std::unordered_map<ITreeViewModel::NodeId, wxTreeItemId> m_nodeToItem;
};

END_FTK_NAMESPACE
