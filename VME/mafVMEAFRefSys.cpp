/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAFRefSys.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-24 11:10:44 $
  Version:   $Revision: 1.3 $
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

#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

#include "mafVMEAFRefSys.h"

#include "mmgGui.h"
#include "mafVMELandmarkCloud.h"
#include "mafPlotMath.h"
#include "mmaMaterial.h"
#include "mafTransform.h"
#include "mafIndent.h"

#include "mafTagArray.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafStorageElement.h"

#include "vtkObjectFactory.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkArrowSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkAppendPolyData.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"


//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEAFRefSys)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEAFRefSys::mafVMEAFRefSys()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  SetDataPipe(dpipe);

  m_vm = NULL;

  DependsOnLinkedNodeOn();

  m_Active      = 0;
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

  dpipe->SetInput(m_ScaleAxis->GetOutput());

  m_XOffset = 0;
  m_YOffset = 0;
  m_ZOffset = 0;
  m_XRotate = 0;
  m_YRotate = 0;
  m_ZRotate = 0;

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
  if(m_vm)
    delete m_vm;
}
//-------------------------------------------------------------------------
int mafVMEAFRefSys::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEAFRefSys *vme_ref_sys=mafVMEAFRefSys::SafeDownCast(a);
    m_Transform->SetMatrix(vme_ref_sys->m_Transform->GetMatrix());
    SetScaleFactor(vme_ref_sys->GetScaleFactor());
    vme_ref_sys->m_scriptText = m_scriptText;
    vme_ref_sys->ConvertTextToVM();
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_ScaleAxis->GetOutput());
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEAFRefSys::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    return (m_Transform->GetMatrix() == ((mafVMEAFRefSys *)vme)->m_Transform->GetMatrix() &&
      m_ScaleFactor == ((mafVMEAFRefSys *)vme)->GetScaleFactor());
  }
  return false;
}

//----------------------------------------------------------------------------
void mafVMEAFRefSys::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
  os<<indent<<"6DOFs: "<<indent<<m_XOffset<<indent<<m_YOffset<<indent<<m_ZOffset<<indent<<m_XRotate<<indent<<m_YRotate<<indent<<m_ZRotate;
  os<<indent<<"Scale: "<<indent<<m_ScaleFactor;
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
mafVMEOutputSurface *mafVMEAFRefSys::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}

//-------------------------------------------------------------------------
void mafVMEAFRefSys::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEAFRefSys::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
void mafVMEAFRefSys::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear();
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
  Update();
  Modified();
}

//-------------------------------------------------------------------------
double mafVMEAFRefSys::GetScaleFactor()
//-------------------------------------------------------------------------
{
  return m_ScaleFactor;
}
//-----------------------------------------------------------------------
int mafVMEAFRefSys::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    parent->StoreDouble("ScaleFactor", m_ScaleFactor);
    parent->StoreInteger("Active", m_Active);
    parent->StoreDouble("XOffset", m_XOffset);
    parent->StoreDouble("YOffset", m_YOffset);
    parent->StoreDouble("ZOffset", m_ZOffset);
    parent->StoreDouble("XRotate", m_XRotate);
    parent->StoreDouble("YRotate", m_YRotate);
    parent->StoreDouble("ZRotate", m_ZRotate);
    m_textSize = m_scriptText.size();
    parent->StoreInteger("ScriptStrings", m_textSize);
    for(int i = 0; i < m_textSize; i++)
    {
      char nm[30];
      sprintf(nm, "ln%d", i);
      parent->StoreText(nm, m_scriptText[i].GetCStr());
    }

    for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
    {
      if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
      {
        std::map<mafString, mafString>::iterator it = m_lmMapping.find(m_vm->getInputs()[i].first.c_str());
        if(it == m_lmMapping.end())
        {
          parent->StoreText(m_vm->getInputs()[i].first.c_str(), m_vm->getInputs()[i].first.c_str());
        }
        else
        {
          parent->StoreText(it->first.GetCStr(), it->second.GetCStr());
        }
      }
      else
      {
        parent->StoreDouble(m_vm->getInputs()[i].first.c_str(), m_vm->getInputs()[i].second->GetScalar());
      }
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}


