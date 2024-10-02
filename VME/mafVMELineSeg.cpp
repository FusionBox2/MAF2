/*=========================================================================

 Program: MAF2
 Module: mafVMESurfaceParametric
 Authors: Roberto Mucci , Stefano Perticoni
 
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
#include "wx/busyinfo.h"
#include "mafVMELineSeg.h"
#include "mafVMEOutputLineSeg.h"
#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
//#include "mafEventSender.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
//#include "mafVMEOutputSurface.h"
#include "mafVMEOutputPolyline.h"
#include "mafDataPipeCustom.h"
#include "wx/string.h"
#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkLine.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
//#include "vtkConeSource.h"
//#include "vtkCylinderSource.h"
//#include "vtkPlaneSource.h"
//#include "vtkCubeSource.h"
//#include "vtkSphereSource.h"
#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLineSource.h"
#include "vtkAppendPolyData.h"
#include "vtkCellData.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPolyLine.h"
#include "wx/busyinfo.h"
#include "vtkPoints.h"
#include <vector>

const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MAF_ID_IMP(mafVMESurfaceParametric::CHANGE_PARAMETER);   // Event rised by change parameter 

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMELineSeg);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMELineSeg::mafVMELineSeg()
//-------------------------------------------------------------------------
{

	

 // m_GeometryType = PARAMETRIC_LINE;
	


	mafNEW(m_Transform);
	//mafVMEOutputPolyline *output=mafVMEOutputPolyline::New(); // an output with no data
	mafVMEOutputLineSeg *output = mafVMEOutputLineSeg::New();
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();
	vtkNEW(pts);
	//vtkNEW(gon);
	//vtkNEW(lineSource1);
	
	//gon->AddInput(lineSource1->GetOutput());
	
	//vtkNEW(polyLine);
	vtkNEW(m_PolyData);
	vtkNEW(cellArray);

	
	//m_PolyData->DeepCopy(gon->GetOutput());




	
  // attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInputData(m_PolyData);
	
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
mafVMELineSeg::~mafVMELineSeg()
//-------------------------------------------------------------------------
{
  vtkDEL(m_PolyData);
  mafDEL(m_Transform);
  vtkDEL(pts);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
std::shared_ptr<mmaMaterial> mafVMELineSeg::GetMaterial()
//-------------------------------------------------------------------------
{
	auto material = mmaMaterial::SafeDownCast(GetAttribute(_R("MaterialAttributes")));
	if (!material)
	{
		material = mmaMaterial::NewSPtr();
		SetAttribute(_R("MaterialAttributes"), material);
	}
	return material;
}

//-------------------------------------------------------------------------
int mafVMELineSeg::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
	  mafVMELineSeg *vmeParametricSurface = mafVMELineSeg::SafeDownCast(a);
    m_Transform->SetMatrix(vmeParametricSurface->m_Transform->GetMatrix());
 
	
	this->setPoints( vmeParametricSurface->getPoints());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInputData(m_PolyData);
    }
    InternalUpdate();
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMELineSeg::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
	  if (
		  m_Transform->GetMatrix() == ((mafVMELineSeg *)vme)->m_Transform->GetMatrix() &&

		  pts->GetNumberOfPoints() == ((mafVMELineSeg *)vme)->getPoints()->GetNumberOfPoints()
		 

		  
		   )
	  {
		  for (int i = 0; i < pts->GetNumberOfPoints(); i++)
		  {
			  if ((pts->GetPoint(i))[0] == ((mafVMELineSeg *)vme)->getPoints()->GetPoint(i)[0] &&
				  (pts->GetPoint(i))[1] == ((mafVMELineSeg *)vme)->getPoints()->GetPoint(i)[1] &&
				  (pts->GetPoint(i))[2] == ((mafVMELineSeg *)vme)->getPoints()->GetPoint(i)[2])
				  ;
			  else
				  return false;

		  }
		  
		  
		  ret = true;
    }
  }
 return ret;
}
//-------------------------------------------------------------------------
mafVMEOutputPolyline *mafVMELineSeg::GetPolylineOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMELineSeg::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMELineSeg::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
void mafVMELineSeg::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
  mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}
//-------------------------------------------------------------------------
mafGUI* mafVMELineSeg::CreateGui()
//-------------------------------------------------------------------------
{
  mafVME::CreateGui();
  if(m_Gui)
  {
    CreateGuiLine();
    m_Gui->Divider(2);

   
    m_Gui->FitGui();
    
	//EnableParametricSurfaceGui( m_GeometryType);
    m_Gui->Update();
  }

  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMELineSeg::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{

  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_GEOMETRY_TYPE:
      {  
       // EnableParametricSurfaceGui(m_GeometryType);
        m_Gui->Update();
        InternalUpdate();
        m_Gui->FitGui();
      }

      
      case CHANGE_VALUE_LINE:

      {
        InternalUpdate();
        e->SetId(CAMERA_UPDATE);
        ForwardUpEvent(e);
      }
      break;
      
      default:
        mafVME::OnEvent(maf_event);
    }
  }
  
  else
  {
    Superclass::OnEvent(maf_event);
  }
}

//-----------------------------------------------------------------------
void mafVMELineSeg::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMELineSeg::InternalUpdate()
//-----------------------------------------------------------------------
{
	

	int nbr = pts->GetNumberOfPoints();



	vtkIdType pointId[2];
	for (int i = 0; i< nbr; i++)
	{
		if (i > 0)
		{
			pointId[0] = i - 1;
			pointId[1] = i;

			cellArray->InsertNextCell(2, pointId);
		}
	}

	m_PolyData->SetPoints(pts);
	m_PolyData->SetLines(cellArray);
	//m_PolyData->Update();
}
//-----------------------------------------------------------------------
vtkPolyData* mafVMELineSeg::getVTKPolydata()
//-----------------------------------------------------------------------
{


	
	return m_PolyData;

}


void mafVMELineSeg::Update()
{

	int nbr = pts->GetNumberOfPoints();
	if (nbr>1)
	{
		SetPoint1(pts->GetPoint(0)[0], pts->GetPoint(0)[1], pts->GetPoint(0)[2]);
		SetPoint2(pts->GetPoint(nbr - 1)[0], pts->GetPoint(nbr - 1)[1], pts->GetPoint(nbr - 1)[2]);
	
	}

	InternalUpdate();

}
//-----------------------------------------------------------------------
void mafVMELineSeg::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);
  parent[_R("Transform")].SetValue(m_Transform->GetMatrix());
  parent[_R("PosPt1")].SetValue(mafToString(PosPt1, 3));
  parent[_R("PosPt2")].SetValue(mafToString(PosPt2, 3));
}

//-----------------------------------------------------------------------
void mafVMELineSeg::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
	Superclass::InternalRestore(node);
  m_Transform->SetMatrix(node[_R("Transform")].As<mafMatrix>());
  mafParseVector(node[_R("PosPt1")].As<mafString>(), PosPt1, 3);
  mafParseVector(node[_R("PosPt2")].As<mafString>(), PosPt2, 3);
}
//-------------------------------------------------------------------------
/*void mafVMELineSeg::SetGeometryType(int parametricSurfaceTypeID)
//-------------------------------------------------------------------------
{
  m_GeometryType = parametricSurfaceTypeID;
  Modified();
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
char** mafVMELineSeg::GetIcon()
//-------------------------------------------------------------------------
{
  #include "mafVMEProcedural.xpm"
  return mafVMEProcedural_xpm;
}



void mafVMELineSeg::CreateGuiLine()
{
  m_GuiLine = new mafGUI(this);
  m_GuiLine->Label(_R("Line"));
  m_GuiLine->Vector(CHANGE_VALUE_LINE, _L("Point 1"), PosPt1);
  m_GuiLine->Vector(CHANGE_VALUE_LINE, _L("Point 2"), PosPt2);
  
 /* m_Position1[0] = wxString::Format("pt1 x: %f", PosPt1[0]);
  m_Position1[1] = wxString::Format("pt1 y: %f", PosPt1[1]);
  m_Position1[2] = wxString::Format("pt1 z: %f", PosPt1[2]);

  m_GuiLine->Label("", &m_Position1[0]);
  m_GuiLine->Label("", &m_Position1[1]);
  m_GuiLine->Label("", &m_Position1[2]);

  m_Position2[0] = wxString::Format("pt2 : %f", PosPt2[0]);
  m_Position2[1] = wxString::Format("pt2 : %f", PosPt2[1]);
  m_Position2[2] = wxString::Format("pt2 : %f", PosPt2[2]);

  m_GuiLine->Label("", &m_Position2[0]);
  m_GuiLine->Label("", &m_Position2[1]);
  m_GuiLine->Label("", &m_Position2[2]);*/
  assert(m_Gui);
  m_Gui->AddGui(m_GuiLine);
}





