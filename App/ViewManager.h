#pragma once

#include "ftkConfigure.h"

#include "ftk/Base/Signal.h"
#include "ftk/Base/String.h"
#include "ftk/Gui/wx/IView.h"

#include <map>
#include <memory>
#include <unordered_map>

BEGIN_FTK_NAMESPACE

class DocumentContext;

class ViewManager
{
public:
	ViewManager();

	gui::wx::IView* create(const base::String& name, wxWindow* parent);

	void remove(gui::wx::IView* viewModel);

	gui::wx::IView* getActive() const;

	void setActive(gui::wx::IView* view);

	void attach(DocumentContext& context);

	void detach();

	base::Connection connectActiveViewChanged(std::function<void()> fn);

	void registerFactory(const base::String& name, std::function<std::unique_ptr<gui::wx::IView>(wxWindow*)> viewFn, std::function<std::shared_ptr<gui::IViewModel>(DocumentContext&)> modelFn);

	std::vector<base::String> factories() const;

private:

	struct FactoryElement
	{
		std::function<std::unique_ptr<gui::wx::IView>(wxWindow*)> viewFn;
		std::function<std::shared_ptr<gui::IViewModel>(DocumentContext&)> modelFn;
	};
	struct ViewEntry
	{
		std::unique_ptr<gui::wx::IView> view;
		std::function<std::shared_ptr<gui::IViewModel>(DocumentContext&)> modelFn;
	};

	std::map<base::String, FactoryElement> m_factories;
	std::unordered_map<gui::wx::IView*, ViewEntry> m_views;
	DocumentContext* m_context = nullptr;
	gui::wx::IView* m_active = nullptr;
	base::Signal<> m_activeViewChanged;
};

END_FTK_NAMESPACE
