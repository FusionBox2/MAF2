/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpLnSurf.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:19:11 $
  Version:   $Revision: 1.6 $
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

#include "lhpOpLnSurf.h"

#include "wx/textfile.h"
#include "wx/arrimpl.cpp"
#include <wx/wxprec.h>
#include "wx/busyinfo.h"
#include <math.h>

#include "mafDecl.h"
#include "mafTagArray.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafOpExplodeCollapse.h"

#include "ftk/Base/RegisteringPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMELandmark.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "mafVectors.h"
#include "forarray.h"
#include "splines.h"
#include "createSplineSurf.h"

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

mafCxxTypeMacro(lhpOpLnSurf)

//----------------------------------------------------------------------------
lhpOpLnSurf::lhpOpLnSurf(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType                = OPTYPE_OP;
  m_Canundo               = true;
  m_Surface               = NULL;
  m_Muscles               = NULL;
  m_Tendons               = NULL;
  m_rhoLine               = 0.0;
  m_rhoSurf               = 0.0;
  m_sgmSurf               = 0.0;
  m_splDim                = 10;
  m_xDim                  = 10;
  m_yDim                  = 10;
  m_parseNames            = 1;
  m_generateLinesSurfaces = 0;
}

//----------------------------------------------------------------------------
lhpOpLnSurf::~lhpOpLnSurf()
//----------------------------------------------------------------------------
{
  mafDEL(m_Surface);
  mafDEL(m_Muscles);
  mafDEL(m_Tendons);
}

