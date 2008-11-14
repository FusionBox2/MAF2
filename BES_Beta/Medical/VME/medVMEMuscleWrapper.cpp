/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMuscleWrapper.cpp,v $
Language:  C++
Date:      $Date: 2008-11-14 17:10:09 $
Version:   $Revision: 1.1.2.1 $
Authors:   Josef Kohout
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

#include "medVMEMuscleWrapper.h"
#include "mafVMEOutputSurface.h"
#include "mmaMaterial.h"
#include "mafEventSource.h"
#include "mafDataPipeCustom.h"
#include "mafGUI.h"
#include "mafStorageElement.h"

#include "vtkMAFDataPipe.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "../vtkMAF/vtkMAFPolyDataDeformation.h"
#include "../vtkMAF/vtkMAFPolyDataDeformation_M1.h"
#include "../vtkMAF/vtkMAFPolyDataDeformation_M2.h"
#include "vtkTubeFilter.h"

#ifdef _DEBUG
#include "vtkMassProperties.h"
#endif

#include <assert.h>
#include "mafMemDbg.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEMuscleWrapper)
//-------------------------------------------------------------------------

const /*static*/ char* medVMEMuscleWrapper::MUSCLEWRAPPER_LINK_NAMES[] = {
  "MuscleVME_RP",
  "WrapperVME1_RP", "WrapperVME2_RP",
  "WrapperVME1", "WrapperVME2",
};

//-------------------------------------------------------------------------
medVMEMuscleWrapper::medVMEMuscleWrapper()
//-------------------------------------------------------------------------
{  
  memset(m_CurVMEs, 0, sizeof(m_CurVMEs));
  memset(m_OldCurves, 0, sizeof(m_OldCurves));

  m_PolyData = vtkPolyData::New();
  m_DeformerType = DEM_WARPING;
  
  m_bNeedUpdate = true;
  m_bDoNotUpdate = false;

  mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data  
  SetOutput(output);  

  DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and MAF
  mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
  dpipe->SetDependOnAbsPose(true);
  SetDataPipe(dpipe);
  dpipe->SetInput(m_PolyData); 
}
//-------------------------------------------------------------------------
medVMEMuscleWrapper::~medVMEMuscleWrapper()
//-------------------------------------------------------------------------
{
  for (int i = 0; i < 2*MAX_WRAPPERS; i++) {
    vtkDEL(m_OldCurves[i]);
  }
  
  vtkDEL(m_PolyData);
  SetOutput(NULL);  
}
//-------------------------------------------------------------------------
int medVMEMuscleWrapper::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    medVMEMuscleWrapper *wrapper = medVMEMuscleWrapper::SafeDownCast(a);
    for (int i = 0; i < LNK_LAST; i++)
    {
      mafNode *linked_node = wrapper->GetLink(
        MUSCLEWRAPPER_LINK_NAMES[i]
      );

      if (linked_node)
        this->SetLink(MUSCLEWRAPPER_LINK_NAMES[i], linked_node);
    }
    
    m_DeformerType = wrapper->m_DeformerType;    
    m_bNeedUpdate = true;
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool medVMEMuscleWrapper::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{  
  if (!Superclass::Equals(vme))
    return false;

  for (int i = 0; i < LNK_LAST; i++)
  {
    if (GetLink(MUSCLEWRAPPER_LINK_NAMES[i]) !=
      ((medVMEMuscleWrapper*)vme)->GetLink(MUSCLEWRAPPER_LINK_NAMES[i])
      )
      return false;
  }
  return true;
}

//-------------------------------------------------------------------------
void medVMEMuscleWrapper::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{  
  //ignored, nothing to do
}

//-------------------------------------------------------------------------
int medVMEMuscleWrapper::InternalInitialize()
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
mmaMaterial *medVMEMuscleWrapper::GetMaterial()
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

