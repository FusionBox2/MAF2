/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEHelAxis.cpp,v $
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
#include "mafVMEHelAxis.h"

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
#include "vtkSphereSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkWeightedLandmarkTransform.h"


#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEHelAxis)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void GetShowTransform(mafVME *proximal, mafTimeStamp ts, const V3d<double>& helicalAxis, const V3d<double>& point, mafMatrix& globalMatrix)
//----------------------------------------------------------------------------
{
  mafMatrix mShowHelical;

  V3d<double> right;
  V3d<double> up = point;
  V3d<double> at = helicalAxis;
  at /= sqrt(at.length2());
  if(up.length2() == 0.0)
  {
    V3d<double> ort(0.0, 0.0, 0.0);
    int         minind  = 0;
    if(fabs(at[1]) < fabs(at[minind]))
      minind = 1;
    if(fabs(at[2]) < fabs(at[minind]))
      minind = 2;
    ort[minind] = 1.0;
    up = at ^ ort;
  }
  up /= sqrt(up.length2());
  right = up ^ at;
  right /= sqrt(right.length2());
  up = at ^ right;
  up /= sqrt(up.length2());

  mShowHelical.SetElement(0, 0, right.x);
  mShowHelical.SetElement(1, 0, right.y);
  mShowHelical.SetElement(2, 0, right.z);
  mShowHelical.SetElement(3, 0, 0.0);
  mShowHelical.SetElement(0, 1, up.x);
  mShowHelical.SetElement(1, 1, up.y);
  mShowHelical.SetElement(2, 1, up.z);
  mShowHelical.SetElement(3, 1, 0.0);
  mShowHelical.SetElement(0, 2, at.x);
  mShowHelical.SetElement(1, 2, at.y);
  mShowHelical.SetElement(2, 2, at.z);
  mShowHelical.SetElement(3, 2, 0.0);
  mShowHelical.SetElement(0, 3, point.x);
  mShowHelical.SetElement(1, 3, point.y);
  mShowHelical.SetElement(2, 3, point.z);
  mShowHelical.SetElement(3, 3, 1.0);

  mafMatrix tmp;

  tmp.Identity();
  if(proximal != NULL)
    GetGlobalMatrix(proximal, ts, tmp, false);

  mafMatrix::Multiply4x4(tmp, mShowHelical, globalMatrix);
  globalMatrix.SetTimeStamp(ts);
}

void mafVMEHelAxis::InternalUpdate()
{
  mafMatrix mtr;
  mafTimeStamp tsTime = GetTimeStamp();
  mafVME *proximal = GetProximal();
  mafVME *distal   = GetDistal();
  /*if(!proximal)
  {
    proximal = AutoSelectProximal(distal);
    SetProximal(proximal);
  }*/
  if(m_Mode == 0)
  {
    GetMomentAxis(proximal, distal, tsTime, m_MinAngle, m_Direction, m_StartPoint, m_Angle, m_Translation);
  }
  else if(m_Mode == 1)
  {
    if(m_MeanChanges)
    {
      m_MeanChanges = 0;
      GetMeanAxis(proximal, distal, m_MinTime, m_MaxTime, m_MinAngle, m_Direction, m_StartPoint, m_Angle);
      m_Translation = 0.0;
    }
  }
  else /*if(m_Mode == 2)*/
  {
    GetMomentAxis(proximal, distal, m_RefTime, m_MinAngle, m_Direction, m_StartPoint, m_Angle, m_Translation, tsTime);
  }


  //adjust orientation of axis according to proximal segment orientation
  bool useRF = false;
  if(m_AligningMode != 0)
    useRF = true;
  mafMatrix proxMatr;
  proxMatr.Identity();
  if(proximal != NULL)
  {
    GetGlobalMatrix(proximal, tsTime, proxMatr, useRF);
  }



  if(AlignAxis(m_Direction, proxMatr, m_AligningMode))
  {
    m_Direction   = -m_Direction;
    m_Angle       = - m_Angle;
    m_Translation = - m_Translation;
  }

  if(m_Gui)
  {
    m_StrDir[0] = mafString::Format(_R("x: %f"),m_Direction[0]);
    m_StrDir[1] = mafString::Format(_R("y: %f"),m_Direction[1]);
    m_StrDir[2] = mafString::Format(_R("z: %f"),m_Direction[2]);
    m_StrPnt[0] = mafString::Format(_R("x: %f"),m_StartPoint[0]);
    m_StrPnt[1] = mafString::Format(_R("y: %f"),m_StartPoint[1]);
    m_StrPnt[2] = mafString::Format(_R("z: %f"),m_StartPoint[2]);
    m_StrAng    = (m_Mode == 1) ? _R("") : mafString::Format(_R("a: %f"),m_Angle);
    m_StrTrl    = (m_Mode == 1) ? _R("") : mafString::Format(_R("t: %f"),m_Translation);
    m_Gui->Update();
  }

  GetShowTransform(proximal, tsTime, m_Direction, m_StartPoint, mtr);
  if(m_Angle == 0.0)
    m_AngleFactor = 0.0;
  else
    m_AngleFactor = 1.0;
  //m_AngleFactor = angle;
  UpdateLengthFactor();
  SetAbsMatrix(mtr, tsTime);
}



