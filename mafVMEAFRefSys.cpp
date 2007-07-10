/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAFRefSys.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-10 20:35:35 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEAFRefSys.h"
#include "mafTagArray.h"
#include "vtkObjectFactory.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkArrowSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"

#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "mafTransform.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafStorageElement.h"
#include "mmgGui.h"
#include "mafPlotMath.h"
#include "mafIndent.h"

#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

mafCxxTypeMacro(mafVMEAFRefSys)

//-----------------------------------------------------------------------
//-------------------------------------------------------------------------
double mafVMEAFRefSys::GetScaleFactor()
//-------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//-------------------------------------------------------------------------
void mafVMEAFRefSys::SetScaleFactor(double scale)
//-------------------------------------------------------------------------
{
  m_ScaleFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  m_ScaleAxisTransform->Identity();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();
  m_ScaleAxis->Update();
  mafEvent cam_event(this,CAMERA_UPDATE);
  this->ForwardUpEvent(cam_event);
  Modified();
}
//-------------------------------------------------------------------------
char **mafVMEAFRefSys::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}


//-------------------------------------------------------------------------
mafVMEAFRefSys::mafVMEAFRefSys() : mafVMEGeneric()
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

  vtkUnsignedCharArray *data;
  float scalar_red[3]   = {255,0,0};
  float scalar_green[3] = {0,255,0};
  float scalar_blu[3]   = {0,0,255};

  m_XArrow = vtkArrowSource::New();
  m_XArrow->SetShaftRadius(m_XArrow->GetTipRadius()/5);
  m_XArrow->SetTipResolution(40);
  m_XArrow->SetTipRadius(m_XArrow->GetTipRadius()/2);
  m_XArrow->Update();

  m_XAxisTransform = vtkTransform::New();
  m_XAxisTransform->PostMultiply();
  m_XAxisTransform->Update();

  m_XAxis = vtkTransformPolyDataFilter::New();
  m_XAxis->SetInput(m_XArrow->GetOutput());
  m_XAxis->SetTransform(m_XAxisTransform);
  m_XAxis->Update();	

  int points = m_XArrow->GetOutput()->GetNumberOfPoints();  

  m_YArrow = vtkArrowSource::New();
  m_YArrow->SetShaftRadius(m_YArrow->GetTipRadius() / 5);
  m_YArrow->SetTipResolution(40);
  m_YArrow->SetTipRadius(m_YArrow->GetTipRadius() / 2);

  m_YAxisTransform = vtkTransform::New();
  m_YAxisTransform->PostMultiply();
  m_YAxisTransform->RotateZ(90);
  m_YAxisTransform->Update();

  m_YAxis = vtkTransformPolyDataFilter::New();
  m_YAxis->SetInput(m_YArrow->GetOutput());
  m_YAxis->SetTransform(m_YAxisTransform);
  m_YAxis->Update();

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
  m_ZAxis->SetInput(m_ZArrow->GetOutput());
  m_ZAxis->SetTransform(m_ZAxisTransform);
  m_ZAxis->Update();

  data = vtkUnsignedCharArray::New();
  data->SetName("AXES");
  data->SetNumberOfComponents(3);
  data->SetNumberOfTuples(points * 3);
  int i;
  for (i = 0; i < points; i++)
    data->SetTuple(i, scalar_red);
  for (i = points; i < 2*points; i++)
    data->SetTuple(i, scalar_green);
  for (i = 2*points; i < 3*points; i++)
    data->SetTuple(i, scalar_blu);

  // this filter do not copy the scalars also if all input 
  m_Axes = vtkAppendPolyData::New();    
  m_Axes->AddInput(m_XAxis->GetOutput()); // data has the scalars.
  m_Axes->AddInput(m_YAxis->GetOutput());
  m_Axes->AddInput(m_ZAxis->GetOutput());

  m_Axes->Update();

  m_ScaleAxisTransform = vtkTransform::New();
  m_ScaleAxisTransform->Scale(m_ScaleFactor,m_ScaleFactor,m_ScaleFactor);
  m_ScaleAxisTransform->Update();

  vtkMAFSmartPointer<vtkPolyData> axes_surface;
  axes_surface = m_Axes->GetOutput();
  axes_surface->SetSource(NULL);
  axes_surface->GetPointData()->SetScalars(data);
  vtkDEL(data);

  m_ScaleAxis  = vtkTransformPolyDataFilter::New();
  m_ScaleAxis->SetInput(axes_surface.GetPointer());
  m_ScaleAxis->SetTransform(m_ScaleAxisTransform);
  m_ScaleAxis->Update();

  //SetData(m_ScaleAxis->GetOutput(), -1);

  dpipe->SetInput(m_ScaleAxis->GetOutput());

  m_XOffset           = 0;
  m_YOffset           = 0;
  m_ZOffset           = 0;
  m_XRotate           = 0;
  m_YRotate           = 0;
  m_ZRotate           = 0;
  m_XoOffset          = m_XOffset;
  m_YoOffset          = m_YOffset;
  m_ZoOffset          = m_ZOffset;
  m_XoRotate          = m_XRotate;
  m_YoRotate          = m_YRotate;
  m_ZoRotate          = m_ZRotate;
}
//-------------------------------------------------------------------------
int mafVMEAFRefSys::InternalInitialize()
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
mmaMaterial *mafVMEAFRefSys::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputSurface *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}

