#include "VTKView.h"

#include "IVTKViewModel.h"

#include "VTKAdapter.h"

#include "mafRWI.h"

BEGIN_FTK_NAMESPACE

VTKView::VTKView(wxWindow* parent)
{
	m_CameraPositionId = CAMERA_PERSPECTIVE;
	m_AxesType = mafAxes::TRIAD;
	m_RWI = std::make_unique<mafRWI>(parent, ONE_LAYER, m_ShowGrid, m_ShowAxes, m_ShowRuler, m_StereoType, m_ShowOrientator, m_AxesType);
	m_RWI->CameraSet(m_CameraPositionId);
	vtkRenderer* renderers[] = { m_RWI->m_RenFront, m_RWI->m_RenBack, m_RWI->m_AlwaysVisibleRenderer };
	m_adapter = std::make_unique<VTKAdapter>(m_RWI.get());
}

VTKView::~VTKView() = default;

wxWindow* VTKView::widget()
{
	return m_RWI->m_RwiBase;
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
	}
}

void VTKView::setActive(bool active)
{
}

END_FTK_NAMESPACE
