#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/IViewModel.h"

#include "ftk/Base/Connection.h"

#include <functional>
#include <string>
#include <vector>

BEGIN_FTK_NAMESPACE

namespace gui
{
	class ITreeViewNode;

	class ITreeViewModel : public IViewModel
	{
	public:
		using NodeId = ITreeViewNode*;

		virtual NodeId root() const = 0;

		virtual void reparentNode(NodeId node, NodeId parent) = 0;

		virtual void renameNode(NodeId node, const std::string& name) = 0;

		virtual void activateNode(NodeId node) = 0;

		virtual std::string getNodeName(NodeId node) const = 0;

		virtual int getNodeStatus(NodeId node) const = 0;

		virtual std::vector<NodeId> selected() const = 0;

		virtual void select(const std::vector<NodeId>& nodes) = 0;

		virtual void clearSelection() = 0;

		virtual bool isSelected(NodeId node) const = 0;

		virtual base::Connection connectNodeAdded(std::function<void(NodeId)> fn) = 0;

		virtual base::Connection connectNodeRemoved(std::function<void(NodeId)> fn) = 0;

		virtual base::Connection connectNodeMoved(std::function<void(NodeId)> fn) = 0;

		virtual base::Connection connectNodeChanged(std::function<void(NodeId)> fn) = 0;
	};
}

END_FTK_NAMESPACE