//-------------------------------------------------------------------------
bool medVMEMuscleWrapper::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}
//-------------------------------------------------------------------------
void medVMEMuscleWrapper::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-----------------------------------------------------------------------
//NB. mafVMEMeter and MAF is really stupid, it generates new data even if
//nothing has changed (especially lot of data is generated during the animation)
//we will need to avoid redundant deformation as much as possible
void medVMEMuscleWrapper::InternalUpdate()
//-----------------------------------------------------------------------
{ 
  if (m_bDoNotUpdate)
    return;

  bool bCurvesUpdated = false;
  if (!m_bNeedUpdate)
  {
    //if no update is required internally, check, if curves did not change
    //Note: it makes no sense to check time stamps as they are always different
    vtkPolyData* pCurves[4];
    for (int i = 0; i < 4; i++)
    {
      pCurves[i] = NULL;
      if (m_CurVMEs[1 + i] != NULL)
      {          
        pCurves[i] = vtkPolyData::SafeDownCast(
          m_CurVMEs[1 + i]->GetOutput()->GetVTKData());            

        if (pCurves[i] != NULL)        
          pCurves[i]->Update(); //force its update
      }
    }

    bCurvesUpdated = true;
    m_bNeedUpdate = !CheckCurves(m_OldCurves, pCurves);
    if (m_bNeedUpdate) 
    {
      for (int i = 0; i < 4; i++)
      {
        vtkDEL(m_OldCurves[i]);        
        if (pCurves[i] != NULL)
        {
          m_OldCurves[i] = vtkPolyData::New();  
          m_OldCurves[i]->DeepCopy(pCurves[i]);
        }
      }
      
    }    
  } //end if m_bNeedUpdate
   
  if (m_bNeedUpdate)
  {    
    //OK, we will need deform muscle
    wxBusyCursor busy;    
    if (m_CurVMEs[0] == NULL)    
    {
      //if there is no valid associated muscle, output is empty
      m_PolyData->SetPoints(NULL);
      m_PolyData->SetPolys(NULL);    
    }
    else
    {
      //there is a valid associated muscle      
      vtkPolyData* pPoly = vtkPolyData::SafeDownCast(
        m_CurVMEs[0]->GetOutput()->GetVTKData());

      assert(pPoly != NULL);
      pPoly->Update();      

     {
        //get curves
        vtkPolyData* pCurves[4];
        for (int i = 0; i < 4; i++)
        {
          pCurves[i] = NULL;
          if (!bCurvesUpdated) {        
            vtkDEL(m_OldCurves[i]);        
          }

          if (m_CurVMEs[1 + i] != NULL)
          {          
            pCurves[i] = vtkPolyData::SafeDownCast(
              m_CurVMEs[1 + i]->GetOutput()->GetVTKData());            

            if (pCurves[i] != NULL)
            {
              if (!bCurvesUpdated)
              {
                pCurves[i]->Update();   //make sure that we have data here
                m_OldCurves[i] = vtkPolyData::New();  
                m_OldCurves[i]->DeepCopy(pCurves[i]);
              }

              //mafVMEMeter (version 20.10.2008) generates corrupted polylines, 
              //they contain duplicated coordinates and edges, e.g.
              //Pts: 0(318,305,-467), 1(379,292,-824), 2(388,310, -871), 3(379,292,-824)
              //Edges: 0-1,1-2,2-3 => vertex 1 and 3 are redundant
              pCurves[i] = FixPolyline(pCurves[i]);
            }
          }
        } //end for

        switch (m_DeformerType)
        {
        case DEM_BLANCO:
          DeformMuscle< vtkMAFPolyDataDeformation_M1 >(pPoly, pCurves);
          break;
        case DEM_SEPPLANES:
          DeformMuscle< vtkMAFPolyDataDeformation_M2 >(pPoly, pCurves);
          break;
        default:
          DeformMuscle< vtkMAFPolyDataDeformation >(pPoly, pCurves);
          break;
        }


        //delete curves
        for (int i = 0; i < 4; i++) {
          vtkDEL(pCurves[i]);          
        }

        //output
#ifdef _DEBUG
        vtkMassProperties* props = vtkMassProperties::New();
        props->SetInput(pPoly); //GetVolume calls Update    
        double dblOrigVolume = props->GetVolume();

        props->SetInput(m_PolyData);  //GetVolume calls Update      
        double dblNewVolume = props->GetVolume();
        props->Delete();

        wxString szMsg = wxString::Format(
          wxT("Deformation done. Volume shrinkage: %.4f (Orig = %.2f, New = %.2f)\n"),
          dblNewVolume / dblOrigVolume, dblOrigVolume, dblNewVolume);

        mafLogMessage(szMsg.c_str());
#ifdef _RPT0
        _RPT0(_CRT_WARN, szMsg.c_str());
#endif
#endif                     
      }
    } //end if muscle exists
    
    m_bDoNotUpdate = true;  //prevent recursion
    GetOutput()->Update();  //this calls recursively our update    
    m_bDoNotUpdate = false;

    m_bNeedUpdate = false;
  } //if (m_bNeedUpdate)  
}

