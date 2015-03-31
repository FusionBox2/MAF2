/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEBSplineLine.cpp,v $
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

#include "mafVMEBSplineLine.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEOutputPolyline.h"
#include "mafGUI.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
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


/*unsigned CalcFibrePoints(mafVMELandmarkCloud *fbr, mafTimeStamp ts, int& minVal, int& maxVal)
{
  std::vector<V3d<double> > output;
  V3d<double> x;
  bool tendon = true;
  output.clear();
  for(wxInt32 nI = 0; nI < fbr->GetNumberOfLandmarks(); nI++)
  {
    fbr->GetLandmark(nI, x.components, ts);
    if(output.size() == 0 || ((output[output.size() - 1] | x) > 4.0))
      output.push_back(x);
    else
    {
      tendon = !tendon;
      if(!tendon)
      {
        output.clear();
        minVal = nI;
      }
      else
      {
        maxVal = nI;
        break;
      }
    }
  }
  return maxVal - minVal;
}*/

unsigned FillSeq(mafVMELandmarkCloud *fbr, mafTimeStamp ts, bool filter, bool extract, std::vector<int>& output)//int& minVal, int& maxVal)
{
  V3d<double> x, lastAdded;
  bool tendon = true;
  output.clear();
  for(wxInt32 nI = 0; nI < fbr->GetNumberOfLandmarks(); nI++)
  {
    if(!filter && fbr->GetLandmarkVisibility(nI, ts))
    {
      output.push_back(nI);
      continue;
    }
    if(!fbr->GetLandmarkVisibility(nI, ts))
      continue;
    fbr->GetLandmark(nI, x.components, ts);
    if(output.size() == 0 || ((lastAdded | x) > 4.0))
    {
      output.push_back(nI);
      lastAdded = x;
    }
    else if(extract)
    {
      tendon = !tendon;
      if(!tendon)
      {
        output.clear();
      }
      else
      {
        break;
      }
    }
  }
  return output.size();
}

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEBSplineLine)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafString mafVMEBSplineLine::GetVisualPipe()
//-------------------------------------------------------------------------
{
  return mafString("mafPipePolyline");
}


//-------------------------------------------------------------------------
mafVMEBSplineLine::mafVMEBSplineLine()
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
  dpipe->SetInput(m_Polyline);

  m_PointsCloudName = _("");

  m_BCurve       = NULL;
  m_TesselSize   = 10;
  m_Order        = 4;
  m_Mode         = BCurve<double>::SPT_MODE_INTERP;
  m_SubMode      = BCurve<double>::SPT_MODE_2NDDER;
  m_Smooth       = 0.0;
  m_FilterIdent  = true;
  m_ExtractFibre = false;
  //m_MaxVal       = 0;
  //m_MinVal       = 0;
  m_SrcSeq.clear();
  m_Loaded = false;
}
//-------------------------------------------------------------------------
mafVMEBSplineLine::~mafVMEBSplineLine()
//-------------------------------------------------------------------------
{
  if(m_BCurve)
    delete m_BCurve;
  vtkDEL(m_Polyline);
  mafDEL(m_Transform);
  mafDEL(m_TmpTransform);
  SetOutput(NULL);
}
//-------------------------------------------------------------------------
int mafVMEBSplineLine::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEBSplineLine *splinePolyline = mafVMEBSplineLine::SafeDownCast(a);
    mafNode *linked_node = splinePolyline->GetPointsCloudLink();
    if (linked_node)
    {
      this->SetPointsCloudLink(linked_node);
    }
    
    m_Transform->SetMatrix(splinePolyline->m_Transform->GetMatrix());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_Polyline);
      m_Polyline->Update();
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEBSplineLine::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEBSplineLine *)vme)->m_Transform->GetMatrix() && \
          GetPointsCloudLink() == ((mafVMEBSplineLine *)vme)->GetPointsCloudLink();
  }
  return ret;
}


//-------------------------------------------------------------------------
mafVMEOutputPolyline *mafVMEBSplineLine::GetPolylineOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEBSplineLine::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEBSplineLine::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVMEBSplineLine::IsDataAvailable()
//-------------------------------------------------------------------------
{
  if(GetPointsCloudLink())
    return GetPointsCloudLink()->IsDataAvailable();
  else
    return false;
}

