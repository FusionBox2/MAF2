/*=========================================================================

 Program: MAF2
 Module: mafVMERefSys
 Authors: Marco Petrone, Paolo Quadrani, Stefano Perticoni
 
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


#include "mafVMERefSys.h"
#include "mafGUI.h"

#include "mafTagArray.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafVMELandmarkCloud.h"
#include "mafMatrix.h"

#include "vtkMath.h"
#include "vtkMatrix4x4.h"

const bool DEBUG_MODE = false;

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMERefSys)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMERefSys::mafVMERefSys()
//-------------------------------------------------------------------------
{
  m_Radio = 0;
  //m_Fixed = 0;
}

//-------------------------------------------------------------------------
mafVMERefSys::~mafVMERefSys()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int mafVMERefSys::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMERefSys::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------
int mafVMERefSys::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
//    parent->StoreInteger("Fixed", m_Fixed);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMERefSys::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
//      node->RestoreInteger("Fixed", m_Fixed);
      return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
mafGUI* mafVMERefSys::CreateGui()
//-------------------------------------------------------------------------
{
  mafID sub_id = -1;

  m_Gui = Superclass::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);

  mafString choises[3]={"Normal","Select Origin","Select Plane"};
  m_Gui->Radio(ID_RADIO,"",&m_Radio,3,choises);

  mafVME *origin_vme = GetOriginVME();
  if (origin_vme && origin_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("OriginVME");
    m_OriginVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)origin_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_OriginVmeName = origin_vme ? origin_vme->GetName() : _("none");
  m_Gui->Button(ID_REF_SYS_ORIGIN,&m_OriginVmeName,_("Origin"), _("Select the vme originABSPosition for the ref sys"));
  m_Gui->Enable(ID_REF_SYS_ORIGIN,origin_vme!=NULL);

  mafVME *point1_vme = GetPoint1VME();
  if (point1_vme && point1_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("Point1VME");
    m_Point1VmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)point1_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_Point1VmeName = point1_vme ? point1_vme->GetName() : _("none");
  m_Gui->Button(ID_POINT1,&m_Point1VmeName,_("Point 1"), _("Select the Point 1"));
  m_Gui->Enable(ID_POINT1,point1_vme!=NULL);

  mafVME *point2_vme = GetPoint2VME();
  if (point2_vme && point2_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId("Point2VME");
    m_Point2VmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)point2_vme)->GetLandmarkName(sub_id) : _("none");
  }
  else
    m_Point2VmeName = point2_vme ? point2_vme->GetName() : _("none");
  m_Gui->Button(ID_POINT2,&m_Point2VmeName,_("Point 2"), _("Select the Point 2"));
  m_Gui->Enable(ID_POINT2,point2_vme!=NULL);

  if(point2_vme && origin_vme && point2_vme)
    m_Radio=2;
  else if(origin_vme)
    m_Radio=1;
  else
    m_Radio=0;

  // vme ref sys fixed
//  m_Gui->Bool(ID_FIXED, _("Click for fix the refsys"), &m_Fixed, 1);

  /*m_Gui->Enable(ID_SCALE_FACTOR, m_Fixed == 0);
  m_Gui->Enable(ID_REF_SYS_ORIGIN, m_Fixed == 0);
  m_Gui->Enable(ID_POINT1, m_Fixed == 0);
  m_Gui->Enable(ID_POINT2, m_Fixed == 0);
  m_Gui->Enable(ID_RADIO, m_Fixed == 0);
  m_Gui->Enable(ID_FIXED, m_Fixed == 0);*/

  m_Gui->Update();
  //this->InternalUpdate();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMERefSys::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_REF_SYS_ORIGIN:
      case ID_POINT1:
      case ID_POINT2:
      {
        mafID button_id = e->GetId();
        mafString title = _("Choose vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafVMERefSys::LandmarkAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
          if (button_id == ID_REF_SYS_ORIGIN)
          {
            SetRefSysLink("OriginVME", n);
            m_OriginVmeName = n->GetName();
          }
          else if (button_id == ID_POINT1)
          {
            SetRefSysLink("Point1VME", n);
            m_Point1VmeName = n->GetName();
          }
          else
          {
            SetRefSysLink("Point2VME", n);
            m_Point2VmeName = n->GetName();
          }
          InternalUpdate();
          m_Gui->Update();
        }
        mafEvent cam_event(this,CAMERA_UPDATE);
        ForwardUpEvent(cam_event);
      }
      break;
      case ID_RADIO:
      {
        if(m_Radio==0)
        {
          // Normal RefSys
          this->RemoveAllLinks();
          m_OriginVmeName = _("none");
          m_Point1VmeName = _("none");
          m_Point2VmeName = _("none");
          m_Gui->Enable(ID_REF_SYS_ORIGIN,false);
          m_Gui->Enable(ID_POINT1,false);
          m_Gui->Enable(ID_POINT2,false);
        }
        else if(m_Radio==1)
        {
          // RefSys with Origin link
          this->RemoveLink("Point1VME");
          this->RemoveLink("Point2VME");
          m_Point1VmeName = _("none");
          m_Point2VmeName = _("none");
          m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
          m_Gui->Enable(ID_POINT1,false);
          m_Gui->Enable(ID_POINT2,false);
        }
        else if(m_Radio==2)
        {
          // RefSys with all the link enabled: originABSPosition, point1ABSPosition and point2ABSPosition
          m_Gui->Enable(ID_REF_SYS_ORIGIN,true);
          m_Gui->Enable(ID_POINT1,true);
          m_Gui->Enable(ID_POINT2,true);
        }
        InternalUpdate();
        m_Gui->Update();
        mafEvent cam_event(this,CAMERA_UPDATE);
        ForwardUpEvent(cam_event);
      }
      break;
      case ID_FIXED:
      {
        /*m_Gui->Enable(ID_SCALE_FACTOR, m_Fixed == 0);
        m_Gui->Enable(ID_REF_SYS_ORIGIN, m_Fixed == 0);

        m_Gui->Enable(ID_POINT1, m_Fixed == 0);
        m_Gui->Enable(ID_POINT2, m_Fixed == 0);
        m_Gui->Enable(ID_RADIO, m_Fixed == 0);

        m_Gui->Enable(ID_FIXED, m_Fixed == 0);*/
      }
      break;
      default:
        Superclass::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-----------------------------------------------------------------------
