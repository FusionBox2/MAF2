/*=========================================================================

 Program: MAF2Medical
 Module: medViewSlicer
 Authors: Daniele Giunchi

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

#include "mafGUI.h"
#include "medViewSlicer.h"
#include "mafViewSlice.h"
#include "mafVme.h"
#include "mafVMESlicer.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafPipeImage3D.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipeSurface.h"
#include "mafPipeSurfaceTextured.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafGizmoTranslate.h"
#include "mafGizmoRotate.h"
#include "mafSceneGraph.h"
#include "mafEvent.h"
#include "mafAbsMatrixPipe.h"
#include "mafAttachCamera.h"
#include "mafInteractorGenericMouse.h"
#include "mafVMESlicer.h"
#include "mafTagArray.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafGUILutPreset.h"
#include "mafVMEOutputSurface.h"
#include "mafAttribute.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafAttachCamera.h"

#include "vtkTransform.h"
#include "vtkLookupTable.h"
#include "vtkDataSet.h"
#include "vtkMath.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointSet.h"
#include "vtkRendererCollection.h"
#include "vtkPointData.h"
#include "vtkDataSetAttributes.h"
#include "vtkPolyDataNormals.h"
#include "vtkCamera.h"
#include "vtkImageData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medViewSlicer);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum ARBITRARY_SUBVIEW_ID
{
	ARBITRARY_VIEW = 0,
	SLICE_VIEW,
};
enum GIZMO_TYPE_ID
{
	GIZMO_TRANSLATE = 0,
	GIZMO_ROTATE,
};
enum AXIS_ID
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS,
};

//----------------------------------------------------------------------------
medViewSlicer::medViewSlicer(const mafString& label, bool show_ruler)
	: medViewCompoundWindowing(label, 1, 2)
	//----------------------------------------------------------------------------
{
	m_CurrentVolume = NULL;
	m_CurrentImage = NULL;
	m_CurrentSlicer = NULL;

	m_SliceCenterSurface[0] = 0.0;
	m_SliceCenterSurface[1] = 0.0;
	m_SliceCenterSurface[2] = 0.0;

	m_SliceCenterSurfaceReset[0] = 0.0;
	m_SliceCenterSurfaceReset[1] = 0.0;
	m_SliceCenterSurfaceReset[2] = 0.0;

}
//----------------------------------------------------------------------------
medViewSlicer::~medViewSlicer()
//----------------------------------------------------------------------------
{
	m_AttachCamera.reset();
}
//----------------------------------------------------------------------------
void medViewSlicer::PackageView()
//----------------------------------------------------------------------------
{
	auto ViewArbitrary = std::make_unique<mafViewVTK>(_R(""), CAMERA_PERSPECTIVE);
	//m_ViewArbitrary->PlugVisualPipe("mafVMESurface", "mafPipeSurfaceSlice");
	ViewArbitrary->PlugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurfaceTextured"));
	ViewArbitrary->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeBox"), MUTEX);
	ViewArbitrary->PlugVisualPipe(_R("mafVMELabeledVolume"), _R("mafPipeBox"), MUTEX);

	auto ViewSlice = std::make_unique<mafViewVTK>(_R(""), CAMERA_CT);
	ViewSlice->PlugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurfaceSlice"));
	ViewSlice->PlugVisualPipe(_R("mafVMESurfaceParametric"), _R("mafPipeSurfaceSlice"));
	ViewSlice->PlugVisualPipe(_R("mafVMEGizmo"), _R("mafPipeGizmo"), NON_VISIBLE);
	ViewSlice->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeBox"), NON_VISIBLE);

	PlugChildView(std::move(ViewArbitrary));
	PlugChildView(std::move(ViewSlice));

}
//----------------------------------------------------------------------------
void medViewSlicer::VmeShow(mafNode* node, bool show)
//----------------------------------------------------------------------------
{
	m_ChildViewList[ARBITRARY_VIEW]->VmeShow(node, show);
	m_ChildViewList[SLICE_VIEW]->VmeShow(node, show);
	mafVME* Vme = mafVME::SafeDownCast(node);
	Vme->Update();
	if (show)
	{
		if (mafVME::StaticDownCast(Vme)->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			mafVME* Volume = mafVME::SafeDownCast(Vme);
			m_CurrentVolume = Volume;

			// get the VTK volume
			vtkDataSet* data = mafVME::StaticDownCast(node)->GetOutput()->GetVTKData();
			//data->Update();	
		}
		else if (Vme->IsA("mafVMESurface") || Vme->IsA("mafVMESurfaceParametric"))
		{
			//a surface is visible only if there is a volume in the view
			if (m_CurrentVolume)
			{
				CameraUpdate();
			}
		}
		else if (Vme->IsA("mafVMESlicer"))
		{
			//Show Slicer
			m_CurrentSlicer = mafVMESlicer::SafeDownCast(node);

			//Set camera of slice view in way that it will follow the volume
			if (!m_AttachCamera)
				m_AttachCamera = std::make_unique<mafAttachCamera>(AccessGUI(), mafViewVTK::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->m_Rwi.get(), this);
			m_AttachCamera->SetStartingMatrix(m_CurrentSlicer->GetOutput()->GetAbsMatrix());
			m_AttachCamera->SetVme(m_CurrentSlicer);
			mafViewVTK::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->CameraReset(m_CurrentSlicer);
		}
		else if (Vme->IsA("mafVMEImage")) {
			m_CurrentImage = mafVMEImage::SafeDownCast(node);
		}
	}
	else//if show=false
	{

		if (Vme->GetOutput()->IsA("mafVMEOutputVolume"))
		{
			m_CurrentVolume = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
		}
		else if (Vme->IsA("mafVMESlicer"))
		{
			m_AttachCamera->SetVme(NULL);
			m_CurrentSlicer = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
			m_LutSlider->Enable(false);
			double normal[3] = { 0,0,1 };
			mafViewSlice::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->CameraSet(CAMERA_CT);
		}
		else if (Vme->IsA("mafVMEImage"))
		{
			m_CurrentImage = NULL;
			m_ColorLUT = NULL;
			m_LutWidget->SetLut(m_ColorLUT);
			m_LutSlider->Enable(false);
			double normal[3] = { 0,0,1 };
			mafViewSlice::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->CameraSet(CAMERA_CT);
		}
	}

	UpdateWindowing(show && this->ActivateWindowing(GetSceneGraph()->GetSelectedVme()),
		GetSceneGraph()->GetSelectedVme()
	);

	{ mafEvent evUnq(this, CAMERA_UPDATE); InvokeEvent(evUnq); }

	//EnableWidgets(m_CurrentVolume != NULL);
}
//----------------------------------------------------------------------------
void medViewSlicer::OnEvent(mafEventBase* maf_event)
//----------------------------------------------------------------------------
{
	if (maf_event->GetSender() == AccessGUI() || maf_event->GetSender() == this->m_LutSlider) // from this view gui
	{
		OnEventThis(maf_event);
	}
	else
	{
		// if no one can handle this event send it to the operation listener
		InvokeEvent(*maf_event);
	}
	//{mafEvent evUnq(this,CAMERA_UPDATE); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
void medViewSlicer::OnEventThis(mafEventBase* maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent* e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case ID_RANGE_MODIFIED:
		{
			if (m_CurrentVolume || m_CurrentImage)
			{
				double low, hi;
				m_LutSlider->GetSubRange(&low, &hi);
				m_ColorLUT->SetTableRange(low, hi);
				{ mafEvent evUnq(this, CAMERA_UPDATE); InvokeEvent(evUnq); }
			}
		}
		break;

		case ID_HELP:
		{
			mafEvent helpEvent;
			helpEvent.SetSender(this);
			mafString viewLabel = GetLabel();
			helpEvent.SetString(&viewLabel);
			helpEvent.SetId(OPEN_HELP_PAGE);
			InvokeEvent(helpEvent);
		}
		break;

		case ID_LUT_CHOOSER:
		{
			if (m_ColorLUT && m_CurrentSlicer)
			{
				double* sr;
				sr = m_ColorLUT->GetRange();
				if (m_LutSlider) m_LutSlider->SetSubRange((long)sr[0], (long)sr[1]);
			}
			else
			{
				wxMessageBox("There is no visualized Slicer");
			}

		}
		CameraUpdate();
		break;
		default:
			mafViewCompound::OnEvent(maf_event);
		}
	}
}
//----------------------------------------------------------------------------
mafView* medViewSlicer::Copy(mafBaseEventHandler* Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
	m_LightCopyEnabled = lightCopyEnabled;
	medViewSlicer* v = new medViewSlicer(GetLabel());
	v->SetListener(Listener);
	v->m_Id = m_Id;
	for (auto& pluggedChild : m_PluggedChildViewList)
	{
		v->m_PluggedChildViewList.emplace_back(pluggedChild->Copy(this));
	}
	v->Create();
	return v;
}
//----------------------------------------------------------------------------
mafGUI* medViewSlicer::CreateGui()
//----------------------------------------------------------------------------
{
	assert(!AccessGUI());
	auto gui = new mafGUI(this);

	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	InvokeEvent(buildHelpGui);

	if (buildHelpGui.GetArg())
	{
		gui->Button(ID_HELP, _R("Help"), _R(""));
	}


	//m_Gui->Button(ID_RESET,"Reset","");
	gui->Divider(2);

	m_LutWidget = gui->Lut(ID_LUT_CHOOSER, _R("lut"), m_ColorLUT);

	gui->Divider();
	gui->Update();

	EnableWidgets(m_CurrentVolume != NULL);
	return gui;
}
//----------------------------------------------------------------------------
void medViewSlicer::VmeRemove(mafNode* node)
//----------------------------------------------------------------------------
{
	if (m_CurrentVolume && node == m_CurrentVolume)
	{
		m_CurrentVolume = NULL;
	}
	if (m_CurrentSlicer == node && node != NULL && m_AttachCamera)
	{
		m_AttachCamera->SetVme(NULL);
		m_CurrentSlicer = NULL;

		double normal[3] = { 0,0,1 };
		mafViewSlice::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->CameraSet(CAMERA_CT);
	}

	Superclass::VmeRemove(node);
}
//----------------------------------------------------------------------------
void medViewSlicer::CameraUpdate()
//----------------------------------------------------------------------------
{
	if (m_AttachCamera)
	{
		//Camera follows the slicer
		m_AttachCamera->UpdateCameraMatrix();
	}


	if (m_CurrentSlicer)
	{
		double normal[3];
		mafViewVTK::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->GetRWI()->GetRenderers()->GetFirstRenderer()->GetActiveCamera()->GetViewPlaneNormal(normal);

		mafNode* root = m_CurrentSlicer->GetRoot();
		for (auto& Inode : *root)
		{
			if (Inode.IsA("mafVMESurface") || Inode.IsA("mafVMESurfaceParametric"))
			{
				if (auto PipeSliceViewSurface = mafPipeSurfaceSlice::SafeDownCast(mafViewVTK::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->GetNodePipe(&Inode)))
				{
					double center[3], surfaceOriginTranslated[3];
					/* mafVMESurface *surface = mafVMESurface::SafeDownCast(Inode);
					 surface->GetSurfaceOutput()->GetVTKData()->GetCenter(center);*/


					double b[6];
					m_CurrentSlicer->GetOutput()->GetBounds(b);
					center[0] = (b[1] + b[0]) / 2;
					center[1] = (b[3] + b[2]) / 2;
					center[2] = (b[5] + b[4]) / 2;

					surfaceOriginTranslated[0] = center[0] + normal[0] * 0.1;
					surfaceOriginTranslated[1] = center[1] + normal[1] * 0.1;
					surfaceOriginTranslated[2] = center[2] + normal[2] * 0.1;

					PipeSliceViewSurface->SetSlice(surfaceOriginTranslated);
					PipeSliceViewSurface->SetNormal(normal);
				}
			}
		}
		mafViewVTK::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->CameraReset(m_CurrentSlicer);
	}

	for (auto& childView : m_ChildViewList)
		childView->CameraUpdate();
}
//----------------------------------------------------------------------------
void medViewSlicer::CreateGuiView()
//----------------------------------------------------------------------------
{
	m_GuiView = new mafGUI(this);

	m_LutSlider = new mafGUILutSlider(m_GuiView, -1, wxPoint(0, 0), wxSize(500, 24));
	m_LutSlider->SetListener(this);
	m_LutSlider->SetSize(500, 24);
	m_LutSlider->SetMinSize(wxSize(500, 24));
	EnableWidgets(m_CurrentVolume != NULL);
	m_GuiView->Add(m_LutSlider);
	m_GuiView->Reparent(m_Win);
}
//----------------------------------------------------------------------------
void medViewSlicer::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	if (auto gui = AccessGUI())
	{
		gui->Enable(ID_LUT_CHOOSER, enable);
		gui->FitGui();
		UpdateGUI();
	}

	//m_LutSlider->Enable(m_CurrentSlicer != NULL);
	m_LutSlider->Enable(enable);

}
//-------------------------------------------------------------------------
int medViewSlicer::GetNodeStatusI(mafNode* vme)
//-------------------------------------------------------------------------
{
	mafSceneNode* n = NULL;
	mafSceneGraph* sgArb = mafViewVTK::StaticDownCast(m_ChildViewList[ARBITRARY_VIEW].get())->GetSceneGraph();
	mafSceneGraph* sgSlice = mafViewVTK::StaticDownCast(m_ChildViewList[SLICE_VIEW].get())->GetSceneGraph();

	if (sgArb != NULL)
	{
		n = sgArb->Vme2Node(vme);
		if (mafVME::StaticDownCast(vme)->GetOutput()->IsA("mafVMEOutputVolume") ||
			vme->IsMAFType(mafVMESurface) ||
			vme->IsMAFType(mafVMESurfaceParametric))
		{
			if (n != NULL)
			{
				n->m_PipeCreatable = true;
			}
		}
		else if (vme->IsMAFType(mafVMESlicer))
		{
			if (n != NULL)
			{
				n->m_PipeCreatable = true;
				n->m_Mutex = true;
			}
		}
	}
	return sgArb ? sgArb->GetNodeStatus(vme) : NODE_NON_VISIBLE;
}