//----------------------------------------------------------------------------
mafOp* lhpOpLnSurf::Copy()
//----------------------------------------------------------------------------
{
  return new lhpOpLnSurf(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpLnSurf::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  int cloudCounter = 0;
  if(vme == NULL)
    return false;

  for(int i=0; i < vme->GetNumberOfChildren(); i++)
  {
    if(vme->GetChild(i)->IsMAFType(mafVMELandmarkCloud)) 
    {
      if(cloudCounter >= 2 && mafVMELandmarkCloud::SafeDownCast(vme->GetChild(i))->GetNumberOfLandmarks() < 2)
        return false;
      cloudCounter++;
    }
  }

  return cloudCounter >= 4;
}

//----------------------------------------------------------------------------
void lhpOpLnSurf::OpRun()   
//----------------------------------------------------------------------------
{
  const mafString choices_string[] = {_L("Lines and surface"), _L("Surface"), _L("Lines")}; 
  if(m_Gui == NULL)
  {
    m_Gui = new mafGUI(this);
    m_Gui->FloatSlider(ID_RHO_SPL, _R("rho spline param"),&m_rhoLine, 0.0, 1000.0);
    m_Gui->FloatSlider(ID_RHO_SRF, _R("rho surface param"),&m_rhoSurf, 0.0, 1000.0);
    m_Gui->FloatSlider(ID_SGM_SRF, _R("sigma surface param"),&m_sgmSurf, 0.0, 1000.0);
    m_Gui->Slider(ID_DIM_SPL, _R("curve split number"),&m_splDim, 4, 100);
    m_Gui->Slider(ID_DIMX_SRF, _R("surface x-split number"),&m_xDim, 4, 100);
    m_Gui->Slider(ID_DIMY_SRF, _R("surface y-split number"),&m_yDim, 4, 100);
    m_Gui->Combo(ID_GEN_LIST, _L("reg. type"), &m_generateLinesSurfaces, 3, choices_string); 
    m_Gui->Bool(ID_PARSE_NAME, _L("parse names"), &m_parseNames);
    m_Gui->SetListener(this);
    m_Gui->Label(_R(""));
    m_Gui->OkCancel();
  }
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpLnSurf::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
    case wxOK:          
      OpStop(OP_RUN_OK);
    break;
    case wxCANCEL:
      OpStop(OP_RUN_CANCEL);
    break;
    case ID_RHO_SPL:
    case ID_RHO_SRF:
    case ID_SGM_SRF:
    case ID_DIM_SPL:
    case ID_DIMX_SRF:
    case ID_DIMY_SRF:
    case ID_GEN_LIST:
    case ID_PARSE_NAME:
    break;
    default:
      mafEventMacro(*maf_event); 
    break;
  }
}
//----------------------------------------------------------------------------
static void _addSegments(const std::vector<V3d<double> >& coords, unsigned from, unsigned to, vtkPoints *pnts, vtkCellArray  *cells)
//----------------------------------------------------------------------------
{
  vtkIdType pts[2];
  if(to <= from || to > coords.size())
    return;
  for(unsigned i = from; i < to; i++)
    pnts->InsertNextPoint(coords[i].components);
  for(unsigned i = from + 1; i < to; i++)
  {
    pts[0] = pnts->GetNumberOfPoints() - (to - i) - 1;
    pts[1] = pts[0] + 1;
    cells->InsertNextCell(2, pts);
  }
}
//----------------------------------------------------------------------------
void lhpOpLnSurf::OpDo()
//----------------------------------------------------------------------------
{
  wxInt32       cloudCounter = 0;
  vtkPoints     *newPtsSurf;
  vtkCellArray  *newCellsSurf;
  vtkPoints     *newPtsMsc;
  vtkCellArray  *newCellsMsc;
  vtkPoints     *newPtsTnd;
  vtkCellArray  *newCellsTnd;
  V3d<double>   x;

  std::vector<std::vector<V3d<double> >*> allValues;
  std::vector<V3d<double> >            coords;
  std::vector<V3d<double> >            smoothed;

  mafDEL(m_Surface);
  mafDEL(m_Muscles);
  mafDEL(m_Tendons);

  newPtsSurf = vtkPoints::New();
  newPtsSurf->Allocate(5000,10000);
  newCellsSurf = vtkCellArray::New();
  newCellsSurf->Allocate(10000,20000);
  newPtsMsc = vtkPoints::New();
  newPtsMsc->Allocate(5000,10000);
  newCellsMsc = vtkCellArray::New();
  newCellsMsc->Allocate(10000,20000);
  newPtsTnd = vtkPoints::New();
  newPtsTnd->Allocate(5000,10000);
  newCellsTnd = vtkCellArray::New();
  newCellsTnd->Allocate(10000,20000);

  //modified by Stefano. 18-9-2003
  wxBusyInfo wait("Please wait, working...");

  for(int i=0; i < m_Input->GetNumberOfChildren(); i++)
  {
    if(m_Input->GetChild(i)->IsMAFType(mafVMELandmarkCloud)) 
    {
      mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input->GetChild(i));
      unsigned            from   = 0;
      bool                tendon = true;
      bool                OriIns = (cloudCounter < 2);
      if(m_parseNames)
      {
        OriIns = false;
        int namelen = strlen(cloud->GetName().GetCStr());
        if(namelen >= 4)
        {
          if(strncmp(cloud->GetName().GetCStr(), "Ori_", 4) == 0 || strncmp(cloud->GetName().GetCStr(), "Ins_", 4) == 0)
            OriIns = true;
        }
      }
      if(OriIns || cloud->GetNumberOfLandmarks() >= 4)
      {
        cloudCounter++;
        coords.clear();
        for(wxInt32 nI = 0; nI < cloud->GetNumberOfLandmarks(); nI++)
        {
          cloud->GetLandmark(nI, x.components);
          if(coords.size() == 0 || ((coords[coords.size() - 1] | x) > 4.0) || cloudCounter <= 2)
            coords.push_back(x);
          else
          {
            _addSegments(coords, from, coords.size(), tendon ? newPtsTnd : newPtsMsc, tendon ? newCellsTnd : newCellsMsc);
            from = coords.size() - 1;
            tendon = !tendon;
          }
        }
        if(!OriIns)
        {
          std::vector<V3d<double> > *arr = new std::vector<V3d<double> >(coords);
          allValues.push_back(arr);
        }
        _addSegments(coords, from, coords.size(), tendon ? newPtsTnd : newPtsMsc, tendon ? newCellsTnd : newCellsMsc);
      }
    }
  }
  produceRegularGrid(allValues, m_splDim, m_rhoLine, 2, true, smoothed);
  createSurface(smoothed, allValues.size(), m_splDim, 2, 2, m_xDim, m_yDim, m_rhoSurf, m_sgmSurf, newPtsSurf, newCellsSurf);
  for(unsigned i = 0; i < allValues.size(); i++)
    delete allValues[i];


  vtkPolyData *musc = vtkPolyData::New();
  vtkPolyData *tend = vtkPolyData::New();
  vtkPolyData *surf = vtkPolyData::New();

  musc->SetPoints(newPtsMsc);
  //musc->Update();
  tend->SetPoints(newPtsTnd);
  //tend->Update();
  surf->SetPoints(newPtsSurf);
  //surf->Update();
  musc->SetLines(newCellsMsc);
  tend->SetLines(newCellsTnd);
  surf->SetPolys(newCellsSurf);
  surf->Squeeze();
  musc->Modified();
  //musc->Update();
  tend->Modified();
  //tend->Update();
  surf->Modified();
  //surf->Update();

  newPtsSurf->Delete();
  newCellsSurf->Delete();
  newPtsMsc->Delete();
  newCellsMsc->Delete();
  newPtsTnd->Delete();
  newCellsTnd->Delete();

  if((3 - m_generateLinesSurfaces) & 1)
  {
    mafTimeStamp t;
    mafString     muscnm(_R("MscFbr_"));
    mafString     tendnm(_R("TndFbr_"));
    t = ((mafVME *)m_Input)->GetTimeStamp();
    mafNEW(m_Muscles);
    mafNEW(m_Tendons);
    muscnm += m_Input->GetName();
    tendnm += m_Input->GetName();
    m_Muscles->SetName(muscnm);
    m_Muscles->SetData(musc,t);
    m_Tendons->SetName(tendnm);
    m_Tendons->SetData(tend,t);

    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));

    m_Muscles->GetTagArray()->SetTag(tag_Nature);
    m_Tendons->GetTagArray()->SetTag(tag_Nature);

    m_Muscles->ReparentTo(m_Input);
    m_Tendons->ReparentTo(m_Input);
    //{mafEvent evUnq(this,VME_ADD,m_Muscles); mafEventMacro(evUnq);}
    //{mafEvent evUnq(this,VME_ADD,m_Tendons); mafEventMacro(evUnq);}
  }
  if((3 - m_generateLinesSurfaces) & 2)
  {
    mafTimeStamp t;
    mafString     sfnm(_R("Surf_"));
    t = ((mafVME *)m_Input)->GetTimeStamp();
    mafNEW(m_Surface);
    sfnm += m_Input->GetName();
    m_Surface->SetName(sfnm);
    m_Surface->SetData(surf, t);

    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));

    m_Surface->GetTagArray()->SetTag(tag_Nature);

    m_Surface->ReparentTo(m_Input);
    //{mafEvent evUnq(this,VME_ADD,m_Surface); mafEventMacro(evUnq);}
  }
  musc->Delete();
  tend->Delete();
  surf->Delete();
  return;
}



//----------------------------------------------------------------------------
void lhpOpLnSurf::OpUndo()
//----------------------------------------------------------------------------
{
  {mafEvent evUnq(this,VME_REMOVE,m_Surface); mafEventMacro(evUnq);}
  {mafEvent evUnq(this,VME_REMOVE,m_Muscles); mafEventMacro(evUnq);}
  {mafEvent evUnq(this,VME_REMOVE,m_Tendons); mafEventMacro(evUnq);}
}