//------------------------------------------------------------------------
//Template to deform the muscle
template< class T >
void medVMEMuscleWrapper::DeformMuscle(vtkPolyData* pMuscle, vtkPolyData** pCurves)
//------------------------------------------------------------------------
{
  vtkMAFSmartPointer< T > pDeformer;

  int nCurves = 0;
  pDeformer->SetNumberOfSkeletons(0);
  for (int i = 0; i < 2; i++)
  {
    if (pCurves[i] != NULL && pCurves[i + 2] != NULL) {
      pDeformer->SetNthSkeleton(nCurves++, pCurves[i], pCurves[i + 2], NULL);          
    }
  }

  //if we have no valid pair of curves, just pass original muscle data
  if (nCurves == 0)
    m_PolyData->DeepCopy(pMuscle);
  else
  {
    //otherwise perform the deformation
    pDeformer->SetInput(pMuscle);
    pDeformer->SetOutput(m_PolyData);

    pDeformer->Update();

    //disconnect PolyData from its source
    pDeformer->SetOutput(NULL);
  }
}

//-------------------------------------------------------------------------
mafGUI* medVMEMuscleWrapper::CreateGui()
//-------------------------------------------------------------------------
{  
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();

  UpdateLinks();

  m_Gui->Label(_("Rest Pose"), true);
  m_Gui->Button(ID_RESTPOSE_MUSCLE_LINK,&m_RestPoseMuscleVmeName,
    _("Muscle"), _("Select the vme with muscle in the rest pose."));
  m_Gui->Button(ID_RESTPOSE_WRAPPER1_LINK, &m_RestPoseWrapperVmeNames[0],
    _("Wrapper 1"), _("Select the first wrapper vme (currently in the rest pose)."));
  m_Gui->Button(ID_RESTPOSE_WRAPPER2_LINK, &m_RestPoseWrapperVmeNames[1],
    _("Wrapper 2"), _("Select another wrapper vme (currently in the rest pose). [Optional]"));

  m_Gui->Divider();

  m_Gui->Label(_("Current Pose"), true);
  m_Gui->Button(ID_DYNAMIC_WRAPPER1_LINK, &m_WrapperVmeNames[0],
    _("Wrapper 1"), _("Select the first dynamic wrapper vme."));
  m_Gui->Button(ID_DYNAMIC_WRAPPER2_LINK, &m_WrapperVmeNames[1],
    _("Wrapper 2"), _("Select another dynamic wrapper vme. [Optional]"));
  

  m_Gui->Divider();
  wxString szModes[3] = {_("orthogonal mapping (M1)"),
        _("accurate ROI mapping (M2)"),_("simple warping (M3)"), };
  m_Gui->Combo(ID_DEFORMATIONMODE, _("Def. mode"), &m_DeformerType, 3,
    szModes, _("Selects the algorithm to be used for the deformation."));

  InternalUpdate();  
  return m_Gui;
}
//-------------------------------------------------------------------------
void medVMEMuscleWrapper::UpdateLinks()
//-------------------------------------------------------------------------
{   
  mafString szNone = _("none");

  if ((m_CurVMEs[0] = GetMuscleVME_RP()) == NULL)
    m_RestPoseMuscleVmeName = szNone;
  else
    m_RestPoseMuscleVmeName = m_CurVMEs[0]->GetName();

  if ((m_CurVMEs[1] = GetWrapper1VME_RP()) == NULL)
    m_RestPoseWrapperVmeNames[0] = szNone;
  else
    m_RestPoseWrapperVmeNames[0] = m_CurVMEs[1]->GetName();

  if ((m_CurVMEs[2] = GetWrapper2VME_RP()) == NULL)
    m_RestPoseWrapperVmeNames[1] = szNone;
  else
    m_RestPoseWrapperVmeNames[1] = m_CurVMEs[2]->GetName();

  if ((m_CurVMEs[3] = GetWrapper1VME()) == NULL)
    m_WrapperVmeNames[0] = szNone;
  else
    m_WrapperVmeNames[0] = m_CurVMEs[3]->GetName();

  if ((m_CurVMEs[4] = GetWrapper2VME()) == NULL)
    m_WrapperVmeNames[1] = szNone;
  else
    m_WrapperVmeNames[1] = m_CurVMEs[4]->GetName();
}

