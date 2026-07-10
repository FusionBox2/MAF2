#pragma once

#include "ftkConfigure.h"

#include "ISelectionController.h"

#include "ftk/Base/Signal.h"
#include "ftk/Base/Connection.h"

#include <unordered_set>
#include <vector>

BEGIN_FTK_NAMESPACE
class DocumentContext;
class DocumentSelection : public ISelectionController
{
public:
	DocumentSelection(DocumentContext& context);

	std::vector<model::data::Node*> selected() const override;

	void select(const std::vector<model::data::Node*>& nodes) override;

	void clearSelection() override;

	bool isSelected(model::data::Node* node) const override;

	base::Connection connectSelectionChanged(std::function<void(model::data::Node*)> fn) override;

private:
	std::unordered_set<model::data::Node*> m_selected;
	base::Signal<model::data::Node*> m_selectionChanged;
	base::Connection m_connection;
};

END_FTK_NAMESPACE
