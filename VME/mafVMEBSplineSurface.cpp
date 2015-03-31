/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEBSplineSurface.cpp,v $
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

#include "mafVMEBSplineSurface.h"
#include "mafTransform.h"
#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafDataPipeCustom.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEGroup.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEBSplineLine.h"
#include "mafVMEOutputPolyline.h"

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


//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEBSplineSurface)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafString mafVMEBSplineSurface::GetVisualPipe()
//-------------------------------------------------------------------------
{
  return mafString("mafPipeSurface");
}

//-------------------------------------------------------------------------
bool mafVMEBSplineSurface::PolylineAccept(mafNode *node)
//-------------------------------------------------------------------------
{
  return(node != NULL && (node->IsA("mafVMEGroup")));
}

//-------------------------------------------------------------------------
mafVMEBSplineSurface::mafVMEBSplineSurface()
//-------------------------------------------------------------------------
{
  mafNEW(m_Transform);
  mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  DependsOnLinkedNodeOn();

  // attach a datapipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);

  mafNEW(m_TmpTransform);


  m_Polygons = NULL;
  vtkNEW(m_Polygons);
  dpipe->SetInput(m_Polygons);

  m_PointsGroupName = "";
  m_BSurface   = NULL;
  m_OrderX     = 4;
  m_OrderY     = 4;
  m_TesselX    = 10;
  m_TesselY    = 10;
  m_Mode       = BSurface<double>::SPT_MODE_INTERP;
  m_SubModeU   = BSurface<double>::SPT_MODE_2NDDER;
  m_SubModeV   = BSurface<double>::SPT_MODE_2NDDER;
  m_SmoothX    = 0.0;
  m_SmoothY    = 0.0;
  m_useBSLines = true;
}
//-------------------------------------------------------------------------
mafVMEBSplineSurface::~mafVMEBSplineSurface()
//-------------------------------------------------------------------------
{
  if(m_BSurface)
    delete m_BSurface;
  vtkDEL(m_Polygons);
  mafDEL(m_Transform);
  mafDEL(m_TmpTransform);
  SetOutput(NULL);
}
//-------------------------------------------------------------------------
int mafVMEBSplineSurface::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEBSplineSurface *splinePolyline = mafVMEBSplineSurface::SafeDownCast(a);
    mafNode *linked_node = splinePolyline->GetPointsGroupLink();
    if (linked_node)
    {
      this->SetPointsGroupLink(linked_node);
    }
    
    m_Transform->SetMatrix(splinePolyline->m_Transform->GetMatrix());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_Polygons);
      m_Polygons->Update();
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEBSplineSurface::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEBSplineSurface *)vme)->m_Transform->GetMatrix() && \
          GetPointsGroupLink() == ((mafVMEBSplineSurface *)vme)->GetPointsGroupLink();

  }
  return ret;
}


//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMEBSplineSurface::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEBSplineSurface::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEBSplineSurface::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVMEBSplineSurface::IsDataAvailable()
//-------------------------------------------------------------------------
{
  if(GetPointsGroupLink())
    return GetPointsGroupLink()->IsDataAvailable();
  else
    return false;
}

