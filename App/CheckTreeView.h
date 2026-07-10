#pragma once

#include "ftkConfigure.h"

#include "ftk/gui/wx/TreeView.h"

BEGIN_FTK_NAMESPACE

class CheckTreeView : public gui::wx::TreeView
{
public:
	CheckTreeView(wxWindow* parent);

	~CheckTreeView() override;

	void setImages(std::unique_ptr<wxImageList> images);

private:
	void OnMouseDown(wxMouseEvent& event);

	void OnMouseUp(wxMouseEvent& event);

	//virtual void OnIconClick(wxTreeItemId item);

	//virtual void ShowContextualMenu(wxMouseEvent& event);

	void setupAdapter();

	//void updateItem(gui::ITreeViewModel::NodeId, wxTreeItemId id);

	//void onItemClicked(wxTreeEvent& e);

	//void initializeImageList();

	std::unique_ptr<wxImageList> m_images;
	//int m_checkedIcon;
	//int m_uncheckedIcon;
};

END_FTK_NAMESPACE
