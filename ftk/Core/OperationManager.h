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
		OperationManager(size_t maxOperations = std::numeric_limits<size_t>::max());

		OperationManager(const OperationManager&) = delete;

		OperationManager& operator=(const OperationManager&) = delete;

		~OperationManager();

		bool submit(std::unique_ptr<Operation> operation, bool storeIt = true);

		bool undo();

		bool redo();
		
		bool canUndo() const;
		
		bool canRedo() const;

		const base::String& getLastOperationName() const;

		const base::String& getNextOperationName() const;

		size_t getMaxOperations() const { return m_maxNoOperations; }

		void clearOperations();

		bool isDirty() const;

		void markSaved();

	protected:
		void store(std::unique_ptr<Operation> operation);

		void reduceUndoList(size_t size);

		void reduceRedoList(size_t size);

		size_t m_maxNoOperations;
		std::list<std::unique_ptr<Operation> > m_undoOperations;
		std::list<std::unique_ptr<Operation> > m_redoOperations;
		std::optional<Operation*> m_lastSavedOperation = nullptr;
	};
}

END_FTK_NAMESPACE
