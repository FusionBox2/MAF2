/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEArrow.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "mafVMEArrow.h"

#include "mafGUI.h"
#include "mafIndent.h"
#include "mmaMaterial.h"
#include "mafTransform.h"

#include "mafTagArray.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEAFRefSys.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafStorageElement.h"
#include "mafMatrix3x3.h"
#include "mafJointAnalysis.h"
#include "mafVectors.h"

#include "vtkSmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkArrowSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkWeightedLandmarkTransform.h"


#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEArrow)
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/*void mafVMEArrow::SetMatrix(const mafMatrix &mat)
//----------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}*/

//-------------------------------------------------------------------------
double mafVMEArrow::GetScaleFactor()
//-------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//-------------------------------------------------------------------------
void mafVMEArrow::SetScaleFactor(double scale)
//-------------------------------------------------------------------------
{
  m_ScaleFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  UpdateScaleFactor();
}
//-------------------------------------------------------------------------
void mafVMEArrow::UpdateScaleFactor()
//-------------------------------------------------------------------------
{
  m_ScaleAxisTransform->Identity();
  m_ScaleAxisTransform->Scale(m_AngleFactor * m_ScaleFactor,m_AngleFactor * m_ScaleFactor,m_AngleFactor * m_ScaleFactor);
  m_ScaleAxisTransform->Update();
  m_ScaleAxis->Update();
  Modified();
}
//-------------------------------------------------------------------------
char **mafVMEArrow::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}


//-------------------------------------------------------------------------
mafVMEArrow::mafVMEArrow() : mafVMEGeneric()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  SetDataPipe(dpipe);

  DependsOnLinkedNodeOn();

  m_ScaleFactor = 1.0;
  m_AngleFactor = 1.0;
  m_StepToNext  = 1.0;

  vtkUnsignedCharArray *data;
  float scalar_red[3]   = {255,0,0};
  float scalar_green[3] = {0,255,0};
  float scalar_blu[3]   = {0,0,255};

  m_ZArrow = vtkArrowSource::New();
  m_ZArrow->SetShaftRadius(m_ZArrow->GetTipRadius() / 5);
  m_ZArrow->SetTipResolution(40);
  m_ZArrow->SetTipRadius(m_ZArrow->GetTipRadius() / 2);
  m_ZArrow->Update();

  m_ZAxisTransform = vtkTransform::New();
  m_ZAxisTransform->PostMultiply();
  m_ZAxisTransform->RotateY(-90);
  m_ZAxisTransform->Update();

  m_ZAxis  = vtkTransformPolyDataFilter::New();
  m_ZAxis->SetInputConnection(m_ZArrow->GetOutputPort());
  m_ZAxis->SetTransform(m_ZAxisTransform);
  m_ZAxis->Update();

  int points = m_ZArrow->GetOutput()->GetNumberOfPoints();

  data = vtkUnsignedCharArray::New();
  data->SetName("AXES");
  data->SetNumberOfComponents(3);
  data->SetNumberOfTuples(points * 3);
  int i;
  for (i = 0; i < points; i++)
    data->SetTuple(i, scalar_red);

  // this filter do not copy the scalars also if all input 
  m_Axes = vtkAppendPolyData::New();    
  m_Axes->AddInputConnection(m_ZAxis->GetOutputPort());

  m_Axes->Update();

  m_ScaleAxisTransform = vtkTransform::New();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();

  vtkNew<vtkPolyData> axes_surface;
  axes_surface->DeepCopy(m_Axes->GetOutput());
  axes_surface->GetPointData()->SetScalars(data);
  vtkDEL(data);

  m_ScaleAxis  = vtkTransformPolyDataFilter::New();
  m_ScaleAxis->SetInputData(axes_surface);
  m_ScaleAxis->SetTransform(m_ScaleAxisTransform);
  m_ScaleAxis->Update();

  //SetData(m_ScaleAxis->GetOutput(), -1);

  dpipe->SetInputConnection(m_ScaleAxis->GetOutputPort());
}
//-------------------------------------------------------------------------
int mafVMEArrow::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *mafVMEArrow::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute(_R("MaterialAttributes"));
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute(_R("MaterialAttributes"), material);
    if (m_Output)
    {
      ((mafVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}

//-------------------------------------------------------------------------
mafVMEArrow::~mafVMEArrow()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_ZArrow);
  vtkDEL(m_ZAxisTransform);
  m_ZAxis->SetTransform(NULL);
  vtkDEL(m_ZAxis);

  vtkDEL(m_Axes);
  vtkDEL(m_ScaleAxisTransform);
  m_ScaleAxis->SetTransform(NULL);
  vtkDEL(m_ScaleAxis);
}

//----------------------------------------------------------------------------
void mafVMEArrow::InternalPreUpdate()
//----------------------------------------------------------------------------
{

}

//-----------------------------------------------------------------------
int mafVMEArrow::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent[_R("Transform")].StoreMatrix(m_Transform->GetMatrix());
    parent.StoreDouble(_R("ScaleFactor"), m_ScaleFactor);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEArrow::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node[_R("Transform")].RestoreMatrix(matrix) ==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      node[_R("ScaleFactor")].RestoreDouble( m_ScaleFactor);
      SetScaleFactor(m_ScaleFactor);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}


//----------------------------------------------------------------------------
void mafVMEArrow::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch (maf_event->GetId())
  {
  case ID_SCALE_FACTOR:
    {
      SetScaleFactor(m_ScaleFactor);
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  default:
    {
      Superclass::OnEvent(maf_event);
      break; 
    }
  }
}

//----------------------------------------------------------------------------
mafGUI *mafVMEArrow::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = Superclass::CreateGui();
  m_Gui->Show(false);
  m_Gui->Double(ID_SCALE_FACTOR,_L("scale"),&m_ScaleFactor);
  m_Gui->Divider();
  m_Gui->Button(ID_PRINT, _R("print"), _R("debug info") );
  m_Gui->Update();
  return m_Gui;
}

//----------------------------------------------------------------------------
void mafVMEArrow::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  os<<indent<<"Scale: "<<indent<<m_ScaleFactor;
}