void mafVMEAFRefSys::SetActive(int active)
{
  if(GetParent() == NULL)
    return;
  m_Active = active;
  if(active)
  {
    for(unsigned i = 0; i < GetParent()->GetNumberOfChildren(); i++)
    {
      mafVMEAFRefSys *afsys = mafVMEAFRefSys::SafeDownCast(GetParent()->GetChild(i));
      if(afsys != NULL && afsys != this)
        afsys->SetActive(0);
    }
  }
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
      node->RestoreDouble("ScaleFactor", m_ScaleFactor);
      node->RestoreInteger("Active", m_Active);
      node->RestoreDouble("XOffset", m_XOffset);
      node->RestoreDouble("YOffset", m_YOffset);
      node->RestoreDouble("ZOffset", m_ZOffset);
      node->RestoreDouble("XRotate", m_XRotate);
      node->RestoreDouble("YRotate", m_YRotate);
      node->RestoreDouble("ZRotate", m_ZRotate);
      node->RestoreInteger("ScriptStrings", m_textSize);
      m_scriptText.resize(m_textSize);
      for(int i = 0; i < m_textSize; i++)
      {
        char nm[30];
        sprintf(nm, "ln%d", i);
        node->RestoreText(nm, m_scriptText[i]);
      }
      ConvertTextToVM();
      for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
      {
        if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
        {
          mafString tmp;
          node->RestoreText(m_vm->getInputs()[i].first.c_str(), tmp);
          m_lmMapping[m_vm->getInputs()[i].first.c_str()] = tmp;
        }
        else
        {
          node->RestoreDouble(m_vm->getInputs()[i].first.c_str(), m_vm->getInputs()[i].second->GetScalar());
        }
      }
      SetScaleFactor(m_ScaleFactor);
      return MAF_OK;
    }
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
mmgGui* mafVMEAFRefSys::CreateGui()
//-------------------------------------------------------------------------
{
  wxString saAxisChoices[2] = {"Prefer Z Axis", "Prefer Y Axis"};

  m_Gui = Superclass::CreateGui();
  m_Gui->Show(false);

  m_Gui->Double(ID_SCALE_FACTOR,_("scale"),&m_ScaleFactor);
  m_Gui->Bool(ID_ACTIVE, _("Active"), &m_Active);
  m_Gui->Divider();

  m_Gui->FloatSlider(ID_X_OFFSET, "X offset",&m_XOffset, -1000.0, 1000.0);
  m_Gui->FloatSlider(ID_Y_OFFSET, "Y offset",&m_YOffset, -1000.0, 1000.0);
  m_Gui->FloatSlider(ID_Z_OFFSET, "Z offset",&m_ZOffset, -1000.0, 1000.0);
  m_Gui->FloatSlider(ID_X_ROTATE, "X rotate",&m_XRotate, -180.0, 180.0);
  m_Gui->FloatSlider(ID_Y_ROTATE, "Y rotate",&m_YRotate, -180.0, 180.0);
  m_Gui->FloatSlider(ID_Z_ROTATE, "Z rotate",&m_ZRotate, -180.0, 180.0);
  m_Gui->Divider();

  for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
  {
    if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
    {
      std::map<mafString, mafString>::iterator it = m_lmMapping.find(m_vm->getInputs()[i].first.c_str());
      if(it == m_lmMapping.end())
        continue;
      m_Gui->Button(ID_FIRSTDYN + i, it->first.GetCStr(), "", "Press to modify");
      m_Gui->Label(it->first.GetCStr(), &(it->second));
      m_buttonMapping[ID_FIRSTDYN + i] = m_vm->getInputs()[i].first.c_str();
    }
    else
    {
      double minlimit = (m_vm->getInputs()[i].second->IsDnLimited()) ? m_vm->getInputs()[i].second->GetDnLimit() : MINDOUBLE;
      double maxlimit = (m_vm->getInputs()[i].second->IsUpLimited()) ? m_vm->getInputs()[i].second->GetUpLimit() : MAXDOUBLE;
      if(m_vm->getInputs()[i].second->IsDnLimited() && m_vm->getInputs()[i].second->IsUpLimited())
        m_Gui->FloatSlider(ID_FIRSTDYN + i, m_vm->getInputs()[i].first.c_str(), &(m_vm->getInputs()[i].second->GetScalar()), minlimit, maxlimit);
      else
        m_Gui->Double(ID_FIRSTDYN + i, m_vm->getInputs()[i].first.c_str(), &(m_vm->getInputs()[i].second->GetScalar()), minlimit, maxlimit);
      m_buttonMapping[ID_FIRSTDYN + i] = m_vm->getInputs()[i].first.c_str();
    }
  }

  m_Gui->Button(ID_PRINT, "print", "debug info" );

  m_Gui->Update();

  return m_Gui;
}