bool mafVMEHelAxis::AlignAxis(const V3d<double>& direction, const mafMatrix& prox, int mode)
{
  const int align_choices[6][3] = {{0, 1, 2}, {0, 2, 1}, {1, 2, 0}, {1, 0, 2}, {2, 0, 1}, {2, 1, 0}};
  int index[3];
  double delta = 0.00001;
  bool invert = false;
  V3d<double> prxdirs[3];
  V3d<double> ha;
  double projs[3];

  if(mode == 0)
    return false;

  for(unsigned i = 0; i < 3; i++)
    index[i] = align_choices[mode - 1][i];
  ha = m_Direction;
  ha /= sqrt(ha.length2());
  for(unsigned i = 0; i < 3; i ++)
  {
    for(unsigned j = 0; j < 3; j++)
    {
      prxdirs[i][j] = prox.GetElement(j, i);
    }
    projs[i] = prxdirs[i] * ha;
  }

  if(projs[index[0]] < -delta)
    invert = true;
  else if(projs[index[0]] < delta)
  {
    if(projs[index[1]] < -delta)
      invert = true;
    else if(projs[index[1]] < delta)
    {
      if(projs[index[2]] < -delta)
        invert = true;
    }
  }
  return invert;
}

//----------------------------------------------------------------------------
void mafVMEHelAxis::SetMatrix(const mafMatrix &mat)
//----------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void mafVMEHelAxis::UpdateCS()
//-------------------------------------------------------------------------
{
  GetOutput()->Update();
  mafEvent *e  = new mafEvent(this,CAMERA_UPDATE);
  ForwardUpEvent(e);
  delete e;
}

//-------------------------------------------------------------------------
double mafVMEHelAxis::GetLengthFactor()
//-------------------------------------------------------------------------
{
  return m_LengthFactor;
}
//-------------------------------------------------------------------------
double mafVMEHelAxis::GetRadiusFactor()
//-------------------------------------------------------------------------
{
  return m_RadiusFactor;
}
//-------------------------------------------------------------------------
void mafVMEHelAxis::SetLengthFactor(double scale)
//-------------------------------------------------------------------------
{
  m_LengthFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  UpdateLengthFactor();
}
//-------------------------------------------------------------------------
void mafVMEHelAxis::SetRadiusFactor(double scale)
//-------------------------------------------------------------------------
{
  m_RadiusFactor = scale;
  if (m_Gui)
  {
    m_Gui->Update();
  }
  UpdateLengthFactor();
}
//-------------------------------------------------------------------------
void mafVMEHelAxis::UpdateLengthFactor()
//-------------------------------------------------------------------------
{
  m_ScaleAxisTransform->Identity();
  m_ScaleAxisTransform->Scale(m_AngleFactor * m_RadiusFactor,m_AngleFactor * m_RadiusFactor,m_AngleFactor * m_LengthFactor);
  m_ScaleAxisTransform->Update();
  m_ScaleAxis->Update();
  Modified();
}
//-------------------------------------------------------------------------
char **mafVMEHelAxis::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}


