/*=========================================================================

 Program: MAF2Medical
 Module: mafViewGlobalSliceCompound
 Authors: Eleonora Mambrini

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

#include "mafViewGlobalSliceCompound.h"
#include "mafViewGlobalSlice.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafNode.h"
#include "mafPipeVolumeSlice.h"
#include "mafPipeSurfaceSlice.h"
#include "mafVMEVolumeGray.h"
#include "mmaVolumeMaterial.h"
#include "mafGUIFloatSlider.h"
#include "mmaVolumeMaterial.h"

#include "vtkLookupTable.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
	ID_VIEW_GLOBAL_SLICE = 0,
};

enum VIEW_WIDGET_ID
{
	ID_FIRST = MINID,
	ID_HELP,
	ID_LAST
};
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewGlobalSliceCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewGlobalSliceCompound::mafViewGlobalSliceCompound(const mafString& label, int num_row, int num_col)
	: medViewCompoundWindowing(label, num_row, num_col)
	//----------------------------------------------------------------------------
{
	/*m_LutWidget = NULL;
	m_LutSlider = NULL;
	m_ColorLUT = NULL;*/
}
//----------------------------------------------------------------------------
mafViewGlobalSliceCompound::~mafViewGlobalSliceCompound()
//----------------------------------------------------------------------------
{
	m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);
}
//----------------------------------------------------------------------------
mafView* mafViewGlobalSliceCompound::Copy(mafBaseEventHandler* Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
	m_LightCopyEnabled = lightCopyEnabled;
	mafViewGlobalSliceCompound* v = new mafViewGlobalSliceCompound(GetLabel(), m_ViewRowNum, m_ViewColNum);
	v->SetListener(Listener);
	v->m_Id = m_Id;
	for (int i = 0; i < m_PluggedChildViewList.size(); i++)
	{
		v->m_PluggedChildViewList.emplace_back(m_PluggedChildViewList[i]->Copy(this));
	}
	v->Create();
	return v;
}

//-------------------------------------------------------------------------
mafGUI* mafViewGlobalSliceCompound::CreateGui()
//-------------------------------------------------------------------------
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

	gui->AddGui(mafViewGlobalSlice::StaticDownCast(m_ChildViewList[ID_VIEW_GLOBAL_SLICE].get())->GetGui());
	m_LutWidget = gui->Lut(ID_LUT_CHOOSER, _R("lut"), m_ColorLUT);
	m_LutWidget->Enable(false);
	gui->Divider(0);
	gui->FitGui();
	gui->Update();
	return gui;
}
//-------------------------------------------------------------------------
void mafViewGlobalSliceCompound::PackageView()
//-------------------------------------------------------------------------
{
	auto ViewGlobalSlice = std::make_unique<mafViewGlobalSlice>(_R(""), CAMERA_OS_P);
	ViewGlobalSlice->PlugVisualPipe(_R("mafVMESurface"), _R("mafPipeSurfaceSlice"));
	ViewGlobalSlice->PlugVisualPipe(_R("mafVMESurfaceParametric"), _R("mafPipeSurfaceSlice"));
	ViewGlobalSlice->PlugVisualPipe(_R("mafVMEVolumeGray"), _R("mafPipeVolumeSlice"));
	ViewGlobalSlice->PlugVisualPipe(_R("medVMELabeledVolume"), _R("mafPipeVolumeSlice"));
	ViewGlobalSlice->PlugVisualPipe(_R("mafVMEMesh"), _R("mafPipeMeshSlice"));
	ViewGlobalSlice->PlugVisualPipe(_R("medVMEAnalog"), _R("mafPipeBox"), NON_VISIBLE);
	ViewGlobalSlice->PlugVisualPipe(_R("mafVMELandmark"), _R("mafPipeSurfaceSlice"));
	ViewGlobalSlice->PlugVisualPipe(_R("mafVMELandmarkCloud"), _R("mafPipeSurfaceSlice"));

	PlugChildView(std::move(ViewGlobalSlice));
}

//----------------------------------------------------------------------------
void mafViewGlobalSliceCompound::OnEvent(mafEventBase* maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent* e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{

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

		default:
			InvokeEvent(*maf_event);
		}
	}
	else
	{
		InvokeEvent(*maf_event);
	}
}
