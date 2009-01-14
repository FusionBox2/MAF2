/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVMEMuscleWrapper.cpp,v $
Language:  C++
Date:      $Date: 2009-01-14 12:07:58 $
Version:   $Revision: 1.1.2.7 $
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
#include "mafGUIValidator.h"
#include "mafStorageElement.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafTransform.h"

#include "vtkMAFDataPipe.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "../vtkMAF/vtkMAFPolyDataDeformation.h"
#include "../vtkMAF/vtkMAFMuscleDecomposition.h"
#include "vtkTubeFilter.h"

#include <assert.h>
#include "mafMemDbg.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMEMuscleWrapper)
//-------------------------------------------------------------------------

const /*static*/ char* medVMEMuscleWrapper::MUSCLEWRAPPER_LINK_NAMES[] = {
  "MuscleVME_RP", "WrapperVME_RP_", "WrapperVME_CP_", "OAreaVME", "IAreaVME",
};

#define DEFAULT_INPUT_MODE    1   //0 = simple, 1 = advanced
#define DEFAULT_OUTPUT_MODE   1   //1 = generates fibers, 0 - deformed surface
#define DEFAULT_FIBERS_TYPE   FT_PENNATE
#define DEFAULT_FIBERS_NUM    50
#define DEFAULT_FIBERS_RES    14
#define DEFAULT_FIBERS_SMOOTH	1
#define DEFAULT_FIBERS_SMOOTHSTEPS	5
#define DEFAULT_FIBERS_SMOOTHWEIGHT	4
#define DEFAULT_FIBERS_THICKNESS 0.25

//-------------------------------------------------------------------------
medVMEMuscleWrapper::medVMEMuscleWrapper()
//-------------------------------------------------------------------------
{  
  m_PolyData = vtkPolyData::New();
    
  m_MuscleVme = NULL;
  for (int i = 0; i < 2; i++){
    m_WrappersVme[i] = m_OIVME[i] = NULL;    
  }
  
  m_nWrappers = 0;
  m_pWrappers = NULL;
    
  m_InputMode = DEFAULT_INPUT_MODE;
  m_VisMode = DEFAULT_OUTPUT_MODE;
  m_FbResolution = DEFAULT_FIBERS_RES;
  m_FbNumFib = DEFAULT_FIBERS_NUM;
  m_FbTemplate = DEFAULT_FIBERS_TYPE; //FT_PENNATE
  m_FbThickness = DEFAULT_FIBERS_THICKNESS;
  m_FbSmooth = DEFAULT_FIBERS_SMOOTH;
  m_FbSmoothSteps = DEFAULT_FIBERS_SMOOTHSTEPS;
  m_FbSmoothWeight = DEFAULT_FIBERS_SMOOTHWEIGHT;
  m_FbDebugShowTemplate = 0;
  
  m_bLinksRestored = false;
  m_bNeedUpdate = false;
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
  //_RPT2(_CRT_WARN, "medVMEMuscleWrapper dtor(%p) - m_pWrappers = %p\n",
  //  this, m_pWrappers);


  DeleteAllWrappers();  

  vtkDEL(m_PolyData);
  SetOutput(NULL);  
}

//------------------------------------------------------------------------
//Removes all wrappers from the list (NOT FROM GUI!)
void medVMEMuscleWrapper::DeleteAllWrappers()
//------------------------------------------------------------------------
{
  while (m_pWrappers != NULL)
  {
    WRAPPER_ITEM* pItem = m_pWrappers;
    m_pWrappers = m_pWrappers->pNext;

    for (int i = 0; i < 2; i++) {
      vtkDEL(pItem->pCurves[i]);
    }

    delete pItem;
  }
}

//-------------------------------------------------------------------------
int medVMEMuscleWrapper::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    medVMEMuscleWrapper *wrapper = medVMEMuscleWrapper::SafeDownCast(a);

    for (int i = 0; i < 2; i++){    
      m_OIVMEName[i] = wrapper->m_OIVMEName[i];
    }

    m_nWrappers = wrapper->m_nWrappers;        
    m_InputMode = wrapper->m_InputMode;
    m_VisMode = wrapper->m_VisMode;
    m_FbTemplate = wrapper->m_FbTemplate;
    m_FbNumFib = wrapper->m_FbNumFib;
    m_FbResolution = wrapper->m_FbResolution;
    m_FbSmooth = wrapper->m_FbSmooth;
    m_FbSmoothSteps = wrapper->m_FbSmoothSteps;
    m_FbSmoothWeight = wrapper->m_FbSmoothWeight;
    m_FbDebugShowTemplate = wrapper->m_FbDebugShowTemplate;

    m_bNeedUpdate = true;
    
    //DeepCopy copied links => restore internal data
    RestoreMeterLinks();

    //BES: 12.1.2009 - DataPipe has NULL input now (although it was set in ctor)
    //=> we need to reassign input for the data pipe
    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe != NULL){
      dpipe->SetInput(m_PolyData);
    }    
    
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool medVMEMuscleWrapper::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (!Superclass::Equals(vme))   //checks also Links
    return false;

  medVMEMuscleWrapper *wrapper = medVMEMuscleWrapper::SafeDownCast(vme);
  if (wrapper == NULL ||
    m_MuscleVme != wrapper->m_MuscleVme  ||  
    m_OIVME[0] != wrapper->m_OIVME[0] ||
    m_OIVME[1] != wrapper->m_OIVME[1] ||
    m_InputMode != wrapper->m_InputMode ||
    m_VisMode != wrapper->m_VisMode ||
    m_FbTemplate != wrapper->m_FbTemplate ||
    m_FbNumFib != wrapper->m_FbNumFib ||
    m_FbResolution != wrapper->m_FbResolution ||
    m_FbSmooth != wrapper->m_FbSmooth ||
    m_FbSmoothSteps != wrapper->m_FbSmoothSteps ||
    m_FbSmoothWeight != wrapper->m_FbSmoothWeight ||
    m_FbDebugShowTemplate != wrapper->m_FbDebugShowTemplate
    )
    return false;

  WRAPPER_ITEM* pItem = m_pWrappers;
  WRAPPER_ITEM* pSrcItem = wrapper->m_pWrappers;
  while (pSrcItem != NULL && pItem != NULL)
  {
    for (int i = 0; i < 2; i++)
    {
      if (pSrcItem->pVmeRP_CP[i] != pItem->pVmeRP_CP[i])
        return false;
    }

    pItem = pItem->pNext;
    pSrcItem = pSrcItem->pNext;
  }
  
  return pItem == pSrcItem; //both must be NULL, if everything was matched
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

//------------------------------------------------------------------------
//Set a new link for the given vme. 
//The link have name with prefix from the link table at index nLinkNameId (see LNK_ enums)
//and suffix nPosId. If nPosId is -1, no suffix is specified
void medVMEMuscleWrapper::StoreMeterLink(mafVME* vme, int nLinkNameId, int nPosId)
//------------------------------------------------------------------------
{
  if (vme != NULL)
  {
    mafString szName;
    if (nPosId >= 0)
      szName = wxString::Format(wxT("%s%d"), MUSCLEWRAPPER_LINK_NAMES[nLinkNameId], nPosId);
    else
      szName = MUSCLEWRAPPER_LINK_NAMES[nLinkNameId];

    SetLink(szName, vme);
  }
}