//-------------------------------------------------------------------------
void mafVMEBSplineLine::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void mafVMEBSplineLine::InternalUpdate() //Multi
//-----------------------------------------------------------------------
{
  if(m_Loaded)
  {
    m_Loaded = false;
    BuildCurve();
  }
  if(m_BCurve == NULL)
    return;
  mafVMELandmarkCloud *vme = GetPointsCloudLink();
  if(vme == NULL) 
    return;
  vme->Update();

  mafTimeStamp ts = GetTimeStamp();

  Array1D<V3d<double> > src, control;
  std::vector<V3d<double> >  result;

  m_TmpTransform->SetMatrix(*vme->GetOutput()->GetAbsMatrix());

  src.SetDimension(m_SrcSeq.size());
  for(unsigned i = 0; i < m_SrcSeq.size(); i++)
  {
    vme->GetLandmark(m_SrcSeq[i], src(i).components, ts);
  }
  /*src.SetDimension(m_MaxVal - m_MinVal);
  for(unsigned i = MinVal; i < m_MaxVal; i++)
  {
    V3d<double> vec;
    vme->GetLandmark(i, vec.components, ts);
    //if(i == m_MinVal || (src[src.size() - 1] | vec > 4.0))
      src[i - m_MinVal] = vec;
  }*/
  for(unsigned i = 0; i < src.GetNumElems(); i++)
  {
    m_TmpTransform->TransformPoint(src(i).components, src(i).components);
  }

  if(m_Mode == 2 && m_Smooth != 0.0)
  {
    for(int j = 0; j < 3; j++)
    {
      //arrays to process separately all components of 3D vectors
      ForArray<double> x;
      ForArray<double> y;
      ForArray<double> s;
      ForArray<double> z;
      ForArray<double> a;

      //initial sizes
      x.assign(src.GetDimension(),0.0);
      y.assign(src.GetDimension(),0.0);
      s.assign(src.GetDimension(),0.0);
      z.assign(src.GetDimension(),0.0);
      a.assign(src.GetDimension(),0.0);

      //filling with values from input parameters
      for(unsigned k = 0; k < src.GetDimension(); k++)
      {
        x[k] = k;
        y[k] = src(k)[j];
        s[k] = m_Smooth;
      }

      //building smoothing 1D splines for each vector component
      double tmp[3];
      Smspline<double>(src.GetDimension(), 2, 0, x, y, s, 0, 0, a, z, 0, tmp[0], tmp[1], tmp[2]);
      for(unsigned k = 0; k < src.GetDimension(); k++)
      {
        src(k)[j] = a[k];
      }
    }
  }

  m_BCurve->GetCPoints(src, control);
  m_BCurve->UpdateCurve(control, result);



  vtkPolyData *polyline = m_Polyline;

  vtkPoints    *newPts;
  vtkCellArray *newCells;
  newPts = vtkPoints::New();
  newPts->Allocate(5000,10000);
  newCells = vtkCellArray::New();
  newCells->Allocate(10000,20000);

  int pointId[2];
  for(unsigned i = 0; i < result.size(); i++)
  {
    newPts->InsertNextPoint(result[i].components);
    if (i == 0)
      continue;
    pointId[0] = i - 1;
    pointId[1] = i;
    newCells->InsertNextCell(2 , pointId);
  }

  polyline->SetPoints(newPts);
  polyline->Update();
  polyline->SetLines(newCells);
  polyline->Modified();
  polyline->Update();
  newPts->Delete();
  newCells->Delete();

  Modified();
}
//-----------------------------------------------------------------------
void mafVMEBSplineLine::InternalPreUpdate()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
int mafVMEBSplineLine::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix("Transform",&m_Transform->GetMatrix());
    parent->StoreInteger("Order", m_Order);
    parent->StoreInteger("Mode", m_Mode);
    parent->StoreInteger("Submode", m_SubMode);
    parent->StoreDouble("Smooth", m_Smooth);
    parent->StoreInteger("Num_parts", m_TesselSize);
    parent->StoreInteger("Filter_ident", m_FilterIdent);
    parent->StoreInteger("Extract_fibre", m_ExtractFibre);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEBSplineLine::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      node->RestoreInteger("Order", m_Order);
      node->RestoreInteger("Mode", m_Mode);
      node->RestoreInteger("Submode", m_SubMode);
      node->RestoreDouble("Smooth", m_Smooth);
      node->RestoreInteger("Num_parts", m_TesselSize);
      node->RestoreInteger("Filter_ident", m_FilterIdent);
      node->RestoreInteger("Extract_fibre", m_ExtractFibre);
      m_Loaded = true;
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void mafVMEBSplineLine::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** mafVMEBSplineLine::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-------------------------------------------------------------------------
void mafVMEBSplineLine::SetPointsCloudLink(mafNode *n)
//-------------------------------------------------------------------------
{
  SetLink("PointsCloud", n);
}
//-------------------------------------------------------------------------
mafVMELandmarkCloud *mafVMEBSplineLine::GetPointsCloudLink()
//-------------------------------------------------------------------------
{
  return mafVMELandmarkCloud::SafeDownCast(GetLink("PointsCloud"));
}
//-------------------------------------------------------------------------
mafGUI* mafVMEBSplineLine::CreateGui()
//-------------------------------------------------------------------------
{
  const wxString mode_choices_string[] = {_("Direct"),_("Maximal influence"), _("Interpolate")};
  const wxString submode_choices_string[] = {_("1st derivative"),_("2nd derivative"), _("Periodic")};

  mafID sub_id = -1;

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();


  mafVME *polyline_vme = GetPointsCloudLink();
  m_PointsCloudName = polyline_vme ? polyline_vme->GetName() : _("none");
  m_Gui->Button(ID_PNTS_CLOUD_LINK,&m_PointsCloudName,_("Points"), _("Select the Points cloud to create the Spline"));
  m_Gui->Label("Points: ", &m_PointsCloudName);
  m_Gui->Integer(ID_ORDER, _("Order"), &m_Order, 1, 10);
  m_Gui->Combo(ID_MODE, _("Mode"), &m_Mode, DIM(mode_choices_string), mode_choices_string);
  m_Gui->Combo(ID_SUBMODE, _("Submode"), &m_SubMode, DIM(submode_choices_string), submode_choices_string);
  m_Gui->FloatSlider(ID_SMOOTH, _("Smooth"),&m_Smooth, 0.0, 1000.0);
  m_Gui->Integer(ID_NUMTESSEL, _("Num parts"), &m_TesselSize, 2, 500);

  m_Gui->Enable(ID_SMOOTH, m_Mode == 2);
  m_Gui->Bool(ID_FILTER, _("Filter ident"), &m_FilterIdent);
  m_Gui->Bool(ID_EXTRACT, _("Extract fibre"), &m_ExtractFibre);
  m_Gui->Enable(ID_EXTRACT, m_FilterIdent != 0);
  m_Gui->Enable(ID_SUBMODE, m_Mode == BCurve<double>::SPT_MODE_INTERP);

  m_Gui->Update();
  //this->InternalUpdate();

  return m_Gui;
}

