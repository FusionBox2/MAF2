/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVMELMCLines.cpp,v $
  Language:  C++
  Date:      $Date: 2007-11-23 10:22:09 $
  Version:   $Revision: 1.9 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpVMELMCLines.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEOutputPolyline.h"
#include "mafGUI.h"
#include "mmaMaterial.h"

#include "vtkSmartPointer.h"
#include "vtkMAFDataPipe.h"
#include "vtkPolyData.h"
#include "vtkCardinalSpline.h"
#include "vtkPoints.h"
#include "vtkTransform.h"
#include "vtkCellArray.h"
#include "vtkTransformPolyDataFilter.h"

#include "splineobjs.h"
#include "splines.h"



#include <assert.h>

#ifndef DIM
#define DIM(a)  (sizeof((a)) / sizeof(*(a)))
#endif

//-------------------------------------------------------------------------
mafCxxTypeMacro(lhpVMELMCLines)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafString lhpVMELMCLines::GetVisualPipe()
//-------------------------------------------------------------------------
{
  return mafString(_R("mafPipePolyline"));
}


//-------------------------------------------------------------------------
lhpVMELMCLines::lhpVMELMCLines()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputPolyline *output=mafVMEOutputPolyline::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  DependsOnLinkedNodeOn();

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);

  mafNEW(m_TmpTransform);


  m_Polyline = NULL;
  vtkNEW(m_Polyline);
  dpipe->SetInputData(m_Polyline);

  m_Looped          = 0;
  m_PointsCloudName = _L("");
}
//-------------------------------------------------------------------------
lhpVMELMCLines::~lhpVMELMCLines()
//-------------------------------------------------------------------------
{
  vtkDEL(m_Polyline);
  mafDEL(m_Transform);
  mafDEL(m_TmpTransform);
  SetOutput(NULL);
}
//-------------------------------------------------------------------------
int lhpVMELMCLines::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    lhpVMELMCLines *splinePolyline = lhpVMELMCLines::SafeDownCast(a);
    mafNode *linked_node = splinePolyline->GetPointsCloudLink();
    if (linked_node)
    {
      this->SetPointsCloudLink(linked_node);
    }
    m_Looped = splinePolyline->m_Looped;
    m_Transform->SetMatrix(splinePolyline->m_Transform->GetMatrix());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInputData(m_Polyline);
      InternalUpdate();
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool lhpVMELMCLines::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((lhpVMELMCLines *)vme)->m_Transform->GetMatrix() && \
          GetPointsCloudLink() == ((lhpVMELMCLines *)vme)->GetPointsCloudLink();
  }
  return ret;
}


//-------------------------------------------------------------------------
mafVMEOutputPolyline *lhpVMELMCLines::GetPolylineOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void lhpVMELMCLines::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool lhpVMELMCLines::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool lhpVMELMCLines::IsDataAvailable()
//-------------------------------------------------------------------------
{
  if(GetPointsCloudLink())
    return GetPointsCloudLink()->IsDataAvailable();
  else
    return false;
}