//-------------------------------------------------------------------------
mafVMEHelAxis::mafVMEHelAxis() : mafVME()
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

  m_LengthFactor = 1.0;
  m_RadiusFactor = 1.0;
  m_AngleFactor  = 1.0;
  m_MinAngle     = 5.0;
  m_MinTime      = 0.0;
  m_MaxTime      = 0.0;
  m_RefTime      = 0.0;
  m_Mode         = 0;
  m_AligningMode = 0;

  m_Angle        = 0.0;
  m_Translation  = 0.0;
  m_StartPoint   = V3d<double>(0.0, 0.0, 0.0);
  m_Direction    = V3d<double>(1.0, 0.0, 0.0);

  m_StrDir[0]    = _R("1.0");
  m_StrDir[1]    = m_StrDir[2] = _R("0.0");
  m_StrPnt[0]    = m_StrPnt[1] = m_StrPnt[2] = _R("0.0");
  m_StrAng       = _R("0.0");
  m_StrTrl       = _R("0.0");


  m_MeanChanges  = 1;

  vtkUnsignedCharArray *data;
  float scalar_red[3]   = {255,0,0};
  float scalar_green[3] = {0,255,0};
  float scalar_blu[3]   = {0,0,255};

  m_ZArrow = vtkArrowSource::New();
  m_ZArrow->SetShaftRadius(m_ZArrow->GetTipRadius() / 5);
  m_ZArrow->SetTipResolution(40);
  m_ZArrow->SetTipRadius(m_ZArrow->GetTipRadius() / 2);
  m_ZArrow->Update();

  m_CenterSphere = vtkSphereSource::New();
  m_CenterSphere->SetRadius(m_ZArrow->GetShaftRadius() * 2);
  m_CenterSphere->SetCenter(0, 0, 0);
  m_CenterSphere->SetPhiResolution(20);
  m_CenterSphere->SetThetaResolution(20);
  m_CenterSphere->Update();
  //m_PolyData->DeepCopy(surf->GetOutput());
  //m_PolyData->Update();


  m_ZAxisTransform = vtkTransform::New();
  m_ZAxisTransform->PostMultiply();
  m_ZAxisTransform->RotateY(-90);
  m_ZAxisTransform->Translate(0, 0, -0.5);
  m_ZAxisTransform->Update();

  m_CenterTransform = vtkTransform::New();
  m_CenterTransform->PostMultiply();
  m_CenterTransform->Update();


  m_ZAxis  = vtkTransformPolyDataFilter::New();
  m_ZAxis->SetInputConnection(m_ZArrow->GetOutputPort());
  m_ZAxis->SetTransform(m_ZAxisTransform);
  m_ZAxis->Update();

  m_Center  = vtkTransformPolyDataFilter::New();
  m_Center->SetInputConnection(m_CenterSphere->GetOutputPort());
  m_Center->SetTransform(m_CenterTransform);
  m_Center->Update();

  int points = m_ZArrow->GetOutput()->GetNumberOfPoints() + m_CenterSphere->GetOutput()->GetNumberOfPoints();

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
  m_Axes->AddInputConnection(m_Center->GetOutputPort());

  m_Axes->Update();

  m_ScaleAxisTransform = vtkTransform::New();
  m_ScaleAxisTransform->Scale(m_RadiusFactor,m_RadiusFactor,m_LengthFactor);
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
  m_ProximalName = _R("");
  m_DistalName = _R("");
}

//-------------------------------------------------------------------------
int mafVMEHelAxis::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEHelAxis *vme_hel_axis = mafVMEHelAxis::SafeDownCast(a);
    m_MinAngle = vme_hel_axis->m_MinAngle;
    m_MinTime  = vme_hel_axis->m_MinTime;
    m_MaxTime  = vme_hel_axis->m_MaxTime;
    m_Mode     = vme_hel_axis->m_Mode;
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEHelAxis::InternalInitialize()
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
mmaMaterial *mafVMEHelAxis::GetMaterial()
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
mafVMEHelAxis::~mafVMEHelAxis()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  SetOutput(NULL);

  vtkDEL(m_CenterSphere);
  vtkDEL(m_CenterTransform);
  vtkDEL(m_Center);

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
void mafVMEHelAxis::InternalPreUpdate()
//----------------------------------------------------------------------------
{

}