//------------------------------------------------------------------------
//Stores all meter links into Links list.
//N.B. This method is supposed to be call from InternalStore
//------------------------------------------------------------------------
void medVMEMuscleWrapper::StoreMeterLinks()
{
  if (!m_bLinksRestored)
    return; //no link to be stored

  //we need to remove all existing links first
  mafLinksMap* pLinks = GetLinks(); 
  bool bNeedRestart;   

  do
  {
    bNeedRestart = false;
    for (mafLinksMap::iterator i = pLinks->begin(); i != pLinks->end(); i++)
    {
      if (
        i->first.Equals(MUSCLEWRAPPER_LINK_NAMES[LNK_RESTPOSE_MUSCLE]) ||
        i->first.Equals(MUSCLEWRAPPER_LINK_NAMES[LNK_FIBERS_ORIGIN]) ||
        i->first.Equals(MUSCLEWRAPPER_LINK_NAMES[LNK_FIBERS_INSERTION]) ||
        i->first.StartsWith(MUSCLEWRAPPER_LINK_NAMES[LNK_RESTPOSE_WRAPPERx]) ||
        i->first.StartsWith(MUSCLEWRAPPER_LINK_NAMES[LNK_DYNPOSE_WRAPPERx])
        )
      {
        RemoveLink(i->first);
        bNeedRestart = true;
        break;
      }
    }
  }while (bNeedRestart);

  //create links, so they can be stored
  StoreMeterLink(m_MuscleVme, LNK_RESTPOSE_MUSCLE);
  StoreMeterLink(m_OIVME[0], LNK_FIBERS_ORIGIN);
  StoreMeterLink(m_OIVME[1], LNK_FIBERS_INSERTION);

  int nId = 0;
  WRAPPER_ITEM* pItem = m_pWrappers;
  while (pItem != NULL)
  {
    StoreMeterLink(pItem->pVmeRP_CP[0], LNK_RESTPOSE_WRAPPERx, nId);
    StoreMeterLink(pItem->pVmeRP_CP[1], LNK_DYNPOSE_WRAPPERx, nId);
    nId++;

    pItem = pItem->pNext;
  }
}

//------------------------------------------------------------------------
//Restore vme with the specified link. 
//Returns NULL, if there is no VME. IMPORTANT: removes vme from the Links list
mafVME* medVMEMuscleWrapper::RestoreMeterLink(int nLinkNameId, int nPosId)
//------------------------------------------------------------------------
{
  mafString szName;
  if (nPosId >= 0)
    szName = wxString::Format(wxT("%s%d"), MUSCLEWRAPPER_LINK_NAMES[nLinkNameId], nPosId);
  else
    szName = MUSCLEWRAPPER_LINK_NAMES[nLinkNameId];
  
  return mafVME::SafeDownCast(GetLink(szName));  
}

//------------------------------------------------------------------------
//Restore all meter links from Links list. 
//N.B. this method is supposed to be called only after all VMEs were restored,
//i.e, it cannot be called from InternalRestore
void medVMEMuscleWrapper::RestoreMeterLinks()
//------------------------------------------------------------------------
{
  //restore links (and remove them as well)  
  m_MuscleVme = RestoreMeterLink(LNK_RESTPOSE_MUSCLE);
  m_OIVME[0] = RestoreMeterLink(LNK_FIBERS_ORIGIN);
  m_OIVME[1] = RestoreMeterLink(LNK_FIBERS_INSERTION);

  DeleteAllWrappers();  //delete wrappers, if they exist (should not be necessary)

  WRAPPER_ITEM* pLastItem = NULL;
  for (int nId = 0; nId < m_nWrappers; nId++)
  {
    WRAPPER_ITEM* pItem = new WRAPPER_ITEM;
    memset(pItem, 0, sizeof(WRAPPER_ITEM));

    pItem->pVmeRP_CP[0] = RestoreMeterLink(LNK_RESTPOSE_WRAPPERx, nId);
    pItem->pVmeRP_CP[1] = RestoreMeterLink(LNK_DYNPOSE_WRAPPERx, nId);      

    if (NULL == (pItem->pLast = pLastItem))
      m_pWrappers = pItem;
    else
      pLastItem->pNext = pItem;

    pLastItem = pItem;
  } //end for

  m_bLinksRestored = true;
}

//------------------------------------------------------------------------
/*virtual*/ int medVMEMuscleWrapper::InternalStore(mafStorageElement *parent)
//------------------------------------------------------------------------
{ 
  //store Links, so they can be saved by base clase  
  StoreMeterLinks();
    
  if (Superclass::InternalStore(parent) == MAF_OK)  //stores material + links to muscle and OI areas VMEs
  {
    parent->StoreInteger("Wrappers_Num", m_nWrappers);
    parent->StoreInteger("InputMode", m_InputMode);
    parent->StoreInteger("VisualMode", m_VisMode);    
    parent->StoreInteger("Fibers_Type", m_FbTemplate);
    parent->StoreInteger("Fibers_Num", m_FbNumFib);
    parent->StoreInteger("Fibers_Res", m_FbResolution);
    parent->StoreDouble("Fibers_Thickness", m_FbThickness);
    parent->StoreInteger("Fibers_Smooth", m_FbSmooth);
    parent->StoreInteger("Smooth_Steps", m_FbSmoothSteps);    
    parent->StoreDouble("Smooth_Weight", m_FbSmoothWeight);    
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
    if (node->RestoreInteger("Wrappers_Num", m_nWrappers) != MAF_OK)
      m_nWrappers = 0;  //no wrapper available

    if (node->RestoreInteger("InputMode", m_InputMode) != MAF_OK)
      m_InputMode = DEFAULT_INPUT_MODE;

    if (node->RestoreInteger("VisualMode", m_VisMode) != MAF_OK)
      m_VisMode = DEFAULT_VISUAL_MODE; 

    if (node->RestoreInteger("Fibers_Type", m_FbTemplate) != MAF_OK)
      m_FbTemplate = DEFAULT_FIBERS_TYPE;

    if (node->RestoreInteger("Fibers_Num", m_FbNumFib) != MAF_OK)
      m_FbNumFib = DEFAULT_FIBERS_NUM;

    if (node->RestoreInteger("Fibers_Res", m_FbResolution) != MAF_OK)
      m_FbResolution = DEFAULT_FIBERS_RES;

    if (node->RestoreDouble("Fibers_Thickness", m_FbThickness) != MAF_OK)
      m_FbThickness = DEFAULT_FIBERS_THICKNESS;

    if (node->RestoreInteger("Fibers_Smooth", m_FbSmooth) != MAF_OK)
      m_FbSmooth = DEFAULT_FIBERS_SMOOTH;

    if (node->RestoreInteger("Smooth_Steps", m_FbSmoothSteps) != MAF_OK)
      m_FbSmoothSteps = DEFAULT_FIBERS_SMOOTHSTEPS;

    if (node->RestoreDouble("Smooth_Weight", m_FbSmoothWeight) != MAF_OK)
      m_FbSmoothWeight = DEFAULT_FIBERS_SMOOTHWEIGHT;
    
    m_bNeedUpdate = true;
    return MAF_OK;
  }

  return MAF_ERROR;  
}

