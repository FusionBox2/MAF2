/*=========================================================================

 Program: MAF2
 Module: mafOpCrop
 Authors: Matteo Giacomoni & Paolo Quadrani
 
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

#include "mafOpCrop3DSurface.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafVMEOutputSurface.h"
#include "mafGizmoROI.h"

#include "mafString.h"
#include "mafVME.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMESurface.h"
#include "vtkMAFDistanceFilter.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkProbeFilter.h"
#include "vtkExtractRectilinearGrid.h"
#include "vtkDoubleArray.h"
#include "vtkIdList.h"
#include "vtkCleanPolyData.h"
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCrop3DSurface);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCrop3DSurface::mafOpCrop3DSurface(const mafString& label, bool showShadingPlane) : Superclass(label)
//----------------------------------------------------------------------------
{

	mafNEW(m_Cloud);
	m_OpType	= OPTYPE_OP;
	m_InputPreserving = false;
	m_Canundo	= true;
  m_ShowHandles = 1;
  m_ShowROI = 1;

	m_InputRG = NULL;
	m_InputSP = NULL;
	m_InputPolyData = NULL;
  m_GizmoROI = NULL;
	m_OutputRG = NULL;
	m_OutputSP = NULL;
	m_OutputPolyData = NULL;

  m_ShowShadingPlane = showShadingPlane;
}
//----------------------------------------------------------------------------
mafOpCrop3DSurface::~mafOpCrop3DSurface()
//----------------------------------------------------------------------------
{
	vtkDEL(m_InputRG);
	vtkDEL(m_InputSP);
	vtkDEL(m_InputPolyData);
	vtkDEL(m_OutputRG);
	vtkDEL(m_OutputSP);
	vtkDEL(m_OutputPolyData);
}

//----------------------------------------------------------------------------
mafOp *mafOpCrop3DSurface::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCrop3DSurface(GetLabel(), m_ShowShadingPlane);
}
//----------------------------------------------------------------------------
bool mafOpCrop3DSurface::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
	mafEvent e(this,VIEW_SELECTED);
	mafEventMacro(e);
	return (node && (node->IsA("mafVMESurface") || (node->IsA("mafVMESurfaceParametric"))) /*&& e.GetBool()*/);
}
//----------------------------------------------------------------------------
void mafOpCrop3DSurface::OpRun()
//----------------------------------------------------------------------------
{
	//m_Input->Modified();
  mafEvent e(this,VIEW_SELECTED);
  mafEventMacro(e);

	mafVME* volume = mafVME::SafeDownCast(m_Input);
	volume->Update();
	// create gizmo roi
	if(!m_TestMode)
	{
		m_GizmoROI = new mafGizmoROI(volume, this,mafGizmoHandle::BOUNDS,NULL,m_ShowShadingPlane);
		m_GizmoROI->Show(e.GetBool());
	}

	if (volume->IsA("mafVMESurfaceParametric"))
	{
		vtkNEW(m_InputPolyData);
		m_InputPolyData->DeepCopy(((mafVMESurfaceParametric*)volume)->GetSurfaceOutput()->GetSurfaceData());
		m_InputPolyData->Update();
		vtkPolyData*	data = ((mafVMESurfaceParametric*)volume)->GetSurfaceOutput()->GetSurfaceData();
		data->GetBounds(m_InputBounds);
		if (!m_TestMode)
			m_GizmoROI->SetBounds(m_InputBounds);
		double spc[3];
		
		m_XSpacing = 1;
		m_YSpacing = 1;
		m_ZSpacing = 1;

	}
	if (volume->IsA("mafVMESurface"))
	{
		vtkNEW(m_InputPolyData);
		
		m_InputPolyData->DeepCopy(((mafVMESurface*)volume)->GetSurfaceOutput()->GetSurfaceData());
		m_InputPolyData->Update();
		vtkPolyData*	data = ((mafVMESurface*)volume)->GetSurfaceOutput()->GetSurfaceData();
		data->GetBounds(m_InputBounds);
		if (!m_TestMode)
			m_GizmoROI->SetBounds(m_InputBounds);
		double spc[3];

		m_XSpacing = 1;
		m_YSpacing = 1;
		m_ZSpacing = 1;

	}

	/*if (volume->GetOutput()->GetVTKData()->IsA("vtkStructuredPoints"))
  {
		vtkNEW(m_InputSP);
		m_InputSP->DeepCopy(vtkStructuredPoints::SafeDownCast(volume->GetOutput()->GetVTKData()));
    m_InputSP->Update();
		m_InputSP->GetBounds(m_InputBounds);	
		if(!m_TestMode)
			m_GizmoROI->SetBounds(m_InputBounds);
    double spc[3];	
    m_InputSP->GetSpacing(spc);
    m_XSpacing = spc[0];
    m_YSpacing = spc[1];
    m_ZSpacing = spc[2];
  }	
	else if (volume->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
	{
		vtkNEW(m_InputRG);
    m_InputRG->DeepCopy(vtkRectilinearGrid::SafeDownCast(volume->GetOutput()->GetVTKData()));		
    m_InputRG->Update();
		m_InputRG->GetBounds(m_InputBounds);
		if(!m_TestMode)
			m_GizmoROI->SetBounds(m_InputBounds);
	}*/








	if(!m_TestMode)
	{
		CreateGui();
		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
}
//----------------------------------------------------------------------------
void mafOpCrop3DSurface::Crop()
//----------------------------------------------------------------------------
{
	if (!m_TestMode)
		m_GizmoROI->GetBounds(m_CroppingBoxBounds);

	//m_Input->Modified();
	mafVME *volume = mafVME::SafeDownCast(m_Input);

	mafVMEOutput *output = volume->GetOutput();

	wxBusyInfo wait0(_("please wait, cropping..."));
	Sleep(1500);
	if (output->GetVTKData()->IsA("vtkRectilinearGrid"))
	{
		vtkRectilinearGrid *rgData = vtkRectilinearGrid::SafeDownCast(output->GetVTKData());

		// get cropping bounds
		double gizmoBounds[6] = { 0, 0, 0, 0, 0, 0 };
		if (!m_TestMode)
			m_GizmoROI->GetBounds(gizmoBounds);
		else
		{
			for (int i = 0; i < 6; i++)
				gizmoBounds[i] = m_CroppingBoxBounds[i];
		}

		// convert from bounds to index
		int boundsIndexArray[6] = { 0, 0, 0, 0, 0, 0 };

		vtkDataArray *coordArray[3] = { NULL, NULL, NULL };
		coordArray[0] = rgData->GetXCoordinates();
		coordArray[1] = rgData->GetYCoordinates();
		coordArray[2] = rgData->GetZCoordinates();

		// for each coordinate array
		for (int numArray = 0; numArray < 3; numArray++)
		{
			int coordId = 0;
			int minId = 0, maxId = 0;

			while (coordArray[numArray]->GetComponent(coordId, 0) < gizmoBounds[2 * numArray])
			{
				minId = coordId + 1;
				coordId++;
			}
			//coordId++;
			while (coordArray[numArray]->GetComponent(coordId, 0) < gizmoBounds[2 * numArray + 1])
			{
				maxId = coordId + 1;
				coordId++;
			}

			boundsIndexArray[2 * numArray] = minId;
			boundsIndexArray[2 * numArray + 1] = maxId;
		}
		vtkExtractRectilinearGrid *extractRG = vtkExtractRectilinearGrid::New();
		extractRG->SetInput(rgData);
		extractRG->SetVOI(boundsIndexArray);
		extractRG->Update();

		vtkNEW(m_OutputRG);
		m_OutputRG->DeepCopy(extractRG->GetOutput());

		extractRG->Delete();
		extractRG = NULL;
	}
	else if (output->GetVTKData()->IsA("vtkStructuredPoints"))
	{
		int voi_dim[6];
		int original_vol_dim[6];
		m_InputSP->GetExtent(original_vol_dim);

		double xBoundLength, yBoundLength, zBoundLength;
		xBoundLength = fabs(m_InputBounds[1] - m_InputBounds[0]);
		yBoundLength = fabs(m_InputBounds[3] - m_InputBounds[2]);
		zBoundLength = fabs(m_InputBounds[5] - m_InputBounds[4]);

		double tolerance = 1e-3; //base tolerance


		if (fabs(m_CroppingBoxBounds[0] - m_InputBounds[0]) / xBoundLength < tolerance)
		{
			voi_dim[0] = original_vol_dim[0];
		}
		else
		{
			voi_dim[0] = ceil((m_CroppingBoxBounds[0] - m_InputBounds[0]) / (m_XSpacing));
		}

		if (fabs(m_CroppingBoxBounds[1] - m_InputBounds[1]) / xBoundLength < tolerance)
		{
			voi_dim[1] = original_vol_dim[1];
		}
		else
		{
			voi_dim[1] = floor((m_CroppingBoxBounds[1] - m_InputBounds[0]) / m_XSpacing);
		}

		if (fabs(m_CroppingBoxBounds[2] - m_InputBounds[2]) / yBoundLength < tolerance)
		{
			voi_dim[2] = original_vol_dim[2];
		}
		else
		{
			voi_dim[2] = ceil((m_CroppingBoxBounds[2] - m_InputBounds[2]) / (m_YSpacing));
		}

		if (fabs(m_CroppingBoxBounds[3] - m_InputBounds[3]) / yBoundLength < tolerance)
		{
			voi_dim[3] = original_vol_dim[3];
		}
		else
		{
			voi_dim[3] = floor((m_CroppingBoxBounds[3] - m_InputBounds[2]) / m_YSpacing);
		}

		if (fabs(m_CroppingBoxBounds[4] - m_InputBounds[4]) / zBoundLength < tolerance)
		{
			voi_dim[4] = original_vol_dim[4];
		}
		else
		{
			voi_dim[4] = ceil((m_CroppingBoxBounds[4] - m_InputBounds[4]) / (m_ZSpacing));
		}

		if (fabs(m_CroppingBoxBounds[5] - m_InputBounds[5]) / zBoundLength < tolerance)
		{
			voi_dim[5] = original_vol_dim[5];
		}
		else
		{
			voi_dim[5] = floor((m_CroppingBoxBounds[5] - m_InputBounds[4]) / m_ZSpacing);
		}


		double in_org[3];
		m_InputSP->GetOrigin(in_org);

		// using the vtkMAFSmartPointer allows you to don't mind the object Delete
		vtkMAFSmartPointer<vtkStructuredPoints> v_esp;
		v_esp->SetOrigin(in_org[0] + voi_dim[0] * m_XSpacing,
			in_org[1] + voi_dim[2] * m_YSpacing,
			in_org[2] + voi_dim[4] * m_ZSpacing);
		v_esp->SetSpacing(m_XSpacing, m_YSpacing, m_ZSpacing);
		v_esp->SetDimensions(voi_dim[1] - voi_dim[0] + 1,
			voi_dim[3] - voi_dim[2] + 1,
			voi_dim[5] - voi_dim[4] + 1);
		v_esp->Modified();

		// I'm using probe filter instead of ExtractVOI (see why before...)
		if (!m_TestMode)
		{
			wxBusyInfo wait(_("please wait, cropping..."));
		}

		vtkMAFSmartPointer<vtkProbeFilter> probeFilter;
		probeFilter->SetInput(v_esp);
		probeFilter->SetSource(m_InputSP);
		probeFilter->Update();

		vtkNEW(m_OutputSP);
		m_OutputSP->DeepCopy(probeFilter->GetOutput());
	}
	if (output->GetVTKData()->IsA("vtkPolyData"))
	{
		vtkCleanPolyData *cleaner = vtkCleanPolyData::New();
		vtkPolyData *rgData = vtkPolyData::SafeDownCast(output->GetVTKData());

		// get cropping bounds
		double gizmoBounds[6] = { 0, 0, 0, 0, 0, 0 };
		if (!m_TestMode)
			m_GizmoROI->GetBounds(gizmoBounds);
		else
		{
			for (int i = 0; i < 6; i++)
				gizmoBounds[i] = m_CroppingBoxBounds[i];
		}

		rgData->DeleteCells();
		
		
		
		vtkPoints* pts = vtkPoints::New();
		

		m_Cloud->Open();
		m_Cloud->SetName(_L("SelectedPoints"));
		m_Cloud->SetRadius(0.2);
		m_Cloud->ReparentTo(m_Input);
		m_Cloud->Open();
		//m_Cloud->RemoveAllChildren();
		m_Cloud->Update();
		for (int i = 0; i < rgData->GetNumberOfPoints(); i++)
		{
			if ((rgData->GetPoint(i)[0]<gizmoBounds[0]) || (rgData->GetPoint(i)[0]>gizmoBounds[1]) || (rgData->GetPoint(i)[1]<gizmoBounds[2]) || (rgData->GetPoint(i)[1]>gizmoBounds[3]) || (rgData->GetPoint(i)[2] < gizmoBounds[4]) || (rgData->GetPoint(i)[2] > gizmoBounds[5]))
			{
				;
			}
			else
			{
				m_Cloud->AppendLandmark(rgData->GetPoint(i)[0], rgData->GetPoint(i)[1], rgData->GetPoint(i)[2], _R("pt"), false);
				pts->InsertNextPoint(rgData->GetPoint(i)[0], rgData->GetPoint(i)[1], rgData->GetPoint(i)[2]);
			}

		}

		m_Cloud->Update();

		
    	vtkNEW(m_OutputPolyData);

		rgData->Delete();
		rgData = NULL;
	

	}

}
//----------------------------------------------------------------------------
void mafOpCrop3DSurface::OpDo()
//----------------------------------------------------------------------------
{
	wxBusyInfo wait0(_("please wait, do..."));
	Sleep(1500);
	if (m_OutputSP)
	{

		
		((mafVMEVolume*)m_Input)->SetData(m_OutputSP, ((mafVME*)m_Input)->GetTimeStamp());
		((mafVMEVolume*)m_Input)->GetOutput()->Update();
		((mafVMEVolume*)m_Input)->Update();
	}
	else if (m_OutputRG)
	{
		
		((mafVMEVolume*)m_Input)->SetData(m_OutputRG, ((mafVME*)m_Input)->GetTimeStamp());
		((mafVMEVolume*)m_Input)->GetOutput()->Update();
		((mafVMEVolume*)m_Input)->Update();
	}
		

	if (m_OutputPolyData)
	{

		
		assert(m_Cloud);

		/*((mafVMESurface*)m_Input)->SetData(m_OutputPolyData, ((mafVME*)m_Input)->GetTimeStamp());
		((mafVMESurface*)m_Input)->GetOutput()->Update();
		((mafVMESurface*)m_Input)->Update();*/
	}

	
	
    // bug# 2628: gizmos do not update after cropping (workaround code)
//	mafEventMacro(mafEvent(this,VME_SHOW,m_Input,false));
//	mafEventMacro(mafEvent(this,VME_SHOW,m_Input,true));
	///////

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	wxBusyInfo wait(_("please wait, do end"));
	Sleep(1500);
}
//----------------------------------------------------------------------------
void mafOpCrop3DSurface::OpUndo()
//----------------------------------------------------------------------------
{
	if (m_InputSP)
	{
		((mafVMEVolume*)m_Input)->SetData(m_InputSP, ((mafVME*)m_Input)->GetTimeStamp());
		((mafVMEVolume*)m_Input)->GetOutput()->Update();
		((mafVMEVolume*)m_Input)->Update();

	}
	else if (m_InputRG)
	{
		((mafVMEVolume*)m_Input)->SetData(m_InputRG, ((mafVME*)m_Input)->GetTimeStamp());
		((mafVMEVolume*)m_Input)->GetOutput()->Update();
		((mafVMEVolume*)m_Input)->Update();
	}
	if (m_InputPolyData)
	{
		((mafVMESurface*)m_Input)->SetData(m_OutputPolyData, ((mafVME*)m_Input)->GetTimeStamp());
		((mafVMESurface*)m_Input)->GetOutput()->Update();
		((mafVMESurface*)m_Input)->Update();
	}
	

	// bug# 2628: gizmos do not update after cropping (workaround code)
	mafEventMacro(mafEvent(this,VME_SHOW,m_Input,false));
	mafEventMacro(mafEvent(this,VME_SHOW,m_Input,true));
	///////

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpCrop3DSurface::UpdateGui()
//----------------------------------------------------------------------------
{
  double bounds[6];
  m_GizmoROI->GetBounds(bounds);
  m_XminXmax[0] = bounds[0];
  m_XminXmax[1] = bounds[1];
  m_YminYmax[0] = bounds[2];
  m_YminYmax[1] = bounds[3];
  m_ZminZmax[0] = bounds[4];
  m_ZminZmax[1] = bounds[5];
  m_Gui->Update();
}

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum CROP_WIDGET_ID
{
  ID_SHOW_HANDLES,
  ID_SHOW_ROI,
  ID_FIRST = MINID,	
  ID_CROP_DIR_X,
  ID_CROP_DIR_Y,
  ID_CROP_DIR_Z,
  ID_RESET_CROPPING_AREA,
  ID_HELP,
};

//----------------------------------------------------------------------------
void mafOpCrop3DSurface::CreateGui()
//----------------------------------------------------------------------------
{
	double bounds[6];
	mafVME *volume = mafVME::SafeDownCast(m_Input);
	volume->GetOutput()->GetVTKData()->GetBounds(bounds);
	m_XminXmax[0] = bounds[0];
	m_XminXmax[1] = bounds[1];
	m_YminYmax[0] = bounds[2];
	m_YminYmax[1] = bounds[3];
	m_ZminZmax[0] = bounds[4];
	m_ZminZmax[1] = bounds[5];

	m_Gui = new mafGUI(this);
	mafEvent buildHelpGui;
	buildHelpGui.SetSender(this);
	buildHelpGui.SetId(GET_BUILD_HELP_GUI);
	mafEventMacro(buildHelpGui);

	if (buildHelpGui.GetArg() == true)
	{
		m_Gui->Button(ID_HELP, _R("Help"),_R(""));	
	}

  m_Gui->Label(_R(""));
  m_Gui->Bool(ID_SHOW_HANDLES, _L("handles"), &m_ShowHandles, 0, _L("toggle gizmo handles visibility"));
  m_Gui->Bool(ID_SHOW_ROI, _R("ROI"), &m_ShowROI, 0, _L("toggle region of interest visibility"));
	m_Gui->Label(_R(""));
	m_Gui->VectorN(ID_CROP_DIR_X, _L("range x"), m_XminXmax, 2, bounds[0], bounds[1]);
	m_Gui->VectorN(ID_CROP_DIR_Y, _L("range y"), m_YminYmax, 2, bounds[2], bounds[3]);
	m_Gui->VectorN(ID_CROP_DIR_Z, _L("range z"), m_ZminZmax, 2, bounds[4], bounds[5]);

  m_Gui->Button(ID_RESET_CROPPING_AREA, _L("reset"), _R(""), _L("reset the cropping area"));
	m_Gui->Label(_R(""));
	m_Gui->OkCancel();

	m_Gui->Divider();

  ShowGui();
}

//----------------------------------------------------------------------------
void mafOpCrop3DSurface::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  double bb[6];
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId())
		{

			case ID_HELP:
			{
				mafEvent helpEvent;
				helpEvent.SetSender(this);
				mafString operationLabel = GetLabel();
				helpEvent.SetString(&operationLabel);
				helpEvent.SetId(OPEN_HELP_PAGE);
				mafEventMacro(helpEvent);
			}
			break;

			case ID_SHOW_HANDLES:
			{
				m_GizmoROI->ShowHandles(m_ShowHandles != 0);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
			case ID_SHOW_ROI:      
			{
				m_GizmoROI->ShowROI(m_ShowROI != 0);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;
			case ID_CROP_DIR_X:
				m_GizmoROI->GetBounds(bb);
				bb[0] = m_XminXmax[0];
				bb[1] = m_XminXmax[1];
				m_Gui->Update();
				m_GizmoROI->SetBounds(bb);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;
			case ID_CROP_DIR_Y:
				m_GizmoROI->GetBounds(bb);
				bb[2] = m_YminYmax[0];
				bb[3] = m_YminYmax[1];
				m_Gui->Update();
				m_GizmoROI->SetBounds(bb);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;
			case ID_CROP_DIR_Z:
				m_GizmoROI->GetBounds(bb);
				bb[4] = m_ZminZmax[0];
				bb[5] = m_ZminZmax[1];
				m_Gui->Update();
				m_GizmoROI->SetBounds(bb);
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;
			case ID_RESET_CROPPING_AREA:
				m_GizmoROI->Reset();
				UpdateGui();
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			break;    
			case wxOK:
				Crop();
				OpStop(OP_RUN_OK);
			break;
			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
			break;
			case ID_TRANSFORM:
				UpdateGui();
			break;
			default:
				mafEventMacro(*e);
			break;
		}	
	}
}

//----------------------------------------------------------------------------
void mafOpCrop3DSurface::OpStop(int result)
//----------------------------------------------------------------------------
{  
  HideGui();
  m_GizmoROI->Show(false);

	cppDEL(m_GizmoROI);
	m_GizmoROI = NULL;
  mafEventMacro(mafEvent(this,result));  
}
//----------------------------------------------------------------------------
void mafOpCrop3DSurface::SetCroppingBoxBounds(double bounds[])
//----------------------------------------------------------------------------
{
	for(int i = 0; i < 6; i++)
		m_CroppingBoxBounds[i] = bounds[i];
}