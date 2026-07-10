#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/String.h"

#include <limits>
#include <list>
#include <memory>
#include <optional>

BEGIN_FTK_NAMESPACE

namespace core
{
	class Operation;

	class OperationManager
	{
	public:
		OperationManager(size_t maxCommands = std::numeric_limits<size_t>::max());

		OperationManager(const OperationManager&) = delete;

		OperationManager& operator=(const OperationManager&) = delete;

		~OperationManager();

		bool Submit(std::unique_ptr<Operation> command, bool storeIt = true);

		bool Undo();

		bool Redo();
		
		bool CanUndo() const;
		
		bool CanRedo() const;

		const base::String& GetLastCommandName() const;

		const base::String& GetNextCommandName() const;

		size_t GetMaxCommands() const { return m_maxNoCommands; }

		void ClearCommands();

		bool IsDirty() const;

		void MarkAsSaved();

	protected:
		void Store(std::unique_ptr<Operation> command);

		void ReduceUndoList(size_t size);

		size_t m_maxNoCommands;
		std::list<std::unique_ptr<Operation> > m_undoCommands;
		std::list<std::unique_ptr<Operation> > m_redoCommands;
		std::optional<Operation*> m_lastSavedCommand = nullptr;
	};
}

END_FTK_NAMESPACE