void mafVMELineSeg::EnableGuiLine()
{
 
  m_GuiLine->Enable(CHANGE_VALUE_LINE, false);
  
  
}




void mafVMELineSeg::SetPoint1(double x1,double y1 ,double z1)
{
	PosPt1[0] = x1;
	PosPt1[1] = y1;
	PosPt1[2] = z1;
	
//	m_Position1[0] = wxString::Format("pt1 x: %f", PosPt1[0]);
//	m_Position1[1] = wxString::Format("pt1 y: %f", PosPt1[1]);
//	m_Position1[2] = wxString::Format("pt1 z: %f", PosPt1[2]);

	if (m_Gui)
	{
		m_GuiLine->Update();
		m_Gui->Update();
	}
}
void mafVMELineSeg::SetPoint2(double x1, double y1, double z1)
{

	PosPt2[0] = x1;
	PosPt2[1] = y1;
	PosPt2[2] = z1;
	
//	m_Position2[0] = wxString::Format("pt2 : %f", PosPt2[0]);
//	m_Position2[1] = wxString::Format("pt2 : %f", PosPt2[1]);
//	m_Position2[2] = wxString::Format("pt2 : %f", PosPt2[2]);

	if (m_Gui)
	{
		m_GuiLine->Update();
		m_Gui->Update();
	}
}

/*void mafVMELineSeg::EnableParametricSurfaceGui(int surfaceTypeID)
{

      EnableGuiLine();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Line gui" << std::endl;
        mafLogMessage(stringStream.str().c_str());
      }


}*/

void mafVMELineSeg::setPoints(vtkPoints* points)
{
	
	pts->Reset();
	for (int i = 0; i < points->GetNumberOfPoints(); i++)
	{
		pts->InsertNextPoint(points->GetPoint(i));
	}
	int nbr = pts->GetNumberOfPoints();
	if (nbr>1)
	{
		SetPoint1(pts->GetPoint(0)[0], pts->GetPoint(0)[1], pts->GetPoint(0)[2]);
		SetPoint2(pts->GetPoint(nbr-1)[0], pts->GetPoint(nbr-1)[1], pts->GetPoint(nbr-1)[2]);
	}
	this->Update();
}

void mafVMELineSeg::resetPoints()
{
	pts->Reset();
	this->Update();

}

vtkPoints* mafVMELineSeg::getPoints()
{
	return pts;

}