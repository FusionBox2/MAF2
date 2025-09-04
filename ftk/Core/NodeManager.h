#pragma once

#include "ftkConfigure.h"

#include "mafBaseEventHandler.h"
#include "mafEventSender.h"

BEGIN_FTK_NAMESPACE

namespace model::data
{
	class Node;
}
using mafNode = model::data::Node;

class mafStorageElement;
class mafStorageElementBuilder;

namespace model::data
{

	class FTK_CORE_EXPORT NodeManager : public mafBaseEventHandler, public mafEventSender
	{
	public:

		NodeManager();

		~NodeManager() override;

		void OnEvent(mafEventBase* maf_event) override;

		bool IsModified() const { return m_Modified; }

		void Modified(bool modified) { m_Modified = modified; }

		void VmeRemove(mafNode* n);

		void NotifyRemove(mafNode* n);

		void NotifyAdd(mafNode* n);

		std::shared_ptr<mafNode> GetRoot();

		bool SetRoot(std::shared_ptr<mafNode> root);

		void Store(mafStorageElementBuilder& element) { InternalStore(element); }

		void Restore(const mafStorageElement& element) { InternalRestore(element); }

	protected:
		virtual void InternalStore(mafStorageElementBuilder& node);
		virtual void InternalRestore(const mafStorageElement& node);

		bool m_Modified = false;
		std::shared_ptr<mafNode> m_Root;
	};
}

using mafNodeManager = model::data::NodeManager;

END_FTK_NAMESPACE
