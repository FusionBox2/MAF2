#include "OperationManager.h"
#include "Operation.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	OperationManager::OperationManager(size_t maxOperations)
		: m_maxNoOperations(maxOperations)
	{
	}

	OperationManager::~OperationManager() = default;

	bool OperationManager::submit(std::unique_ptr<Operation> operation, bool storeIt)
	{
		if (!operation)
		{
			return false;
		}

		if (!operation->execute())
		{
			return false;
		}

		if (!operation->canUndo())
		{
			reduceUndoList(0);
		}
		else if (storeIt)
		{
			store(std::move(operation));
		}
		return true;
	}

	bool OperationManager::undo()
	{
		if (canUndo())
		{
			if (m_undoOperations.back()->undo())
			{
				m_redoOperations.splice(begin(m_redoOperations), m_undoOperations, --end(m_undoOperations));
				return true;
			}
		}
		return false;
	}

	bool OperationManager::redo()
	{
		if (canRedo())
		{
			if (m_redoOperations.front()->execute())
			{
				m_undoOperations.splice(end(m_undoOperations), m_redoOperations, begin(m_redoOperations));
				return true;
			}
		}
		return false;
	}

	bool OperationManager::canUndo() const
	{
		return !m_undoOperations.empty();
	}

	bool OperationManager::canRedo() const
	{
		return !m_redoOperations.empty();
	}

	const base::String& OperationManager::getLastOperationName() const
	{
		return m_undoOperations.back()->getName();
	}

	const base::String& OperationManager::getNextOperationName() const
	{
		return m_redoOperations.back()->getName();
	}

	void OperationManager::clearOperations()
	{
		m_undoOperations.clear();
		m_redoOperations.clear();
		if (m_lastSavedOperation != nullptr)
		{
			m_lastSavedOperation = std::nullopt;
		}
	}

	bool OperationManager::isDirty() const
	{
		if (!m_lastSavedOperation)
		{
			return true;
		}
		if (m_undoOperations.empty())
		{
			return m_lastSavedOperation != nullptr;
		}
		return m_lastSavedOperation != m_undoOperations.back().get();
	}

	void OperationManager::markSaved()
	{
		m_lastSavedOperation = !m_undoOperations.empty() ? m_undoOperations.back().get() : nullptr;
	}

	void OperationManager::store(std::unique_ptr<Operation> operation)
	{
		if (!operation)
		{
			return;
		}

		m_redoOperations.clear();

		if (m_maxNoOperations > 0)
		{
			reduceUndoList(m_maxNoOperations - 1);
			m_undoOperations.push_back(std::move(operation));
		}
	}

	void OperationManager::reduceUndoList(size_t size)
	{
		while (m_undoOperations.size() > size)
		{
			if (m_lastSavedOperation == m_undoOperations.front().get())
			{
				m_lastSavedOperation = std::nullopt;
			}
			m_undoOperations.pop_front();
		}
	}

	void OperationManager::reduceRedoList(size_t size)
	{
		while (m_redoOperations.size() > size)
		{
			if (m_lastSavedOperation == m_redoOperations.front().get())
			{
				m_lastSavedOperation = std::nullopt;
			}
			m_undoOperations.pop_back();
		}
	}
}

END_FTK_NAMESPACE