#pragma region GetVMEs
//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetMuscleVME_RP()
//-------------------------------------------------------------------------
{
  return m_MuscleVme;
}
//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetWrapperVME_RP(int nIndex)
//-------------------------------------------------------------------------
{
  WRAPPER_ITEM* pItem = m_pWrappers;
  while (nIndex != 0 && pItem != NULL) 
  {
    pItem = pItem->pNext;
    nIndex--;
  }  

  return pItem == NULL ? NULL : pItem->pVmeRP_CP[0];
}

//-------------------------------------------------------------------------
mafVME *medVMEMuscleWrapper::GetWrapperVME(int nIndex)
//-------------------------------------------------------------------------
{
  WRAPPER_ITEM* pItem = m_pWrappers;
  while (nIndex != 0 && pItem != NULL) 
  {
    pItem = pItem->pNext;
    nIndex--;
  }  

  return pItem == NULL ? NULL : pItem->pVmeRP_CP[1];
}

//-------------------------------------------------------------------------
//Gets the origin area VME in its current pose
mafVME* medVMEMuscleWrapper::GetFibersOriginVME()
//-------------------------------------------------------------------------
{
  return m_OIVME[0];
}

//-------------------------------------------------------------------------
//Gets the insertion area VME in its current pose
mafVME* medVMEMuscleWrapper::GetFibersInsertionVME()
//-------------------------------------------------------------------------
{
  return m_OIVME[1];
}
#pragma endregion

//-----------------------------------------------------------------------
//NB. mafVMEMeter and MAF is really stupid, it generates new data even if
//nothing has changed (especially lot of data is generated during the animation)
//we will need to avoid redundant deformation as much as possible
void medVMEMuscleWrapper::InternalUpdate()
//-----------------------------------------------------------------------
{ 
  if (m_bDoNotUpdate)
    return;

  //this happens when the user just checks VME without its selection  
  //or deletes some linked VME from VME tree
  if (!m_bLinksRestored)
    RestoreMeterLinks();  //so we restore links      

  //update curves of every Wrapper, if needed
  bool bCurvesUpdated = false;

  WRAPPER_ITEM* pItem = m_pWrappers;
  while (pItem != NULL)
  {
    //in simple mode, we need CP vme only
    //in advanced mode, both VMEs (RP and CP) must be present
    if ((m_InputMode == 0 && pItem->pVmeRP_CP[1] != NULL) ||
      (pItem->pVmeRP_CP[0] != NULL && pItem->pVmeRP_CP[1] != NULL))
    {           
      for (int i = 0; i < 2; i++)
      { 
        vtkPolyData* pPoly;
        if (m_InputMode != 0 || i != 0)
        {
          //RP in advanced mode or CP
          pPoly = vtkPolyData::SafeDownCast(pItem->pVmeRP_CP[i]->GetOutput()->GetVTKData());
          pPoly->Update();    //force update
        }
        else
        {
          //RP in simple mode => we need to get the data for time 0
          double t = pItem->pVmeRP_CP[1]->GetTimeStamp();
          SetVmeTimeStamp(pItem->pVmeRP_CP[1], 0);
          
          pPoly = vtkPolyData::SafeDownCast(pItem->pVmeRP_CP[1]->GetOutput()->GetVTKData());
          pPoly->Update();    //force update
                  
          SetVmeTimeStamp(pItem->pVmeRP_CP[1], t);
        }

        unsigned long nNewCheckSum = ComputeCheckSum(pPoly);
        if (nNewCheckSum != pItem->VMECheckSums[i])
        {
          //the curve has changed => we need to create a new refined curve
          vtkDEL(pItem->pCurves[i]);

          //mafVMEMeter (version 20.10.2008) generates corrupted polylines, 
          //they contain duplicated coordinates and edges, e.g.
          //Pts: 0(318,305,-467), 1(379,292,-824), 2(388,310, -871), 3(379,292,-824)
          //Edges: 0-1,1-2,2-3 => vertex 1 and 3 are redundant
          pItem->pCurves[i] = FixPolyline(pPoly);
          pItem->VMECheckSums[i] = nNewCheckSum;
          bCurvesUpdated = true;
        }      
      } //end for
    } //end if

    pItem = pItem->pNext;    
  } //endwhile
         
  if (m_bNeedUpdate || bCurvesUpdated)
  {    
    //OK, we will need deform muscle
    wxBusyCursor busy;    
    if (m_MuscleVme == NULL)    
    {
      //if there is no valid associated muscle, output is empty
      m_PolyData->SetPoints(NULL);
      m_PolyData->SetPolys(NULL);    
    }
    else
    {
      //there is a valid associated muscle      
      vtkPolyData* pPoly;
      if (m_InputMode != 0)
      {
        pPoly = vtkPolyData::SafeDownCast(m_MuscleVme->GetOutput()->GetVTKData());
        pPoly->Update();
      }
      else
      {
        double t = m_MuscleVme->GetTimeStamp();
        m_MuscleVme->SetTimeStamp(0);

        pPoly = vtkPolyData::SafeDownCast(m_MuscleVme->GetOutput()->GetVTKData());
        pPoly->Update();    //force update

        m_MuscleVme->SetTimeStamp(t);
      }
     
      DeformMuscle(pPoly);
    } //end if muscle exists

    m_bDoNotUpdate = true;  //prevent recursion
    GetOutput()->Update();  //this calls recursively our update    
    m_bDoNotUpdate = false;

    m_bNeedUpdate = false;
  } //if (m_bNeedUpdate)  
}

//------------------------------------------------------------------------
//Sets the new time for the given vme and ALL vmes linked to it
void medVMEMuscleWrapper::SetVmeTimeStamp(mafVME* vme, double t)
//------------------------------------------------------------------------
{
  vme->SetTimeStamp(t);
  mafLinksMap* pLinks = vme->GetLinks();   

  for (mafLinksMap::iterator i = pLinks->begin(); i != pLinks->end(); i++)
  {
    mafVME* n = mafVME::SafeDownCast(i->second.m_Node);
    if (n != NULL)
      n->SetTimeStamp(t);
  }
}

//------------------------------------------------------------------------
//Deforms the muscle according to existing wrappers.
//It also invokes the generation of fibers, if required.
void medVMEMuscleWrapper::DeformMuscle(vtkPolyData* pMuscle)
//------------------------------------------------------------------------
{
  vtkMAFSmartPointer< vtkMAFPolyDataDeformation > pDeformer;
  
  int nCurves = 0;
  pDeformer->SetNumberOfSkeletons(0);

  //BES: 14.1.2009 - added correspondence to avoid problems when rest pose is 
  //very different from the current pose
  vtkIdList* pCorrespondence = vtkIdList::New();
  pCorrespondence->InsertNextId(0);
  pCorrespondence->InsertNextId(0);

  WRAPPER_ITEM* pItem = m_pWrappers;
  while (pItem != NULL)
  {    
    if (pItem->pCurves[0] != NULL && pItem->pCurves[1] != NULL) {
      pDeformer->SetNthSkeleton(nCurves++, pItem->pCurves[0], pItem->pCurves[1], pCorrespondence);          
    }
    
    pItem = pItem->pNext;
  }

  pCorrespondence->Delete();
  
  //if we have no valid pair of curves, just pass original muscle data
  if (nCurves == 0)
  {
    if (m_VisMode == 0)
      m_PolyData->DeepCopy(pMuscle);
    else
      GenerateFibers(pMuscle);    //we want to generate muscles
  }
  else
  {
    //otherwise perform the deformation
    pDeformer->SetInput(pMuscle);

    if (m_VisMode == 0)  //we do not want to generate fibers    
    {
      pDeformer->SetOutput(m_PolyData);    
      pDeformer->Update();

      //disconnect PolyData from its source
      pDeformer->SetOutput(NULL);
    }
    else
    {
      //generate fibers
      pDeformer->Update();
      GenerateFibers(pDeformer->GetOutput());    
    }
  }
}

