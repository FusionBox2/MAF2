#pragma once

#include "ftkConfigure.h"

#include "ftk/Gui/wx/IView.h"

#include <memory>

class mafRWI;

BEGIN_FTK_NAMESPACE

class IVTKViewModel;
class VTKAdapter;

class VTKView : public gui::wx::IView
{
public:
	VTKView(wxWindow* parent);

	~VTKView() override;

	wxWindow* widget() override;

	std::shared_ptr<gui::IViewModel> getModel() const override;

	void setModel(std::shared_ptr<gui::IViewModel> viewModel) override;

	void setActive(bool active) override;

	PropertyList getProperties() override;

private:
	bool m_ShowAxes = true;  ///< Flag used to show/hide axes in low left corner of the view
	bool m_ShowGrid = false;
	bool m_ShowRuler = false;
	bool m_ShowOrientator = false;
	int m_StereoType = 0; ///< Indicate the stereo type to use with the view
	int m_CameraPositionId; ///< Integer representing a preset for camera position, focal point and view up.
	int m_AxesType;
	wxWindow* m_widget = nullptr;
	std::unique_ptr<mafRWI> m_RWI;
	std::shared_ptr<IVTKViewModel> m_viewModel;
	std::unique_ptr<VTKAdapter> m_adapter;
	base::Connection m_rwiValuesChanged;
	base::Connection m_rwiPropertiesChanged;
	base::Connection m_modelValuesChanged;
	base::Connection m_modelPropertiesChanged;
};

END_FTK_NAMESPACE
