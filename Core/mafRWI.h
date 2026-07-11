#pragma once

#include "ftk/Core/WithProperties.h"
#include "ftk/wxVTK/wxVTKWindow.h"
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafAxes.h"

#include "vtkMAF/vtkMAFGridActor.h"

#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

namespace model::data
{
	class Node;
}
using mafNode = model::data::Node;

class vtkMAFProfilingActor;
class vtkMAFSimpleRulerActor2D;
class vtkMAFTextOrientator;

class mafSceneGraph;
class mafAxes;
class mafGUI;
class mafGUIPicButton;

enum RWI_LAYERS
{
	ONE_LAYER = 0,
	TWO_LAYER
};

class MAF_EXPORT mafRWI : public mafBaseEventHandler, public mafEventSender, public core::WithProperties
{
public:
	mafRWI();
	mafRWI(wxWindow* parent, RWI_LAYERS layers = ONE_LAYER, bool use_grid = false, bool show_axes = false, bool show_ruler = false, int stereo = 0, bool show_orientator = false, int axesType = mafAxes::TRIAD);
	~mafRWI() override;

	void OnEvent(mafEventBase* maf_event) override;

	/** Create all the elements necessary to build the rendering scene.*/
	void CreateRenderingScene(wxWindow* parent, RWI_LAYERS layers = ONE_LAYER, bool use_grid = false, bool show_axes = false, bool show_ruler = false, int stereo = 0, bool show_orientator = false, int axesType = mafAxes::TRIAD);

	/** Reset the camera position. If vme is passed as parameter, the camera is resetted to fill the vme into the view. */
	void CameraReset(mafNode* vme = nullptr, double zoom = 1);

	/** Reset the camera position according to the bounds. */
	void CameraReset(double bounds[6], double zoom = 1);

	/** Update the renderwindow. */
	void CameraUpdate();

	/** Set the camera position. */
	void CameraSet(int cam_position, double zoom = 1);


	/** Set the camera position, set custom pos , custom viewUp , zoom and if projection is parallel */
	void CameraSet(double pos[3], double viewUp[3], double zoom = 1., bool parallelProjection = false);

	/** Set the render window's size. */
	void SetSize(int x, int y, int w, int h);

	/** Show the render window. */
	void Show(bool show);

	/** Set the normal for the grid actor. */
	void SetGridNormal(int normal_id);

	/** Set the position for the grid actor. */
	void SetGridPosition(double position);

	/** Set the visibility for the grid actor. */
	void SetGridVisibility(bool show);

	/** Set the color for the grid actor. */
	void SetGridColor(const wxColor& col);

	/** Set the background color for the render window. */
	void SetBackgroundColor(const wxColor& col);

	/** Set the stereo type. */
	void SetStereo(int stereo_type);

	/** Set the visibility for the axes actor. */
	void SetAxesVisibility(bool show);

	/** Set the visibility for the rule actor. */
	void SetRuleVisibility(bool show = true);

	/** Set the visibility for the rule actor. */
	void SetOrientatorVisibility(bool show = true);

	/** Set the visibility for the rule actor. */
	void SetOrientatorSingleActorVisibility(bool showLeft = true, bool showDown = true, bool showRight = true, bool showUp = true);

	/** Set the text for the rule actor. */
	void SetOrientatorSingleActorText(const char* textLeft, const char* textDown, const char* textRight, const char* textUp);

	/** Set the scale factor to convert the data scale to the visualized scale of the rule.
	By default the ruler shows the same scale of the data. */
	void SetRulerScaleFactor(const double& scale_factor);

	/** Set the label of the ruler. Example the unit measure of the data. */
	void SetRulerLegend(const mafString& ruler_legend);

	/** Set Orientator Text Properties */
	void SetOrientatorProperties(double rgbText[3], double rgbBackground[3], double scale = 1);

	/** Set Profiling Information Text Visibility*/
	void SetProfilingActorVisibility(bool show);

	/** Update scale factor and legend.
	This method is called from logic to update measure unit according to the application settings.*/
	void UpdateRulerUnit();

	virtual mafGUI* GetGui();

	/** Update member variables and GUI elements according to vtkCamera position, focal point, view up, ...*/
	void UpdateCameraParameters();

	/** Allow to add/remove current vtkCamera to the list of vtkCamera linked together*/
	void LinkCamera(bool linc_camera = true);

	/* Return active ruler for using its methods*/
	vtkMAFSimpleRulerActor2D* GetRuler();

	/* Reset camera clipping range using visualized actors */
	void ResetCameraClippingRange();

	PropertyList getProperties() override;

	mafSceneGraph* m_Sg = nullptr;
	wxVTKWindow* m_RwiBase = nullptr;
	vtkSmartPointer<vtkRenderer> m_RenFront; ///< Renderer used to show actors on the first layer
	vtkSmartPointer<vtkRenderer> m_RenBack; ///< Renderer used to show actors on the second layer
	vtkSmartPointer<vtkRenderer> m_AlwaysVisibleRenderer; /// < Renderer used to superimpose utility stuff to main render window
	vtkSmartPointer<vtkRenderWindow> m_RenderWindow;
	vtkSmartPointer<vtkLight> m_Light;
	vtkSmartPointer<vtkCamera> m_Camera;
	int m_CameraPositionId; ///< Integer representing a preset for camera position, focal point and view up
	double m_CameraPosition[3]; ///< Vector representing the camera position
	double m_FocalPoint[3]; ///< Vector representing the camera focal point

	double m_CameraViewUp[3]; ///< Vector representing the camera view-up
	double m_CameraOrientation[3]; ///< Vector representing the camera orientation

	double m_StepCameraOrientation = 10.0; ///< Step with which rotate the camera around its focal point.
	double m_TopBottomAccumulation = 0.0;
	double m_LeftRigthAccumulation = 0.0;
	double m_TopBottomAccumulationLast = 0.0;
	double m_LeftRigthAccumulationLast = 0.0;

protected:
	/** Compute the bounds for the visible actors; if vme is passed, the bounds of vme are calculated. */
	double* ComputeVisibleBounds(mafNode* node = nullptr);
	mafGUI* CreateGui();

	mafGUI* m_Gui = nullptr;
	wxColour m_BGColour;
	wxColour m_GridColour;
	double m_GridPosition = 0.0;
	mafGUIPicButton* m_CameraButtons[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
	wxBoxSizer* m_Sizer = nullptr;

	vtkSmartPointer<vtkMAFGridActor> m_Grid; ///< Actor representing a grid showed into the render window.
	int m_ShowGrid = 0; ///< Flag used to show/hide the grid.
	int m_GridNormal = GRID_Z;
	int m_ShowAxes = 0;  ///< Flag used to show/hide axes in low left corner of the view
	int m_LinkCamera = 0;///< Flag that enable to synchronize camera interaction to other camera

	double m_RulerScaleFactor = 1.0;
	mafString m_RulerLegend;
	vtkSmartPointer<vtkMAFSimpleRulerActor2D> m_Ruler;
	int m_ShowRuler = 0; ///< Flag used to show/hide ruler actor into a parallel view
	int m_StereoType = 0;
	vtkSmartPointer<vtkMAFTextOrientator> m_Orientator;
	int m_ShowProfilingInformation = 0;
	int m_ShowOrientator = 0;
	vtkSmartPointer<vtkMAFProfilingActor> m_ProfilingActor;
	mafString m_StereoMovieDir;
	int       m_StereoMovieEnable = 0;
	int       m_AxesType = mafAxes::TRIAD;
	std::unique_ptr<mafAxes> m_Axes; ///< Actor representing a global reference system.
};