//#define _DEBUG_SAVE_VME
#ifdef _DEBUG_SAVE_VME
#include "mafVMESurface.h"
#endif
//------------------------------------------------------------------------
//Generates fibers for the given muscle
void medVMEMuscleWrapper::GenerateFibers(vtkPolyData* pMuscle)
//------------------------------------------------------------------------
{
  vtkMAFMuscleFibers* pFibres = NULL;
  switch (m_FbTemplate)
  {
  case FT_PARALLEL: pFibres = vtkMAFParallelMuscleFibers::New(); break;
  case FT_PENNATE: pFibres = vtkMAFPennateMuscleFibers::New(); break;
  case FT_CURVED: pFibres = vtkMAFCurvedMuscleFibers::New(); break;
  case FT_FANNED: pFibres = vtkMAFFannedMuscleFibers::New(); break;
  case FT_RECTUS: pFibres = vtkMAFRectusMuscleFibers::New(); break;
  }

  if (pFibres == NULL)
    return;

  vtkPoints* ori_points = CreatePointsFromVME(m_OIVME[0]);
  vtkPoints* ins_points = CreatePointsFromVME(m_OIVME[1]);

  vtkMAFMuscleDecomposition* pMD = vtkMAFMuscleDecomposition::New();
  pMD->SetInput(pMuscle);
  pMD->SetFibersTemplate(pFibres);
  pMD->SetNumberOfFibres(m_FbNumFib);
  pMD->SetResolution(m_FbResolution);
  pMD->SetOriginArea(ori_points);
  pMD->SetInsertionArea(ins_points);
  pMD->SetSmoothFibers(m_FbSmooth);
  pMD->SetSmoothSteps(m_FbSmoothSteps);
  pMD->SetSmoothFactor(m_FbSmoothWeight);
  pMD->SetDebugOutputMode(m_FbDebugShowTemplate);

  if (m_FbThickness == 0.0)
    pMD->SetOutput(m_PolyData);

  pMD->Update();

  vtkDEL(ori_points);
  vtkDEL(ins_points);

  if (m_FbThickness == 0.0)
    pMD->SetOutput(NULL); //disconnect output
  else
  {
    vtkTubeFilter* pTube = vtkTubeFilter::New();
    pTube->SetInput(pMD->GetOutput());
    //pTube->UseDefaultNormalOff();        
    //pTube->SetCapping(true);
    pTube->SetNumberOfSides(8);
    pTube->SetRadius(m_FbThickness); //0.01);
    pTube->SetOutput(m_PolyData);
    pTube->Update();

    pTube->SetOutput(NULL);
    pTube->Delete();
  }
  pMD->Delete();
  pFibres->Delete();

#ifdef _DEBUG_SAVE_VME
  //this code saves the output as a new VME
  mafVMESurface* VME;
  mafNEW(VME);
  VME->ReparentTo(this);
  VME->SetData(m_PolyData, 0);
  VME->SetName(wxString::Format("CONTOURS_%d", m_FbResolution));  
  
  mafEvent ev(this, VME_ADD, VME);
  this->ForwardUpEvent(ev);
#endif
}


//------------------------------------------------------------------------
//Compute checksum for VTK polydata.
unsigned long medVMEMuscleWrapper::ComputeCheckSum(vtkPolyData* pPoly)
//------------------------------------------------------------------------
{
  vtkDataArray* pDA = pPoly->GetPoints()->GetData();
  
  unsigned long nChecksum = pDA->GetSize();
  unsigned long* pData = (unsigned long*)pDA->GetVoidPointer(0);
  int nSize = nChecksum / sizeof(unsigned long);  //points are written as floats (4B) or doubles (8B); sizeof(unsigned long) = 4B

  for (int i = 0; i < nSize; i++){
    nChecksum = nChecksum ^ pData[i];     //very simple checksum
  }

  return nChecksum | 1; //checksum may not be zero (because 0 is used as Checksum not computed)
}

//------------------------------------------------------------------------
//Creates points form landmark cloud vme, landmark, etc.
//N.B. the caller is responsible for deleting the returned object.
vtkPoints* medVMEMuscleWrapper::CreatePointsFromVME(mafVME* vme)
//------------------------------------------------------------------------
{
  vtkPoints* pRet = NULL;

  mafVMELandmarkCloud* cloud = mafVMELandmarkCloud::SafeDownCast(vme);
  if (cloud != NULL)
  {
    int N = cloud->GetNumberOfLandmarks();
    if (N != 0)
    {
      pRet = vtkPoints::New();
      pRet->SetNumberOfPoints(N);
      for (int i = 0; i < N; i++) 
      {
        double x[3];
        cloud->GetLandmarkPosition(i, x);

        pRet->SetPoint(i, x);
      }
    }
  }
  else
  {
    mafVMELandmark* landmark = mafVMELandmark::SafeDownCast(vme);
    if (landmark != NULL)
    {
      double x[3];
      landmark->GetPoint(x);

      pRet = vtkPoints::New();
      pRet->InsertNextPoint(x);
    }
    else if (vme != NULL)
    {
      //general data
      vtkDataSet* ds = vme->GetOutput()->GetVTKData();
      if (ds != NULL)
      {
        int N = ds->GetNumberOfPoints();
        if (N != 0)
        {
          pRet = vtkPoints::New();
          pRet->SetNumberOfPoints(N);
          for (int i = 0; i < N; i++) {
            pRet->SetPoint(i, ds->GetPoint(i));
          }
        }
      }
    }
  }

  if (pRet != NULL)
  {
    //coordinates are local => they do not change when the surface to which
    //those landmarks are pinned moves => we need to get absolute positions
    mafTransform* transform;

    mafNEW(transform);
    transform->SetMatrix(*vme->GetOutput()->GetAbsMatrix());

    int N = pRet->GetNumberOfPoints();
    for (int i = 0; i < N; i++)
    {
      double x[3];
      transform->TransformPoint(pRet->GetPoint(i), x);
      pRet->SetPoint(i, x);
    }


    mafDEL(transform);
  }

  return pRet;
}

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