void mafVMERefSys::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
}
//-----------------------------------------------------------------------
void mafVMERefSys::CalculateMatrix(mafMatrix &m, mafTimeStamp ts)
//-----------------------------------------------------------------------
{
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Entering InternalUpdate for: " << this->GetName() << " VME"  << std::endl;
    mafLogMessage(stringStream.str().c_str());
  }
  if(ts < 0)
    ts = GetTimeStamp();
  mafVME *point1VME = GetPoint1VME();
  mafVME *point2VME = GetPoint2VME();
  mafVME *originVME = GetOriginVME();

  if(point1VME && point2VME && originVME)
  {
    double point1ABSPosition[3],point2ABSPosition[3],originABSPosition[3],useless[3];
    mafSmartPointer<mafTransform> TmpTransform;

    //Get the position of the originABSPosition
    if(originVME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("OriginVME") != -1)
    {
      ((mafVMELandmarkCloud *)originVME)->GetLandmarkPosition(GetLinkSubId("OriginVME"), originABSPosition, ts);
      mafTransform t;
      t.SetMatrix(*originVME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(originABSPosition, originABSPosition);
    
    }
    else if(originVME->IsMAFType(mafVMELandmark))
    {
      originVME->GetOutput()->Update();  
      originVME->GetOutput()->GetAbsPose(originABSPosition, useless, ts);
    }

    if (DEBUG_MODE)
    {
      LogVector3(originABSPosition, "originABSPosition abs position");
    }


    //Get the position of the point 1
    if(point1VME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("Point1VME") != -1)
    {
      ((mafVMELandmarkCloud *)point1VME)->GetLandmarkPosition(GetLinkSubId("Point1VME"),point1ABSPosition,ts);
      mafTransform t;
      t.SetMatrix(*point1VME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(point1ABSPosition, point1ABSPosition);
    }
    else if(point1VME->IsMAFType(mafVMELandmark))
    {
      point1VME->GetOutput()->Update();  
      point1VME->GetOutput()->GetAbsPose(point1ABSPosition, useless, ts);
    }
    if (DEBUG_MODE)
    {
      LogVector3(point1ABSPosition, "point1ABSPosition abs position");
    }

    //Get the position of the point 2
    if(point2VME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("Point2VME") != -1)
    {
      ((mafVMELandmarkCloud *)point2VME)->GetLandmarkPosition(GetLinkSubId("Point2VME"),point2ABSPosition,ts);
      mafTransform t;
      t.SetMatrix(*point2VME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(point2ABSPosition, point2ABSPosition);
    }
    else if(point2VME->IsMAFType(mafVMELandmark))
    {
      point2VME->GetOutput()->Update();  
      point2VME->GetOutput()->GetAbsPose(point2ABSPosition, useless,ts);
    }
    if (DEBUG_MODE)
    {
      LogVector3(point1ABSPosition, "point2ABSPosition abs position");
    }

    double point1OriginVector[3],point2OriginVector[3],point1Point2CrossProductVector[3];

    point1OriginVector[0] = point1ABSPosition[0] - originABSPosition[0];
    point1OriginVector[1] = point1ABSPosition[1] - originABSPosition[1];
    point1OriginVector[2] = point1ABSPosition[2] - originABSPosition[2];

    point2OriginVector[0] = point2ABSPosition[0] - originABSPosition[0];
    point2OriginVector[1] = point2ABSPosition[1] - originABSPosition[1];
    point2OriginVector[2] = point2ABSPosition[2] - originABSPosition[2];

    vtkMath::Normalize(point1OriginVector);
    vtkMath::Normalize(point2OriginVector);

    vtkMath::Cross(point1OriginVector,point2OriginVector,point1Point2CrossProductVector);
    vtkMath::Normalize(point1Point2CrossProductVector);
    vtkMath::Cross(point1Point2CrossProductVector,point1OriginVector,point2OriginVector);

    vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
    matrix_translation->Identity();
    for(int i=0;i<3;i++)
      matrix_translation->SetElement(i,3,originABSPosition[i]);

    vtkMatrix4x4 *matrix_rotation=vtkMatrix4x4::New();
    matrix_rotation->Identity();
    for(int i=0;i<3;i++)
      matrix_rotation->SetElement(i,0,point1OriginVector[i]);
    for(int i=0;i<3;i++)
      matrix_rotation->SetElement(i,1,point2OriginVector[i]);
    for(int i=0;i<3;i++)
      matrix_rotation->SetElement(i,2,point1Point2CrossProductVector[i]);

    mafMatrix a;
    a.SetVTKMatrix(matrix_rotation);
    mafMatrix b;
    b.SetVTKMatrix(matrix_translation);
    mafMatrix::Multiply4x4(b,a,m);

    vtkDEL(matrix_rotation);
    vtkDEL(matrix_translation);
    return;
  }
  if(originVME)
  {
    double origin[3],orientation[3];

    //Get the position of the origin
    if(originVME->IsMAFType(mafVMELandmarkCloud) && GetLinkSubId("OriginVME") != -1)
    {
      ((mafVMELandmarkCloud *)originVME)->GetLandmarkPosition(GetLinkSubId("OriginVME"),origin,ts);
      mafTransform t;
      t.SetMatrix(*originVME->GetOutput()->GetAbsMatrix());
      t.TransformPoint(origin,origin);
    }
    else if(originVME->IsMAFType(mafVMELandmark))
    {
      originVME->GetOutput()->Update();  
      originVME->GetOutput()->GetAbsPose(origin, orientation, ts);
    }

    vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
    matrix_translation->Identity();
    for(int i=0;i<3;i++)
      matrix_translation->SetElement(i,3,origin[i]);

    m.SetVTKMatrix(matrix_translation);

    vtkDEL(matrix_translation);
    return;
  }
  {
    vtkMatrix4x4 *matrix_translation=vtkMatrix4x4::New();
    matrix_translation->Identity();

    m.SetVTKMatrix(matrix_translation);
    vtkDEL(matrix_translation);
    return;
  }
}
//-------------------------------------------------------------------------
mafVME *mafVMERefSys::GetPoint1VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("Point1VME"));
}
//-------------------------------------------------------------------------
mafVME *mafVMERefSys::GetPoint2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("Point2VME"));
}
//-------------------------------------------------------------------------
mafVME *mafVMERefSys::GetOriginVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("OriginVME"));
}

void mafVMERefSys::LogVector3( double *vector , const char *logMessage /*= NULL*/ )
{
  std::ostringstream stringStream;
  if (logMessage)stringStream << logMessage << std::endl;
  stringStream << "Vector components: [" << vector[0] << " , " << vector[1] << " , " << vector[2] << " ]" << std::endl;
  stringStream << "Vector module: " << vtkMath::Norm(vector) << std::endl; 

  mafLogMessage(stringStream.str().c_str());
}

void mafVMERefSys::LogPoint3( double *point, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  stringStream << "Point coordinates: [" << point[0] << " , " << point[1] << " , " << point[2] << " ]" << std::endl;
  mafLogMessage(stringStream.str().c_str());
}

void mafVMERefSys::LogMAFMatrix4x4( mafMatrix *mat, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  mat->Print(stringStream);
  mafLogMessage(stringStream.str().c_str());
}

void mafVMERefSys::LogVTKMatrix4x4( vtkMatrix4x4 *mat, const char *logMessage )
{
  std::ostringstream stringStream;
  if (logMessage) stringStream << logMessage << std::endl;
  mat->Print(stringStream);
  mafLogMessage(stringStream.str().c_str());
}