//-----------------------------------------------------------------------
int mafVMEHelAxis::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix(_R("Transform"),&m_Transform->GetMatrix());
    //code for backward compatibility
    parent->StoreDouble(_R("ScaleFactor"), m_LengthFactor);
    parent->StoreDouble(_R("RadiusFactor"), m_RadiusFactor);
    parent->StoreDouble(_R("LengthFactor"), m_LengthFactor);
    parent->StoreDouble(_R("MinAngle"), m_MinAngle);
    parent->StoreDouble(_R("MinTime"), m_MinTime);
    parent->StoreDouble(_R("MaxTime"), m_MaxTime);
    parent->StoreInteger(_R("Mode"), m_Mode);
    parent->StoreInteger(_R("AligningMode"), m_AligningMode);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEHelAxis::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix(_R("Transform"),&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      //code for backward compatibility
      node->RestoreDouble(_R("ScaleFactor"), m_LengthFactor);
      m_RadiusFactor = m_LengthFactor;
      node->RestoreDouble(_R("RadiusFactor"), m_RadiusFactor);
      node->RestoreDouble(_R("LengthFactor"), m_LengthFactor);
      node->RestoreDouble(_R("MinAngle"), m_MinAngle);
      node->RestoreDouble(_R("MinTime"), m_MinTime);
      node->RestoreDouble(_R("MaxTime"), m_MaxTime);
      node->RestoreInteger(_R("Mode"), m_Mode);
      node->RestoreInteger(_R("AligningMode"), m_AligningMode);
      m_MeanChanges = 1;
      SetRadiusFactor(m_RadiusFactor);
      SetLengthFactor(m_LengthFactor);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}

void mafVMEHelAxis::SetProximal(mafVME *vme)
{
  return;
  assert(false);
  if(vme)
  {
    SetLink(_R("ProximalSegment"), vme);
    m_ProximalName = vme->GetName();
  }
  else 
  {
    RemoveLink(_R("ProximalSegment"));
    m_ProximalName = _R("");
  }
  if(m_Gui)
    m_Gui->Update();
}

void mafVMEHelAxis::SetDistal(mafVME *vme)
{
  if(vme)
  {
    SetLink(_R("DistalSegment"), vme);
    m_DistalName = vme->GetName();
  }
  else 
  {
    RemoveLink(_R("DistalSegment"));
    m_DistalName = _R("");
  }
  if(m_Gui)
    m_Gui->Update();
}

mafVME *mafVMEHelAxis::GetProximal()
{
  return GetParent();
  if(mafVME *vme = mafVME::SafeDownCast(GetLink(_R("ProximalSegment"))))
    return vme;
  return NULL;
}
mafVME *mafVMEHelAxis::GetDistal()
{
  //return GetParent();
  if(mafVME *vme = mafVME::SafeDownCast(GetLink(_R("DistalSegment"))))
    return vme;
  return NULL;
}

