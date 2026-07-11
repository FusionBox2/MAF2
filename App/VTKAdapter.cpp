#include "VTKAdapter.h"

#include "IVTKViewNode.h"

#include "mafRWI.h"

#include <vtkAssembly.h>
#include <vtkRenderer.h>

#include <array>

BEGIN_FTK_NAMESPACE

VTKAdapter::VTKAdapter(mafRWI *rwi)
	: m_rwi(rwi)
{
}

VTKAdapter::~VTKAdapter() = default;

void VTKAdapter::setModel(IVTKViewModel* model)
{
	std::array<vtkRenderer*, 3> renderers{m_rwi->m_RenFront, m_rwi->m_RenBack, m_rwi->m_AlwaysVisibleRenderer};
	if (m_model)
	{
		for (size_t i = 0; i < renderers.size(); i++)
		{
			if (renderers[i])
			{
				renderers[i]->RemoveActor(m_model->root()->getAssembly(i));
			}
		}
	}
	m_connections.clear();

	m_model = model;

	if (m_model)
	{
		for (size_t i = 0; i < renderers.size(); i++)
		{
			if (renderers[i])
			{
				renderers[i]->AddActor(m_model->root()->getAssembly(i));
			}
		}
		m_connections.push_back(m_model->connectSceneUpdated([this]() {m_rwi->CameraUpdate(); }));
		m_connections.push_back(m_model->connectSceneReset([this](const std::array<double, 6>& val) {m_rwi->CameraReset(const_cast<double*>(val.data())); }));
	}
	m_rwi->CameraUpdate();
}

void VTKAdapter::onNodeChanged(IVTKViewModel::NodeId n)
{
	m_rwi->CameraUpdate();
}

END_FTK_NAMESPACE
