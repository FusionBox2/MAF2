/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEBSplineVolume.cpp,v $
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

#include "mafVMEBSplineVolume.h"
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
mafCxxTypeMacro(mafVMEBSplineVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafString mafVMEBSplineVolume::GetVisualPipe()
//-------------------------------------------------------------------------
{
  return mafString(_R("mafPipeSurface"));
}

//-------------------------------------------------------------------------
bool mafVMEBSplineVolume::PolylineAccept(mafNode *node)
//-------------------------------------------------------------------------
{
  return(node != NULL && (node->IsA("mafVMEGroup")));
}

//-------------------------------------------------------------------------
mafVMEBSplineVolume::mafVMEBSplineVolume()
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
  dpipe->SetInputData(m_Polygons);

  m_PointsGroupName = _R("");
  m_BVolume    = NULL;
  m_OrderX     = 4;
  m_OrderY     = 4;
  m_OrderZ     = 4;
  m_TesselX    = 5;
  m_TesselY    = 5;
  m_TesselZ    = 5;
  m_SourceX    = 10;
  m_SourceY    = 10;
  m_SourceZ    = 10;
  m_Mode       = BVolume<double>::SPT_MODE_DIRECT;
}
//-------------------------------------------------------------------------
mafVMEBSplineVolume::~mafVMEBSplineVolume()
//-------------------------------------------------------------------------
{
  if(m_BVolume)
    delete m_BVolume;
  vtkDEL(m_Polygons);
  mafDEL(m_Transform);
  mafDEL(m_TmpTransform);
  SetOutput(NULL);
}
//-------------------------------------------------------------------------
int mafVMEBSplineVolume::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEBSplineVolume *splinePolyline = mafVMEBSplineVolume::SafeDownCast(a);

    m_Transform->SetMatrix(splinePolyline->m_Transform->GetMatrix());

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInputData(m_Polygons);
      InternalUpdate();
    }
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEBSplineVolume::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = m_Transform->GetMatrix() == ((mafVMEBSplineVolume *)vme)->m_Transform->GetMatrix();
  }
  return ret;
}


//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMEBSplineVolume::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
  return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEBSplineVolume::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
bool mafVMEBSplineVolume::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool mafVMEBSplineVolume::IsDataAvailable()
//-------------------------------------------------------------------------
{
  return true;
  /*if(GetPointsGroupLink())
    return GetPointsGroupLink()->IsDataAvailable();
  else
    return false;*/
}

//-------------------------------------------------------------------------
void mafVMEBSplineVolume::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
void mafVMEBSplineVolume::InternalUpdate() //Multi
//-----------------------------------------------------------------------
{
  //wxBusyCursor wait;
  if(m_BVolume == NULL)
    return;
  Array3D<V3d<double> > src, control;
  std::vector<V3d<double> >  result;

  src.SetDimension(m_SourceX, m_SourceY, m_SourceZ);

  for(unsigned i = 0; i < m_SourceX; i++)
  {
    for(unsigned j = 0; j < m_SourceY; j++)
    {
      for(unsigned k = 0; k < m_SourceY; k++)
      {
        src(i, j, k) = V3d<double>(1.0 * i, 1.0 * j, 1.0 * k);
      }
    }
  }

  m_BVolume->GetCPoints(src, control);
  m_BVolume->UpdateVolume(control, result);
  double volume = m_BVolume->Volume(control);

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

  vtkIdType pointId[3];
  unsigned supl = 0;
  for(unsigned i = 1; i < m_TesselX; i++)
  {
    for(unsigned j = 0; j < m_TesselY - 1; j++)
    {
      pointId[0] = supl + m_TesselY * i + j;
      pointId[1] = supl + m_TesselY * (i - 1) + j + 1;
      pointId[2] = supl + m_TesselY * (i - 1) + j;
      newCells->InsertNextCell(3 , pointId);
      pointId[0] = supl + m_TesselY * i + j;
      pointId[1] = supl + m_TesselY * i + j + 1;
      pointId[2] = supl + m_TesselY * (i - 1) + j + 1;
      newCells->InsertNextCell(3 , pointId);
    }
  }
  supl += m_TesselX * m_TesselY;

  for(unsigned i = 1; i < m_TesselX; i++)
  {
    for(unsigned j = 0; j < m_TesselY - 1; j++)
    {
      pointId[0] = supl + m_TesselY * i + j;
      pointId[1] = supl + m_TesselY * (i - 1) + j + 1;
      pointId[2] = supl + m_TesselY * (i - 1) + j;
      newCells->InsertNextCell(3 , pointId);
      pointId[0] = supl + m_TesselY * i + j;
      pointId[1] = supl + m_TesselY * i + j + 1;
      pointId[2] = supl + m_TesselY * (i - 1) + j + 1;
      newCells->InsertNextCell(3 , pointId);
    }
  }
  supl += m_TesselX * m_TesselY;

  for(unsigned i = 1; i < m_TesselX; i++)
  {
    for(unsigned j = 0; j < m_TesselZ - 1; j++)
    {
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * (i - 1) + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j;
      newCells->InsertNextCell(3 , pointId);
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * i + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j + 1;
      newCells->InsertNextCell(3 , pointId);
    }
  }
  supl += m_TesselX * m_TesselZ;

  for(unsigned i = 1; i < m_TesselX; i++)
  {
    for(unsigned j = 0; j < m_TesselZ - 1; j++)
    {
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * (i - 1) + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j;
      newCells->InsertNextCell(3 , pointId);
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * i + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j + 1;
      newCells->InsertNextCell(3 , pointId);
    }
  }
  supl += m_TesselX * m_TesselZ;

  for(unsigned i = 1; i < m_TesselY; i++)
  {
    for(unsigned j = 0; j < m_TesselZ - 1; j++)
    {
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * (i - 1) + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j;
      newCells->InsertNextCell(3 , pointId);
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * i + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j + 1;
      newCells->InsertNextCell(3 , pointId);
    }
  }
  supl += m_TesselY * m_TesselZ;

  for(unsigned i = 1; i < m_TesselY; i++)
  {
    for(unsigned j = 0; j < m_TesselZ - 1; j++)
    {
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * (i - 1) + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j;
      newCells->InsertNextCell(3 , pointId);
      pointId[0] = supl + m_TesselZ * i + j;
      pointId[1] = supl + m_TesselZ * i + j + 1;
      pointId[2] = supl + m_TesselZ * (i - 1) + j + 1;
      newCells->InsertNextCell(3 , pointId);
    }
  }
  supl += m_TesselY * m_TesselZ;

  polygons->SetPoints(newPts);
  //polygons->Update();
  polygons->SetPolys(newCells);
  polygons->Modified();
  //polygons->Update();
  newPts->Delete();
  newCells->Delete();

  Modified();
}
//-----------------------------------------------------------------------
void mafVMEBSplineVolume::InternalPreUpdate()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void mafVMEBSplineVolume::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);
  parent[_R("Transform")].SetValue(m_Transform->GetMatrix());
}