//------------------------------------------------------------------------
//Returns true, if curves are identical (although they are different objects)
bool medVMEMuscleWrapper::CheckCurves(vtkPolyData** pOldCurves, vtkPolyData** pNewCurves)
//------------------------------------------------------------------------
{
  for (int i = 0; i < 2*MAX_WRAPPERS; i++)
  {
    //only one curve
    if ((pOldCurves[i] == NULL && pNewCurves[i] != NULL) ||
      (pOldCurves[i] != NULL && pNewCurves[i] == NULL))
      return false;
    
    if (pOldCurves[i] != NULL)
    {
      //both curves are valid but might be different objects
      int nPoints = pOldCurves[i]->GetNumberOfPoints();
      if (pNewCurves[i]->GetNumberOfPoints() != nPoints)
        return false;

      //int nCells = pOldCurves[i]->GetNumberOfCells();
      //if (pNewCurves[i]->GetNumberOfCells() != nCells)
      //  return false;

      //compare coordinates
      for (int j = 0; j < nPoints; j++)
      {
        double coord1[3], coord2[3];
        pOldCurves[i]->GetPoint(j, coord1);
        pNewCurves[i]->GetPoint(j, coord2);
        for (int k = 0; k < 3; k++)
        {
          if (coord1[k] != coord2[k])
            return false;
        }
      }

      ////compare edges
      ////pOldCurves[i]->BuildCells(); (they should have it)
      //pNewCurves[i]->BuildCells();
      //for (int j = 0; j < nCells; j++)
      //{
      //  vtkIdType npts, *pOldPts, *pNewPts;
      //  pOldCurves[i]->GetCellPoints(j, npts, pOldPts);
      //  pNewCurves[i]->GetCellPoints(j, npts, pNewPts);

      //  for (int k = 0; k < npts; k++)
      //  {
      //    if (pOldPts[k] != pNewPts[k])
      //      return false;
      //  }
      //}
    }
  }

  return true;
}

//-------------------------------------------------------------------------
void medVMEMuscleWrapper::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    int nId = e->GetId();
    if (nId >= ID_RESTPOSE_MUSCLE_LINK && nId <= ID_DYNAMIC_WRAPPER2_LINK)
    {      
      OnSelectLink(e);
      return;
    }
    else if (nId == ID_DEFORMATIONMODE)
    {
      m_bNeedUpdate = true;
      InternalUpdate();
      
      //force update
      mafEvent ev(this, VME_SELECTED,this);
      this->ForwardUpEvent(&ev);
      return;
    }
  }

  Superclass::OnEvent(maf_event);
}

//------------------------------------------------------------------------
//Handles the click on one of select VME buttons.
/*virtual*/ void medVMEMuscleWrapper::OnSelectLink(mafEvent* ev)
//------------------------------------------------------------------------
{  
  mafString title;
  int nLinkId = ev->GetId() - ID_RESTPOSE_MUSCLE_LINK;
  if (nLinkId == LNK_RESTPOSE_MUSCLE)
    title = _("Choose muscle vme link (in rest pose)");
  else if (nLinkId == LNK_RESTPOSE_WRAPPER1 || nLinkId == LNK_RESTPOSE_WRAPPER2)
    title = _("Choose wrapper vme link (in rest pose)");
  else
    title = _("Choose wrapper vme link");
  
  ev->SetId(VME_CHOOSE);  
  if (nLinkId == LNK_RESTPOSE_MUSCLE)
    ev->SetArg((long)&medVMEMuscleWrapper::VMEAcceptMuscle);
  else
    ev->SetArg((long)&medVMEMuscleWrapper::VMEAcceptWrapper);
  ev->SetString(&title);
  ForwardUpEvent(ev);

  mafNode *n = ev->GetVme();
  if (n != NULL)
  {
    SetLink(MUSCLEWRAPPER_LINK_NAMES[nLinkId], n);    
    UpdateLinks();

    m_bNeedUpdate = true;
    
    m_Gui->Update();
    InternalUpdate();

    //force update
    mafEvent ev(this, VME_SELECTED,this);
    this->ForwardUpEvent(&ev);

  }
}

//------------------------------------------------------------------------
/*static*/ bool medVMEMuscleWrapper::VMEAcceptMuscle(mafNode *node) 
//------------------------------------------------------------------------
{
  mafVME* vme = mafVME::SafeDownCast(node);
  if (vme != NULL)
  {    
    if (vme->GetOutput()->IsA("mafVMEOutputSurface"))
      //|| vme->GetOutput()->IsA("mafVMEOutputMesh")
      return true;
  }

  return false;
};

//------------------------------------------------------------------------
/*static*/ bool medVMEMuscleWrapper::VMEAcceptWrapper(mafNode *node) 
//------------------------------------------------------------------------
{
  mafVME* vme = mafVME::SafeDownCast(node);
  if (vme != NULL)
  {    
    if (vme->GetOutput()->IsA("mafVMEOutputMeter") ||
      vme->GetOutput()->IsA("mafVMEOutputWrappedMeter")
      )      
      return true;
  }

  return false;
};