//-------------------------------------------------------------------------
void mafVMEBSplineSurface::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void mafVMEBSplineSurface::InternalUpdate() //Multi
//-----------------------------------------------------------------------
{
  //wxBusyCursor wait;

  mafVMEGroup *vme = mafVMEGroup::SafeDownCast(GetPointsGroupLink());
  if (vme == NULL) 
    return;
  std::vector<mafVMELandmarkCloud*>  clds;
  std::vector<mafVMEOutputPolyline*> plns;
  for(unsigned i= 0; i < vme->GetNumberOfChildren(); i++)
  {
    if(!m_useBSLines)
    {
      mafVMELandmarkCloud *cl = mafVMELandmarkCloud::SafeDownCast(vme->GetChild(i));
      if(cl)
      {
        cl->Update();
        clds.push_back(cl);
      }
    }
    else
    {
      mafVMEBSplineLine *bln = mafVMEBSplineLine::SafeDownCast(vme->GetChild(i));
      if(bln)
        bln->Update();
      mafVMEOutputPolyline *opln = (bln == NULL) ? NULL : bln->GetPolylineOutput();
      if(opln)
        plns.push_back(opln);
    }
  }

  Array2D<V3d<double> > src, control;
  std::vector<V3d<double> >  result;
  if(!m_useBSLines)
  {
    if(clds.size() < 2)
      return;
    for(unsigned i = 0; i < clds.size(); i++)
    {
      if(clds[i]->GetNumberOfLandmarks() < 2 || clds[i]->GetNumberOfLandmarks() != clds[0]->GetNumberOfLandmarks())
        return;
    }

    mafTimeStamp ts = GetTimeStamp();


    src.SetDimension(clds[0]->GetNumberOfLandmarks(), clds.size());

    for(unsigned i = 0; i < clds[0]->GetNumberOfLandmarks(); i++)
    {
      for(unsigned j = 0; j < clds.size(); j++)
      {
        V3d<double> pnt;
        clds[j]->GetLandmark(i, pnt.components, ts);
        m_TmpTransform->SetMatrix(*clds[j]->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(pnt.components, pnt.components);
        src(i, j) = pnt;
      }
    }
  }
  else
  {
    if(plns.size() < 2)
      return;
    for(unsigned i = 0; i < plns.size(); i++)
    {
      if(plns[i]->GetPolylineData()->GetNumberOfPoints() < 2 || plns[i]->GetPolylineData()->GetNumberOfPoints() != plns[0]->GetPolylineData()->GetNumberOfPoints())
        return;
    }

    mafTimeStamp ts = GetTimeStamp();

    src.SetDimension(plns[0]->GetPolylineData()->GetNumberOfPoints(), plns.size());

    for(unsigned i = 0; i < plns[0]->GetPolylineData()->GetNumberOfPoints(); i++)
    {
      for(unsigned j = 0; j < plns.size(); j++)
      {
        V3d<double> pnt;
        plns[j]->GetPolylineData()->GetPoints()->GetPoint(i, pnt.components);
        //m_TmpTransform->SetMatrix(*clds[j]->GetOutput()->GetAbsMatrix());
        //m_TmpTransform->TransformPoint(pnt.components, pnt.components);
        src(i, j) = pnt;
      }
    }
  }
  if(m_Mode == 2 && (m_SmoothX != 0.0 && m_SmoothY != 0.0))
  {
    for(unsigned k = 0; k < 3; k++)
    {
      //set of working arrays for each 3D vector component
      ForArray<double> x;
      ForArray<double> y;
      ForArray<double> z;
      ForArray<double> zl;
      ForArray<double> zr;
      ForArray<double> zu;
      ForArray<double> zd;
      ForArray<double> zxy;
      ForArray<double> rho;
      ForArray<double> sgm;
      ForArray<double> zx;
      ForArray<double> zy;
      //assert(gridValue.size() == numlines * numpnts);

      x.assign(src.GetDimension().x, 0.0);
      y.assign(src.GetDimension().y, 0.0);
      z.assign(src.GetDimension().x * src.GetDimension().y, 0.0);
      zl.assign(src.GetDimension().y, 0.0);
      zr.assign(src.GetDimension().y, 0.0);
      zu.assign(src.GetDimension().x, 0.0);
      zd.assign(src.GetDimension().x, 0.0);
      zxy.assign(src.GetDimension().x * src.GetDimension().y, 0.0);
      rho.assign(src.GetDimension().x, 0.0);
      sgm.assign(src.GetDimension().y, 0.0);
      zx.assign(src.GetDimension().x * src.GetDimension().y, 0.0);
      zy.assign(src.GetDimension().x * src.GetDimension().y, 0.0);

      //initialization with input params
      for(unsigned i = 0; i < src.GetDimension().x; i++)
      {
        x[i] = i;
        zu[i] = 0.;
        zd[i] = 0.;
        rho[i] = m_SmoothX;
      }
      for(unsigned i = 0; i < src.GetDimension().y; i++)
      {
        y[i] = i;
        zl[i] = 0.;
        zr[i] = 0.;
        sgm[i] = m_SmoothY;
      }
      {
        unsigned count = 0;
        for(unsigned j = 0; j < src.GetDimension().y; j++)
        {
          for(unsigned i = 0; i < src.GetDimension().x; i++)
          {
            z[count] = src(i, j)[k];
          }
        }
        assert(count == z.size());
      }
      for(int i = 0; i < 4; i++)
        zxy[i] = 0.;

      //creating smoothing 2D splines for each 3D vector component
      double      argx = 0;
      double      argy = 0;
      double      resval;
      //Splint2(numpnts, x[k], (unsigned)numlines, y[k], z[k], zl[k], zr[k], zu[k], zd[k], zx[k], zy[k], zxy[k], 
      //        0, 3, 2, argx, argy, resval[k]);
      Smspl2(src.GetDimension().x, x, src.GetDimension().y, y, z, zl, zr, zu, zd, rho, sgm, zx, zy, zxy, 0, 2, 2, argx, argy, resval);
      {
        unsigned count = 0;
        for(unsigned j = 0; j < src.GetDimension().y; j++)
        {
          for(unsigned i = 0; i < src.GetDimension().x; i++)
          {
            src(i, j)[k] = z[count];
          }
        }
        assert(count == z.size());
      }
    }
  }
  m_BSurface->GetCPoints(src, control);
  m_BSurface->UpdateSurface(control, result);


  vtkPolyData *polygons = m_Polygons;

  vtkPoints    *newPts;
  vtkCellArray *newCells;
  newPts = vtkPoints::New();
  newPts->Allocate(5000,10000);
  newCells = vtkCellArray::New();
  newCells->Allocate(10000,20000);


  for(unsigned i = 0; i < result.size(); i++)
  {
    newPts->InsertNextPoint(result[i].components);
  }

  int pointId[3];
  for(unsigned i = 1; i < m_TesselX; i++)
  {
    for(unsigned j = 0; j < m_TesselY - 1; j++)
    {
      pointId[0] = m_TesselY * i + j;
      pointId[1] = m_TesselY * (i - 1) + j + 1;
      pointId[2] = m_TesselY * (i - 1) + j;
      newCells->InsertNextCell(3 , pointId);
      pointId[0] = m_TesselY * i + j;
      pointId[1] = m_TesselY * i + j + 1;
      pointId[2] = m_TesselY * (i - 1) + j + 1;
      newCells->InsertNextCell(3 , pointId);
    }
  }


  /*{
    int pointId[3];
    for(unsigned i = 0; i < clds[0]->GetNumberOfLandmarks(); i++)
    {
      for(unsigned j = 0; j < clds.size(); j++)
      {
        double coord[3];
        clds[j]->GetLandmark(i, coord, ts);
        m_TmpTransform->SetMatrix(*clds[j]->GetOutput()->GetAbsMatrix());
        m_TmpTransform->TransformPoint(coord, coord);
        newPts->InsertNextPoint(coord);
      }
      if (i > 0)
      {
        for(unsigned j = 0; j < clds.size() - 1; j++)
        {
          pointId[0] = clds.size() * i + j;
          pointId[1] = clds.size() * (i - 1) + j + 1;
          pointId[2] = clds.size() * (i - 1) + j;
          newCells->InsertNextCell(3 , pointId);
          pointId[0] = clds.size() * i + j;
          pointId[1] = clds.size() * i + j + 1;
          pointId[2] = clds.size() * (i - 1) + j + 1;
          newCells->InsertNextCell(3 , pointId);
        }
      }
    }
  }*/

  polygons->SetPoints(newPts);
  polygons->Update();
  polygons->SetPolys(newCells);
  polygons->Modified();
  polygons->Update();
  newPts->Delete();
  newCells->Delete();

  Modified();
}
//-----------------------------------------------------------------------
void mafVMEBSplineSurface::InternalPreUpdate()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
int mafVMEBSplineSurface::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    if(parent->StoreMatrix("Transform",&m_Transform->GetMatrix())==MAF_OK)
      return MAF_OK;
  }
  return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEBSplineSurface::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix);
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void mafVMEBSplineSurface::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** mafVMEBSplineSurface::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-------------------------------------------------------------------------
void mafVMEBSplineSurface::SetPointsGroupLink(mafNode *n)
//-------------------------------------------------------------------------
{
  SetLink("PointsGroup", n);
}
//-------------------------------------------------------------------------
void mafVMEBSplineSurface::SetElemLink(mafNode *n, int index)
//-------------------------------------------------------------------------
{
  char nm[50];
  sprintf(nm, "Elem%d", index);
  SetLink(nm, n);
}
//-------------------------------------------------------------------------
mafVME *mafVMEBSplineSurface::GetPointsGroupLink()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink("PointsGroup"));
}
//-------------------------------------------------------------------------
mafGUI* mafVMEBSplineSurface::CreateGui()
//-------------------------------------------------------------------------
{
  const mafString mode_choices_string[] = {_("Direct"),_("Maximal influence"), _("Interpolate")};
  const mafString submode_choices_string[] = {_("1st derivative"),_("2nd derivative"), _("Periodic")};

  mafID sub_id = -1;

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();


  mafVME *polyline_vme1 = GetPointsGroupLink();
  m_PointsGroupName = polyline_vme1 ? polyline_vme1->GetName() : _("none");
  m_Gui->Button(ID_PNTS_GROUP_LINK,&m_PointsGroupName,_("Points"), _("Select the Points cloud to create the Spline"));
  m_Gui->Integer(ID_ORDERX, _("Order X"), &m_OrderX, 1, 10);
  m_Gui->Integer(ID_ORDERY, _("Order Y"), &m_OrderY, 1, 10);
  m_Gui->Combo(ID_MODE, _("Mode"), &m_Mode, DIM(mode_choices_string), mode_choices_string);
  m_Gui->Combo(ID_SUBMODEU, _("Submode X"), &m_SubModeU, DIM(submode_choices_string), submode_choices_string);
  m_Gui->Combo(ID_SUBMODEV, _("Submode Y"), &m_SubModeV, DIM(submode_choices_string), submode_choices_string);

  m_Gui->Bool(ID_USEBSLINES, _("Use BSLines"), &m_useBSLines);

  m_Gui->FloatSlider(ID_SMOOTHX, _("Smooth X"),&m_SmoothX, 0.0, 1000.0);
  m_Gui->FloatSlider(ID_SMOOTHY, _("Smooth Y"),&m_SmoothY, 0.0, 1000.0);

  m_Gui->Integer(ID_NUMTESSELX, _("Num parts X"), &m_TesselX, 2, 500);
  m_Gui->Integer(ID_NUMTESSELY, _("Num parts Y"), &m_TesselY, 2, 500);

  m_Gui->Enable(ID_SMOOTHX, m_Mode == 2);
  m_Gui->Enable(ID_SMOOTHY, m_Mode == 2);

  m_Gui->Enable(ID_SUBMODEU, m_Mode == BCurve<double>::SPT_MODE_INTERP);
  m_Gui->Enable(ID_SUBMODEV, m_Mode == BCurve<double>::SPT_MODE_INTERP);

  m_Gui->Update();
  //this->InternalUpdate();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEBSplineSurface::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_SUBMODEU:
    case ID_SUBMODEV:
      {
        if(m_BSurface)
        {
          m_BSurface->SetSpatSubModeU(m_SubModeU);
          m_BSurface->SetSpatSubModeV(m_SubModeV);
          InternalUpdate();
          Modified();
          mafEvent cam_event(this,CAMERA_UPDATE);
          ForwardUpEvent(cam_event);
          m_Gui->Update();
        }
        break;
      }

    case ID_NUMTESSELX:
    case ID_NUMTESSELY:
      if(m_BSurface)
        m_BSurface->SetUniformTessel(m_TesselX, m_TesselY);
    case ID_SMOOTHX:
    case ID_SMOOTHY:
      {
        if(m_BSurface)
        {
          InternalUpdate();
          Modified();
          mafEvent cam_event(this,CAMERA_UPDATE);
          ForwardUpEvent(cam_event);
          m_Gui->Update();
        }
        break;
      }
    case ID_MODE:
    case ID_USEBSLINES:
    case ID_ORDERX:
    case ID_ORDERY:
    case ID_PNTS_GROUP_LINK:
      {
        mafVMEGroup *vme = NULL;
        if(e->GetId() == ID_MODE)
        {
          vme = mafVMEGroup::SafeDownCast(GetPointsGroupLink());
          m_Gui->Enable(ID_SMOOTHX, m_Mode == 2);
          m_Gui->Enable(ID_SMOOTHY, m_Mode == 2);

          m_Gui->Enable(ID_SUBMODEU, m_Mode == BCurve<double>::SPT_MODE_INTERP);
          m_Gui->Enable(ID_SUBMODEV, m_Mode == BCurve<double>::SPT_MODE_INTERP);

          if(m_Mode == 2)
          {
            m_OrderX = 4;
            m_OrderY = 4;
            m_Gui->Update();
          }
        }
        else if(e->GetId() == ID_ORDERX || e->GetId() == ID_ORDERY)
        {
          vme = mafVMEGroup::SafeDownCast(GetPointsGroupLink());
          if(m_Mode == 2 && (m_OrderX != 4 || m_OrderY != 4))
          {
            m_Mode = 0;
            m_Gui->Enable(ID_SMOOTHX, m_Mode == 2);
            m_Gui->Enable(ID_SMOOTHY, m_Mode == 2);
            m_Gui->Update();
          }
        }
        else if(e->GetId() == ID_USEBSLINES)
        {
          vme = mafVMEGroup::SafeDownCast(GetPointsGroupLink());
        }
        else if(e->GetId() == ID_PNTS_GROUP_LINK)
        {
          mafID button_id = e->GetId();
          mafString title = _("Choose vme");
          e->SetId(VME_CHOOSE);
          e->SetArg((long)&mafVMEBSplineSurface::PolylineAccept);
          e->SetString(&title);
          ForwardUpEvent(e);
          vme = mafVMEGroup::SafeDownCast(e->GetVme());
          if(vme != NULL)
          {
            SetPointsGroupLink(vme);
            m_PointsGroupName = vme->GetName();
          }
        }
        if(vme != NULL)
        {
          std::vector<mafVMELandmarkCloud*>  clds;
          std::vector<mafVMEOutputPolyline*> plns;
          for(unsigned i= 0; i < vme->GetNumberOfChildren(); i++)
          {
            if(!m_useBSLines)
            {
              mafVMELandmarkCloud *cl = mafVMELandmarkCloud::SafeDownCast(vme->GetChild(i));
              if(cl)
              {
                cl->Update();
                clds.push_back(cl);
                SetElemLink(cl, clds.size() - 1);
              }
            }
            else
            {
              mafVMEBSplineLine *bln = mafVMEBSplineLine::SafeDownCast(vme->GetChild(i));
              if(bln)
                bln->Update();
              mafVMEOutputPolyline *opln = (bln == NULL) ? NULL : bln->GetPolylineOutput();
              if(opln)
              {
                plns.push_back(opln);
                SetElemLink(bln, plns.size() - 1);
              }
            }
          }
          delete m_BSurface;
          int NumPntsX = (m_useBSLines) ? plns[0]->GetPolylineData()->GetNumberOfPoints(): clds[0]->GetNumberOfLandmarks();
          int NumPntsY = (m_useBSLines) ? plns.size() : clds.size();
          if(m_Mode == 2)
          {
            int supl = (m_Mode == 2) ? 2 : 0;
            m_BSurface = new BSurface<double>(m_OrderX, m_OrderX + NumPntsX + supl, m_OrderY, m_OrderY + NumPntsY + supl);
          }
          else
          {
            std::vector<double> keyvaluesU;
            std::vector<double> keyvaluesV;

            for(unsigned i = 0; i < m_OrderX; i++)
              keyvaluesU.push_back(0.0);//(0.000001 * i);
            for(unsigned i = 1; i <= NumPntsX - m_OrderX; i++)
              keyvaluesU.push_back(i);
            for(unsigned i = 0; i < m_OrderX; i++)
              keyvaluesU.push_back((NumPntsX - m_OrderX + 1)/* - 0.000001 * (m_OrderX - 1 - i)*/);


            for(unsigned i = 0; i < m_OrderY; i++)
              keyvaluesV.push_back(0.0);//(0.000001 * i);
            for(unsigned i = 1; i <= NumPntsY - m_OrderY; i++)
              keyvaluesV.push_back(i);
            for(unsigned i = 0; i < m_OrderY; i++)
              keyvaluesV.push_back((NumPntsY - m_OrderY + 1)/* - 0.000001 * (m_OrderY - 1 - i)*/);


            BBasis<double> surfbasisU(m_OrderX, keyvaluesU);
            BBasis<double> surfbasisV(m_OrderY, keyvaluesV);
            m_BSurface  = new BSurface<double>(surfbasisU, surfbasisV);
          }
          m_BSurface->SetSpatMode(m_Mode);
          m_BSurface->SetUniformTessel(m_TesselX, m_TesselY);
        }

        InternalUpdate();
        GetSurfaceOutput()->Update();
        mafEvent cam_event(this,CAMERA_UPDATE);
        ForwardUpEvent(cam_event);
        m_Gui->Update();
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
int mafVMEBSplineSurface::InternalInitialize()
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
mmaMaterial *mafVMEBSplineSurface::GetMaterial()
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
