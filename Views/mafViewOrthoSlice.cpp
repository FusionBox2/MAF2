/*=========================================================================

 Program: MAF2Medical
 Module: mafViewOrthoSlice
 Authors: Stefano Perticoni, Gianluigi Crimi, Grazia Di Cosmo

 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafViewOrthoSlice.h"
#include "mafViewSlice.h"
#include "mafPipeVolumeSlice_BES.h"
#include "mafGUILutSwatch.h"
#include "mafGUILutPreset.h"
#include "mafGUI.h"
#include "mafGUIFloatSlider.h"
#include "mafGUILutSlider.h"
#include "mafEventInteraction.h"
#include "mafEventSender.h"

#include "mmaVolumeMaterial.h"
#include "mafVMESurface.h"
#include "mafVMEVolume.h"
#include "mafVMEOutputVolume.h"
#include "mafIndent.h"
#include "mafGizmoSlice.h"
#include "mafVMEGizmo.h"
#include "mafPipeSurfaceSlice.h"
#include "medVisualPipeSlicerSlice.h"
#include "mafPipePolylineSlice.h"
#include "mafPipePolyline.h"
#include "mafDeviceButtonsPadMouse.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkCamera.h"
#include "vtkRendererCollection.h"
#include "mafPipeMeshSlice.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewOrthoSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
mafViewOrthoSlice::mafViewOrthoSlice(const mafString& label)
	: mafViewCompound(label, 2, 2)
	//----------------------------------------------------------------------------
{
	m_LutSlider = NULL;
	m_LutWidget = NULL;
	m_ColorLUT = NULL;
	m_CurrentVolume = NULL;
	m_GizmoHandlePosition[0] = m_GizmoHandlePosition[1] = m_GizmoHandlePosition[2] = 0.0;

	for (int j = 0; j < 3; j++)
	{
		m_Gizmo[j] = NULL;
	}

	m_Side = 0;
	m_Snap = 0;

	m_AllSurface = 0;
	m_Border = 1;
	m_PolylineRadiusSize = 1;

	m_CanPlugVisualPipes = true;

	// Added by Losi 11.25.2009
	m_EnableGPU = FALSE;
	m_TrilinearInterpolationOn = TRUE;

}
//----------------------------------------------------------------------------
mafViewOrthoSlice::~mafViewOrthoSlice()
//----------------------------------------------------------------------------
{
	m_VMElist.clear();
}
//----------------------------------------------------------------------------
mafView* mafViewOrthoSlice::Copy(mafBaseEventHandler* Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
	m_LightCopyEnabled = lightCopyEnabled;
	mafViewOrthoSlice* v = new mafViewOrthoSlice(GetLabel());
	v->SetListener(Listener);
	v->m_Id = m_Id;
	for (int i = 0; i < m_PluggedChildViewList.size(); i++)
	{
		v->m_PluggedChildViewList.emplace_back(m_PluggedChildViewList[i]->Copy(this));
	}
	v->Create();
	return v;
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::VmeShow(mafNode* node, bool show)
//----------------------------------------------------------------------------
{
	wxWindowDisabler wait1;
	wxBusyCursor wait2;

	// Disable Visual pipes plug at run time
	m_CanPlugVisualPipes = false;

	// Detect selected vme pos
	int pos = -1;
	for (int i = 0; i < m_VMElist.size(); i++)
		if (node == m_VMElist[i])
			pos = i;

	// if i want to show a vme that is not in list i add it to the list
	if (show && pos == -1)
		m_VMElist.push_back(node);
	// else if i want to un-show a vme i remove it only if is in the list
	else if (!show && pos >= 0)
		m_VMElist.erase(m_VMElist.begin() + pos);



	// Enable perspective View for every VME
	m_ChildViewList[PERSPECTIVE_VIEW]->VmeShow(node, show);
	// Disable ChildView XN, YN and ZN when no Volume is selected
	if (m_CurrentVolume)
		for (int j = 1; j < m_ChildViewList.size(); j++)
		{
			m_ChildViewList[j]->VmeShow(node, show);
			if (show && j == YN_VIEW && mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(node)))
				mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(node))->SetFlipNormalOff();
		}

	if (((mafVME*)node)->GetOutput()->IsA("mafVMEOutputVolume"))
	{
		for (int j = 1; j < m_ChildViewList.size(); j++)
			m_ChildViewList[j]->VmeShow(node, show);

		if (show)
		{
			// Create Ortho Stuff
			CreateOrthoslicesAndGizmos(node);

			// Definig radius of polylines at the equivalent side of the medium voxel
			double bounds[6], edges[3], vol, nPoints;
			vtkDataSet* volOutput;
			volOutput = ((mafVME*)node)->GetOutput()->GetVTKData();
			volOutput->GetBounds(bounds);
			nPoints = volOutput->GetNumberOfPoints();
			edges[0] = bounds[1] - bounds[0];
			edges[1] = bounds[3] - bounds[2];
			edges[2] = bounds[5] - bounds[4];
			vol = edges[0] * edges[1] * edges[2];
			m_PolylineRadiusSize = pow(vol / nPoints, 1.0 / 3.0) / 2.0;
		}
		else
		{
			DestroyOrthoSlicesAndGizmos();
		}

		// When one volume is selected/unselected we enable/disable ChildViews for all vme selected
		for (int i = 0; i < m_VMElist.size(); i++)
			for (int j = 1; j < m_ChildViewList.size(); j++)
			{
				m_ChildViewList[j]->VmeShow(m_VMElist[i], show);
				if (show && j == YN_VIEW && mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(m_VMElist[i])))
					mafPipeMeshSlice::SafeDownCast(m_ChildViewList[j]->GetNodePipe(m_VMElist[i]))->SetFlipNormalOff();
				ApplyViewSettings(m_VMElist[i]);
			}
	}
	else if (show)
		ApplyViewSettings(node);

	//CameraUpdate();
	EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::VmeRemove(mafNode* node)
//----------------------------------------------------------------------------
{
	if (m_CurrentVolume && node == m_CurrentVolume.get())
	{
		// Disable ChildViews
		for (int j = 1; j < m_ChildViewList.size(); j++)
			m_ChildViewList[j]->VmeShow(node, false);
		DestroyOrthoSlicesAndGizmos();
		EnableWidgets(false);
	}
	// Remove node from list
	int pos = -1;
	for (int i = 0; i < m_VMElist.size(); i++)
		if (node == m_VMElist[i])
			pos = i;
	if (pos >= 0)
		m_VMElist.erase(m_VMElist.begin() + pos);

	Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::CreateGuiView()
//----------------------------------------------------------------------------
{
	m_GuiView = new mafGUI(this);
	m_LutSlider = new mafGUILutSlider(m_GuiView, -1, wxPoint(0, 0), wxSize(500, 24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500, 24);
	m_LutSlider->SetMinSize(wxSize(500, 24));
	m_GuiView->Add(m_LutSlider);
	m_GuiView->Reparent(m_Win);
	EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::OnEvent(mafEventBase* maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent* e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
			/*    case ID_SIDE_ORTHO:
				{
				  if (m_Side == 1)
				  {
					((mafViewSlice *)m_ChildViewList[CHILD_XN_VIEW])->CameraSet(CAMERA_RX_RIGHT);
					((mafViewSlice*)m_ChildViewList[CHILD_XN_VIEW])->CameraUpdate();
				  }
				  else
				  {
					((mafViewSlice *)m_ChildViewList[CHILD_XN_VIEW])->CameraSet(CAMERA_RX_LEFT);
					((mafViewSlice*)m_ChildViewList[CHILD_XN_VIEW])->CameraUpdate();
				  }
				}
				break;*/
		case ID_BORDER_CHANGE:
		{
			OnEventSetThickness();
		}
		break;
		case ID_ALL_SURFACE:
		{
			if (m_AllSurface)
			{
				mafNode* node = GetSceneGraph()->GetSelectedVme();
				mafVME* vme = (mafVME*)node;
				mafNode* root = vme->GetRoot();
				SetThicknessForAllSurfaceSlices(root);
			}
		}
		break;
		case ID_LUT_CHOOSER:
		{
			auto currentVolumeMaterial = mafVMEOutputVolume::StaticDownCast(m_CurrentVolume->GetOutput())->GetMaterial();
			currentVolumeMaterial->UpdateFromTables();
			for (auto& childView : m_ChildViewList)
			{
				auto p = mafPipeVolumeSlice_BES::StaticDownCast(mafViewSlice::StaticDownCast(childView.get())->GetNodePipe(m_CurrentVolume));
				p->SetColorLookupTable(m_ColorLUT);
			}
			double* sr;
			sr = m_ColorLUT->GetRange();
			m_LutSlider->SetSubRange((long)sr[0], (long)sr[1]);
			CameraUpdate();
		}
		break;
		case ID_RANGE_MODIFIED:
		{
			if (mafViewSlice::StaticDownCast(m_ChildViewList[0].get())->VolumeIsVisible())
			{
				double low, hi;
				m_LutSlider->GetSubRange(&low, &hi);
				m_ColorLUT->SetTableRange(low, hi);
				auto currentVolumeMaterial = mafVMEOutputVolume::StaticDownCast(m_CurrentVolume->GetOutput())->GetMaterial();
				currentVolumeMaterial->UpdateFromTables();
				CameraUpdate();
			}
		}
		break;
		case MOUSE_UP:
		case MOUSE_MOVE:
		{
			// get the gizmo that is being moved
			long gizmoId = e->GetArg();
			double pos[3];
			vtkPoints* p = (vtkPoints*)e->GetVtkObj();
			if (p == NULL) return;
			p->GetPoint(0, pos);
			this->SetSlicePosition(gizmoId, p);
		}
		break;
		case ID_SNAP:
		{
			if (this->m_CurrentVolume == NULL && m_Snap)
			{
				wxMessageBox("You can't switch to snap modality!");
				m_Snap = 0;
				UpdateGUI();
			}
			else
			{
				for (int i = GIZMO_XN; i < GIZMOS_NUMBER; i++)
				{
					if (m_Snap == 1)
						m_Gizmo[i]->SetGizmoMovingModalityToSnap();
					else
						m_Gizmo[i]->SetGizmoMovingModalityToBound();
				}
			}
		}
		break;
		case ID_RESET_SLICES:
		{
			assert(m_CurrentVolume);
			this->ResetSlicesPosition(m_CurrentVolume.get());
		}
		break;
		// Added by Losi 11.25.2009
		case ID_ENABLE_GPU:
		{
			if (m_CurrentVolume)
			{
				for (auto& childView : m_ChildViewList)
				{
					if (auto p = mafPipeVolumeSlice_BES::SafeDownCast(mafViewSlice::StaticDownCast(childView.get())->GetNodePipe(m_CurrentVolume)))
					{
						p->SetEnableGPU(m_EnableGPU);
					}
				}
				this->CameraUpdate();
			}
		}
		break;
		case ID_TRILINEAR_INTERPOLATION:
		{
			if (m_CurrentVolume)
			{
				for (auto& childView : m_ChildViewList)
				{
					if (auto p = mafPipeVolumeSlice_BES::SafeDownCast(mafViewSlice::StaticDownCast(childView.get())->GetNodePipe(m_CurrentVolume)))
					{
						p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
					}
				}
				this->CameraUpdate();
			}
		}
		break;
		default:
			mafViewCompound::OnEvent(maf_event);
		}
	}
}
//-------------------------------------------------------------------------
mafGUI* mafViewOrthoSlice::CreateGui()
//-------------------------------------------------------------------------
{
	mafString layout_choices[3] = { _R("default"),_R("layout 1"),_R("layout 2") };

	assert(!AccessGUI());

	auto gui = mafView::CreateGui();

	gui->Combo(ID_LAYOUT_CHOOSER, _R("layout"), &m_LayoutConfiguration, 3, layout_choices);
	gui->Divider();
	m_LutWidget = gui->Lut(ID_LUT_CHOOSER, _R("lut"), m_ColorLUT);
	gui->Divider(2);

	wxString sidesName[2];
	sidesName[0] = "left";
	sidesName[1] = "right";
	// m_Gui->Radio(ID_SIDE_ORTHO, "side", &m_Side, 2, sidesName, 2);

	gui->Bool(ID_SNAP, _R("Snap on grid"), &m_Snap, 1);

	gui->Button(ID_RESET_SLICES, _R("reset slices"), _R(""));
	gui->Divider();

	gui->Bool(ID_ALL_SURFACE, _R("All Surface"), &m_AllSurface);
	gui->FloatSlider(ID_BORDER_CHANGE, _R("Border"), &m_Border, 1.0, 5.0);

	EnableWidgets(m_CurrentVolume != NULL);
	for (int i = 1; i < m_ChildViewList.size(); i++)
	{
		m_ChildViewList[i]->GetGui();
	}

	// Added by Losi 11.25.2009
	if (m_CurrentVolume)
	{
		for (auto& childView : m_ChildViewList)
		{
			if (auto p = mafPipeVolumeSlice_BES::SafeDownCast(mafViewSlice::StaticDownCast(childView.get())->GetNodePipe(m_CurrentVolume)))
			{
				p->SetEnableGPU(m_EnableGPU);
				p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
			}
		}
	}
	gui->Divider(1);
	//m_Gui->Bool(ID_ENABLE_GPU,"Enable GPU",&m_EnableGPU,1);
	gui->Bool(ID_TRILINEAR_INTERPOLATION, _R("Interpolation"), &m_TrilinearInterpolationOn, 1);

	gui->Divider();
	return gui;
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::PlugVisualPipeInSliceViews(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
	if (m_CanPlugVisualPipes)
	{
		for (int v = PERSPECTIVE_VIEW; v < VIEWS_NUMBER; v++)
			if (v != PERSPECTIVE_VIEW && m_ChildViewList[v])
				mafViewSlice::StaticDownCast(m_ChildViewList[v].get())->PlugVisualPipe(vme_type, pipe_type, MUTEX);

	}
	else
	{
		mafLogMessage(_M("OthoSlice Error: You cannot plug visual pipes after visualization operation"));
	}
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::PlugVisualPipeInPerspective(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
	if (m_CanPlugVisualPipes)
	{
		if (auto perspectiveView = mafViewSlice::StaticDownCast(m_ChildViewList[PERSPECTIVE_VIEW].get()))
			perspectiveView->PlugVisualPipe(vme_type, pipe_type, MUTEX);

	}
	else
	{
		mafLogMessage(_M("OthoSlice Error: You cannot plug visual pipes after visualization operation"));
	}
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::PackageView()
//----------------------------------------------------------------------------
{
	int cam_pos[4] = { CAMERA_OS_P, CAMERA_OS_X, CAMERA_OS_Y, CAMERA_OS_Z };

	mafString viewName[4] = { _R("perspective"),_R("camera x"),_R("camera y"),_R("camera z") };

	bool TICKs[4] = { false,false,true,true };
	for (int v = PERSPECTIVE_VIEW; v < VIEWS_NUMBER; v++)
	{
		auto view = std::make_unique<mafViewSlice>(viewName[v], cam_pos[v], false, false, false, 0, TICKs[v]);
		view->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeVolumeSlice_BES"), MUTEX);
		view->PlugVisualPipe(_R("medVMELabeledVolume"), _R("mafPipeVolumeSlice_BES"), MUTEX);
		view->PlugVisualPipe(_R("mafVMEVolumeLarge"), _R("mafPipeVolumeSlice_BES"), MUTEX);   //BES: 3.11.2009
		view->PlugVisualPipe(_R("mafVMEImage"), _R("mafPipeBox"), NON_VISIBLE);
		view->PlugVisualPipe(_R("medVMESegmentationVolume"), _R("mafPipeVolumeSlice_BES"), MUTEX);
		// plug surface slice visual pipe in not perspective views
		if (v != PERSPECTIVE_VIEW)
		{
			view->PlugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurfaceSlice"), MUTEX);
			view->PlugVisualPipe(_R("mafVMESurfaceParametric"), _R("mafPipeSurfaceSlice"), MUTEX);
			view->PlugVisualPipe(_R("mafVMEMesh"), _R("mafPipeMeshSlice"));
			view->PlugVisualPipe(_R("mafVMELandmark"), _R("mafPipeSurfaceSlice"), MUTEX);
			view->PlugVisualPipe(_R("mafVMELandmarkCloud"), _R("mafPipeSurfaceSlice"), MUTEX);
			view->PlugVisualPipe(_R("mafVMEPolyline"), _R("mafPipePolylineSlice"));
			view->PlugVisualPipe(_R("mafVMEPolylineSpline"), _R("mafPipePolylineSlice"));
			view->PlugVisualPipe(_R("mafVMEMeter"), _R("mafPipePolyline"));
			view->PlugVisualPipe(_R("medVMEMuscleWrapper"), _R("mafPipeSurfaceSlice"), MUTEX);
		}
		else
		{
			view->PlugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurface"), MUTEX);
			view->PlugVisualPipe(_R("medVMEMuscleWrapper"), _R("mafPipeSurface"), MUTEX);
		}
		PlugChildView(std::move(view));

	}
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	if (auto gui = AccessGUI())
	{
		gui->Enable(ID_LUT_CHOOSER, enable);
		gui->Enable(ID_RESET_SLICES, enable);
	}
	m_LutSlider->Enable(enable);

}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::GizmoCreate()
//----------------------------------------------------------------------------
{
	if (m_Gizmo[0] || m_Gizmo[1] || m_Gizmo[2]) GizmoDelete();

	if (m_CurrentVolume)
	{
		int gizmoId;
		double colors[] = { 1,0,0,  0,1,0,  0,0,1 };
		double direction[] = { mafGizmoSlice::GIZMO_SLICE_X,mafGizmoSlice::GIZMO_SLICE_Y,mafGizmoSlice::GIZMO_SLICE_Z };

		// creates the gizmos
		for (gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
		{
			double sliceOrigin[3];
			auto p = mafPipeVolumeSlice_BES::SafeDownCast(mafViewSlice::StaticDownCast(m_ChildViewList[0].get())->GetNodePipe(m_CurrentVolume));
			double normal[3];
			p->GetSlice(sliceOrigin, normal);

			m_Gizmo[gizmoId] = new mafGizmoSlice(m_CurrentVolume, this);
			m_Gizmo[gizmoId]->CreateGizmoSliceInLocalPositionOnAxis(gizmoId, direction[gizmoId], sliceOrigin[gizmoId]);
			m_Gizmo[gizmoId]->SetColor(&colors[gizmoId * 3]);
			m_Gizmo[gizmoId]->SetGizmoMovingModalityToBound();
		}

		// put them in the right views:
		// perspective view
		m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), true);
		m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), true);
		m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), true);

		// ZN view
		m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), true);
		m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), true);

		// YN view
		m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), true);
		m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), true);

		// ZN view
		m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), true);
		m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), true);
	}


}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::GizmoDelete()
//----------------------------------------------------------------------------
{
	// set gizmos visibility to false
	// perspective view
	for (int i = 0; i < 3; i++)
	{
		if (!m_Gizmo[i]) return;
	}
	m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), false);
	m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), false);
	m_ChildViewList[0]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), false);

	// ZN view
	m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), false);
	m_ChildViewList[1]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), false);

	// YN view
	m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_XN]->GetOutput(), false);
	m_ChildViewList[3]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), false);

	// XN view
	m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_YN]->GetOutput(), false);
	m_ChildViewList[2]->VmeShow(m_Gizmo[GIZMO_ZN]->GetOutput(), false);
	for (int i = 0; i < 3; i++)
	{
		cppDEL(m_Gizmo[i]);
	}
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::SetSlicePosition(long activeGizmoId, vtkPoints* p)
//----------------------------------------------------------------------------
{
	// gizmos update correctly in every views so this method is needed to update slice also
	/*  */

	// always update the perspective view

	mafVME* g[3];
	double pos[3], orient[3];

	p->GetPoint(0, m_GizmoHandlePosition);

	switch (activeGizmoId)
	{
	case (GIZMO_XN):
	{
		// update the X normal child view
		mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_XN_VIEW].get())->SetSliceLocalOrigin(m_GizmoHandlePosition);
	}
	break;


	case (GIZMO_YN):
	{
		// update the Y normal child view
		mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_YN_VIEW].get())->SetSliceLocalOrigin(m_GizmoHandlePosition);
	}
	break;

	case (GIZMO_ZN):
	{
		// update the Z normal child view
		mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_ZN_VIEW].get())->SetSliceLocalOrigin(m_GizmoHandlePosition);
	}
	break;

	}

	for (int gizmoId = GIZMO_XN; gizmoId < GIZMOS_NUMBER; gizmoId++)
	{
		// get the moved gizmo pose
		if (gizmoId == activeGizmoId) continue;

		g[gizmoId] = this->m_Gizmo[gizmoId]->GetOutput();
		g[gizmoId]->GetOutput()->GetPose(pos, orient);
		m_GizmoHandlePosition[gizmoId] = (double)pos[gizmoId];
	}

	// always update the child perspective view
	mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_PERSPECTIVE_VIEW].get())->SetSliceLocalOrigin(m_GizmoHandlePosition);


	this->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::OnEventSetThickness()
