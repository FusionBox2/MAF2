#include "DocumentSelection.h"

#include "DocumentContext.h"

BEGIN_FTK_NAMESPACE

DocumentSelection::DocumentSelection(DocumentContext& context)
{
	m_connection = context.getDocument()->connectNodeRemoved([this](const NodeRemoved& e)
	{
		if (auto it = m_selected.find(e.node); it != m_selected.end())
		{
			m_selected.erase(it);
			m_selectionChanged.emit(e.node);
		}
	});
}

std::vector<model::data::Node*> DocumentSelection::selected() const
{
	return { m_selected.begin(), m_selected.end() };
}

void DocumentSelection::select(const std::vector<model::data::Node*>& nodes)
{
	std::unordered_set<model::data::Node*> newSelected{ nodes.begin(),nodes.end() };
	std::unordered_set<model::data::Node*> selected;
	std::unordered_set<model::data::Node*> deselected;

	for (auto& node : newSelected)
	{
		if (m_selected.count(node) == 0)
		{
			selected.insert(node);
		}
	}
	for (auto& node : m_selected)
	{
		if (newSelected.count(node) == 0)
		{
			deselected.insert(node);
		}
	}
	std::swap(m_selected, newSelected);
	for (auto& node : selected)
	{
		m_selectionChanged.emit(node);
	}
	for (auto& node : deselected)
	{
		m_selectionChanged.emit(node);
	}
}

void DocumentSelection::clearSelection()
{
	std::unordered_set<model::data::Node*> oldSelection;
	std::swap(m_selected, oldSelection);
	for (auto& node : oldSelection)
	{
		m_selectionChanged.emit(node);
	}
}

bool DocumentSelection::isSelected(model::data::Node* node) const
{
	return m_selected.count(node) != 0;
}

base::Connection DocumentSelection::connectSelectionChanged(std::function<void(model::data::Node*)> fn)
{
	return m_selectionChanged.connect(std::move(fn));
}

END_FTK_NAMESPACE