//-------------------------------------------------------------------------
void lhpVMELMCLines::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void lhpVMELMCLines::InternalUpdate() //Multi
//-----------------------------------------------------------------------
{
  mafVMELandmarkCloud *vme = GetPointsCloudLink();

  mafTimeStamp ts = GetTimeStamp();

  std::vector<V3d<double> >  src;

  if(vme)
  {
    mafMatrix trf;
    vme->Update();
    vme->GetOutput()->GetAbsMatrix(trf, ts);
    m_TmpTransform->SetMatrix(trf);


    int NumLMs = vme->GetNumberOfLandmarks();
    src.resize(NumLMs);
    for(unsigned i = 0; i < NumLMs; i++)
    {
      vme->GetLandmark(i, src[i].components, ts);
    }
    if(m_Looped)
      src.push_back(src[0]);
    for(unsigned i = 0; i < src.size(); i++)
    {
      m_TmpTransform->TransformPoint(src[i].components, src[i].components);
    }

    m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
    m_TmpTransform->Invert();
    for(unsigned i = 0; i < src.size(); i++)
    {
      m_TmpTransform->TransformPoint(src[i].components, src[i].components);
    }
  }


  vtkPolyData *polyline = m_Polyline;

  vtkPoints    *newPts;
  vtkCellArray *newCells;
  newPts = vtkPoints::New();
  newPts->Allocate(5000,10000);
  newCells = vtkCellArray::New();
  newCells->Allocate(10000,20000);

  vtkIdType pointId[2];
  for(unsigned i = 0; i < src.size(); i++)
  {
    newPts->InsertNextPoint(src[i].components);
    if (i == 0)
      continue;
    pointId[0] = i - 1;
    pointId[1] = i;
    newCells->InsertNextCell(2 , pointId);
  }

  polyline->SetPoints(newPts);
  polyline->SetLines(newCells);
  newPts->Delete();
  newCells->Delete();

  Modified();
}
//-----------------------------------------------------------------------
void lhpVMELMCLines::InternalPreUpdate()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
int lhpVMELMCLines::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger(_R("Looped"), m_Looped);
    if(parent->StoreMatrix(_R("Transform"),m_Transform->GetMatrix())==MAF_OK)
      return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int lhpVMELMCLines::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    node->RestoreInteger(_R("Looped"), m_Looped);
    if (node->RestoreMatrix(_R("Transform"),matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void lhpVMELMCLines::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** lhpVMELMCLines::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-------------------------------------------------------------------------
void lhpVMELMCLines::SetPointsCloudLink(mafNode *n)
//-------------------------------------------------------------------------
{
  SetLink(_R("PointsCloud"), n);
}
//-------------------------------------------------------------------------
mafVMELandmarkCloud *lhpVMELMCLines::GetPointsCloudLink()
//-------------------------------------------------------------------------
{
  return mafVMELandmarkCloud::SafeDownCast(GetLink(_R("PointsCloud")));
}
//-------------------------------------------------------------------------
mafGUI* lhpVMELMCLines::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();


  mafVME *polyline_vme = GetPointsCloudLink();
  m_PointsCloudName = polyline_vme ? polyline_vme->GetName() : _L("none");
  m_Gui->Button(ID_PNTS_CLOUD_LINK,_L("Points"), _L("Select the Points cloud to create the Spline"));
  m_Gui->Label(_R("Points: "), &m_PointsCloudName);
  m_Gui->Bool(ID_LOOPED, _L("Looped"), &m_Looped);

  m_Gui->Update();
  //this->InternalUpdate();
  return m_Gui;
}

//-------------------------------------------------------------------------
bool lhpVMELMCLines::PolylineAccept(mafNode *node)
//-------------------------------------------------------------------------
{
  return(node != NULL && node->IsA("mafVMELandmarkCloud"));
}

void lhpVMELMCLines::SetCloud(mafVMELandmarkCloud *cloud)
{
  if(cloud)
  {
    SetPointsCloudLink(cloud);
    m_PointsCloudName = cloud->GetName();
  }
  else
  {
    RemoveLink(_R("PointsCloud"));
    m_PointsCloudName = _L("none");
  }

  InternalUpdate();
  Modified();
  GetPolylineOutput()->Update();
  mafEvent cam_event(this,CAMERA_UPDATE);
  ForwardUpEvent(cam_event);
  if(m_Gui)
    m_Gui->Update();
}

//-------------------------------------------------------------------------
void lhpVMELMCLines::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_PNTS_CLOUD_LINK:
        {
          mafVMELandmarkCloud *vme = NULL;
          {
            mafID button_id = e->GetId();
            mafString title = _L("Choose vme");
            e->SetId(VME_CHOOSE);
            e->SetArg((intptr_t)&lhpVMELMCLines::PolylineAccept);
            e->SetString(&title);
            ForwardUpEvent(e);
            vme = mafVMELandmarkCloud::SafeDownCast(e->GetVme());
            if(vme != NULL)
            {
              SetCloud(vme);
            }
          }
          break;
        }
      case ID_LOOPED:
        {
          Modified();
          Update();
          GetOutput()->Update();
          mafEvent cam_event(this,CAMERA_UPDATE);
          ForwardUpEvent(cam_event);
          break;
        }
      default:
      mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}

//-------------------------------------------------------------------------
int lhpVMELMCLines::InternalInitialize()
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
mmaMaterial *lhpVMELMCLines::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute(_R("MaterialAttributes"));
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute(_R("MaterialAttributes"), material);
  }
  return material;
}
