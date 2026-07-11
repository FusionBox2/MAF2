#include "VTKView.h"

#include "IVTKViewModel.h"

#include "VTKAdapter.h"

#include "ftk/wxVTK/wxVTKWindow.h"

#include "mafRWI.h"

#include <vtkRenderer.h>

BEGIN_FTK_NAMESPACE

VTKView::VTKView(wxWindow* parent)
{
	auto wxvtk = new wxVTKWindow(parent, wxID_ANY);
	m_widget = wxvtk;
	m_CameraPositionId = CAMERA_PERSPECTIVE;
	m_AxesType = mafAxes::TRIAD;
	m_RWI = std::make_unique<mafRWI>(wxvtk->GetRenderWindow(), ONE_LAYER, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType, m_ShowOrientator, m_AxesType);
	m_RWI->CameraSet(m_CameraPositionId);
	vtkRenderer* renderers[] = { m_RWI->m_RenFront, m_RWI->m_RenBack, m_RWI->m_AlwaysVisibleRenderer };
	m_adapter = std::make_unique<VTKAdapter>(m_RWI.get());
	m_rwiValuesChanged = m_RWI->connectValuesChanged([this]() {m_valuesChanged.emit(); });
	m_rwiPropertiesChanged = m_RWI->connectPropertiesChanged([this]() {m_propertiesChanged.emit(); });
}

VTKView::~VTKView() = default;

wxWindow* VTKView::widget()
{
	return m_widget;
}

std::shared_ptr<gui::IViewModel> VTKView::getModel() const
{
	return m_viewModel;
}

void VTKView::setModel(std::shared_ptr<gui::IViewModel> viewModel)
{
	m_adapter->setModel(nullptr);
	m_viewModel = std::static_pointer_cast<IVTKViewModel>(viewModel);
	if (viewModel)
	{
		m_adapter->setModel(m_viewModel.get());
		m_modelValuesChanged = m_viewModel->connectValuesChanged([this]() {m_valuesChanged.emit(); });
		m_modelPropertiesChanged = m_viewModel->connectPropertiesChanged([this]() {m_propertiesChanged.emit(); });
	}
	else
	{
		m_modelValuesChanged = {};
		m_modelPropertiesChanged = {};
	}
	m_propertiesChanged.emit();
}

void VTKView::setActive(bool active)
{
}

core::WithProperties::PropertyList VTKView::getProperties()
{
	auto result = core::WithProperties::getProperties();
	for (auto& rp : m_RWI->getProperties())
	{
		result.push_back(std::move(rp));
	}
	if (m_viewModel)
	{
		for (auto& mp : m_viewModel->getProperties())
		{
			result.push_back(std::move(mp));
		}
	}
	return result;
}

END_FTK_NAMESPACE