//-------------------------------------------------------------------------
mafVMEAFRefSys::~mafVMEAFRefSys()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_XArrow);
  vtkDEL(m_XAxisTransform);
  m_XAxis->SetTransform(NULL);
  vtkDEL(m_XAxis);

  vtkDEL(m_YArrow);
  vtkDEL(m_YAxisTransform);
  m_YAxis->SetTransform(NULL);
  vtkDEL(m_YAxis);

  vtkDEL(m_ZArrow);
  vtkDEL(m_ZAxisTransform);
  m_ZAxis->SetTransform(NULL);
  vtkDEL(m_ZAxis);

  vtkDEL(m_Axes);	
  vtkDEL(m_ScaleAxisTransform);	
  m_ScaleAxis->SetTransform(NULL);
  vtkDEL(m_ScaleAxis);
}

//-------------------------------------------------------------------------
void mafVMEAFRefSys::SetTransf(double x, double y, double z, double xr, double yr, double zr)
//-------------------------------------------------------------------------
{
  m_XOffset = x;
  m_YOffset = y;
  m_ZOffset = z;
  m_XRotate = xr;
  m_YRotate = yr;
  m_ZRotate = zr;
  UpdateCS();
}

void mafVMEAFRefSys::InternalPreUpdate()
{

}

void mafVMEAFRefSys::InternalUpdate()
{
  //UpdateCS();
  //GetOutput()->Update();
  /*mafEvent *e	= new mafEvent(this,CAMERA_UPDATE);
  ForwardUpEvent(e);
  delete e;*/
  //Modified();
}

//-------------------------------------------------------------------------
void mafVMEAFRefSys::UpdateCS()
//-------------------------------------------------------------------------
{
  //DiMatrix       mAF;
  DiMatrix       mTran;
  DiMatrix       mTrano;
  DiMatrix       mTranoi;
  DiMatrix       mCurMtr;
  DiMatrix       mCurMtrLeft;
  wxInt32        nI;
  std::vector<mafTimeStamp>   mpStamps;
  vtkMatrix4x4  *mVTK = NULL;
  mafMatrix      mfMtr;
  DiV4d          pos, rot;
  DiV4d          poso, roto;

  pos.x = (float)m_XOffset;
  pos.y = (float)m_YOffset;
  pos.z = (float)m_ZOffset;
  pos.w = 1;
  rot.x = (float)m_XRotate * (diPI / 180.0);
  rot.y = (float)m_YRotate * (diPI / 180.0);
  rot.z = (float)m_ZRotate * (diPI / 180.0);
  rot.w = 1;
  poso.x = m_XoOffset;
  poso.y = m_YoOffset;
  poso.z = m_ZoOffset;
  poso.w = 1;
  roto.x = m_XoRotate * (diPI / 180.0);
  roto.y = m_YoRotate * (diPI / 180.0);
  roto.z = m_ZoRotate * (diPI / 180.0);
  roto.w = 1;

  mafTransfComposeMatrixStright(&mTran, &rot, &pos);
  mafTransfComposeMatrixStright(&mTrano, &roto, &poso);

  DiMatrixInvert(&mTrano, &mTranoi);
  DiMatrixCopy(&mTran, &mTrano);
  DiMatrixMultiply(&mTrano, &mTranoi, &mTran);
  {
    vtkNEW(mVTK);
    GetParent()->GetTimeStamps(mpStamps);
    for(nI = 0; nI < mpStamps.size(); nI++)
    {
      //m_Hierarchy->GetMatrix(GetParent(), mpStamps[nI], &mAF);
      GetOutput()->GetAbsMatrix(mfMtr, mpStamps[nI]);
      mflMatrixToDi(mfMtr.GetVTKMatrix(), &mCurMtr);
      mafTransfRightLeftConv(&mCurMtr, &mCurMtrLeft);
      DiMatrixMultiply(&mTran, &mCurMtrLeft, &mTranoi);
      mafTransfRightLeftConv(&mTranoi, &mTrano);
      mVTK->Identity();
      DiMatrixToVTK(&mTrano, mVTK);
      //DiMatrixToVTK(&mAF, mVTK);
      SetAbsMatrix(mVTK, mpStamps[nI]);
    }
    vtkDEL(mVTK);
  }

  m_XoOffset = m_XOffset;
  m_YoOffset = m_YOffset;
  m_ZoOffset = m_ZOffset;
  m_XoRotate = m_XRotate;
  m_YoRotate = m_YRotate;
  m_ZoRotate = m_ZRotate;

  Modified();
  GetOutput()->Update();
  mafEvent *e = new mafEvent(this,CAMERA_UPDATE);
  ForwardUpEvent(e);
  delete e;

}
//-----------------------------------------------------------------------
int mafVMEAFRefSys::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    parent->StoreDouble("m_ScaleFactor", m_ScaleFactor);
    parent->StoreDouble("m_XOffset", m_XOffset);
    parent->StoreDouble("m_YOffset", m_YOffset);
    parent->StoreDouble("m_ZOffset", m_ZOffset);
    parent->StoreDouble("m_XRotate", m_XRotate);
    parent->StoreDouble("m_YRotate", m_YRotate);
    parent->StoreDouble("m_ZRotate", m_ZRotate);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEAFRefSys::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      node->RestoreDouble("m_ScaleFactor", m_ScaleFactor);
      node->RestoreDouble("m_XOffset", m_XOffset);
      node->RestoreDouble("m_YOffset", m_YOffset);
      node->RestoreDouble("m_ZOffset", m_ZOffset);
      node->RestoreDouble("m_XRotate", m_XRotate);
      node->RestoreDouble("m_YRotate", m_YRotate);
      node->RestoreDouble("m_ZRotate", m_ZRotate);
      m_XoOffset = m_XOffset;
      m_YoOffset = m_YOffset;
      m_ZoOffset = m_ZOffset;
      m_XoRotate = m_XRotate;
      m_YoRotate = m_YRotate;
      m_ZoRotate = m_ZRotate;
      SetScaleFactor(m_ScaleFactor);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
