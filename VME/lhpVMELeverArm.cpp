/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVMELeverArm.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.35 $
  Authors:   Marco Petrone, Paolo Quadrani
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

#include "lhpVMELeverArm.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mmaMeter.h"
#include "mmaMaterial.h"
#include "mafEventSender.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mmuIdFactory.h"
#include "mafGUI.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFSmartPointer.h"
#include "mafRWI.h"
#include "mafGUIDialogPreview.h"

#include "vtkMAFDataPipe.h"
#include "vtkMath.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkLine.h"
#include "vtkLineSource.h"
#include "vtkAppendPolyData.h"
#include "vtkProbeFilter.h"
#include "vtkXYPlotActor.h"
#include "vtkTextProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkCellArray.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"


#include <assert.h>

MAF_ID_IMP(lhpVMELeverArm::LENGTH_THRESHOLD_EVENT);

//-------------------------------------------------------------------------
mafCxxTypeMacro(lhpVMELeverArm)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
lhpVMELeverArm::lhpVMELeverArm()
//-------------------------------------------------------------------------
{
  m_Distance      = -1.0;
  
  m_HAxisVmeName  = _R("");
  m_LineVmeName   = _R("");
  
  mafNEW(m_Transform);
  mafVMEOutputPolyline *output = mafVMEOutputPolyline::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  vtkNEW(m_LineSource);
  vtkNEW(m_Goniometer);
  vtkNEW(m_PolyData);
  
  m_StartPointGlobal[0] = 0;
  m_StartPointGlobal[1] = 0;
  m_StartPointGlobal[2] = 0;

  m_Goniometer->AddInput(m_LineSource->GetOutput());

  m_PolyData->DeepCopy(m_Goniometer->GetOutput());

  mafNEW(m_TmpTransform);

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_PolyData);
}
//-------------------------------------------------------------------------
lhpVMELeverArm::~lhpVMELeverArm()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  vtkDEL(m_LineSource);
  vtkDEL(m_Goniometer);
  mafDEL(m_TmpTransform);
  vtkDEL(m_PolyData);
  SetOutput(NULL);
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    lhpVMELeverArm *meter = lhpVMELeverArm::SafeDownCast(a);
    mafNode *linked_node = mafNode::SafeDownCast(meter->GetLink(_R("HAxisVME")));
    if (linked_node)
    {
      this->SetLink(_R("HAxisVME"), linked_node);
    }
    linked_node = mafNode::SafeDownCast(meter->GetLink(_R("LineVME")));
    if (linked_node)
    {
      this->SetLink(_R("LineVME"), linked_node);
    }
    m_Transform->SetMatrix(meter->m_Transform->GetMatrix());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_Goniometer->GetOutput());
      m_Goniometer->Update();
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool lhpVMELeverArm::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((lhpVMELeverArm *)vme)->m_Transform->GetMatrix() && \
          GetLink(_R("HAxisVME")) == ((lhpVMELeverArm *)vme)->GetLink(_R("HAxisVME")) && \
          GetLink(_R("LineVME")) == ((lhpVMELeverArm *)vme)->GetLink(_R("LineVME"));
  }
  return ret;
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::InternalInitialize()
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
mmaMaterial *lhpVMELeverArm::GetMaterial()
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
//-------------------------------------------------------------------------
mafVMEOutputPolyline *lhpVMELeverArm::GetPolylineOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputPolyline *)GetOutput();
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}
//-------------------------------------------------------------------------
bool lhpVMELeverArm::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}
//-----------------------------------------------------------------------
void lhpVMELeverArm::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes();
}
//-----------------------------------------------------------------------
void lhpVMELeverArm::InternalUpdate()
//-----------------------------------------------------------------------
{
  GetMeterAttributes()->m_ThresholdEvent = GetGenerateEvent();
  GetMeterAttributes()->m_DeltaPercent   = GetDeltaPercent();
  GetMeterAttributes()->m_InitMeasure    = GetInitMeasure();

  double threshold = GetMeterAttributes()->m_InitMeasure * (1 + GetMeterAttributes()->m_DeltaPercent / 100.0);

  UpdateLinks();
  mafTimeStamp currTs = GetTimeStamp();

  //if (GetMeterMode() == lhpVMELeverArm::POINT_DISTANCE)
  {
    mafVME *haxis_vme = GetHAxisVME();
    mafVME *line_vme  = GetLineVME();

    bool haxis_ok = true, line_ok = true;

    V3d<double> pivot;
    V3d<double> haxis;
    std::vector<std::pair<V3d<double>, V3d<double> > > segments;
    if (haxis_vme && line_vme)
    {
      // start is a landmark, consider also visibility
      mafMatrix tmAxis;
      haxis_vme->GetOutput()->Update();
      haxis_vme->GetOutput()->GetAbsMatrix(tmAxis, currTs);
      pivot[0] = tmAxis.GetElement(0, 3);
      pivot[1] = tmAxis.GetElement(1, 3);
      pivot[2] = tmAxis.GetElement(2, 3);
      haxis[0] = tmAxis.GetElement(0, 2);
      haxis[1] = tmAxis.GetElement(1, 2);
      haxis[2] = tmAxis.GetElement(2, 2);

      mafMatrix tmLine;
      mafVMEOutputPolyline *line = mafVMEOutputPolyline::SafeDownCast(mafVME::SafeDownCast(line_vme)->GetOutput());
      line->Update();
      line->GetAbsMatrix(tmLine, currTs);
      line->GetPolylineData();

      mafSmartPointer<mafTransform> matr;
      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transf;
      matr->SetMatrix(tmLine);
      transf->SetInput(line->GetPolylineData());
      transf->SetTransform(matr->GetVTKTransform());
      transf->Update();


      vtkPoints    *pts;
      vtkCellArray *lines;
      vtkPolyData  *polyline = transf->GetOutput();
      lines = polyline->GetLines();
      pts   = polyline->GetPoints();

      vtkIdType npnts  = pts->GetNumberOfPoints();
      vtkIdType nlines = lines->GetNumberOfCells();



      std::vector<V3d<double> > coords;
      coords.resize(npnts);
      for(unsigned i = 0; i < npnts; i++)
      {
        pts->GetPoint(i, coords[i].components);
      }

      vtkIdType npts = 0;
      vtkIdType *indx = 0;

      for (lines->InitTraversal(); lines->GetNextCell(npts, indx); )
      {
        if(npts == 2)
        {
          segments.push_back(std::make_pair(coords[indx[0]], coords[indx[1]]));
        }
      }
    }
    else
    {
      haxis_ok = false;
      line_ok   = false;
    }

    if (haxis_ok && line_ok && segments.size() > 0)
    {
      V3d<double> origin = segments[0].first;
      V3d<double> direct = segments[0].second - segments[0].first;


      double alph = 0.0;
      double beta = 0.0;

      double hal2 = haxis.length2();
      double dil2 = direct.length2();
      if(hal2 > 1e-6 && dil2 > 1e-6)
      {
        haxis  /= sqrt(hal2);
        direct /= sqrt(dil2);

        double dircos = haxis * direct;
        if(fabs(dircos) < 1.0 - 1e-6)
        {
          beta  = (pivot - origin) * direct;
          beta += ((origin - pivot) * haxis) * dircos;
          beta /= 1 - dircos * dircos;
        }
        alph = (origin - pivot) * haxis + beta * dircos;
      }

      m_StartPoint = pivot + alph * haxis;
      m_EndPoint = origin + beta * direct;

      // compute distance between points
      m_Distance = sqrt(vtkMath::Distance2BetweenPoints(m_StartPoint.components, m_EndPoint.components));

      if(GetMeterMeasureType() == lhpVMELeverArm::RELATIVE_MEASURE)
        m_Distance -= GetMeterAttributes()->m_InitMeasure;

      // compute start point in local coordinate system
      double local_start[3];
      m_TmpTransform->SetMatrix(GetOutput()->GetAbsTransform()->GetMatrix());
      m_TmpTransform->Invert();
      m_TmpTransform->TransformPoint(m_StartPoint.components, local_start);  // m_TmpTransform needed to fix a memory leaks of GetInverse()
      //GetAbsMatrixPipe()->GetInverse()->TransformPoint(StartPoint,local_start);

      // compute end point in local coordinate system
      double local_end[3];
      m_TmpTransform->TransformPoint(m_EndPoint.components,local_end);

      m_StartPointGlobal[0] = local_start[0];
      m_StartPointGlobal[1] = local_start[1];
      m_StartPointGlobal[2] = local_start[2];

      m_LineSource->SetPoint1(local_start[0],local_start[1],local_start[2]);
      m_LineSource->SetPoint2(local_end[0],local_end[1],local_end[2]);
      m_LineSource->Update();
      m_Goniometer->Modified();
    }
    else
      m_Distance = -1;

    GetOutput()->Update();
    InvokeEvent(this, VME_OUTPUT_DATA_UPDATE);

    if(GetMeterMeasureType() == lhpVMELeverArm::ABSOLUTE_MEASURE && GetMeterAttributes()->m_ThresholdEvent > 0 && m_Distance >= 0 && m_Distance >= threshold)
      InvokeEvent(this,LENGTH_THRESHOLD_EVENT);
  }

  m_Goniometer->Update();
  vtkPolyData *polydata = m_Goniometer->GetOutput();
  int num = m_Goniometer->GetOutput()->GetNumberOfPoints();
  int pointId[2];
  vtkMAFSmartPointer<vtkCellArray> cellArray;
  for(int i = 0; i< num;i++)
  {
    if (i > 0)
    {             
      pointId[0] = i - 1;
      pointId[1] = i;
      cellArray->InsertNextCell(2 , pointId);  
    }
  }

  m_PolyData->SetPoints(m_Goniometer->GetOutput()->GetPoints());
  m_PolyData->SetLines(cellArray);
  m_PolyData->Update();

}
//-----------------------------------------------------------------------
int lhpVMELeverArm::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix(_R("Transform"),&m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int lhpVMELeverArm::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix(_R("Transform"),&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void lhpVMELeverArm::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
//char** lhpVMELeverArm::GetIcon() 
//-------------------------------------------------------------------------
//{
//  #include "lhpVMELeverArm.xpm"
//  return lhpVMELeverArm_xpm;
//}
//-------------------------------------------------------------------------
mmaMeter *lhpVMELeverArm::GetMeterAttributes()
//-------------------------------------------------------------------------
{
  mmaMeter *meter_attributes = (mmaMeter *)GetAttribute(_R("MeterAttributes"));
  if (meter_attributes == NULL)
  {
    meter_attributes = mmaMeter::New();
    SetAttribute(_R("MeterAttributes"), meter_attributes);
  }
  return meter_attributes;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetDistanceRange(double min, double max)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DistanceRange[0] = min;
  GetMeterAttributes()->m_DistanceRange[1] = max;
}
//-------------------------------------------------------------------------
double *lhpVMELeverArm::GetDistanceRange() 
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DistanceRange;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetMeterColorMode(int mode)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_ColorMode = mode;
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::GetMeterColorMode()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_ColorMode;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetMeterMeasureType(int type)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_MeasureType = type;
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::GetMeterMeasureType()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_MeasureType;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetMeterRepresentation(int representation)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Representation = representation;
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::GetMeterRepresentation()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Representation;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetMeterCapping(int capping)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_Capping = capping;
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::GetMeterCapping()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_Capping;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetGenerateEvent(int generate)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_GenerateEvent = generate;
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::GetGenerateEvent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_GenerateEvent;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetInitMeasure(double init_measure)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_InitMeasure = init_measure;
}
//-------------------------------------------------------------------------
double lhpVMELeverArm::GetInitMeasure()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_InitMeasure;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetMeterRadius(double radius)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_TubeRadius = radius;
}
//-------------------------------------------------------------------------
double lhpVMELeverArm::GetMeterRadius()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_TubeRadius;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::SetDeltaPercent(int delta_percent)
//-------------------------------------------------------------------------
{
  GetMeterAttributes()->m_DeltaPercent = delta_percent;
}
//-------------------------------------------------------------------------
int lhpVMELeverArm::GetDeltaPercent()
//-------------------------------------------------------------------------
{
  return GetMeterAttributes()->m_DeltaPercent;
}
//-------------------------------------------------------------------------
double lhpVMELeverArm::GetDistance()
//-------------------------------------------------------------------------
{
  return m_Distance;
}
//-------------------------------------------------------------------------
mafGUI* lhpVMELeverArm::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();

  UpdateLinks();
  
  m_Gui->Button(ID_AXIS_LINK,&m_HAxisVmeName,_L("Axis"), _L("Select the helical axis for the lever arm"));
  m_Gui->Button(ID_LINE_LINK,&m_LineVmeName,_L("Line"), _L("Select the muscle curve for the lever arm"));

  m_Gui->Divider();
  InternalUpdate();
  GetPolylineOutput()->Update();

  return m_Gui;
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::UpdateLinks()
//-------------------------------------------------------------------------
{
  mafID sub_id = -1;
  mafVME *haxis_vme = GetHAxisVME();
  mafVME *line_vme = GetLineVME();

  if (haxis_vme && haxis_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId(_R("HAxisVME"));
    m_HAxisVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)haxis_vme)->GetLandmarkName(sub_id) : _L("none");
  }
  else
    m_HAxisVmeName = haxis_vme ? haxis_vme->GetName() : _L("none");

  if (line_vme && line_vme->IsMAFType(mafVMELandmarkCloud))
  {
    sub_id = GetLinkSubId(_R("LineVME"));
    m_LineVmeName = (sub_id != -1) ? ((mafVMELandmarkCloud *)line_vme)->GetLandmarkName(sub_id) : _L("none");
  }
  else
    m_LineVmeName = line_vme ? line_vme->GetName() : _L("none");
}
//-------------------------------------------------------------------------
void lhpVMELeverArm::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_AXIS_LINK:
      case ID_LINE_LINK:
      {
        mafID button_id = e->GetId();
        mafString title = _L("Choose lever arm vme link");
        e->SetId(VME_CHOOSE);
        if (button_id == ID_AXIS_LINK)
          e->SetArg((intptr_t)&lhpVMELeverArm::AxisAccept);
        else
          e->SetArg((intptr_t)&lhpVMELeverArm::LineAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL && n != this)
        {
          if (button_id == ID_AXIS_LINK)
          {
            SetMeterLink("HAxisVME", n);
            m_HAxisVmeName = n->GetName();
          }
          else //if (button_id == ID_LINE_LINK)
          {
            SetMeterLink("LineVME", n);
            m_LineVmeName = n->GetName();
          }
          m_Gui->Update();
          //InternalUpdate();
        }
      }
      break;
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
void lhpVMELeverArm::SetMeterLink(const char *link_name, mafNode *n)
//-------------------------------------------------------------------------
{
  if (n->IsMAFType(mafVMELandmark))
  {
    SetLink(_R(link_name),n->GetParent(),((mafVMELandmarkCloud *)n->GetParent())->FindLandmarkIndex(n->GetName()));
  }
  else
    SetLink(_R(link_name), n);
}
//-------------------------------------------------------------------------
mafVME *lhpVMELeverArm::GetHAxisVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(_R("HAxisVME")));
}
//-------------------------------------------------------------------------
mafVME *lhpVMELeverArm::GetLineVME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(_R("LineVME")));
}
