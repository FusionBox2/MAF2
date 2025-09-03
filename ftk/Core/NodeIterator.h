#pragma once

#include "ftkConfigure.h"

#include <stdexcept>
#include <vector>

BEGIN_FTK_NAMESPACE

namespace model::data
{
    template<typename Node>
    class NodeIterator final
    {
    public:
        using value_type = Node;
        using pointer = Node*;
        using reference = value_type&;
    	using NodePtr = pointer;

    	NodeIterator(NodePtr root = nullptr);

        bool operator==(const NodeIterator& other) const;

        reference operator*() const noexcept;

        pointer operator->() const noexcept;

        NodeIterator& operator++();

        NodeIterator operator++(int);

        NodeIterator& operator--();

        NodeIterator operator--(int);

        NodePtr GetCurrentNode() const;

        void IgnoreVisibleToTraverse(bool ignore) { m_IgnoreVisibleToTraverse = ignore; }

        NodeIterator& GoToNextNode();

        NodeIterator& GoToPreviousNode();

        NodeIterator& GoToFirstNode();

        NodeIterator& GoToLastNode();

        enum class TraversalMode : uint8_t { PreOrder = 0, PostOrder };

    	TraversalMode GetTraversalMode() const { return m_TraversalMode; }

        void SetTraversalMode(TraversalMode mode);

        void SetTraversalModeToPreOrder() { SetTraversalMode(TraversalMode::PreOrder); }

        void SetTraversalModeToPostOrder() { SetTraversalMode(TraversalMode::PostOrder); }

    private:

        NodePtr FindLeftMostLeaf(NodePtr node);

        NodePtr FindRightMostLeaf(NodePtr node);

        NodePtr m_RootNode = nullptr;
        NodePtr m_CurrentNode = nullptr;
        TraversalMode m_TraversalMode = TraversalMode::PreOrder;
        bool  m_IgnoreVisibleToTraverse = false;
        std::vector<size_t> m_CurrentIdx;
    };

    template<typename Node>
    NodeIterator<Node>::NodeIterator(NodePtr root)
        : m_RootNode(std::move(root))
    {
    }

    template<typename Node>
    bool NodeIterator<Node>::operator==(const NodeIterator<Node>& other) const
    {
        return m_RootNode == other.m_RootNode && m_CurrentNode == other.m_CurrentNode;
    }

    template<typename Node>
    NodeIterator<Node>::reference NodeIterator<Node>::operator*() const noexcept
    {
        return *m_CurrentNode;
    }

    template<typename Node>
    NodeIterator<Node>::pointer NodeIterator<Node>::operator->() const noexcept
    {
        return m_CurrentNode;
    }

    template<typename Node>
    NodeIterator<Node>& NodeIterator<Node>::operator++()
    {
        GoToNextNode();
        return *this;
    }

    template<typename Node>
    NodeIterator<Node> NodeIterator<Node>::operator++(int)
    {
        NodeIterator result = *this;
        ++(*this);
        return result;
    }

    template<typename Node>
    NodeIterator<Node>& NodeIterator<Node>::operator--()
    {
        GoToPreviousNode();
        return *this;
    }

    template<typename Node>
    NodeIterator<Node> NodeIterator<Node>::operator--(int)
    {
        NodeIterator result = *this;
        --(*this);
        return result;
    }

    template<typename Node>
    NodeIterator<Node>::NodePtr NodeIterator<Node>::GetCurrentNode() const
    {
        return m_CurrentNode;
    }

    template<typename Node>
    NodeIterator<Node>::NodePtr NodeIterator<Node>::FindLeftMostLeaf(NodePtr node)
    {
        while (node && node->GetNumberOfChildren() > 0)
        {
            node = node->GetChild(0).get();
            m_CurrentIdx.push_back(0);
        }
        return node;
    }

    template<typename Node>
    NodeIterator<Node>::NodePtr NodeIterator<Node>::FindRightMostLeaf(NodePtr node)
    {
        while (node && node->GetNumberOfChildren() > 0)
        {
            m_CurrentIdx.push_back(node->GetNumberOfChildren() - 1);
            node = node->GetChild(node->GetNumberOfChildren() - 1).get();
        }
        return node;
    }

    template<typename Node>
    NodeIterator<Node>& NodeIterator<Node>::GoToFirstNode()
    {
        m_CurrentIdx.clear();
        switch (m_TraversalMode)
        {
        case TraversalMode::PreOrder:
            m_CurrentNode = m_RootNode;
            break;
        case TraversalMode::PostOrder:
            m_CurrentNode = FindLeftMostLeaf(m_RootNode);
            break;
        }

        if (!m_CurrentNode)
        {
            return *this;
        }

        if (!m_CurrentNode->IsVisible() && m_IgnoreVisibleToTraverse)
        {
            return GoToNextNode();
        }
        return *this;
    }