//----------------------------------------------------------------------------
{
	if (m_AllSurface)
	{
		auto node = this->GetSceneGraph()->GetSelectedVme();
		auto vme = mafVME::StaticDownCast(node);
		auto root = vme->GetRoot();
		SetThicknessForAllSurfaceSlices(root);
	}
	else
	{
		auto node = this->GetSceneGraph()->GetSelectedVme();
		auto SN = this->GetSceneGraph()->Vme2Node(node);

		if (auto pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if (auto pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if (auto pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}

		if (auto pipe = medVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if (auto pipe = medVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
		if (auto pipe = medVisualPipeSlicerSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(node)))
		{
			pipe->SetThickness(m_Border);
		}
	}
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
	mafIndent indent(tabs);

	os << indent << "mafViewOrthoSlice" << '\t' << this << std::endl;

	//print components view information

	for (int v = PERSPECTIVE_VIEW; v < VIEWS_NUMBER; v++)
	{
		m_ChildViewList[v]->Print(os, 1);
	}
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::CreateOrthoslicesAndGizmos(mafNode* node)
//-------------------------------------------------------------------------
{
	if (node == nullptr)
	{
		mafLogMessage(_M("node = NULL"));
		return;
	}

	m_CurrentVolume = mafVME::SafeDownCast(GetSceneGraph()->Vme2Node(node)->m_Vme);
	if (m_CurrentVolume == nullptr)
	{
		mafLogMessage(_M("current volume = NULL"));
		return;
	}

	double colorsX[] = { 1,0,0 };
	double colorsY[] = { 0,1,0 };
	double colorsZ[] = { 0,0,1 };
	auto currentVolumeMaterial = ((mafVMEOutputVolume*)m_CurrentVolume->GetOutput())->GetMaterial();
	double sr[2], vtkDataCenter[3];
	vtkDataSet* vtkData = m_CurrentVolume->GetOutput()->GetVTKData();
	//vtkData->Update();
	vtkData->GetCenter(vtkDataCenter);
	vtkData->GetCenter(m_GizmoHandlePosition);
	vtkData->GetScalarRange(sr);
	m_ColorLUT = currentVolumeMaterial->m_ColorLut;
	m_LutWidget->SetLut(m_ColorLUT);
	m_LutSlider->SetRange((long)sr[0], (long)sr[1]);
	m_LutSlider->SetSubRange((long)currentVolumeMaterial->m_TableRange[0], (long)currentVolumeMaterial->m_TableRange[1]);
	for (auto& childView : m_ChildViewList)
	{
		auto p = mafPipeVolumeSlice_BES::StaticDownCast(mafViewSlice::StaticDownCast(childView.get())->GetNodePipe(m_CurrentVolume));
		p->SetEnableGPU(m_EnableGPU);
		p->SetTrilinearInterpolation(m_TrilinearInterpolationOn);
		p->SetColorLookupTable(m_ColorLUT);
	}
	mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_XN_VIEW].get())->SetSliceLocalOrigin(m_GizmoHandlePosition);
	mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_XN_VIEW].get())->SetTextColor(colorsX);
	mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_YN_VIEW].get())->SetSliceLocalOrigin(m_GizmoHandlePosition);
	mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_YN_VIEW].get())->SetTextColor(colorsY);
	mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_ZN_VIEW].get())->SetSliceLocalOrigin(m_GizmoHandlePosition);
	mafViewSlice::StaticDownCast(m_ChildViewList[CHILD_ZN_VIEW].get())->SetTextColor(colorsZ);
	GizmoCreate();
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::DestroyOrthoSlicesAndGizmos()
//-------------------------------------------------------------------------
{
	// Destroy Ortho Stuff
	if (m_CurrentVolume == NULL)
	{
		mafLogMessage(_M("current volume = NULL"));
		return;
	}
	m_CurrentVolume->RemoveObserver(this);
	m_CurrentVolume = NULL;
	GizmoDelete();
}
//-------------------------------------------------------------------------
void mafViewOrthoSlice::ResetSlicesPosition(mafNode* node)
//-------------------------------------------------------------------------
{
	// workaround... :(
	// maybe we need some mechanism to execute view code from op?
	this->VmeShow(node, false);
	this->VmeShow(node, true);
	CameraUpdate();
}
//----------------------------------------------------------------------------
void mafViewOrthoSlice::SetThicknessForAllSurfaceSlices(mafNode* root)
//----------------------------------------------------------------------------
{
	for (auto& node : *root)
	{
		if (mafVME::StaticDownCast(&node)->GetOutput()->IsA("mafVMEOutputSurface")) //if(node->IsA("mafVMESurface"))
		{
			if (auto pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_XN_VIEW]->GetNodePipe(&node)))
			{
				pipe->SetThickness(m_Border);
			}
			if (auto pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_YN_VIEW]->GetNodePipe(&node)))
			{
				pipe->SetThickness(m_Border);
			}
			if (auto pipe = mafPipeSurfaceSlice::SafeDownCast(m_ChildViewList[CHILD_ZN_VIEW]->GetNodePipe(&node)))
			{
				pipe->SetThickness(m_Border);
			}
		}
	}
}
//----------------------------------------------------------------------------
bool mafViewOrthoSlice::IsPickedSliceView()
//----------------------------------------------------------------------------
{
	if (auto  rwi = GetGlobalMouse()->GetRenderWindow())
	{
		for (auto& childView : m_ChildViewList)
		{
			if (childView->IsMAFType(mafViewSlice))
			{
				if (mafViewSlice::StaticDownCast(childView.get())->GetRWI() == rwi && mafViewSlice::StaticDownCast(childView.get())->GetRWI()->GetRenderers()->GetFirstRenderer()->GetActiveCamera()->GetParallelProjection())
					return true;
			}
			else if (childView->IsMAFType(mafViewCompound))
			{
				if (mafViewCompound::StaticDownCast(childView.get())->GetSubView()->GetRWI() == rwi)
					return false;
			}
			else if (mafViewVTK::StaticDownCast(childView.get())->GetRWI() == rwi)
			{
				return false;
			}
		}
	}
	return false;
}

//----------------------------------------------------------------------------
void mafViewOrthoSlice::ApplyViewSettings(mafNode* node)
//----------------------------------------------------------------------------
{
	if (mafVME::StaticDownCast(node)->GetOutput()->IsA("mafVMEOutputPolyline"))
	{
		for (int i = CHILD_ZN_VIEW; i <= CHILD_YN_VIEW; i++)
		{
			auto pipeSlice = mafPipePolylineSlice::SafeDownCast(mafViewSlice::StaticDownCast(m_ChildViewList[i].get())->GetNodePipe(node));
			if (pipeSlice)
			{
				if (!node->IsA("mafVMEMeter"))
					pipeSlice->SetRadius(m_PolylineRadiusSize);
				pipeSlice->FillOn();
			}
		}
	}
}