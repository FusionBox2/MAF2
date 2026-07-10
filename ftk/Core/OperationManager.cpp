#include "OperationManager.h"
#include "Operation.h"

BEGIN_FTK_NAMESPACE

namespace core
{
	OperationManager::OperationManager(size_t maxCommands)
		: m_maxNoCommands(maxCommands)
	{
	}

	OperationManager::~OperationManager() = default;

	bool OperationManager::Submit(std::unique_ptr<Operation> command, bool storeIt)
	{
		if (!command)
		{
			return false;
		}

		if (!command->Do())
		{
			return false;
		}

		if (!command->CanUndo())
		{
			ReduceUndoList(0);
		}
		else if (storeIt)
		{
			Store(std::move(command));
		}
		return true;
	}

	void OperationManager::Store(std::unique_ptr<Operation> command)
	{
		if (!command)
		{
			return;
		}

		m_redoCommands.clear();

		if (m_maxNoCommands > 0)
		{
			ReduceUndoList(m_maxNoCommands - 1);
			m_undoCommands.push_back(std::move(command));
		}
	}

	void OperationManager::ReduceUndoList(size_t size)
	{
		while (m_undoCommands.size() > size)
		{
			if (m_lastSavedCommand == m_undoCommands.front().get())
			{
				m_lastSavedCommand = std::nullopt;
			}
			m_undoCommands.pop_front();
		}
	}

	bool OperationManager::Undo()
	{
		if (CanUndo())
		{
			if (m_undoCommands.back()->Undo())
			{
				m_redoCommands.splice(begin(m_redoCommands), m_undoCommands, --end(m_undoCommands));
				return true;
			}
		}
		return false;
	}

	bool OperationManager::Redo()
	{
		if (CanRedo())
		{
			if (m_redoCommands.front()->Do())
			{
				m_undoCommands.splice(end(m_undoCommands), m_redoCommands, begin(m_redoCommands));
				return true;
			}
		}
		return false;
	}

	bool OperationManager::CanUndo() const
	{
		return !m_undoCommands.empty();
	}

	bool OperationManager::CanRedo() const
	{
		return !m_redoCommands.empty();
	}

	const base::String& OperationManager::GetLastCommandName() const
	{
		return m_undoCommands.back()->GetName();
	}

	const base::String& OperationManager::GetNextCommandName() const
	{
		return m_redoCommands.back()->GetName();
	}

	void OperationManager::ClearCommands()
	{
		m_undoCommands.clear();
		m_redoCommands.clear();
		if (m_lastSavedCommand != nullptr)
		{
			m_lastSavedCommand = std::nullopt;
		}
	}

	bool OperationManager::IsDirty() const
	{
		if (!m_lastSavedCommand)
		{
			return true;
		}
		if (m_undoCommands.empty())
		{
			return m_lastSavedCommand != nullptr;
		}
		return m_lastSavedCommand != m_undoCommands.back().get();
	}

	void OperationManager::MarkAsSaved()
	{
		m_lastSavedCommand = !m_undoCommands.empty() ? m_undoCommands.back().get() : nullptr;
	}
}

END_FTK_NAMESPACE