//----------------------------------------------------------------------------
bool mafVMEAFRefSys::AcceptLandmark(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMELandmark));
}


bool mafVMEAFRefSys::ConvertTextToVM()
{
  if(m_vm != NULL)
    delete m_vm;
  m_vm = new VecManVM<double>;
  for(unsigned i = 0; i < m_scriptText.size(); i++)
  {
    if(!m_vm->processString(m_scriptText[i].GetCStr()))
      return false;
  }
  for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
  {
    if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
    {
      m_lmMapping[m_vm->getInputs()[i].first.c_str()] = m_vm->getInputs()[i].first.c_str();
    }
    else
    {
      //actions for processing scalar inputs, actually there is nothing to do
    }
  }
  return true;
}

void mafVMEAFRefSys::SetScriptText(const std::vector<mafString>& script)
{
  m_scriptText = script;
  ConvertTextToVM();
}

void mafVMEAFRefSys::LoadScriptFromFile(const mafString& filename)
{
  FILE *fp = fopen(filename, "rt");
  if(fp == NULL)
  {
    return;
  }

  int const maxStrLen = 1000;
  char      sLine[maxStrLen];
  char      *pRet;
  m_scriptText.clear();

  while(true)
  {
    pRet = fgets(sLine, maxStrLen, fp);
    if(pRet == NULL)
      break;
    m_scriptText.push_back(mafString(pRet));
  }

  fclose(fp);
  ConvertTextToVM();
}

//-------------------------------------------------------------------------
void mafVMEAFRefSys::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
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
      GetOutput()->Update();
      mafEvent cam_event(this,CAMERA_UPDATE);
      ForwardUpEvent(cam_event);
      break;
    }
  case ID_SCALE_FACTOR:
    {
      wxLogMessage("ID_SCALE_FACTOR %f", m_ScaleFactor);
      SetScaleFactor(m_ScaleFactor);
      GetOutput()->Update();
      mafEvent cam_event(this,CAMERA_UPDATE);
      this->ForwardUpEvent(cam_event);
      Modified();
      break;
    }
  case ID_ACTIVE:
    {
      wxLogMessage("ID_ACTIVE %d", m_Active);
      SetActive(m_Active);
      break;
    }
  default:
    {
      if(maf_event->GetId() >= ID_FIRSTDYN)
      {
        std::map<int, mafString>::iterator itbtn = m_buttonMapping.find(maf_event->GetId());
        if(itbtn != m_buttonMapping.end())
        {
          std::map<mafString, mafString>::iterator itlm = m_lmMapping.find(itbtn->second);
          if(itlm != m_lmMapping.end())
          {
            mafString title = "Choose landmark";
            mafEvent e(this,VME_CHOOSE, &title);
            e.SetArg((long)&mafVMEAFRefSys::AcceptLandmark);
            e.SetString(&title);
            e.SetId(VME_CHOOSE);
            ForwardUpEvent(e);
            if(e.GetVme() != NULL)
            {
              itlm->second = e.GetVme()->GetName();
            }
          }
        }
        Modified();
        Update();
        GetOutput()->Update();
        mafEvent cam_event(this,CAMERA_UPDATE);
        ForwardUpEvent(cam_event);
        break;
      }
      Superclass::OnEvent(maf_event);
      break; 
    }
  }
}
//-------------------------------------------------------------------------
char **mafVMEAFRefSys::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-----------------------------------------------------------------------
void mafVMEAFRefSys::InternalPreUpdate()
//-----------------------------------------------------------------------
{
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
  Modified();
  InternalUpdate();
}

