#pragma once
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "ftkConfigure.h"
#include "mafGUITree.h"
#include "mafPics.h"
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <map>

BEGIN_FTK_NAMESPACE

namespace model::data
{
    class Node;
}
using mafNode = model::data::Node;
class mafView;

namespace gui::wx
{
    class MAF_EXPORT CheckTree : public Tree
    {
    public:
        class CheckTreeItemData : public TreeItemData
        {
        public:
            CheckTreeItemData(intptr_t node_id, std::shared_ptr<mafNode> n = nullptr) : TreeItemData(node_id), sharednode(n) {}
            ~CheckTreeItemData() override = default;
            void SetSharedNode(std::shared_ptr<mafNode> n) { sharednode = n; }
            std::shared_ptr<mafNode> GetSharedNode() const { return sharednode; }
        private:
            std::shared_ptr<mafNode> sharednode;
        };
        /** constructor .*/
        CheckTree(wxWindow* parent, wxWindowID id = -1, bool CloseButton = false, bool HideTitle = false);
        /** destructor .*/
        ~CheckTree() override;

        void VmeExpand(mafNode* vme);
        void VmeCollapse(mafNode* vme);
        void VmeExpandSubTree(mafNode* vme);
        void VmeCollapseSubTree(mafNode* vme);
        void VmeExpandVisible(mafNode* vme);
        /** Add the vme to the checked tree and create the related icon. */
        void VmeAdd(std::shared_ptr<mafNode> n);

        /** Remove the vme from the checked tree. */
        void VmeRemove(mafNode* n);

        /** Select the vme node of the checked tree. */
        void VmeSelected(mafNode* n);

        /** Return the current selected node.*/
        mafNode* GetSelectedNode() { return m_SelectedNode; };

        /** Return true if the icon associated to the tree item is checked.*/
        bool IsIconChecked(wxTreeItemId item);

        /** Update the vme node icon into the checked tree. */
        void VmeShow(mafNode* n, bool show);

        /** Update the vme node label with the new vme name. */
        void VmeModified(mafNode* n);

        /** Update the vme tree nodes with the new vme visibility for the selected view. */
        void ViewSelected(mafView* view);

        /** Update the vme tree nodes when a view is deleted. */
        void ViewDeleted(mafView* view);

        // todo: 
        // setting m_enableSelect to false
        // doesn't prevent to change the selection using the keyboard !!
        //SIL. 7-4-2005: 

          /** Enable the selection of a node tree.*/
        void EnableSelect(bool enable);

        /** Retrieve wxwindows widget pointer of the tree.*/
        wxTreeCtrl* GetTree() { return m_NodeTree; }

    protected:
        /** Update the vme nodes icon. */
        void VmeUpdateIcon(mafNode* n);

        /** Update all the vme nodes icon. */
        void TreeUpdateIcon();

        /** Return the status of the node according to the vme visibility. */
        virtual int GetVmeStatus(mafNode* vme);

        /** retrieve the icon-index for a vme given the classname */
        int ClassNameToIcon(const mafString& classname);

        /** Fill the image list considering the visibility vme type, and vme availability.
                 The result is a list of images, and one image can be associated to the label that represent the node of the tree in the wxwidget.*/
        virtual void InitializeImageList();

        /** Given two bitmaps, it creates a third bitmap.
                 The  result is a merge of the others with the right side of the first image that is the left side of the second one.*/
        wxBitmap MergeIcons(wxBitmap state, wxBitmap vme);

    public:
        /** Called by the Custom-Tree-Event-Handler */
        void OnMouseDown(wxMouseEvent& event);

        /** Called by the Custom-Tree-Event-Handler */
        void OnMouseUp(wxMouseEvent& event);

        /** Called by the Custom-Tree-Event-Handler */
        void OnMouseEvent(wxMouseEvent& event);

        /** Called by the Custom-Tree-Event-Handler - via OnMouseDown*/
        virtual void OnIconClick(wxTreeItemId item);

        /** Called by the Custom-Tree-Event-Handler */
        virtual void ShowContextualMenu(wxMouseEvent& event);

        /** respond to Selection Changed */
        void OnSelectionChanged(wxTreeEvent& event) override;

    protected:
        mafView* m_View;
        mafNode* m_SelectedNode;
        //bool      m_CheckCrypto;
        bool     m_CanSelect;
        wxMenu* m_RMenu;

        typedef std::map<mafString, int> MapClassNameToIcon;
        MapClassNameToIcon m_MapClassNameToIcon;
    };
}

using mafGUICheckTree = gui::wx::CheckTree;

END_FTK_NAMESPACE
