#include "ViewManager.h"

BEGIN_FTK_NAMESPACE

ViewManager::ViewManager() = default;

gui::wx::IView* ViewManager::create(const base::String& name, wxWindow* parent)
{
	if (auto it = m_factories.find(name); it != end(m_factories))
	{
		auto view = it->second.viewFn(parent);
		auto rawView = view.get();
		ViewEntry ve = { std::move(view), it->second.modelFn};
		if (m_context)
		{
			ve.view->setModel(ve.modelFn(*m_context));
		}
		m_views.emplace(rawView, std::move(ve));
		return rawView;
	}
	return nullptr;
}

void ViewManager::remove(gui::wx::IView* view)
{
	if (m_active == view)
	{
		setActive(nullptr);
	}
	
	if (auto it = m_views.find(view); it != end(m_views))
	{
		view->setModel(nullptr);
		m_views.erase(it);
	}
}

gui::wx::IView* ViewManager::getActive() const
{
	return m_active;
}

void ViewManager::setActive(gui::wx::IView* view)
{
	if (m_active == view)
	{
		return;
	}

	if (m_active)
	{
		m_active->setActive(false);
	}

	m_active = view;

	if (m_active)
	{
		m_active->setActive(true);
	}

	m_activeViewChanged.emit();
}

void ViewManager::attach(DocumentContext& context)
{
	for (auto& view : m_views)
	{
		view.second.view->setModel(view.second.modelFn(context));
	}
	m_context = &context;
}

void ViewManager::detach()
{
	for (auto& view : m_views)
	{
		view.second.view->setModel(nullptr);
	}
	m_context = nullptr;
}

base::Connection ViewManager::connectActiveViewChanged(std::function<void()> fn)
{
	return m_activeViewChanged.connect(fn);
}

void ViewManager::registerFactory(const base::String& name, std::function<std::unique_ptr<gui::wx::IView>(wxWindow*)> viewFn, std::function<std::shared_ptr<gui::IViewModel>(DocumentContext&)> modelFn)
{
	FactoryElement fe = { std::move(viewFn), std::move(modelFn) };
	m_factories.emplace(name, std::move(fe));
}

std::vector<base::String> ViewManager::factories() const
{
	std::vector<base::String> result;
	for (auto& entry : m_factories)
	{
		result.push_back(entry.first);
	}
	return result;
}

END_FTK_NAMESPACE
