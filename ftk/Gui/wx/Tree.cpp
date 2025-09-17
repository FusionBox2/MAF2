#include "ftk/Gui/wx/Tree.h"
#include "mafDecl.h"
#include "mafPics.h"
#include "mafNode.h"

#include <list>
//----------------------------------------------------------------------------
// EVENT_TABLE
//----------------------------------------------------------------------------
namespace gui::wx
{
    //----------------------------------------------------------------------------
    Tree::Tree(wxWindow* parent, wxWindowID id, bool CloseButton, bool HideTitle)
        :mafGUINamedPanel(parent, id, CloseButton, HideTitle)
        //----------------------------------------------------------------------------
    {
        m_PreventNotify = false;

        m_NodeTree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize(100, 100)), wxNO_BORDER | wxTR_HAS_BUTTONS);
        m_NodeTree->Bind(wxEVT_TREE_SEL_CHANGED, [this](wxTreeEvent& event) {OnSelectionChanged(event); });
        Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {OnSize(event); });
        m_Sizer->Add(m_NodeTree, 1, wxEXPAND);

        //default image list
        wxBitmap bmp = mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_GRAY"));
        int w = bmp.GetWidth();
        int h = bmp.GetHeight();

        m_NodeImages = new wxImageList(w, h, FALSE, 4);
        m_NodeImages->Add(bmp);
        m_NodeImages->Add(mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_RED")));
        m_NodeImages->Add(mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_BLUE")));
        m_NodeImages->Add(mafPictureFactory::GetPictureFactory()->GetBmp(_R("NODE_YELLOW")));
        m_NodeTree->SetImageList(m_NodeImages);

        Reset();
    }
    //----------------------------------------------------------------------------
    Tree::~Tree()
        //----------------------------------------------------------------------------
    {
        if (m_NodeTable)
        {
            m_NodeTable->DeleteContents(true);
            delete m_NodeTable;
        }
        delete m_NodeImages;
    }
    //----------------------------------------------------------------------------
    void Tree::Reset()
        //----------------------------------------------------------------------------
    {
        m_NodeTree->DeleteAllItems();

        if (m_NodeTable != NULL)
            delete m_NodeTable; // short way to delete all associated objects
        m_NodeTable = new wxHashTable(wxKEY_INTEGER, 100);
        m_NodeTable->DeleteContents(true);
        m_NodeRoot = 0;
    }
    //----------------------------------------------------------------------------
    bool Tree::AddNode(NodeID node_id, NodeID parent_id, wxString label, int icon, Tree::TreeItemData* data)
        //----------------------------------------------------------------------------
    {
        /*
        - se parent_id = 0 to create the root
        - icon must be a valid index in the m_NodeImages - otherwise it is clamped
        - fails if node_id already exist
        - parent_id must exist
        - return true on success
        */

        icon = CheckIconId(icon);

        wxTreeItemId  item, parent_item;

        // check if already inserted
        if (NodeExist(node_id)) return false;

        if (data == nullptr)
            data = new TreeItemData(node_id);
        if (parent_id == 0 && m_NodeRoot == 0)
        {
            item = m_NodeTree->AddRoot(label, icon, icon, data);
            m_NodeRoot = node_id;
        }
        else
        {
            if (!NodeExist(parent_id)) return false;
            parent_item = ItemFromNode(parent_id);
            //insert normally
            item = m_NodeTree->AppendItem(parent_item, label, icon, icon, data);
            // expand parent node
            m_NodeTree->SetItemHasChildren(parent_item, true);
            m_NodeTree->Expand(parent_item);
        }

        //insert [node_id -> item] in the table
        TreeTableElement* el = new TreeTableElement(item);
        m_NodeTable->Put(node_id, el);

        return true;
    }
    //----------------------------------------------------------------------------
    bool Tree::DeleteNode(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        /*
        - node_id must exist
        - return true on success
        - check that node_id exist
        - calls DeleteNode2 to delete all the item in the subtree keeping m_NodeTable consistent
        - keep parent->HasChildren consistent
        - if the item was selected, select the parent node (). [ to prevent the shown property-gui to become inconsistent ]
        */

        if (!NodeExist(node_id)) return false;

        wxTreeItemId item = ItemFromNode(node_id);
        wxTreeItemId parent_item = m_NodeTree->GetItemParent(item);

        // move the selection (if not deleting the root => parent=NULL)
        if (parent_item && m_NodeTree->IsSelected(item))
        {
            m_NodeTree->SelectItem(parent_item);
        }

        // if the old parent has no other children, set HasChildren to false => hide the widget to expand the subtree
        // but skip if we are deleting the root => parent=NULL
        if (parent_item && m_NodeTree->GetChildrenCount(parent_item, false) <= 1)
        {
            m_NodeTree->SetItemHasChildren(parent_item, false);
        }

        DeleteNode2(node_id);

        if (node_id == m_NodeRoot)
            m_NodeRoot = 0; // if we deleted the root we can create a new one
        return true;
    }
    //----------------------------------------------------------------------------
    void Tree::DeleteNode2(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        /*
        - private function called by DeleteNode. Delete recursively a node and its subtree
        - doesn't check if node exist
        - doesn't handle the selection
        - keep m_NodeTable consistent
        */

        wxTreeItemIdValue cookie;
        wxTreeItemId item = ItemFromNode(node_id);
        wxTreeItemId child = m_NodeTree->GetFirstChild(item, cookie);
        while (child.IsOk())
        {
            DeleteNode2(NodeFromItem(child));
            child = m_NodeTree->GetNextChild(item, cookie);
        }
        m_NodeTree->Delete(item);

        wxObject* el = m_NodeTable->Delete(node_id);
        if (el != NULL)
            delete el;
    }
    //----------------------------------------------------------------------------
    bool Tree::SetNodeLabel(NodeID node_id, wxString label)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id))
            return false;
        wxTreeItemId item = ItemFromNode(node_id);
        m_NodeTree->SetItemText(item, label);

        wxTreeItemId parent = m_NodeTree->GetItemParent(item);
        if (parent.IsOk())
        {
        }
        return true;
    }
    //----------------------------------------------------------------------------
    wxString Tree::GetNodeLabel(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id))
            return "";
        wxTreeItemId item = ItemFromNode(node_id);
        return m_NodeTree->GetItemText(item);
    }
    //----------------------------------------------------------------------------
    bool Tree::NodeHasChildren(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id))
            return false;
        wxTreeItemId item = ItemFromNode(node_id);
        return m_NodeTree->ItemHasChildren(item);
    }
    //----------------------------------------------------------------------------
    Tree::NodeID Tree::GetNodeParent(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id))
            return false;
        wxTreeItemId  item = ItemFromNode(node_id);
        item = m_NodeTree->GetItemParent(item);
        if (node_id != 1)
        {
            return NodeFromItem(item);
        }
        else
        {
            return 0;
        }
    }

    //----------------------------------------------------------------------------
    bool Tree::SetNodeParent(NodeID node_id, NodeID parent_id)
        //----------------------------------------------------------------------------
    {
        /*
        - node_id must exist
        - fails if parent_id is a children of node_id
        - return true on success
        - check that node_id exist
        - calls DeleteNode2 to delete all the item in the subtree keeping m_NodeTable consistent
        - keep parent->HasChildren consistent
        - if the item was selected, select the parent node. [ to prevent the shown property-gui to become inconsistent ]
        */

        if (node_id == parent_id)  return false;

        if (!NodeExist(node_id))   return false;
        if (!NodeExist(parent_id)) return false;

        // check that node_id is not an ancestor of parent_id
        wxTreeItemId i = ItemFromNode(parent_id);;
        while (i = m_NodeTree->GetItemParent(i))
        {
            if (NodeFromItem(i) == node_id) return false;
        }

        // if the old parent has no other children, set HasChildren to false => hide the widget to expand the subtree
        i = ItemFromNode(node_id);;
        wxTreeItemId old_parent_item = m_NodeTree->GetItemParent(i);
        if (m_NodeTree->GetChildrenCount(old_parent_item, false) <= 1)
        {
            m_NodeTree->SetItemHasChildren(old_parent_item, false);
        }

        // Now the checks are made - start to move
        SetNodeParent2(node_id, parent_id);

        // Set hasChildren of the new parent and open it
        i = ItemFromNode(parent_id);;
        m_NodeTree->SetItemHasChildren(i, true);
        m_NodeTree->SortChildren(i);

        m_NodeTree->Expand(i);

        return true;
    }
    //----------------------------------------------------------------------------
    void Tree::SetNodeParent2(NodeID node_id, NodeID parent_id)
        //----------------------------------------------------------------------------
    {
        /*
        - private function called by SetNodeParent
        - copy old_node under parent_id
        - move recursively all the children under the new node
        - delete old_node
        - keep m_NodeTable,HasChildren,IsExpanded consistent
        */

        wxTreeItemId item = ItemFromNode(node_id);
        wxTreeItemId parent_item = ItemFromNode(parent_id);
        int          icon = m_NodeTree->GetItemImage(item);
        wxString     label = m_NodeTree->GetItemText(item);
        wxTreeItemId new_item = m_NodeTree->AppendItem(parent_item, label, icon, icon, new TreeItemData(node_id));
        bool         HasChildren = m_NodeTree->ItemHasChildren(item);
        bool         IsExpanded = m_NodeTree->IsExpanded(item);

        // update the table
        TreeTableElement* el = (TreeTableElement*)m_NodeTable->Get(node_id);
        assert(el != NULL);
        el->SetItem(new_item);

        // move recursively the sub tree under new_item
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_NodeTree->GetFirstChild(item, cookie);
        while (child.IsOk())
        {
            SetNodeParent2(NodeFromItem(child), node_id);
            child = m_NodeTree->GetNextChild(item, cookie);
        }

        //Synchronize HasChildren and Expanded
        m_NodeTree->SetItemHasChildren(new_item, HasChildren);
        if (IsExpanded) m_NodeTree->Expand(new_item); else m_NodeTree->Collapse(new_item);

        m_NodeTree->Delete(item);
    }
    //----------------------------------------------------------------------------
    bool Tree::SetNodeIcon(NodeID node_id, int icon)
        //----------------------------------------------------------------------------
    {
        icon = CheckIconId(icon);
        if (!NodeExist(node_id)) return false;
        wxTreeItemId  item = ItemFromNode(node_id);
        m_NodeTree->SetItemImage(item, icon);
        m_NodeTree->SetItemImage(item, icon, wxTreeItemIcon_Selected);
        return true;
    }
    //----------------------------------------------------------------------------
    bool Tree::NodeExist(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        TreeTableElement* el = (TreeTableElement*)m_NodeTable->Get(node_id);
        return el != NULL;
    }
    //----------------------------------------------------------------------------
    wxTreeItemId Tree::ItemFromNode(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        TreeTableElement* el = (TreeTableElement*)m_NodeTable->Get(node_id);
        assert(el != NULL);
        return el->GetItem();
    }
    //----------------------------------------------------------------------------
    Tree::NodeID Tree::NodeFromItem(wxTreeItemId& item)
        //----------------------------------------------------------------------------
    {
        TreeItemData* nd = (TreeItemData*)m_NodeTree->GetItemData(item);
        if (nd == NULL) return 0;
        return nd->GetNode();
    }
    //----------------------------------------------------------------------------
    void Tree::OnSelectionChanged(wxTreeEvent& event)
        //----------------------------------------------------------------------------
    {
        wxTreeItemId i;
        if (m_PreventNotify) return;

        i = event.GetItem();
        if (i.IsOk())
        {
            mafEvent evUnq(this, VME_SELECT); evUnq.SetArg(NodeFromItem(i)); InvokeEvent(evUnq);
        }
        event.Skip();
    }
    //----------------------------------------------------------------------------
    bool Tree::SelectNode(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id)) return false;
        wxTreeItemId  item = ItemFromNode(node_id);
        m_PreventNotify = true;
        m_NodeTree->SelectItem(item);
        m_PreventNotify = false;
        return true;
    }
    //----------------------------------------------------------------------------
    void Tree::OnSize(wxSizeEvent& event)
        //----------------------------------------------------------------------------
    {
        event.Skip();
        m_NodeTree->Refresh();
    }
    //----------------------------------------------------------------------------
    int Tree::CheckIconId(int icon)
        //----------------------------------------------------------------------------
    {
        if (!m_NodeImages) return 0;
        if (icon < 0)
        {
            mafLogMessage(_M(_R("Tree: icon id = ") + mafToString(icon) + _R(" out of range ")));
            return 0;
        }
        if (icon >= m_NodeImages->GetImageCount())
        {
            mafLogMessage(_M(_R("Tree: icon id = ") + mafToString(icon) + _R(" out of range ")));
            return m_NodeImages->GetImageCount() - 1;
        }
        return icon;
    }
    //----------------------------------------------------------------------------
    void Tree::SetImageList(wxImageList* img)
        //----------------------------------------------------------------------------
    {
        if (m_NodeRoot != 0)
        {
            mafLogMessage(_M("warning: Tree::SetImageList must be called before adding any node"));
            // if you replace the image-list with a shorter one 
            // the icon-index actually in use by the existing nodes 
            // can become inconsistent 

            //return; //SIL. 7-4-2005: - commented 4 testing -- to be reinserted
        }

        if (img == m_NodeImages)
            return;
        cppDEL(m_NodeImages);
        m_NodeImages = img;
        m_NodeTree->SetImageList(m_NodeImages);
    }
    //----------------------------------------------------------------------------
    int Tree::GetNodeIcon(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id))
            return 0;
        wxTreeItemId  item = ItemFromNode(node_id);
        if (!item.IsOk())
            return 0;
        return m_NodeTree->GetItemImage(item);
    }
    //----------------------------------------------------------------------------
    void Tree::SortChildren(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (node_id == 0)
            m_NodeTree->SortChildren(m_NodeTree->GetRootItem());
        else
            m_NodeTree->SortChildren(ItemFromNode(node_id));
    }
    class TreeTraversal
    {
    public:
        TreeTraversal(wxTreeCtrl* tree)
        {
            m_tree = tree;
        }

        // do traverse the tree: visit all items (recursively by default) under the
        // given one; return true if all items were traversed or false if the
        // traversal was aborted because OnVisit returned false
        bool DoTraverse(const wxTreeItemId& root, bool recursively = true);

        // override this function to do whatever is needed for each item, return
        // false to stop traversing
        virtual bool OnVisit(const wxTreeItemId& item) = 0;

    protected:
        wxTreeCtrl* GetTree() const { return m_tree; }

    private:
        bool Traverse(const wxTreeItemId& root, bool recursively);

        wxTreeCtrl* m_tree;

        TreeTraversal(const TreeTraversal&);
        TreeTraversal& operator=(const TreeTraversal&);
    };

    // ----------------------------------------------------------------------------
    // tree traversal
    // ----------------------------------------------------------------------------

    bool TreeTraversal::DoTraverse(const wxTreeItemId& root, bool recursively)
    {
        return Traverse(root, recursively);
    }

    bool TreeTraversal::Traverse(const wxTreeItemId& root, bool recursively)
    {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = m_tree->GetFirstChild(root, cookie);
        while (child.IsOk())
        {
            // depth first traversal
            if (recursively)
                Traverse(child, true);
            else
                OnVisit(child);
            child = m_tree->GetNextChild(root, cookie);
        }
        OnVisit(root);
        return true;
    }

    // internal class for counting tree items
    class mafGUITraverseSort : public TreeTraversal
    {
    public:
        mafGUITraverseSort(wxTreeCtrl* tree, const wxTreeItemId& root, bool recursively) : TreeTraversal(tree)
        {
            DoTraverse(root, recursively);
        }

        bool OnVisit(const wxTreeItemId& item) override
        {
            GetTree()->SortChildren(item);
            return true;
        }
    private:
        mafGUITraverseSort(const mafGUITraverseSort&);
        mafGUITraverseSort& operator=(const mafGUITraverseSort&);
    };


    //----------------------------------------------------------------------------
    void Tree::SortSubTree(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (node_id == 0)
        {
            mafGUITraverseSort(m_NodeTree, m_NodeTree->GetRootItem(), true);
        }
        else
        {
            mafGUITraverseSort(m_NodeTree, ItemFromNode(node_id), true);
        }
    }
    //----------------------------------------------------------------------------
    void Tree::CollapseNode(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id)) return;
        wxTreeItemId  item;
        item = ItemFromNode(node_id);
        if (!item.IsOk()) return;
        m_NodeTree->Collapse(item);
    }
    //----------------------------------------------------------------------------
    void Tree::ExpandNode(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id)) return;
        wxTreeItemId  item;
        item = ItemFromNode(node_id);
        if (!item.IsOk()) return;
        m_NodeTree->Expand(item);
    }
    namespace
    {
        void CollapseAllChildren(wxTreeCtrl* tree, wxTreeItemId& item)
        {
            wxTreeItemIdValue cookie;
            for (wxTreeItemId idCurr = tree->GetFirstChild(item, cookie); idCurr.IsOk(); idCurr = tree->GetNextChild(item, cookie))
            {
                CollapseAllChildren(tree, idCurr);
            }
            if (item != tree->GetRootItem() || !tree->HasFlag(wxTR_HIDE_ROOT))
                tree->Collapse(item);
        }
        void ExpandAllChildren(wxTreeCtrl* tree, wxTreeItemId& item)
        {
            if (item != tree->GetRootItem() || !tree->HasFlag(wxTR_HIDE_ROOT))
                tree->Expand(item);
            wxTreeItemIdValue cookie;
            for (wxTreeItemId idCurr = tree->GetFirstChild(item, cookie); idCurr.IsOk(); idCurr = tree->GetNextChild(item, cookie))
            {
                ExpandAllChildren(tree, idCurr);
            }
        }
    }
    //----------------------------------------------------------------------------
    void Tree::CollapseNodeSubTree(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id)) return;
        wxTreeItemId  item;
        item = ItemFromNode(node_id);
        if (!item.IsOk()) return;

        m_NodeTree->Freeze();
        CollapseAllChildren(m_NodeTree, item);
        m_NodeTree->Thaw();
    }
    //----------------------------------------------------------------------------
    void Tree::ExpandNodeSubTree(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id)) return;
        wxTreeItemId  item;
        item = ItemFromNode(node_id);
        if (!item.IsOk()) return;

        m_NodeTree->Freeze();
        ExpandAllChildren(m_NodeTree, item);
        m_NodeTree->Thaw();
    }
    //----------------------------------------------------------------------------
    void Tree::ExpandNodeVisible(NodeID node_id)
        //----------------------------------------------------------------------------
    {
        if (!NodeExist(node_id)) return;
        wxTreeItemId  item;
        item = ItemFromNode(node_id);
        if (!item.IsOk()) return;
        m_NodeTree->EnsureVisible(item);
    }
}