//----------------------------------------------------------------------------
void mafVMEHelAxis::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch (maf_event->GetId())
  {
  case ID_MIN_ANGLE:
  case ID_MIN_TIME:
  case ID_MAX_TIME:
    {
      m_MeanChanges = 1;
      Modified();
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_MODE:
    {
      m_MeanChanges = 1;
      m_Gui->Enable(ID_MIN_TIME, m_Mode == 1);
      m_Gui->Enable(ID_MAX_TIME, m_Mode == 1);
      m_Gui->Enable(ID_REF_TIME, m_Mode == 2);
      Modified();
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_LENGTH_FACTOR:
    {
      SetLengthFactor(m_LengthFactor);
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_RADIUS_FACTOR:
    {
      SetRadiusFactor(m_RadiusFactor);
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_ALIGNING:
    {
      Modified();
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      break;
    }
  case ID_PROXIMAL:
    {
      mafString s(_L("Choose cloud"));
      mafEvent e(this,VME_CHOOSE, &s, NULL/*, (long)&lhpOpRegisterLMScripted::ClosedCloudAccept*/);
      this->ForwardUpEvent(e);
      mafVME *vme = mafVME::SafeDownCast(e.GetVme());
      SetProximal(vme);
      break;
    }
  case ID_DISTAL:
  {
    mafString s(_L("Choose cloud"));
    mafEvent e(this,VME_CHOOSE, &s, NULL/*, (long)&lhpOpRegisterLMScripted::ClosedCloudAccept*/);
    this->ForwardUpEvent(e);
    mafVME *vme = mafVME::SafeDownCast(e.GetVme());
    SetDistal(vme);
    break;
  }
  case ID_RESETPROXIMAL:
  {
    //SetProximal(AutoSelectProximal(GetDistal()));
    SetProximal(NULL);
    break;
  }
  case ID_RESETDISTAL:
    {
      SetDistal(NULL);
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
mafGUI *mafVMEHelAxis::CreateGui()
//----------------------------------------------------------------------------
{
  const mafString mode_choices[] = {_L("Instant"), _L("Mean"), _L("Relative")};
  const mafString align_choices[] = {_L("None"), _L("XYZ"), _L("XZY"), _L("YZX"), _L("YXZ"), _L("ZXY"), _L("ZYX")};
  m_Gui = Superclass::CreateGui();
  m_Gui->Show(false);
  m_Gui->Double(ID_RADIUS_FACTOR,_L("radius scale"),&m_RadiusFactor);
  m_Gui->Double(ID_LENGTH_FACTOR,_L("length scale"),&m_LengthFactor);
  m_Gui->Double(ID_MIN_ANGLE, _L("Min angle"), &m_MinAngle, 0.0);
  m_Gui->Combo(ID_MODE, _L("Mode"), &m_Mode, 3, mode_choices, _L("Select mode"));
  m_Gui->Combo(ID_ALIGNING, _L("Align"), &m_AligningMode, 7, align_choices, _L("Select aligning"));

  m_Gui->Label(_L("Proximal :"),true);
  if(mafVME *vme = GetProximal())
    m_ProximalName = vme->GetName();
  m_Gui->Label(&m_ProximalName);
  //m_Gui->Button(ID_PROXIMAL,_("Proximal"));
  //m_Gui->Button(ID_RESETPROXIMAL,_("Reset proximal"));

  m_Gui->Label(_L("Distal :"),true);
  if(mafVME *vme = GetDistal())
    m_DistalName = vme->GetName();
  m_Gui->Label(&m_DistalName);
  m_Gui->Button(ID_DISTAL,_L("Distal"));
  m_Gui->Button(ID_RESETDISTAL,_L("Reset distal"));

  m_Gui->Double(ID_MIN_TIME, _L("Min time"), &m_MinTime, 0.0);
  m_Gui->Double(ID_MAX_TIME, _L("Max time"), &m_MaxTime);
  m_Gui->Double(ID_REF_TIME, _L("Ref time"), &m_RefTime);
  m_Gui->Divider();
  m_Gui->Enable(ID_MIN_TIME, m_Mode == 1);
  m_Gui->Enable(ID_MAX_TIME, m_Mode == 1);
  m_Gui->Enable(ID_REF_TIME, m_Mode == 2);
  m_Gui->Divider();
  m_Gui->Label(_R("Direction:"));
  m_StrDir[0] = mafString::Format(_R("x: %f"),m_Direction[0]);
  m_StrDir[1] = mafString::Format(_R("y: %f"),m_Direction[1]);
  m_StrDir[2] = mafString::Format(_R("z: %f"),m_Direction[2]);
  m_Gui->Label(_R(""), &m_StrDir[0]);
  m_Gui->Label(_R(""), &m_StrDir[1]);
  m_Gui->Label(_R(""), &m_StrDir[2]);
  m_Gui->Label(_R("Rotation Center:"));
  m_StrPnt[0] = mafString::Format(_R("x: %f"),m_StartPoint[0]);
  m_StrPnt[1] = mafString::Format(_R("y: %f"),m_StartPoint[1]);
  m_StrPnt[2] = mafString::Format(_R("z: %f"),m_StartPoint[2]);
  m_Gui->Label(_R(""), &m_StrPnt[0]);
  m_Gui->Label(_R(""), &m_StrPnt[1]);
  m_Gui->Label(_R(""), &m_StrPnt[2]);
  m_Gui->Label(_R("Angle:"));
  m_StrAng    = (m_Mode == 1) ? _R("") : mafString::Format(_R("%f"),m_Angle);
  m_Gui->Label(_R(""), &m_StrAng);
  m_Gui->Label(_R("Translation:"));
  m_StrTrl    = (m_Mode == 1) ? _R("") : mafString::Format(_R("%f"),m_Translation);
  m_Gui->Label(_R(""), &m_StrTrl);
  m_Gui->Update();
  return m_Gui;
}

//----------------------------------------------------------------------------
void mafVMEHelAxis::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  os<<indent<<"RadiusScale: "<<indent<<m_RadiusFactor;
  os<<indent<<"LengthScale: "<<indent<<m_LengthFactor;
}