void mafVMEAFRefSys::GetTransf(double &x, double &y, double &z, double &xr, double &yr, double &zr)
//------------------------------------------------------------------------------
{
  x  = m_XOffset;
  y  = m_YOffset;
  z  = m_ZOffset;
  xr = m_XRotate;
  yr = m_YRotate;
  zr = m_ZRotate;
  return;
}



//----------------------------------------------------------------------------
void mafVMEAFRefSys::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch (maf_event->GetId())
  {
  case ID_X_OFFSET:
  case ID_Y_OFFSET:
  case ID_Z_OFFSET:
  case ID_X_ROTATE:
  case ID_Y_ROTATE:
  case ID_Z_ROTATE:
    {
      wxLogMessage("ID_TRANSFORM %f %f %f %f %f %f",m_XOffset, m_YOffset, m_ZOffset, m_XRotate, m_YRotate, m_ZRotate);
      SetTransf(m_XOffset, m_YOffset, m_ZOffset, m_XRotate, m_YRotate, m_ZRotate);
      Modified();
      Update();
      mafEvent *e = new mafEvent(this,CAMERA_UPDATE);
      ForwardUpEvent(e);
      cppDEL(e);

      //m_guiVmeInfo->Update();
      //mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
    }
  case ID_SCALE_FACTOR:
    {
      //wxLogMessage("ID_UPDATE_CURRTIME %d",this->m_UpdateCurrentTimeOnly);
      //m_guiVmeInfo->Update();
      SetScaleFactor(m_ScaleFactor);
      break;
    }
  case ID_LOAD_DICTIONARY:
    {
      break;
    }
  default:
    {
      mafVMEGeneric::OnEvent(maf_event);
      break; 
    }
  }
}


mmgGui *mafVMEAFRefSys::CreateGui()
//----------------------------------------------------------------------------
{
  wxString saAxisChoices[2] = {"Prefer Z Axis", "Prefer Y Axis"};

  m_Gui = Superclass::CreateGui();
  m_Gui->Show(false);

  m_Gui->Double(ID_SCALE_FACTOR,_("scale"),&m_ScaleFactor);
  m_Gui->Divider();
  //actual code
  {
    m_Gui->FloatSlider(ID_X_OFFSET, "X offset",&m_XOffset, -180.0, 180.0);
    m_Gui->FloatSlider(ID_Y_OFFSET, "Y offset",&m_YOffset, -180.0, 180.0);
    m_Gui->FloatSlider(ID_Z_OFFSET, "Z offset",&m_ZOffset, -180.0, 180.0);
    m_Gui->FloatSlider(ID_X_ROTATE, "X rotate",&m_XRotate, -180.0, 180.0);
    m_Gui->FloatSlider(ID_Y_ROTATE, "Y rotate",&m_YRotate, -180.0, 180.0);
    m_Gui->FloatSlider(ID_Z_ROTATE, "Z rotate",&m_ZRotate, -180.0, 180.0);
  }
  //actual code - end
  m_Gui->Divider();
  m_Gui->Button(ID_PRINT, "print", "debug info" );

  m_Gui->Update();

  return m_Gui;
}

void mafVMEAFRefSys::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  os<<indent<<"6DOFs: "<<indent<<m_XOffset<<indent<<m_YOffset<<indent<<m_ZOffset<<indent<<m_XRotate<<indent<<m_YRotate<<indent<<m_ZRotate;
  os<<indent<<"Scale: "<<indent<<m_ScaleFactor;
}