//-------------------------------------------------------------------------
void medViewSlicer::UpdateWindowing(bool enable, mafNode* node)
//-------------------------------------------------------------------------
{
	EnableWidgets(enable);

	//Windowing can be applied on Slicers or on Images
	mafVMESlicer* Slicer = NULL;
	mafVMEImage* Image = NULL;

	mafVME* Vme = mafVME::SafeDownCast(node);

	if (Vme->IsA("mafVMESlicer")) {
		Slicer = mafVMESlicer::SafeDownCast(node);
	}
	else if (Vme->IsA("mafVMEImage")) {
		Image = mafVMEImage::SafeDownCast(node);
	}

	if (Slicer) {
		if (enable)
		{
			SlicerWindowing(Slicer);
		}
		else
		{
			m_LutSlider->SetRange(-100, 100);
			m_LutSlider->SetSubRange(-100, 100);
		}
	}
	else if (Image) {
		if (enable)
		{
			ImageWindowing(Image);
		}
		else
		{
			m_LutSlider->SetRange(-100, 100);
			m_LutSlider->SetSubRange(-100, 100);
		}
	}

}


//-------------------------------------------------------------------------
bool medViewSlicer::ActivateWindowing(mafNode* node)
//-------------------------------------------------------------------------
{
	bool conditions = false;
	bool nodeHasPipe = false;

	auto Vme = mafVME::SafeDownCast(node);
	Vme->Update();

	if (Vme->IsA("mafVMESlicer") && m_CurrentSlicer) {

		auto slicer = mafVMESlicer::SafeDownCast(node);
		if (auto vol = mafVMEVolumeGray::SafeDownCast(m_CurrentSlicer->GetSlicedVMELink()))
		{
			conditions = true;
		}
		conditions = conditions && m_CurrentVolume;
	}

	else if (node->IsA("mafVMEImage")) {

		conditions = true;

		for (auto& childView : m_ChildViewList) {

			auto pipe = mafPipeImage3D::StaticDownCast(childView->GetNodePipe(node));
			conditions = (conditions && (pipe && pipe->IsGrayImage()));
		}
		//conditions = conditions & m_CurrentImage;
	}

	return conditions;
}


//-------------------------------------------------------------------------
void medViewSlicer::SlicerWindowing(mafVMESlicer* slicer)
//-------------------------------------------------------------------------
{

	mafVMEVolumeGray* vol = mafVMEVolumeGray::SafeDownCast(
		slicer->GetSlicedVMELink());
	if (vol)
	{
		double sr[2];
		vol->GetOutput()->GetVTKData()->GetScalarRange(sr);

		auto currentSurfaceMaterial = m_CurrentSlicer->GetMaterial();
		m_ColorLUT = m_CurrentSlicer->GetMaterial()->m_ColorLut;
		m_CurrentSlicer->GetMaterial()->UpdateProp();
		m_ColorLUT->SetTableRange(sr[0], sr[1]);

		if (m_LutWidget)
		{
			m_LutWidget->SetLut(m_ColorLUT);
			if (m_LutSlider)
			{
				m_LutSlider->SetRange((long)sr[0], (long)sr[1]);
				m_LutSlider->SetSubRange((long)sr[0], (long)sr[1]);
			}
		}

	}
}