#pragma region GetVMEs
//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetMuscleVME_RP()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(
    MUSCLEWRAPPER_LINK_NAMES[LNK_RESTPOSE_MUSCLE]
  ));
}
//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetWrapper1VME_RP()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(
    MUSCLEWRAPPER_LINK_NAMES[LNK_RESTPOSE_WRAPPER1]
  ));
}
//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetWrapper2VME_RP()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(
    MUSCLEWRAPPER_LINK_NAMES[LNK_RESTPOSE_WRAPPER2]
  ));
}
//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetWrapper1VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(
    MUSCLEWRAPPER_LINK_NAMES[LNK_DYNPOSE_WRAPPER1]
  ));
}
//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetWrapper2VME()
//-------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(GetLink(
    MUSCLEWRAPPER_LINK_NAMES[LNK_DYNPOSE_WRAPPER2]
  ));
}
#pragma endregion

//------------------------------------------------------------------------
//Creates a new polydata without duplicate vertices and edges that might be in the input.
vtkPolyData* medVMEMuscleWrapper::FixPolyline(vtkPolyData* input)
//------------------------------------------------------------------------
{
  int nInputPoints = input->GetNumberOfPoints();
  int nOutputPoints = 0;

  typedef double VCoord[3];
  VCoord* pPtCoords = new VCoord[nInputPoints];
  vtkIdType* pPtIdMap = new vtkIdType[nInputPoints];
  
  //we need the same data type
  vtkPoints* out_points = vtkPoints::New(input->GetPoints()->GetDataType());

  for (int i = 0; i < nInputPoints; i++)
  {
    input->GetPoint(i, pPtCoords[i]);
    
    bool bFound = false;
    for (int j = 0; j < i; j++)
    {
      if (
        pPtCoords[j][0] == pPtCoords[i][0] &&
        pPtCoords[j][1] == pPtCoords[i][1] &&
        pPtCoords[j][2] == pPtCoords[i][2]
      )
      {
        pPtIdMap[i] = pPtIdMap[j];
        bFound = true;
        break;
      }
    } //end for

    if (!bFound)
    {
      out_points->InsertNextPoint(pPtCoords[i]);
      pPtIdMap[i] = nOutputPoints;
      nOutputPoints++;
    }
  } //end for i

  delete[] pPtCoords;   //no longer needed

  //process edges
  input->BuildCells();
  int nInEdges = input->GetNumberOfCells();
  int nOutEdges = 0;

  typedef int Edge[2];
  Edge* pEdges = new Edge[nInEdges];
  for (int i = 0; i < nInEdges; i++)
  {
    vtkIdType nPts, *pPtIds;
    input->GetCellPoints(i, nPts, pPtIds);

    pEdges[nOutEdges][0] = pPtIdMap[pPtIds[0]];
    pEdges[nOutEdges][1] = pPtIdMap[pPtIds[1]];
    if (pEdges[nOutEdges][0] == pEdges[nOutEdges][1])
      continue; //invalid edge

    bool bFound = false;
    for (int j = 0; j < nOutEdges; j++)
    {
      if (
        (pEdges[nOutEdges][0] == pEdges[j][0] &&
        pEdges[nOutEdges][1] == pEdges[j][1]) ||
        (pEdges[nOutEdges][0] == pEdges[j][1] &&
        pEdges[nOutEdges][1] == pEdges[j][0])
        )
      {
        bFound = true;
        break;
      }
    }

    if (!bFound)
      nOutEdges++;
  }

  delete[] pPtIdMap;  //no longer needed

  //save edges
  vtkCellArray* out_lines = vtkCellArray::New();
  for (int i = 0; i < nOutEdges; i++) {
    out_lines->InsertNextCell(2, pEdges[i]);
  }

  delete[] pEdges; //no longer needed

  vtkPolyData* pRet = vtkPolyData::New();
  pRet->SetPoints(out_points);
  pRet->SetLines(out_lines);

  out_points->Delete();  //no longer needed
  out_lines->Delete();  //no longer needed
  return pRet;
}

//------------------------------------------------------------------------
/*virtual*/ int medVMEMuscleWrapper::InternalStore(mafStorageElement *parent)
//------------------------------------------------------------------------
{
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger("DeformMode", m_DeformerType);
    return MAF_OK;
  }
  return MAF_ERROR;
}

//------------------------------------------------------------------------
/*virtual*/ int medVMEMuscleWrapper::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {    
    if (node->RestoreInteger("DeformMode", m_DeformerType) != MAF_OK)
      m_DeformerType = DEM_WARPING;      

    m_bNeedUpdate = true;
    return MAF_OK;
  }

  return MAF_ERROR;  
}