    template<typename Node>
    NodeIterator<Node>& NodeIterator<Node>::GoToLastNode()
    {
        m_CurrentIdx.clear();
        switch (m_TraversalMode)
        {
        case TraversalMode::PreOrder:
            m_CurrentNode = FindRightMostLeaf(m_RootNode);
            break;
        case TraversalMode::PostOrder:
            m_CurrentNode = m_RootNode;
            break;
        }

        if (!m_CurrentNode)
        {
            return *this;
        }

        if (!m_CurrentNode->IsVisible() && m_IgnoreVisibleToTraverse)
        {
            return GoToPreviousNode();
        }
        return *this;
    }

    template<typename Node>
    NodeIterator<Node>& NodeIterator<Node>::GoToNextNode()
    {
        if (!m_CurrentNode)
        {
            throw std::out_of_range("");
        }
        switch (m_TraversalMode)
        {
        case TraversalMode::PreOrder:
        {
            if (m_CurrentNode->GetNumberOfChildren() > 0)
            {
                m_CurrentNode = m_CurrentNode->GetChild(0).get();
                m_CurrentIdx.push_back(0);
                break;
            }

            if (m_CurrentNode == m_RootNode)
            {
                m_CurrentNode = nullptr;
                break;
            }

            auto parent = m_CurrentNode->GetParent();
            size_t idx = m_CurrentIdx.back();
            m_CurrentIdx.pop_back();

            while (parent != m_RootNode && idx == parent->GetNumberOfChildren() - 1)
            {
                parent = parent->GetParent();
                idx = m_CurrentIdx.back();
                m_CurrentIdx.pop_back();
            }

            if (idx == parent->GetNumberOfChildren() - 1)
            {
                m_CurrentNode = nullptr;
                break;
            }

            idx++;
            m_CurrentNode = parent->GetChild(idx).get();
            m_CurrentIdx.push_back(idx);
            break;
        }
        case TraversalMode::PostOrder:
        {
            if (m_CurrentNode == m_RootNode)
            {
                m_CurrentNode = nullptr;
                break;
            }

            auto parent = m_CurrentNode->GetParent();
            size_t idx = m_CurrentIdx.back();
            m_CurrentIdx.pop_back();

            if (idx == parent->GetNumberOfChildren() - 1)
            {
                m_CurrentNode = parent;
                break;
            }

            idx++;
            m_CurrentIdx.push_back(idx);
            m_CurrentNode = FindLeftMostLeaf(parent->GetChild(idx).get());

            break;
        }
        }

        if (!m_CurrentNode)
        {
            return *this;
        }

        if (!m_CurrentNode->IsVisible() && m_IgnoreVisibleToTraverse)
        {
            return GoToNextNode();
        }
        return *this;
    }

    template<typename Node>
    NodeIterator<Node>& NodeIterator<Node>::GoToPreviousNode()
    {
        if (!m_CurrentNode)
        {
            throw std::out_of_range("");
        }
        switch (m_TraversalMode)
        {
        case TraversalMode::PreOrder:
        {
            if (m_CurrentNode == m_RootNode)
            {
                m_CurrentNode = nullptr;
                break;
            }

            auto parent = m_CurrentNode->GetParent();
            size_t idx = m_CurrentIdx.back();
            m_CurrentIdx.pop_back();

            if (idx == 0)
            {
                m_CurrentNode = parent;
                break;
            }

            idx--;
            m_CurrentIdx.push_back(idx);
            m_CurrentNode = FindRightMostLeaf(parent->GetChild(idx).get());

            break;
        }
        case TraversalMode::PostOrder:
        {
            if (m_CurrentNode->GetNumberOfChildren() > 0)
            {
                size_t idx = m_CurrentNode->GetNumberOfChildren() - 1;
                m_CurrentIdx.push_back(idx);
                m_CurrentNode = m_CurrentNode->GetChild(idx).get();
                break;
            }

            if (m_CurrentNode == m_RootNode)
            {
                m_CurrentNode = nullptr;
                break;
            }

            auto parent = m_CurrentNode->GetParent();
            size_t idx = m_CurrentIdx.back();
            m_CurrentIdx.pop_back();

            while (parent != m_RootNode && idx == 0)
            {
                parent = parent->GetParent();
                idx = m_CurrentIdx.back();
                m_CurrentIdx.pop_back();
            }

            if (idx == 0)
            {
                m_CurrentNode = nullptr;
                break;
            }

            idx--;
            m_CurrentIdx.push_back(idx);
            m_CurrentNode = parent->GetChild(idx).get();

            break;
        }
        }

        if (!m_CurrentNode)
        {
            return *this;
        }

        if (!m_CurrentNode->IsVisible() && m_IgnoreVisibleToTraverse)
        {
            return GoToPreviousNode();
        }
        return *this;
    }

    template<typename Node>
    void NodeIterator<Node>::SetTraversalMode(TraversalMode mode)
    {
        m_TraversalMode = mode;
        GoToFirstNode();
    }
}