//-----------------------------------------------------------------------
void mafVMEBSplineVolume::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  Superclass::InternalRestore(node);
  m_Transform->SetMatrix(node[_R("Transform")].As<mafMatrix>());
}

//-----------------------------------------------------------------------
void mafVMEBSplineVolume::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}
//-------------------------------------------------------------------------
char** mafVMEBSplineVolume::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMESurface.xpm"
  return mafVMESurface_xpm;
}
//-------------------------------------------------------------------------
mafGUI* mafVMEBSplineVolume::CreateGui()
//-------------------------------------------------------------------------
{
  const mafString mode_choices_string[] = {_L("Direct"),_L("Maximal influence")};//, _("Interpolate")};

  mafID sub_id = -1;

  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();


  m_PointsGroupName = _L("none");
  m_Gui->Button(ID_PNTS_GROUP_LINK,&m_PointsGroupName,_L("Points"), _L("Select the Points cloud to create the Spline"));
  m_Gui->Integer(ID_ORDERX, _L("Order X"), &m_OrderX, 1, 10);
  m_Gui->Integer(ID_ORDERY, _L("Order Y"), &m_OrderY, 1, 10);
  m_Gui->Integer(ID_ORDERZ, _L("Order Z"), &m_OrderZ, 1, 10);
  m_Gui->Combo(ID_MODE, _L("Mode"), &m_Mode, DIM(mode_choices_string), mode_choices_string);

  m_Gui->Integer(ID_NUMSOURCEX, _L("Num source X"), &m_SourceX, 5, 500);
  m_Gui->Integer(ID_NUMSOURCEY, _L("Num source Y"), &m_SourceY, 5, 500);
  m_Gui->Integer(ID_NUMSOURCEZ, _L("Num source Y"), &m_SourceZ, 5, 500);

  m_Gui->Integer(ID_NUMTESSELX, _L("Num parts X"), &m_TesselX, 2, 500);
  m_Gui->Integer(ID_NUMTESSELY, _L("Num parts Y"), &m_TesselY, 2, 500);
  m_Gui->Integer(ID_NUMTESSELZ, _L("Num parts Y"), &m_TesselZ, 2, 500);

  m_Gui->Update();
  //this->InternalUpdate();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEBSplineVolume::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_NUMTESSELX:
    case ID_NUMTESSELY:
    case ID_NUMTESSELZ:
      {
        if(m_BVolume)
        {
          std::vector<double> gridU;
          std::vector<double> gridV;
          std::vector<double> gridW;
          std::vector<V3d<double> > tess;
          fillUniform(m_TesselX, m_BVolume->GetUBasis().GetMinParam(), m_BVolume->GetUBasis().GetMaxParam(), gridU);
          fillUniform(m_TesselY, m_BVolume->GetVBasis().GetMinParam(), m_BVolume->GetVBasis().GetMaxParam(), gridV);
          fillUniform(m_TesselZ, m_BVolume->GetWBasis().GetMinParam(), m_BVolume->GetWBasis().GetMaxParam(), gridW);

          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridV.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[j], gridW[0]));
            }
          }
          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridV.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[j], gridW[gridW.size() - 1]));
            }
          }
          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[0], gridW[j]));
            }
          }
          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[gridV.size() - 1], gridW[j]));
            }
          }
          for(unsigned i = 0; i < gridV.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[0], gridV[i], gridW[j]));
            }
          }
          for(unsigned i = 0; i < gridV.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[gridU.size() - 1], gridV[i], gridW[j]));
            }
          }
          m_BVolume->SetTessel(tess);
          InternalUpdate();
          Modified();
          mafEvent cam_event(this,CAMERA_UPDATE);
          ForwardUpEvent(cam_event);
          m_Gui->Update();
        }
        break;
      }
    case ID_MODE:
    case ID_ORDERX:
    case ID_ORDERY:
    case ID_ORDERZ:
    case ID_NUMSOURCEX:
    case ID_NUMSOURCEY:
    case ID_NUMSOURCEZ:
    case ID_PNTS_GROUP_LINK:
      {
        if(e->GetId() == ID_MODE)
        {
          if(m_BVolume)
          {
            m_BVolume->SetSpatMode(m_Mode);
            InternalUpdate();
            GetSurfaceOutput()->Update();
            mafEvent cam_event(this,CAMERA_UPDATE);
            ForwardUpEvent(cam_event);
            m_Gui->Update();
            break;
          }
        }
        else if(e->GetId() == ID_ORDERX || e->GetId() == ID_ORDERY || e->GetId() == ID_ORDERZ)
        {
        }
        else if(e->GetId() == ID_PNTS_GROUP_LINK)
        {
        }
        //if(vme != NULL)
        {
          delete m_BVolume;
          int NumPntsX = m_SourceX;//(m_useBSLines) ? plns[0]->GetPolylineData()->GetNumberOfPoints(): clds[0]->GetNumberOfLandmarks();
          int NumPntsY = m_SourceY;//(m_useBSLines) ? plns.size() : clds.size();
          int NumPntsZ = m_SourceZ;//(m_useBSLines) ? plns.size() : clds.size();
          int supl = 0;//(m_Mode == 2) ? 2 : 0;

          {
            std::vector<double> keyvaluesU;
            std::vector<double> keyvaluesV;
            std::vector<double> keyvaluesW;

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


            for(unsigned i = 0; i < m_OrderZ; i++)
              keyvaluesW.push_back(0.0);//(0.000001 * i);
            for(unsigned i = 1; i <= NumPntsZ - m_OrderZ; i++)
              keyvaluesW.push_back(i);
            for(unsigned i = 0; i < m_OrderZ; i++)
              keyvaluesW.push_back((NumPntsZ - m_OrderZ + 1)/* - 0.000001 * (m_OrderZ - 1 - i)*/);


            BBasis<double> surfbasisU(m_OrderX, keyvaluesU);
            BBasis<double> surfbasisV(m_OrderY, keyvaluesV);
            BBasis<double> surfbasisW(m_OrderZ, keyvaluesW);
            m_BVolume = new BVolume<double>(surfbasisU, surfbasisV, surfbasisW);
          }


          //m_BVolume = new BVolume<double>(m_OrderX, m_OrderX + NumPntsX + supl, m_OrderY, m_OrderY + NumPntsY + supl, m_OrderZ, m_OrderZ + NumPntsZ + supl);
          m_BVolume->SetSpatMode(m_Mode);
          std::vector<double> gridU;
          std::vector<double> gridV;
          std::vector<double> gridW;
          std::vector<V3d<double> > tess;
          fillUniform(m_TesselX, m_BVolume->GetUBasis().GetMinParam(), m_BVolume->GetUBasis().GetMaxParam(), gridU);
          fillUniform(m_TesselY, m_BVolume->GetVBasis().GetMinParam(), m_BVolume->GetVBasis().GetMaxParam(), gridV);
          fillUniform(m_TesselZ, m_BVolume->GetWBasis().GetMinParam(), m_BVolume->GetWBasis().GetMaxParam(), gridW);

          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridV.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[j], gridW[0]));
            }
          }
          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridV.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[j], gridW[gridW.size() - 1]));
            }
          }
          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[0], gridW[j]));
            }
          }
          for(unsigned i = 0; i < gridU.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[i], gridV[gridV.size() - 1], gridW[j]));
            }
          }
          for(unsigned i = 0; i < gridV.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[0], gridV[i], gridW[j]));
            }
          }
          for(unsigned i = 0; i < gridV.size(); i++)
          {
            for(unsigned j = 0; j < gridW.size(); j++)
            {
              tess.push_back(V3d<double>(gridU[gridU.size() - 1], gridV[i], gridW[j]));
            }
          }
          m_BVolume->SetTessel(tess);
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
int mafVMEBSplineVolume::InternalInitialize()
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
mmaMaterial *mafVMEBSplineVolume::GetMaterial()
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
