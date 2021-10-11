/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAFRefSys.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.7 $
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

#include "mafGUI.h"
#include "mafVMELandmarkCloud.h"
#include "mmaMaterial.h"
#include "mafTransform.h"
#include "mafIndent.h"

#include "mafTagArray.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"
#include "mafStorageElement.h"
#include "mafMatrix3x3.h"
#include "mafJointAnalysis.h"

#ifndef DIM
#define DIM(a)  (sizeof((a)) / sizeof(*(a)))
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEAFRefSys)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEAFRefSys::mafVMEAFRefSys()
//-------------------------------------------------------------------------
{
  m_vm      = NULL;
  m_Active  = 0;
  m_VMValid = false;
  m_BoneID  = ID_AFS_NOTDEFINED;
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
    m_XOffset = vme_ref_sys->m_XOffset;
    m_YOffset = vme_ref_sys->m_YOffset;
    m_ZOffset = vme_ref_sys->m_ZOffset;
    m_XRotate = vme_ref_sys->m_XRotate;
    m_YRotate = vme_ref_sys->m_YRotate;
    m_ZRotate = vme_ref_sys->m_ZRotate;
    m_scriptText = vme_ref_sys->m_scriptText;
    ConvertTextToVM(false);
    m_lmMapping = vme_ref_sys->m_lmMapping;
    m_BoneID    = vme_ref_sys->m_BoneID;
    m_Active    = vme_ref_sys->m_Active;
    for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
    {
      if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
      {
      }
      else
      {
        m_vm->getInputs()[i].second->GetScalar() = vme_ref_sys->m_vm->getInputs()[i].second->GetScalar();
      }
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEAFRefSys::SetParent(mafNode *newparent)
//-------------------------------------------------------------------------
{
  int rep = Superclass::SetParent(newparent);
  if(rep != MAF_OK || newparent == NULL || !m_Active)
    return rep;
  for(unsigned j = 0; j < newparent->GetNumberOfChildren(); j++)
  {
    mafVMEAFRefSys *sys = mafVMEAFRefSys::SafeDownCast(newparent->GetChild(j));
    if(sys == NULL || sys == this)
      continue;
    sys->SetActive(false);
  }
  return MAF_OK;
}


//----------------------------------------------------------------------------
void mafVMEAFRefSys::Print(std::ostream& os, const int tabs)// const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
  os<<indent<<"6DOFs: "<<indent<<m_XOffset<<indent<<m_YOffset<<indent<<m_ZOffset<<indent<<m_XRotate<<indent<<m_YRotate<<indent<<m_ZRotate;
  os<<indent<<"Scale: "<<indent<<m_ScaleFactor;
}



//-----------------------------------------------------------------------
int mafVMEAFRefSys::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger(_R("Active"), m_Active);
    parent->StoreInteger(_R("BoneID"), m_BoneID);
    parent->StoreDouble(_R("XOffset"), m_XOffset);
    parent->StoreDouble(_R("YOffset"), m_YOffset);
    parent->StoreDouble(_R("ZOffset"), m_ZOffset);
    parent->StoreDouble(_R("XRotate"), m_XRotate);
    parent->StoreDouble(_R("YRotate"), m_YRotate);
    parent->StoreDouble(_R("ZRotate"), m_ZRotate);
    m_textSize = m_scriptText.size();
    parent->StoreInteger(_R("ScriptStrings"), m_textSize);
    for(int i = 0; i < m_textSize; i++)
    {
      mafString nm = mafString::Format(_R("ln%d"), i);
      parent->StoreText(nm, m_scriptText[i]);
    }

    for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
    {
      if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
      {
        auto it = m_lmMapping.find(_R(m_vm->getInputs()[i].first.c_str()));
        if(it == m_lmMapping.end())
        {
          parent->StoreText(_R(m_vm->getInputs()[i].first.c_str()), _R(m_vm->getInputs()[i].first.c_str()));
        }
        else
        {
          parent->StoreText(it->first, it->second);
        }
      }
      else
      {
        parent->StoreDouble(_R(m_vm->getInputs()[i].first.c_str()), m_vm->getInputs()[i].second->GetScalar());
      }
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}


void mafVMEAFRefSys::SetBoneID(int ID)
{
  m_BoneID = ID;
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
    //if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      node->RestoreInteger(_R("Active"), m_Active);
      node->RestoreInteger(_R("BoneID"), m_BoneID);
      node->RestoreDouble(_R("XOffset"), m_XOffset);
      node->RestoreDouble(_R("YOffset"), m_YOffset);
      node->RestoreDouble(_R("ZOffset"), m_ZOffset);
      node->RestoreDouble(_R("XRotate"), m_XRotate);
      node->RestoreDouble(_R("YRotate"), m_YRotate);
      node->RestoreDouble(_R("ZRotate"), m_ZRotate);
      node->RestoreInteger(_R("ScriptStrings"), m_textSize);
      m_scriptText.resize(m_textSize);
      for(int i = 0; i < m_textSize; i++)
      {
        mafString nm = mafString::Format(_R("ln%d"), i);
        node->RestoreText(nm, m_scriptText[i]);
      }
      ConvertTextToVM(false);
      for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
      {
        if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
        {
          mafString tmp;
          node->RestoreText(_R(m_vm->getInputs()[i].first.c_str()), tmp);
          m_lmMapping[_R(m_vm->getInputs()[i].first.c_str())] = tmp;
        }
        else
        {
          node->RestoreDouble(_R(m_vm->getInputs()[i].first.c_str()), m_vm->getInputs()[i].second->GetScalar());
        }
      }
      SetScaleFactor(m_ScaleFactor);
      return MAF_OK;
    }
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mafGUI* mafVMEAFRefSys::CreateGui()
//-------------------------------------------------------------------------
{
	const mafString bone_choices_string[] = {_L("Undefined"),_L("Pelvis"), _L("Right thigh"), _L("Left thigh"), _L("Right shank"), _L("Left shank"), _L("Right foot"), _L("Left foot")};
  m_Gui = Superclass::CreateGui();
  m_Gui->Show(false);

  m_Gui->Bool(ID_ACTIVE, _L("Active"), &m_Active);
  m_Gui->Divider();

  m_Gui->FloatSlider(ID_X_OFFSET, _R("X offset"),&m_XOffset, -1000.0, 1000.0);
  m_Gui->FloatSlider(ID_Y_OFFSET, _R("Y offset"),&m_YOffset, -1000.0, 1000.0);
  m_Gui->FloatSlider(ID_Z_OFFSET, _R("Z offset"),&m_ZOffset, -1000.0, 1000.0);
  m_Gui->FloatSlider(ID_X_ROTATE, _R("X rotate"),&m_XRotate, -180.0, 180.0);
  m_Gui->FloatSlider(ID_Y_ROTATE, _R("Y rotate"),&m_YRotate, -180.0, 180.0);
  m_Gui->FloatSlider(ID_Z_ROTATE, _R("Z rotate"),&m_ZRotate, -180.0, 180.0);
  m_Gui->Divider();

  for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
  {
    if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
    {
      auto it = m_lmMapping.find(_R(m_vm->getInputs()[i].first.c_str()));
      if(it == m_lmMapping.end())
        continue;
      m_Gui->Button(ID_FIRSTDYN + i, it->first, _R(""), _R("Press to modify"));
      m_Gui->Label(it->first, &(it->second));
      m_buttonMapping[ID_FIRSTDYN + i] = _R(m_vm->getInputs()[i].first.c_str());
    }
    else
    {
      double minlimit = (m_vm->getInputs()[i].second->IsDnLimited()) ? m_vm->getInputs()[i].second->GetDnLimit() : MINDOUBLE;
      double maxlimit = (m_vm->getInputs()[i].second->IsUpLimited()) ? m_vm->getInputs()[i].second->GetUpLimit() : MAXDOUBLE;
      if(m_vm->getInputs()[i].second->IsDnLimited() && m_vm->getInputs()[i].second->IsUpLimited())
        m_Gui->FloatSlider(ID_FIRSTDYN + i, _R(m_vm->getInputs()[i].first.c_str()), &(m_vm->getInputs()[i].second->GetScalar()), minlimit, maxlimit);
      else
        m_Gui->Double(ID_FIRSTDYN + i, _R(m_vm->getInputs()[i].first.c_str()), &(m_vm->getInputs()[i].second->GetScalar()), minlimit, maxlimit);
      m_buttonMapping[ID_FIRSTDYN + i] = _R(m_vm->getInputs()[i].first.c_str());
    }
  }

  m_Gui->Combo(ID_SELECT_BONEID, _R("Bone ID"), &m_BoneID, DIM(bone_choices_string), bone_choices_string);

  m_Gui->Update();

  return m_Gui;
}

bool mafVMEAFRefSys::ConvertTextToVM(bool buildMapping)
{
  if(m_vm != NULL)
    delete m_vm;
  m_vm = new VecManVM<double>;
  for(unsigned i = 0; i < m_scriptText.size(); i++)
  {
    if(!m_vm->ProcessString(m_scriptText[i].GetCStr()))
      return false;
  }
  for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
  {
    if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
    {
      if(buildMapping)
      {
        m_lmMapping[_R(m_vm->getInputs()[i].first.c_str())] = _R(m_vm->getInputs()[i].first.c_str());
        SetRefSysLink(m_vm->getInputs()[i].first.c_str(), GetParent());
      }
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
  ConvertTextToVM(true);
}

void mafVMEAFRefSys::LoadScriptFromFile(const mafString& filename)
{
  FILE *fp = fopen(filename.GetCStr(), "rt");
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
    m_scriptText.push_back(mafString(_R(pRet)));
  }

  fclose(fp);
  ConvertTextToVM(true);
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
  case ID_SELECT_BONEID:
    break;
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
            mafString title = _R("Choose landmark");
            mafEvent e(this,VME_CHOOSE, &title);
            e.SetArg((intptr_t)&mafVMERefSysAbstract::LandmarkAccept);
            e.SetString(&title);
            e.SetId(VME_CHOOSE);
            ForwardUpEvent(e);
            mafNode *n = e.GetVme();
            if(n != NULL)
            {
              itlm->second = n->GetName();
              SetRefSysLink(itlm->first.GetCStr(), n->GetParent());
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
  InternalUpdateMatrix();
}

//-----------------------------------------------------------------------
bool mafVMEAFRefSys::UpdateVM(mafTimeStamp ts)
//-----------------------------------------------------------------------
{
  mafVMELandmarkCloud *parentLMC = mafVMELandmarkCloud::SafeDownCast(GetParent());
  if(m_vm == NULL)
    return false;
  m_VMValid = false;
  m_VMTime  = ts;
  m_vm->Preexecute();
  for(unsigned i = 0; i < m_vm->getInputs().size(); i++)
  {
    if(m_vm->getInputs()[i].second->GetType() == Param<double>::VECTOR)
    {
      V3d<double>         vec;
      mafVMELandmarkCloud *lmcLink = NULL;
      int                 ind      = -1;
      {
        auto it = m_lmMapping.find(_R(m_vm->getInputs()[i].first.c_str()));
        if(it == m_lmMapping.end())
          continue;
        mafVMELandmarkCloud *tmpLink = mafVMELandmarkCloud::SafeDownCast(GetLink(_R(m_vm->getInputs()[i].first.c_str())));
        lmcLink = (tmpLink != NULL) ? tmpLink : parentLMC;
        if(lmcLink != NULL)
        {
          SetRefSysLink(m_vm->getInputs()[i].first.c_str(), lmcLink);
          mafString refname;
          refname = it->second.Lower();
          int numberOfLandmarks = lmcLink->GetNumberOfLandmarks();
          for(int i = 0; i < numberOfLandmarks; i++)
          {
            mafString lm_name = lmcLink->GetLandmarkName(i).Lower();
            if(lm_name == refname)
            {
              ind = i;
              break;
            }
          }
        }
      }
      if(lmcLink == NULL || ind == -1 || !lmcLink->GetLandmarkVisibility(ind, ts))
        continue;
      mafMatrix cloudAbs;
      double invec[4];
      double outvec[4];
      lmcLink->GetOutput()->GetAbsMatrix(cloudAbs, ts);
      lmcLink->GetLandmark(ind, vec.components, ts);
      for(unsigned indx = 0; indx < 3; indx++)
        invec[indx] = vec[indx];
      invec[3] = 1.0;
      cloudAbs.MultiplyPoint(invec, outvec);
      for(unsigned indx = 0; indx < 3; indx++)
        vec[indx] = outvec[indx];

      m_vm->getInputs()[i].second->GetVector() = vec;
    }
    else
    {
      //actions for processing scalar inputs, actually there is nothing to do
    }
    m_vm->getInputs()[i].second->GetValid()  = true;
  }

  bool result = m_vm->Execute();
  if(result)
    m_VMValid = true;
  return result;
}


//-----------------------------------------------------------------------
void mafVMEAFRefSys::CalculateMatrix(mafMatrix& mat, mafTimeStamp ts)
//-----------------------------------------------------------------------
{
  bool calculated = UpdateVM(ts);

  mafMatrix mTran, mTrant;
  V4d<double>    pos, rot;

  V3d<double> x, y, z, p;

  Param<double> *XParam = NULL;
  Param<double> *YParam = NULL;
  Param<double> *ZParam = NULL;
  Param<double> *PParam = NULL;

  if(m_vm)
  {
      XParam = m_vm->GetParam("X");
      YParam = m_vm->GetParam("Y");
      ZParam = m_vm->GetParam("Z");
      PParam = m_vm->GetParam("P");
  }

  if(XParam == NULL || XParam->GetType() != Param<double>::VECTOR || !XParam->IsValid())
    calculated = false;
  else 
    x = XParam->GetVector();
  if(YParam == NULL || YParam->GetType() != Param<double>::VECTOR || !YParam->IsValid())
    calculated = false;
  else 
    y = YParam->GetVector();
  if(ZParam == NULL || ZParam->GetType() != Param<double>::VECTOR || !ZParam->IsValid())
    calculated = false;
  else 
    z = ZParam->GetVector();
  if(PParam == NULL || PParam->GetType() != Param<double>::VECTOR || !PParam->IsValid())
    calculated = false;
  else 
    p = PParam->GetVector();

  std::pair<const char*, V3d<double>*> vects[4] = {std::make_pair("X", &x), std::make_pair("Y", &y), std::make_pair("Z", &z), std::make_pair("P", &p)};

  for(unsigned i = 0; i < 4 && calculated; i++)
  {
    if(!m_vm->GetVector(vects[i].first, *vects[i].second))
      calculated = false;
  }

  if(calculated)
  {
    mTrant.SetElement(0, 0, x.x);
    mTrant.SetElement(1, 0, x.y);
    mTrant.SetElement(2, 0, x.z);
    mTrant.SetElement(3, 0, 0.0);
    mTrant.SetElement(0, 1, y.x);
    mTrant.SetElement(1, 1, y.y);
    mTrant.SetElement(2, 1, y.z);
    mTrant.SetElement(3, 1, 0.0);
    mTrant.SetElement(0, 2, z.x);
    mTrant.SetElement(1, 2, z.y);
    mTrant.SetElement(2, 2, z.z);
    mTrant.SetElement(3, 2, 0.0);
    mTrant.SetElement(0, 3, p.x);
    mTrant.SetElement(1, 3, p.y);
    mTrant.SetElement(2, 3, p.z);
    mTrant.SetElement(3, 3, 1.0);
  }
  else
  {
    mTrant.Identity();
  }


  pos.x = m_XOffset;
  pos.y = m_YOffset;
  pos.z = m_ZOffset;
  pos.w = 1;
  rot.x = m_XRotate * mafMatrix3x3::DegreesToRadians();
  rot.y = m_YRotate * mafMatrix3x3::DegreesToRadians();
  rot.z = m_ZRotate * mafMatrix3x3::DegreesToRadians();
  rot.w = 1;


  mafTransfComposeMatrixStright(&mTran, &rot, &pos);
  mafMatrix::Multiply4x4(mTrant, mTran, mat);
  mat.SetTimeStamp(ts);
}

//-----------------------------------------------------------------------
bool mafVMEAFRefSys::GetVector(const char *name, mafTimeStamp ts, V3d<double>& output)
//-----------------------------------------------------------------------
{
  if(m_vm == NULL) 
    return false;
  Param<double> *tmp = NULL;

  UpdateVM(ts);

  tmp = m_vm->GetParam(name);

  if(tmp == NULL || tmp->GetType() != Param<double>::VECTOR || !tmp->IsValid())
    return false;
  output = tmp->GetVector();
  return true;
}

//-----------------------------------------------------------------------
bool mafVMEAFRefSys::GetScalar(const char *name, mafTimeStamp ts, double&  output)
//-----------------------------------------------------------------------
{
  if(m_vm == NULL) 
    return false;
  Param<double> *tmp = NULL;

  UpdateVM(ts);

  tmp = m_vm->GetParam(name);

  if(tmp == NULL || tmp->GetType() != Param<double>::SCALAR || !tmp->IsValid())
    return false;
  output = tmp->GetScalar();
  return true;
}
