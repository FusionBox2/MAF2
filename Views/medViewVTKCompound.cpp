/*=========================================================================

 Program: MAF2Medical
 Module: medViewVTKCompound
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

#include "medViewVTKCompound.h"
#include "mafViewImage.h"
#include "mafViewVTK.h"
#include "mafRWI.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafPipeImage3D.h"
#include "mafGUIViewWin.h"
#include "mafGUI.h"
#include "mafGUILutSlider.h"
#include "mafGUILutSwatch.h"
#include "mafNode.h"
#include "mafVMEImage.h"
#include "mafVMEOutputVolume.h"
#include "mafGUIFloatSlider.h"
#include "mafVMEOutputImage.h"
#include "mafVME.h"

#include "medPipeVectorFieldMapWithArrows.h"

#include "vtkLookupTable.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkTexture.h"

#include "mmaVolumeMaterial.h"


//----------------------------------------------------------------------------
// constants:
//----------------------------------------------------------------------------

enum SUBVIEW_ID
{
	ID_VIEW_VTK = 0,
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(medViewVTKCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medViewVTKCompound::medViewVTKCompound(const mafString& label, int num_row, int num_col)
	: medViewCompoundWindowing(label, num_row, num_col)
	//----------------------------------------------------------------------------
{
	m_ViewVTK = NULL;
}
//----------------------------------------------------------------------------
medViewVTKCompound::~medViewVTKCompound()
//----------------------------------------------------------------------------
{
	/*m_ColorLUT = NULL;
	cppDEL(m_LutWidget);
	cppDEL(m_LutSlider);*/
}
//----------------------------------------------------------------------------
mafView* medViewVTKCompound::Copy(mafBaseEventHandler* Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
	m_LightCopyEnabled = lightCopyEnabled;
	medViewVTKCompound* v = new medViewVTKCompound(GetLabel(), m_ViewRowNum, m_ViewColNum);
	v->SetListener(Listener);
	v->m_Id = m_Id;
	for (auto& pluggedView : m_PluggedChildViewList)
	{
		v->m_PluggedChildViewList.emplace_back(pluggedView->Copy(this));
	}
	v->Create();
	return v;
}

//-------------------------------------------------------------------------
mafGUI* medViewVTKCompound::CreateGui()
//-------------------------------------------------------------------------
{
	assert(!AccessGUI());
	auto gui = new mafGUI(this);
	if (mafViewVTK::SafeDownCast(m_ChildViewList[ID_VIEW_VTK].get()) && mafViewVTK::SafeDownCast(m_ChildViewList[ID_VIEW_VTK].get())->GetGui())
	{
		gui->AddGui(mafViewVTK::StaticDownCast(m_ChildViewList[ID_VIEW_VTK].get())->GetGui());
	}
	m_LutWidget = gui->Lut(ID_LUT_CHOOSER, _R("lut"), m_ColorLUT);
	m_LutWidget->Enable(false);
	gui->Divider();
	gui->FitGui();
	gui->Update();
	return gui;
}
//-------------------------------------------------------------------------
void medViewVTKCompound::PackageView()
//-------------------------------------------------------------------------
{
	assert(m_ViewVTK);
	PlugChildView(std::move(m_ViewVTK));
}

//-------------------------------------------------------------------------
bool medViewVTKCompound::ActivateWindowing(mafNode* node)
//-------------------------------------------------------------------------
{
	bool conditions = false;

	if (mafVME::StaticDownCast(node)->IsA("mafVMEImage")) {

		conditions = true;

		for (auto& childView : m_ChildViewList)
		{
			//m_ChildViewList[i]->VmeSelect(node, select);

			auto pipe = mafPipeImage3D::StaticDownCast(childView->GetNodePipe(node));
			conditions = (conditions && (pipe && pipe->IsGrayImage()));
		}
	}

	return conditions;
}


//ACTUALLY NEVER USED
//-------------------------------------------------------------------------
void medViewVTKCompound::SetExternalView(std::unique_ptr<mafViewVTK> childView)
//-------------------------------------------------------------------------
{
	if (m_ViewVTK == nullptr) {
		m_ViewVTK = std::move(childView);
	}
}
//-------------------------------------------------------------------------
void medViewVTKCompound::CameraUpdate()
//-------------------------------------------------------------------------
{
	// Added patch to update scalar and vector attributes while changing timeframe with the timebar 
	// (valid only for medPipeVectorFieldMapWithArrows).
	mafSceneGraph* sg = GetSceneGraph();

	// Do it for each node attached to the view
	for (mafSceneNode* node = sg->GetNodeList(); node; node = node->m_Next)
	{
		if (node->m_Vme)
		{

			auto vme = mafVME::StaticDownCast(node->m_Vme);

			assert(vme);
			auto maf_pipe = GetNodePipe(vme);
			if (maf_pipe && strcmp(maf_pipe->GetTypeName(), "medPipeVectorFieldMapWithArrows") == 0)
			{
				auto pipe = medPipeVectorFieldMapWithArrows::StaticDownCast(maf_pipe);
				if (pipe) {
					pipe->UpdateVTKPipe();
				}
			}
		}
	}

	Superclass::CameraUpdate();
}