#pragma region GUI and Events Handling
//-------------------------------------------------------------------------
mafGUI* medVMEMuscleWrapper::CreateGui()
//-------------------------------------------------------------------------
{  
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  
  RestoreMeterLinks();
  m_MuscleVmeName = m_MuscleVme == NULL ? wxT("") : m_MuscleVme->GetName();
  for (int i = 0; i < 2; i++){
    m_OIVMEName[i] = m_OIVME[i] == NULL ? wxT("") : m_OIVME[i]->GetName();
  }

#pragma region Generated Code from wxFormBuilder  
  wxBoxSizer* bSizer18 = new wxBoxSizer( wxVERTICAL );
  wxStaticBoxSizer* sbSizer14 = new wxStaticBoxSizer( 
    new wxStaticBox( m_Gui, wxID_ANY, wxT("Operational Mode") ), wxVERTICAL );

  wxString radioBox1Choices[] = { wxT("Simple"), wxT("Advanced") };
  int radioBox1NChoices = sizeof( radioBox1Choices ) / sizeof( wxString );
  wxRadioBox* radioBox1 = new wxRadioBox( m_Gui, ID_INPUTMODE, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, radioBox1NChoices, 
    radioBox1Choices, 1, wxRA_SPECIFY_ROWS );  
  radioBox1->SetToolTip( 
    wxT("Specify the mode that will be used for the wrapping. In the simple mode, "
    "the input muscle VME at the time 0 is deformed according to the differences "
    "between outputs from wrappers at the time 0 and outputs from wrappers at the "
    "current time. In the advanced mode, the user needs to specify different wrappers "
    "for the rest pose (they may not change in the time) and for the current pose.") );

  sbSizer14->Add( radioBox1, 0, wxALL|wxEXPAND, 1 );

  wxCheckBox* checkBox9 = new wxCheckBox( m_Gui, ID_GENERATE_FIBERS, 
    wxT("Generate fibers"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBox9->SetToolTip( 
    wxT("If checked, the output are muscle fibers instead of deformed surface.") );

  sbSizer14->Add( checkBox9, 0, wxALL, 5 );
  bSizer18->Add( sbSizer14, 0, wxEXPAND, 1 );

  wxStaticBoxSizer* sbSizer8 = new wxStaticBoxSizer( 
    new wxStaticBox( m_Gui, wxID_ANY, wxT("Muscle Surface") ), wxVERTICAL );

  wxBoxSizer* bSizer19 = new wxBoxSizer( wxHORIZONTAL );
  wxTextCtrl* MuscleCtrl = new wxTextCtrl( m_Gui, wxID_ANY, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  bSizer19->Add( MuscleCtrl, 1, wxALL, 1 );

  wxButton* bttnSelectMuscle = new wxButton( m_Gui, 
    ID_RESTPOSE_MUSCLE_LINK, wxT("Select"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
  bttnSelectMuscle->SetToolTip( wxT("Selects the VME representing the muscle surface to be wrapped.") );

  bSizer19->Add( bttnSelectMuscle, 0, wxALL, 1 );
  sbSizer8->Add( bSizer19, 1, wxEXPAND, 1 );
  bSizer18->Add( sbSizer8, 0, wxEXPAND, 1 );

  wxStaticBoxSizer* sbSizer9 = new wxStaticBoxSizer( 
    new wxStaticBox( m_Gui, wxID_ANY, wxT("Wrappers") ), wxVERTICAL );

  m_WrappersCtrl = new wxListCtrl( m_Gui, ID_LIST_WRAPPERS, wxDefaultPosition, 
    wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL );
  sbSizer9->Add( m_WrappersCtrl, 1, wxALL|wxEXPAND, 1 );

  wxBoxSizer* bSizer121 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer121->Add( new wxPanel( m_Gui, wxID_ANY, wxDefaultPosition, 
    wxDefaultSize, wxTAB_TRAVERSAL ), 1, wxALL, 5 );

  m_BttnRemoveWrapper = new wxButton( m_Gui, ID_REMOVEWRAPPER, wxT("Remove"), 
    wxDefaultPosition, wxSize( 50,-1 ), 0 );
  m_BttnRemoveWrapper->Enable( false );
  m_BttnRemoveWrapper->SetToolTip( wxT("Removes selected wrapper.") );
  bSizer121->Add( m_BttnRemoveWrapper, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

  sbSizer9->Add( bSizer121, 0, wxEXPAND, 1 );

#pragma region Add Wrapper
  wxStaticBoxSizer* sbSizer13 = new wxStaticBoxSizer( 
    new wxStaticBox( m_Gui, wxID_ANY, wxT("Add New Wrapper") ), wxVERTICAL );

  wxBoxSizer* bSizer5 = new wxBoxSizer( wxHORIZONTAL );
  m_LabelRP = new wxStaticText( m_Gui, wxID_ANY, wxT("RP:"), wxDefaultPosition, 
    wxSize( 25,-1 ), wxALIGN_RIGHT );  
  m_LabelRP->Enable( false );

  bSizer5->Add( m_LabelRP, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  m_RPNameCtrl = new wxTextCtrl( m_Gui, ID_RESTPOSE_WRAPPER_LINK, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  m_RPNameCtrl->Enable( false );

  bSizer5->Add( m_RPNameCtrl, 1, wxALL, 1 );

  m_BttnSelRP = new wxButton( m_Gui, ID_SELECT_RP, wxT("Select"), 
    wxDefaultPosition, wxSize( 50,-1 ), 0 );
  m_BttnSelRP->Enable( false );
  m_BttnSelRP->SetToolTip( 
    wxT("Selects VME with the wrapper (in the rest pose). \n\n"
        "Note: the mesh deformation is governed by the difference between the output "
        "from wrappers in the rest pose and the current pose.") );

  bSizer5->Add( m_BttnSelRP, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
  sbSizer13->Add( bSizer5, 0, wxEXPAND, 1 );

  wxBoxSizer* bSizer51 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer51->Add( new wxStaticText( m_Gui, wxID_ANY, wxT("CP:"), wxDefaultPosition, 
    wxSize( 25,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  wxTextCtrl* CPNameCtrl = new wxTextCtrl( m_Gui, ID_CURRENTPOSE_WRAPPER_LINK, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  bSizer51->Add( CPNameCtrl, 1, wxALL, 1 );

  wxButton* bttnSelCP = new wxButton( m_Gui, ID_SELECT_CP, wxT("Select"), 
    wxDefaultPosition, wxSize( 50,-1 ), 0 );
  bttnSelCP->SetToolTip( 
    wxT("Selects VME with the wrapper (in the current pose). \n\n"
        "Note: the mesh deformation is governed by the difference between the output "
        "from wrappers in the rest pose and the current pose.") );

  bSizer51->Add( bttnSelCP, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
  sbSizer13->Add( bSizer51, 0, wxEXPAND, 1 );

  wxBoxSizer* bSizer12 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer12->Add( new wxPanel( m_Gui, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
    wxTAB_TRAVERSAL ), 1, wxALL, 1 );

  m_BttnAddWrapper = new wxButton( m_Gui, ID_ADDWRAPPER, wxT("Add"), 
    wxDefaultPosition, wxSize( 50,-1 ), 0 );
  m_BttnAddWrapper->Enable( false );
  m_BttnAddWrapper->SetToolTip( wxT("Adds a new wrapper") );

  bSizer12->Add( m_BttnAddWrapper, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1 );
  sbSizer13->Add( bSizer12, 1, wxEXPAND, 1 );
  sbSizer9->Add( sbSizer13, 0, wxEXPAND, 1 );
  bSizer18->Add( sbSizer9, 1, wxEXPAND, 1 );
#pragma endregion Add Wrapper

#pragma region Fibers Options
  wxStaticBoxSizer* sbSizer15 = new wxStaticBoxSizer( 
    new wxStaticBox( m_Gui, wxID_ANY, wxT("Fibers Options") ), wxVERTICAL );

  wxBoxSizer* bSizer511 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer511->Add( new wxStaticText( m_Gui, wxID_ANY, wxT("O:"), wxDefaultPosition, 
    wxSize( 30,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  wxTextCtrl* OAreaName = new wxTextCtrl( m_Gui, wxID_ANY, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  bSizer511->Add( OAreaName, 1, wxALL, 1 );

  wxButton* bttnSelOA = new wxButton( m_Gui, ID_FIBERS_ORIGIN_LINK, wxT("Select"), 
    wxDefaultPosition, wxSize( 50,-1 ), 0 );
  bttnSelOA->SetToolTip( 
    wxT("[OPTIONAL] Select the VME (a single landmark or a landmark cloud) representing "
    "the origin area.\n\nN.B. This information (together with I) is used to determine "
    "the direction of fibers. If neither O nor I is specified, the generated "
    "fibers might be incorrect.") );
  bSizer511->Add( bttnSelOA, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
  sbSizer15->Add( bSizer511, 0, wxEXPAND, 1 );

  wxBoxSizer* bSizer5111 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer5111->Add( new wxStaticText( m_Gui, wxID_ANY, wxT("I:"), wxDefaultPosition, 
    wxSize( 30,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  wxTextCtrl* IAreaName = new wxTextCtrl( m_Gui, wxID_ANY, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
  bSizer5111->Add( IAreaName, 1, wxALL, 1 );

  wxButton* bttnSelIA = new wxButton( m_Gui, ID_FIBERS_INSERTION_LINK, wxT("Select"), 
    wxDefaultPosition, wxSize( 50,-1 ), 0 );
  bttnSelIA->SetToolTip( 
    wxT("Select the VME (a single landmark or a landmark cloud) representing the "
    "insertion area.\n\nN.B. This information (together with O) is used to determine "
    "the direction of fibers. If neither O nor I is specified, the generated "
    "fibers might be incorrect.") );
  bSizer5111->Add( bttnSelIA, 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
  sbSizer15->Add( bSizer5111, 0, wxEXPAND, 1 );
   
  wxBoxSizer* bSizer51111 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer51111->Add( new wxStaticText( m_Gui, wxID_ANY, wxT("Type:"), wxDefaultPosition, 
    wxSize( 30,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  wxString choice1Choices[] = { wxT("parallel"), wxT("pennate"), wxT("curved"), 
    wxT("fanned"), wxT("rectus") };

  int choice1NChoices = sizeof( choice1Choices ) / sizeof( wxString );
  wxComboBox* choice1 = new wxComboBox( m_Gui, ID_FIBERS_TEMPLATE, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, choice1NChoices, choice1Choices, wxCB_READONLY );  
  choice1->SetToolTip( wxT("Selects the geometry type of fibers for the current muscle.") );
  bSizer51111->Add( choice1, 1, wxALL, 1 );
  
  bSizer51111->Add( new wxStaticText( m_Gui, wxID_ANY, wxT("Num.:"), wxDefaultPosition, 
    wxSize( 35,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  wxTextCtrl* textCtrl20 = new wxTextCtrl( m_Gui, ID_FIBERS_NUMFIB, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, 0 );
  textCtrl20->SetToolTip( wxT("Specifies the number of fibers to be created within muscle volume.") );
  bSizer51111->Add( textCtrl20, 1, wxALL, 1 );  
  sbSizer15->Add( bSizer51111, 0, wxEXPAND, 1 );


  wxBoxSizer* bSizer58 = new wxBoxSizer( wxHORIZONTAL );  
  bSizer58->Add( new wxStaticText( m_Gui, wxID_ANY, wxT("Res.:"), wxDefaultPosition, 
    wxSize( 30,-1 ), wxALIGN_RIGHT ), 1, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  wxTextCtrl* textCtrl201 = new wxTextCtrl( m_Gui, ID_FIBERS_RESOLUTION, wxEmptyString, 
    wxDefaultPosition, wxDefaultSize, 0 );
  textCtrl201->SetToolTip( wxT("Specifies the resolution of fiber.") );
  bSizer58->Add( textCtrl201, 1, wxALL, 1 );
  
  bSizer58->Add( new wxStaticText( m_Gui, wxID_ANY, wxT("Thick.:"), 
    wxDefaultPosition, wxSize( 35,-1 ), wxALIGN_RIGHT ), 0, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  wxTextCtrl* textCtrl2011 = new wxTextCtrl( m_Gui, ID_FIBERS_THICKNESS, 
    wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  textCtrl2011->SetToolTip( 
    wxT("Specifies the thickness of fibers. If the thickness is 0, fibers are "
    "represented by polylines, otherwise they are represented by cylinders with "
    "radius equaled to the given thickness.") );

  bSizer58->Add( textCtrl2011, 1, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  sbSizer15->Add( bSizer58, 0, wxEXPAND, 1 );

  wxCheckBox* checkBox10 = new wxCheckBox( m_Gui, ID_FIBERS_SMOOTH, wxT("Smooth fibers"), 
    wxDefaultPosition, wxDefaultSize, 0 );  
  checkBox10->SetToolTip( wxT("If checked, a smoothing process is applied on the generated fibers") );

  sbSizer15->Add( checkBox10, 0, wxALL, 5 );


#pragma region Smoothing Options
  wxStaticBoxSizer* sbSizer16;
  sbSizer16 = new wxStaticBoxSizer( new wxStaticBox( m_Gui, wxID_ANY, wxT("Smoothing Options") ), wxVERTICAL );

  wxBoxSizer* bSizer6;
  bSizer6 = new wxBoxSizer( wxHORIZONTAL );

  m_SmLabel1 = new wxStaticText( m_Gui, wxID_ANY, wxT("Steps:"), wxDefaultPosition, wxSize( 30,-1 ), wxALIGN_RIGHT );  
  bSizer6->Add( m_SmLabel1, 1, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  m_SmStepsCtrl = new wxTextCtrl( m_Gui, ID_FIBERS_SMOOTH_STEPS, wxEmptyString, wxDefaultPosition, wxSize( 30,-1 ), 0 );
  m_SmStepsCtrl->SetToolTip( wxT("Specifies the number of smoothing iterations (higher value means more smoothed fibres)") );

  bSizer6->Add( m_SmStepsCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  m_SmLabel2 = new wxStaticText( m_Gui, wxID_ANY, wxT("Weight:"), wxDefaultPosition, wxSize( 45,-1 ), wxALIGN_RIGHT );  
  bSizer6->Add( m_SmLabel2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 1 );

  m_SmWeightCtrl = new wxTextCtrl( m_Gui, ID_FIBERS_SMOOTH_STEPS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
  m_SmWeightCtrl->SetToolTip( wxT("Specifies the number of smoothing iterations (higher value means more smoothed fibres)") );

  bSizer6->Add( m_SmWeightCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 1 );
  sbSizer16->Add( bSizer6, 0, wxEXPAND, 5 );
  sbSizer15->Add( sbSizer16, 0, wxEXPAND, 5 );
#pragma endregion Smoothing Options

  wxCheckBox* checkBox11 = new wxCheckBox( m_Gui, ID_FIBERS_DEBUG_SHOWTEMPLATE, wxT("Show template"), wxDefaultPosition, wxDefaultSize, 0 );
  checkBox11->SetToolTip( wxT("If checked, the output is a set of fibres with a cube - target cube") );

  sbSizer15->Add( checkBox11, 0, wxALL, 5 );
#pragma endregion Fibers Options

  bSizer18->Add( sbSizer15, 0, wxEXPAND, 1 );

#pragma endregion

  //create headers for the listctrl
  wxString cols[2] = { wxT("RP"), wxT("CP") };
  for (int i = 0; i < 2; i++){
    m_WrappersCtrl->InsertColumn(i, cols[i]);
  }

  //populate list
  WRAPPER_ITEM* pItem = m_pWrappers;
  while (pItem != NULL)
  {
    AddWrapper(pItem);
    pItem = pItem->pNext;
  }

  //validators for the first part
  radioBox1->SetValidator(mafGUIValidator(this, ID_INPUTMODE, radioBox1, &m_InputMode));
  checkBox9->SetValidator(mafGUIValidator(this, ID_GENERATE_FIBERS, checkBox9, &m_VisMode));
  MuscleCtrl->SetValidator(mafGUIValidator(this, wxID_ANY, MuscleCtrl, &m_MuscleVmeName));
  bttnSelectMuscle->SetValidator(mafGUIValidator(this, ID_RESTPOSE_MUSCLE_LINK, bttnSelectMuscle));

  //validators for Wrappers

  m_BttnSelRP->SetValidator(mafGUIValidator(this, ID_SELECT_RP, m_BttnSelRP));
  bttnSelCP->SetValidator(mafGUIValidator(this, ID_SELECT_CP, bttnSelCP));
  m_BttnAddWrapper->SetValidator(mafGUIValidator(this, ID_ADDWRAPPER, m_BttnAddWrapper));
  m_BttnRemoveWrapper->SetValidator(mafGUIValidator(this, ID_REMOVEWRAPPER, m_BttnRemoveWrapper));
  m_RPNameCtrl->SetValidator(mafGUIValidator(this, ID_RESTPOSE_WRAPPER_LINK, m_RPNameCtrl, &m_WrappersVmeName[0]));
  CPNameCtrl->SetValidator(mafGUIValidator(this, ID_CURRENTPOSE_WRAPPER_LINK, CPNameCtrl, &m_WrappersVmeName[1]));

  //validators for Fiber Options
  OAreaName->SetValidator(mafGUIValidator(this, wxID_ANY, OAreaName, &m_OIVMEName[0]));
  IAreaName->SetValidator(mafGUIValidator(this, wxID_ANY, IAreaName, &m_OIVMEName[1]));
  bttnSelOA->SetValidator(mafGUIValidator(this, ID_FIBERS_ORIGIN_LINK, bttnSelOA));
  bttnSelIA->SetValidator(mafGUIValidator(this, ID_FIBERS_INSERTION_LINK, bttnSelIA));
  choice1->SetValidator(mafGUIValidator(this, ID_FIBERS_TEMPLATE, choice1, &m_FbTemplate));
  textCtrl20->SetValidator(mafGUIValidator(this, ID_FIBERS_NUMFIB, textCtrl20, &m_FbNumFib, 1, 10000));
  textCtrl201->SetValidator(mafGUIValidator(this, ID_FIBERS_RESOLUTION, textCtrl201, &m_FbResolution, 1, 499));     
  textCtrl2011->SetValidator(mafGUIValidator(this, ID_FIBERS_THICKNESS, textCtrl2011, &m_FbThickness, 0.0, MAXDOUBLE, -1));
  checkBox10->SetValidator(mafGUIValidator(this, ID_FIBERS_SMOOTH, checkBox10, &m_FbSmooth));
  m_SmStepsCtrl->SetValidator(mafGUIValidator(this, ID_FIBERS_SMOOTH_STEPS, m_SmStepsCtrl, &m_FbSmoothSteps, 1, 100));
  m_SmWeightCtrl->SetValidator(mafGUIValidator(this, ID_FIBERS_SMOOTH_WEIGHT, m_SmWeightCtrl, &m_FbSmoothWeight, 0.0, MAXDOUBLE, -1));
  checkBox11->SetValidator(mafGUIValidator(this, ID_FIBERS_DEBUG_SHOWTEMPLATE, checkBox11, &m_FbDebugShowTemplate));  

  m_Gui->Add(bSizer18);
  m_Gui->FitGui();

  UpdateControls();
  InternalUpdate();  
  return m_Gui;
}

//-------------------------------------------------------------------------
void medVMEMuscleWrapper::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{  
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    bool bNeedUpdate = false;
    bool bEventHandled = true;

    int nId = e->GetId();
    switch (nId)
    {
    //input mode has changed from simple to advanced or vice versa
    case ID_INPUTMODE:
      if (m_InputMode == 1 && m_pWrappers != NULL)
      {
        wxMessageBox(_("Wrappers inserted in simple mode might not work in advanced mode."),
           _("Warning"), wxCENTRE | wxOK | wxICON_INFORMATION);
      }

      UpdateControls();      
      bNeedUpdate = true;
      break;
    

    case ID_RESTPOSE_MUSCLE_LINK:
      bNeedUpdate = SelectVme(_("Choose muscle vme link (in the rest pose)"),
        (long)&medVMEMuscleWrapper::VMEAcceptMuscle, m_MuscleVme, m_MuscleVmeName);
      break;

    case ID_SELECT_RP:
      if (SelectVme(_("Choose wrapper vme link (in the rest pose)"),
        (long)&medVMEMuscleWrapper::VMEAcceptWrapper, m_WrappersVme[0], m_WrappersVmeName[0])
        )        
        m_BttnAddWrapper->Enable(m_WrappersVme[1] != NULL);
      break;

    case ID_SELECT_CP:
      if (SelectVme(_("Choose wrapper vme link (in the current pose)"),
        (long)&medVMEMuscleWrapper::VMEAcceptWrapper, m_WrappersVme[1], m_WrappersVmeName[1])
        )
          m_BttnAddWrapper->Enable(m_InputMode == 0 || m_WrappersVme[0] != NULL);      
      break;

    case ID_ADDWRAPPER:
      AddWrapper(m_WrappersVme[0], m_WrappersVme[1]);
      m_WrappersVme[0] = m_WrappersVme[1] = NULL;
      m_WrappersVmeName[0] = m_WrappersVmeName[1] = wxT("");
      m_BttnAddWrapper->Enable(FALSE);
      m_Gui->Update();

      m_nWrappers++;
      m_BttnRemoveWrapper->Enable();
      bNeedUpdate = true;
      break;

    case ID_REMOVEWRAPPER:
      {
        //find the selected item
        int nIndex = m_WrappersCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED); 
        if (nIndex >= 0)
        {
          RemoveWrapper(nIndex);

          m_BttnRemoveWrapper->Enable((--m_nWrappers) != 0);
          bNeedUpdate = true;
        }

        break;
      }

    case ID_FIBERS_ORIGIN_LINK:
      bNeedUpdate = SelectVme(_("Choose a landmark cloud (or a landmark) that represent the origin area of muscle."),
        (long)&medVMEMuscleWrapper::VMEAcceptOIAreas, m_OIVME[0], m_OIVMEName[0]);

      bNeedUpdate &= m_VisMode != 0;
      break;

    case ID_FIBERS_INSERTION_LINK:
      bNeedUpdate = SelectVme(_("Choose a landmark cloud (or a landmark) that represent the insertion area of muscle."),
        (long)&medVMEMuscleWrapper::VMEAcceptOIAreas, m_OIVME[1], m_OIVMEName[1]);

      bNeedUpdate &= m_VisMode != 0;
      break;

    default:
      if (nId >= ID_GENERATE_FIBERS && nId <= ID_FIBERS_DEBUG_SHOWTEMPLATE)
      {
        bNeedUpdate = nId == ID_GENERATE_FIBERS || m_VisMode != 0;      

        if (nId == ID_FIBERS_SMOOTH)
          UpdateControls();
      }
      else             
        bEventHandled = false;      
      
      break;
    } //end switch

    if (bNeedUpdate)
    {
      m_bNeedUpdate = true;
      InternalUpdate();
      
      //force redrawing
      mafEvent ev(this, VME_SELECTED,this);
      this->ForwardUpEvent(&ev);
      return;
    }

    if (bEventHandled)
      return;    
  }
  
  Superclass::OnEvent(maf_event);

  if (maf_event->GetId() == NODE_DETACHED_FROM_TREE)
  {
    m_bNeedUpdate = true;
    m_bLinksRestored = false; 

    Modified();
  }
}

//------------------------------------------------------------------------
//Adds a new wrapper into the list of wrappers and GUI list
void medVMEMuscleWrapper::AddWrapper(mafVME* pRP, mafVME* pCP)
//------------------------------------------------------------------------
{
  WRAPPER_ITEM* pItem = new WRAPPER_ITEM;
  memset(pItem, 0, sizeof(WRAPPER_ITEM));
  
  pItem->pVmeRP_CP[0] = pRP;
  pItem->pVmeRP_CP[1] = pCP;

  AddWrapper(pItem);
}
//------------------------------------------------------------------------
//Adds a new wrapper into the list of wrappers and GUI list
void medVMEMuscleWrapper::AddWrapper(WRAPPER_ITEM* pItem)
//------------------------------------------------------------------------
{
  int nCount = m_WrappersCtrl->GetItemCount();
  if (nCount == 0)
    m_pWrappers = pItem;
  else
  {
    WRAPPER_ITEM* pPrev = (WRAPPER_ITEM*)m_WrappersCtrl->GetItemData(nCount - 1);
    pItem->pLast = pPrev;
    pPrev->pNext = pItem;
  }
  
  wxString szName[2];
  for (int i = 0; i < 2; i++)
  {
    if (pItem->pVmeRP_CP[i] != NULL)
      szName[i] = pItem->pVmeRP_CP[i]->GetName();
  }  

  m_WrappersCtrl->InsertItem(nCount, szName[0]);
  m_WrappersCtrl->SetItem(nCount, 1, szName[1]);
  m_WrappersCtrl->SetItemData(nCount, (long)pItem);
  m_WrappersCtrl->SetItemState(nCount, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
    wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
  m_WrappersCtrl->EnsureVisible(nCount);

  //and save changes into Links
  StoreMeterLinks(); 
}

//------------------------------------------------------------------------
//Remove wrapper from the GUI and releases its memory
void medVMEMuscleWrapper::RemoveWrapper(int nIndex)
//------------------------------------------------------------------------
{
  WRAPPER_ITEM* pItem = (WRAPPER_ITEM*)m_WrappersCtrl->GetItemData(nIndex);
  if (pItem->pLast != NULL)
    pItem->pLast->pNext = pItem->pNext;
  else
    m_pWrappers = pItem->pNext;

  if (pItem->pNext != NULL)
    pItem->pNext->pLast = pItem->pLast;
  
  vtkDEL(pItem->pCurves[0]);
  vtkDEL(pItem->pCurves[1]);
  delete pItem;

  m_WrappersCtrl->DeleteItem(nIndex);
  if (nIndex == m_WrappersCtrl->GetItemCount())
    nIndex--;

  if (nIndex >= 0)
  {
    m_WrappersCtrl->SetItemState(nIndex, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
      wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    m_WrappersCtrl->EnsureVisible(nIndex);
  }

  //and save changes into Links
  StoreMeterLinks();
}

//------------------------------------------------------------------------
//Updates the visibility (etc) of GUI controls
void medVMEMuscleWrapper::UpdateControls()
//------------------------------------------------------------------------
{
  m_LabelRP->Enable(m_InputMode != 0);
  m_RPNameCtrl->Enable(m_InputMode != 0);
  m_BttnSelRP->Enable(m_InputMode != 0);
  m_BttnAddWrapper->Enable(m_WrappersVme[1] != NULL &&
                           (m_InputMode == 0 || m_WrappersVme[0] != NULL));

  m_SmLabel1->Enable(m_FbSmooth != 0);
  m_SmLabel2->Enable(m_FbSmooth != 0);
  m_SmStepsCtrl->Enable(m_FbSmooth != 0);
  m_SmWeightCtrl->Enable(m_FbSmooth != 0);
}

//------------------------------------------------------------------------
//Shows dialog (with the message in title) where the user selects vme.
//The VME that can be selected are defined by accept_callback.
//If no VME is selected, the routine returns false, otherwise it returns
//reference to the VME, its name and updates GUI
bool medVMEMuscleWrapper::SelectVme(mafString title, 
    long accept_callback, mafVME*& pOutVME, mafString& szOutVmeName)
//------------------------------------------------------------------------
{
  mafEvent ev(this, VME_CHOOSE, accept_callback);
  ev.SetString(&title);
  ForwardUpEvent(ev);

  mafVME* vme = mafVME::SafeDownCast(ev.GetVme());
  if (vme == NULL)
    return false;
  
  szOutVmeName = (pOutVME = vme)->GetName();  
  m_Gui->Update();       
  return true;
}

#pragma region Accept VME Routines
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
}

//------------------------------------------------------------------------
/*static*/ bool medVMEMuscleWrapper::VMEAcceptWrapper(mafNode *node) 
//------------------------------------------------------------------------
{
  mafVME* vme = mafVME::SafeDownCast(node);
  if (vme != NULL)
  {    
    if (
      vme->GetOutput()->IsA("mafVMEOutputMeter") ||
      vme->GetOutput()->IsA("medVMEOutputWrappedMeter") ||
      vme->GetOutput()->IsA("medVMEOutputComputeWrapping")  //TODO: why we cannot live with medVMEOutputWrappedMeter only?
      )      
      return true;
  }

  return false;
}

//------------------------------------------------------------------------
/*static*/ bool medVMEMuscleWrapper::VMEAcceptOIAreas(mafNode *node) 
//------------------------------------------------------------------------
{
  mafVME* vme = mafVME::SafeDownCast(node);
  return vme != NULL && 
    (vme->IsA("mafVMELandmarkCloud") || vme->IsA("mafVMELandmark"));    
}
#pragma endregion Accept VME Routines
#pragma endregion GUI and Events Handling