//-----------------------------------------------------------------------
void mafVMEAFRefSys::InternalUpdate()
//-----------------------------------------------------------------------
{
  mafVMELandmarkCloud *parentLMC = NULL;
  if(GetParent() != NULL)
    parentLMC = mafVMELandmarkCloud::SafeDownCast(GetParent());

  bool calculated = (parentLMC != NULL);

  if(calculated)
  {
    for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
    {
      if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
      {
        V3d<double> vec;
        std::map<mafString, mafString>::iterator it = m_lmMapping.find(m_vm->getInputs()[i].first.c_str());
        if(it == m_lmMapping.end())
        {
          calculated = false;
          break;
        }
        int  ind = parentLMC->FindLandmarkIndex(it->second.GetCStr());
        if(ind == -1)
        {
          calculated = false;
          break;
        }
        parentLMC->GetLandmark(ind, vec.val, -1);

        m_vm->getInputs()[i].second->GetVector() = vec;
      }
      else
      {
        //actions for processing scalar inputs, actually there is nothing to do
      }
    }
  }

  if (calculated)
    calculated = m_vm->execute();

  DiMatrix       mTran, mTrant, mTrano;
  vtkMatrix4x4  *mVTK = NULL;
  mafMatrix      mfMtr;
  DiV4d          pos, rot;

  V3d<double> x, y, z, p;
  if(calculated)
  {
    Param<double>* it = m_vm->getParam("X");
    if(it != NULL && it->GetType() == Param<double>::VECTOR )
      x = it->GetVector();
    else
      calculated = false;
  }
  if(calculated)
  {
    Param<double>* it = m_vm->getParam("Y");
    if(it != NULL && it->GetType() == Param<double>::VECTOR )
      y = it->GetVector();
    else
      calculated = false;
  }
  if(calculated)
  {
    Param<double>* it = m_vm->getParam("Z");
    if(it != NULL && it->GetType() == Param<double>::VECTOR )
      z = it->GetVector();
    else
      calculated = false;
  }
  if(calculated)
  {
    Param<double>* it = m_vm->getParam("P");
    if(it != NULL && it->GetType() == Param<double>::VECTOR )
      p = it->GetVector();
    else
      calculated = false;
  }

  if(calculated)
  {

    double xy = (x ^ y) * z;
    double xz = (z ^ x) * y;
    double yz = (y ^ z) * x;

    mTrant.vRight.x = x.x; mTrant.vRight.y = x.y; mTrant.vRight.z = x.z; mTrant.vRight.w = 0.0;
    mTrant.vUp.x    = y.x; mTrant.vUp.y    = y.y; mTrant.vUp.z    = y.z; mTrant.vUp.w    = 0.0;
    mTrant.vAt.x    = z.x; mTrant.vAt.y    = z.y; mTrant.vAt.z    = z.z; mTrant.vAt.w    = 0.0;
    mTrant.vPos.x   = p.x; mTrant.vPos.y   = p.y; mTrant.vPos.z   = p.z; mTrant.vPos.w   = 1.0;
  }
  else
  {
    DiMatrixIdentity(&mTrant);
    DiMatrixIdentity(&mTrano);
  }


  pos.x = (float)m_XOffset;
  pos.y = (float)m_YOffset;
  pos.z = (float)m_ZOffset;
  pos.w = 1;
  rot.x = (float)m_XRotate * (diPI / 180.0);
  rot.y = (float)m_YRotate * (diPI / 180.0);
  rot.z = (float)m_ZRotate * (diPI / 180.0);
  rot.w = 1;


  mafTransfComposeMatrixStright(&mTran, &rot, &pos);
  vtkNEW(mVTK);
  {
    x.x = mTran.vRight.x; x.y = mTran.vRight.y; x.z = mTran.vRight.z;
    y.x = mTran.vUp.x   ; y.y = mTran.vUp.y   ; y.z = mTran.vUp.z   ;
    z.x = mTran.vAt.x   ; z.y = mTran.vAt.y   ; z.z = mTran.vAt.z   ;
    p.x = mTran.vPos.x  ; p.y = mTran.vPos.y  ; p.z = mTran.vPos.z  ;

    double xy = (x ^ y) * z;
    double xz = (z ^ x) * y;
    double yz = (y ^ z) * x;
  }



  mafTransfRightLeftConv(&mTrant, &mTrano);
  mafTransfRightLeftConv(&mTran, &mTrant);
  DiMatrixMultiply(&mTrant, &mTrano, &mTran);
  DiMatrixCopy(&mTran, &mTrant);

  mVTK->Identity();
  DiMatrixToVTK(&mTrant, mVTK);
  SetMatrix(mVTK);
  vtkDEL(mVTK);


  SetScaleFactor(m_ScaleFactor);
  Modified();
}