//-------------------------------------------------------------------------
bool mafVMEBSplineLine::PolylineAccept(mafNode *node)
//-------------------------------------------------------------------------
{
  return(node != NULL && node->IsA("mafVMELandmarkCloud") && (mafVMELandmarkCloud::SafeDownCast(node)->GetNumberOfLandmarks() >= 4));
}


//-------------------------------------------------------------------------
void mafVMEBSplineLine::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_SUBMODE:
      {
        if(m_BCurve)
        {
          m_BCurve->SetSpatSubMode(m_SubMode);
          InternalUpdate();
          Modified();
          GetPolylineOutput()->Update();
          mafEvent cam_event(this,CAMERA_UPDATE);
          ForwardUpEvent(cam_event);
          m_Gui->Update();
        }
        break;
      }
      case ID_NUMTESSEL:
        if(m_BCurve)
          m_BCurve->SetUniformTessel(m_TesselSize);//no break here! execution should continue to SMOOTH
      case ID_SMOOTH:
      {
        if(m_BCurve)
        {
          InternalUpdate();
          Modified();
          GetPolylineOutput()->Update();
          mafEvent cam_event(this,CAMERA_UPDATE);
          ForwardUpEvent(cam_event);
          m_Gui->Update();
        }
        break;
      }
      case ID_MODE:
      case ID_EXTRACT:
      case ID_FILTER:
      case ID_ORDER:
      case ID_PNTS_CLOUD_LINK:
        {
          mafVMELandmarkCloud *vme = NULL;
          if(e->GetId() == ID_MODE)
          {
            vme = GetPointsCloudLink();
            m_Gui->Enable(ID_SUBMODE, m_Mode == BCurve<double>::SPT_MODE_INTERP);
            m_Gui->Enable(ID_SMOOTH, m_Mode == 2);
            if(m_Mode == 2)
            {
              m_Order = 4;
              m_Gui->Update();
            }
          }
          else if(e->GetId() == ID_EXTRACT || e->GetId() == ID_FILTER)
          {
            vme = GetPointsCloudLink();
            m_Gui->Enable(ID_EXTRACT, m_FilterIdent != 0);
          }
          else if(e->GetId() == ID_ORDER)
          {
            vme = GetPointsCloudLink();
            if(m_Mode == 2 && m_Order != 4)
            {
              m_Mode = 0;
              m_Gui->Enable(ID_SMOOTH, m_Mode == 2);
              m_Gui->Update();
            }
          }
          else if(e->GetId() == ID_PNTS_CLOUD_LINK)
          {
            mafID button_id = e->GetId();
            mafString title = _("Choose vme");
            e->SetId(VME_CHOOSE);
            e->SetArg((long)&mafVMEBSplineLine::PolylineAccept);
            e->SetString(&title);
            ForwardUpEvent(e);
            vme = mafVMELandmarkCloud::SafeDownCast(e->GetVme());
            if(vme != NULL)
            {
              SetPointsCloudLink(vme);
              m_PointsCloudName = vme->GetName();
            }
          }
          if(vme)
          {
            FillSeq(vme, vme->GetTimeStamp(), m_FilterIdent != 0, m_ExtractFibre != 0, m_SrcSeq);
            unsigned numPnt = m_SrcSeq.size();
            delete m_BCurve;
            if(m_Mode == 2)
            {
              int supl = (m_Mode == 2) ? 2 : 0;
              m_BCurve = new BCurve<double>(m_Order, numPnt + supl + m_Order);
            }
            else
            {
              std::vector<double> keyvalues;
              for(unsigned i = 0; i < m_Order; i++)
                keyvalues.push_back(0.0);//(0.000001 * i);
              for(unsigned i = 1; i <= numPnt - m_Order; i++)
                keyvalues.push_back(i);
              for(unsigned i = 0; i < m_Order; i++)
                keyvalues.push_back((numPnt - m_Order + 1)/* - 0.000001 * (m_Order - 1 - i)*/);
              BBasis<double> curvebasis(m_Order, keyvalues);
              m_BCurve  = new BCurve<double>(curvebasis);
            }
            m_BCurve->SetUniformTessel(m_TesselSize);
            m_BCurve->SetSpatMode(m_Mode);
            InternalUpdate();
            Modified();
            GetPolylineOutput()->Update();
            mafEvent cam_event(this,CAMERA_UPDATE);
            ForwardUpEvent(cam_event);
            m_Gui->Update();
          }
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

void mafVMEBSplineLine::BuildCurve()
{
  mafVMELandmarkCloud *vme = GetPointsCloudLink();
  if(m_Gui)
  {
    m_Gui->Enable(ID_SUBMODE, m_Mode == BCurve<double>::SPT_MODE_INTERP);
    m_Gui->Enable(ID_SMOOTH, m_Mode == 2);
    m_Gui->Enable(ID_EXTRACT, m_FilterIdent != 0);
  }
  if(m_Mode == 2)
  {
    m_Order = 4;
    if(m_Gui)
    {
      m_Gui->Update();
    }
  }
  if(m_Mode == 2 && m_Order != 4)
  {
    m_Mode = 0;
    if(m_Gui)
    {
      m_Gui->Enable(ID_SMOOTH, m_Mode == 2);
      m_Gui->Update();
    }
  }
  if(vme)
  {
    FillSeq(vme, vme->GetTimeStamp(), m_FilterIdent != 0, m_ExtractFibre != 0, m_SrcSeq);
    unsigned numPnt = m_SrcSeq.size();
    delete m_BCurve;
    if(m_Mode == 2)
    {
      int supl = (m_Mode == 2) ? 2 : 0;
      m_BCurve = new BCurve<double>(m_Order, numPnt + supl + m_Order);
    }
    else
    {
      std::vector<double> keyvalues;
      for(unsigned i = 0; i < m_Order; i++)
        keyvalues.push_back(0.0);//(0.000001 * i);
      for(unsigned i = 1; i <= numPnt - m_Order; i++)
        keyvalues.push_back(i);
      for(unsigned i = 0; i < m_Order; i++)
        keyvalues.push_back((numPnt - m_Order + 1)/* - 0.000001 * (m_Order - 1 - i)*/);
      BBasis<double> curvebasis(m_Order, keyvalues);
      m_BCurve  = new BCurve<double>(curvebasis);
    }
    m_BCurve->SetUniformTessel(m_TesselSize);
    m_BCurve->SetSpatMode(m_Mode);
    InternalUpdate();
    Modified();
    GetPolylineOutput()->Update();
    mafEvent cam_event(this,CAMERA_UPDATE);
    ForwardUpEvent(cam_event);
    if(m_Gui)
    {
      m_Gui->Update();
    }
  }

}

//-------------------------------------------------------------------------
int mafVMEBSplineLine::InternalInitialize()
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
mmaMaterial *mafVMEBSplineLine::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
  }
  return material;
}
