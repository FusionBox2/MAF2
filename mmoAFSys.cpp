/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoAFSys.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-10 19:13:46 $
  Version:   $Revision: 1.1 $
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

#ifdef __GNUG__
    #pragma implementation "mmoAFSys.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#include "wx/busyinfo.h"
#include "wx/textfile.h"


#include "mafDecl.h"
#include "mafOp.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mmoAFSys.h"

#include "mafDictionary.h"
#include "mafVMEAFRefSys.h"
#include "mafVMELandmarkCloud.h"
#include "mafSmartPointer.h"

//----------------------------------------------------------------------------
// Required for MSVC
//----------------------------------------------------------------------------
#ifdef _MSC_FULL_VER
#pragma warning (disable: 4786)
#endif

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Forward Refs
//----------------------------------------------------------------------------
enum mafAFBonesList
{
  MBLAF_NA     = -1,
  MBLAF_FIRST  = 0,
  MBLAF_PELVIS = 0,
  MBLAF_RTHIGH = 1,
  MBLAF_LTHIGH  ,
  MBLAF_RSHANK  ,
  MBLAF_LSHANK  ,
  MBLAF_RFOOT   ,
  MBLAF_LFOOT   ,
  MBLAF_UNKNOWN ,
  MBLAF_LAST
};
struct mafAFAltLandmarksInfo
{
  wxChar const   *m_OldName;
  wxChar const   *m_NewName;
  mafAFBonesList m_BoneID;
};
struct mafAFAltBonesInfo
{
  wxChar const    *m_OldName;
  mafAFBonesList  m_BoneID;
};

struct mafAFLMsForCnv
{
  mafAFBonesList m_ID;
  wxChar         *m_LMNames[4];
  unsigned int   m_NumLMs;
  bool           (*m_func)(DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
};

bool _mafBuildPelvisAF       (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
bool _mafBuildLeftThighAF    (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
bool _mafBuildLeftShankAF    (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
bool _mafBuildLeftFootAF     (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
bool _mafBuildRightThighAF   (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
bool _mafBuildRightShankAF   (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
bool _mafBuildRightFootAF    (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);
bool _mafBuildUnknownAF      (DiV4d const *vpLandmarks, DiMatrix *mpMatrix);


static mafAFLMsForCnv _caAFBuildInfo[MBLAF_LAST] =
{
  {MBLAF_PELVIS , {"RAS", "LAS", "RPS", "LPS"}, 4, _mafBuildPelvisAF       },
  {MBLAF_RTHIGH , {"RLE", "RME", "RFH", ""   }, 3, _mafBuildRightThighAF   },
  {MBLAF_LTHIGH , {"LLE", "LME", "LFH", ""   }, 3, _mafBuildLeftThighAF    },
  {MBLAF_RSHANK , {"RHF", "RTT", "RLM", "RMM"}, 4, _mafBuildRightShankAF   },
  {MBLAF_LSHANK , {"LHF", "LTT", "LLM", "LMM"}, 4, _mafBuildLeftShankAF    },
  {MBLAF_RFOOT  , {"RCA", "RFM", "RSM", "RVM"}, 4, _mafBuildRightFootAF    },
  {MBLAF_LFOOT  , {"LCA", "LFM", "LSM", "LVM"}, 4, _mafBuildLeftFootAF     },
  {MBLAF_UNKNOWN, {"PT1", "PT2", "PT3", "PT4"}, 4, _mafBuildUnknownAF      }
};

static mafAFAltBonesInfo _saKnownBones[]  =
{
  {"IPE", MBLAF_PELVIS},
  {"RTH", MBLAF_RTHIGH},
  {"LTH", MBLAF_LTHIGH},
  {"RSH", MBLAF_RSHANK},
  {"LSH", MBLAF_LSHANK},
  {"RFO", MBLAF_RFOOT},
  {"LFO", MBLAF_LFOOT},
};
static mafAFAltLandmarksInfo _saKnownLandmarks[]  =
{
  //pelvic
  {"RAS", "RIAS", MBLAF_PELVIS},
  {"LAS", "LIAS", MBLAF_PELVIS},
  {"RPS", "RIPS", MBLAF_PELVIS},
  {"LPS", "LIPS", MBLAF_PELVIS},
  {"RAC", "RIAC", MBLAF_PELVIS}, // needed for thigh OVP only
  {"LAC", "LIAC", MBLAF_PELVIS}, // needed for thigh OVP only

  //femur
  {"RFH", "RFCH", MBLAF_RTHIGH},
  {"RLE", "RFLE", MBLAF_RTHIGH},
  {"RME", "RFME", MBLAF_RTHIGH},

  {"LFH", "LFCH", MBLAF_LTHIGH},
  {"LLE", "LFLE", MBLAF_LTHIGH},
  {"LME", "LFME", MBLAF_LTHIGH},

  //shank segment: tibia und fibula
  {"RHF", "RFAX", MBLAF_RSHANK},
  {"RTT", "RTTC", MBLAF_RSHANK},
  {"RLM", "RFAL", MBLAF_RSHANK},
  {"RMM", "RTAM", MBLAF_RSHANK},

  {"LHF", "LFAX", MBLAF_LSHANK},
  {"LTT", "LTTC", MBLAF_LSHANK},
  {"LLM", "LFAL", MBLAF_LSHANK},
  {"LMM", "LTAM", MBLAF_LSHANK},

  //foot segment segment
  {"RCA", "RFCC", MBLAF_RFOOT},
  {"RFM", "RFM1", MBLAF_RFOOT},
  {"RSM", "RFM2", MBLAF_RFOOT},
  {"RVM", "RFM5", MBLAF_RFOOT},

  {"LCA", "LFCC", MBLAF_LFOOT},
  {"LFM", "LFM1", MBLAF_LFOOT},
  {"LSM", "LFM2", MBLAF_LFOOT},
  {"LVM", "LFM5", MBLAF_LFOOT},

  {"PT1", "PNT1", MBLAF_UNKNOWN},
  {"PT2", "PNT2", MBLAF_UNKNOWN},
  {"PT3", "PNT3", MBLAF_UNKNOWN},
  {"PT4", "PNT4", MBLAF_UNKNOWN}
};

//----------------------------------------------------------------------------
wxString const * LookupStdName(std::vector<std::pair<wxString, wxString> >&  m_dictionary, wxString const *name)
//----------------------------------------------------------------------------
{
  wxInt32      nI; 

  for(nI = 0; nI < m_dictionary.size(); nI++)
  {
    if(m_dictionary[nI].second == (*name))
    {
      return &m_dictionary[nI].first;
    }
    //already a ref one
    if(m_dictionary[nI].first == (*name))
    {
      return &m_dictionary[nI].first;
    }
  }
  //failed lookup
  return NULL;
}

//----------------------------------------------------------------------------
wxString const * LookupUserName(std::vector<std::pair<wxString, wxString> >&  m_dictionary, wxString const *name)
//----------------------------------------------------------------------------
{
  wxInt32      nI; 

  for(nI = 0; nI < m_dictionary.size(); nI++)
  {
    if(m_dictionary[nI].first == (*name))
    {
      return &m_dictionary[nI].second;
    }
    //already a ref one
    if(m_dictionary[nI].second == (*name))
    {
      return &m_dictionary[nI].second;
    }
  }
  //failed lookup
  return NULL;
}

//----------------------------------------------------------------------------
mafAFBonesList GetBoneIDByName(wxString const &sName, mafAFAltBonesInfo *pBonesList = NULL, wxInt32 nListSize = 0)
//----------------------------------------------------------------------------
{
  wxInt32 nI;

  //use internal data if no external
  if(pBonesList == NULL)
  {
    pBonesList = _saKnownBones;
    nListSize  = DIM(_saKnownBones);
  }

  for(nI = 0; nI < nListSize; nI++)
  {
    if(sName == pBonesList[nI].m_OldName)
    {
      return (pBonesList[nI].m_BoneID);
    }
  }
  return (MBLAF_UNKNOWN);
}


//----------------------------------------------------------------------------
mafAFAltLandmarksInfo *FindLandmark(wxString const *pOldAcronim, wxString const *pNewAcronim, mafAFAltLandmarksInfo *pLandmarksList = NULL, wxInt32 nListSize = 0)
//----------------------------------------------------------------------------
{
  wxInt32 nI;

  //use internal data if no external
  if(pLandmarksList == NULL)
  {
    pLandmarksList = _saKnownLandmarks;
    nListSize      = DIM(_saKnownLandmarks);
  }

  for(nI = 0; nI < nListSize; nI++)
  {
    //try old one for search
    if(pOldAcronim != NULL)
    {
      if((*pOldAcronim) == pLandmarksList[nI].m_OldName)
      {
        return (pLandmarksList + nI);
      }
    }
    if(pNewAcronim != NULL)
    {
      if((*pNewAcronim) == pLandmarksList[nI].m_NewName)
      {
        return (pLandmarksList + nI);
      }
    }
  }
  return (NULL);
}



/*
* converts technical frames to anatomical
* @memo    
* @return  TRUE on success
* @param   fdpDesc
* @param   fdpResult
*/
//----------------------------------------------------------------------------
bool BuildAFTransformMatrix(std::vector<std::pair<wxString, wxString> >& m_dictionary, mafVME *pVME, mafTimeStamp ts, DiMatrix *mpOut)
//----------------------------------------------------------------------------
{
  DiInt32              nI,nJ;
  DiV4d                *vpLandmarks;
  mafAFBonesList         nBoneID;   
  mafAFLMsForCnv         *pConvert;
  wxInt32              nLandmarksFound;
  mafVMELandmarkCloud  *pCloud;
  char const           *lmName;
  wxString const       *sLMName;
  wxString              sTempString;
  mafAFAltLandmarksInfo  *pInfo;
  DiDouble              dx, dy, dz;  
  //having hierarchy is not important here: we just need a dictionary to know who is who
  nBoneID = GetBoneIDByName(wxString(pVME->GetName()));
  if(nBoneID == MBLAF_UNKNOWN)
  {
    if(LookupStdName(m_dictionary, &wxString(pVME->GetName())) != NULL)
    {
      //may be it will become known after dictionary application?
      nBoneID = GetBoneIDByName(*LookupStdName(m_dictionary, &wxString(pVME->GetName())));
    }
  }
  //now proceed as we have
  pConvert = _caAFBuildInfo + nBoneID;
  //check order
  wxASSERT(pConvert->m_ID == nBoneID);
  //check VME for having landmarks
  if(!pVME->IsA("mafVMELandmarkCloud"))
  {
    DiMatrixIdentity(mpOut);
    return (FALSE);
  }
  //cast
  pCloud = (mafVMELandmarkCloud *)pVME;
  //fill landmarks array

  //get memory
  vpLandmarks = (DiV4d *)malloc(pConvert->m_NumLMs * sizeof(DiV4d));
  memset(vpLandmarks, 0xFF, pConvert->m_NumLMs * sizeof(DiV4d));

  nLandmarksFound = 0;
  for(nI = 0; nI < pCloud->GetNumberOfLandmarks(); nI++)
  {
    lmName = pCloud->GetLandmarkName(nI);
    if(LookupStdName(m_dictionary, &wxString(lmName)) != NULL)
    {
      sLMName = LookupStdName(m_dictionary, &wxString(lmName));
    }
    else
    {
      sTempString = lmName;
      sLMName = &sTempString;
    }
    pInfo = FindLandmark(sLMName, sLMName);
    if(pInfo == NULL)
    {     
      //check for all
      for(nJ = 0; nJ < pConvert->m_NumLMs; nJ++)
      {
        if(*sLMName == pConvert->m_LMNames[nJ])
        {
          pCloud->GetLandmark(nI, dx, dy, dz, ts);
          vpLandmarks[nJ].x = dx; 
          vpLandmarks[nJ].y = dy;
          vpLandmarks[nJ].z = dz;
          vpLandmarks[nJ].w = 1.0;
          nLandmarksFound ++;
        }
      }
    }
    else
    {
      //check for all
      for(nJ = 0; nJ < pConvert->m_NumLMs; nJ++)
      {
        if(strcmp(pInfo->m_NewName, pConvert->m_LMNames[nJ]) == 0 ||
          strcmp(pInfo->m_OldName, pConvert->m_LMNames[nJ]) == 0 )
        {
          pCloud->GetLandmark(nI, dx, dy, dz, ts);
          vpLandmarks[nJ].x = dx; 
          vpLandmarks[nJ].y = dy;
          vpLandmarks[nJ].z = dz;
          vpLandmarks[nJ].w = 1.0;
          nLandmarksFound ++;
        }
      }
    }
  }
  if(pConvert->m_ID == MBLAF_UNKNOWN)
  {
    //last landmark may or may not present
    if(nLandmarksFound < pConvert->m_NumLMs - 1)
    {
      goto l_Failure;
    }
    if(nLandmarksFound == pConvert->m_NumLMs - 1)
    {
      //only last landmark may not present check w component
      if(*((wxUint32 *)&vpLandmarks[pConvert->m_NumLMs - 1].w) != 0xFFFFFFFF)
      {
        goto l_Failure;
      }
    }
  }
  else if(nLandmarksFound < pConvert->m_NumLMs)
  {
l_Failure:
    free(vpLandmarks);
    DiMatrixIdentity(mpOut);
    return (FALSE);
  }     
  pConvert->m_func(vpLandmarks, mpOut);

  free(vpLandmarks);

  return true;
}



//----------------------------------------------------------------------------
void GetMatrix(std::vector<std::pair<wxString, wxString> >&  m_dictionary, mafVME *vme, mafTimeStamp ts, DiMatrix *pMat)
//----------------------------------------------------------------------------
{
  mafMatrix matrix;
  DiMatrix  mRightTransf;
  DiMatrix  mLeftMatrix;
  DiMatrix  mResult;

  ///This function can be called with zero this!
  //vme->GetOutput()->GetMatrix(matrix, ts);
  //mflMatrixToDi(matrix.GetVTKMatrix(), pMat);
  DiMatrixIdentity(pMat);
  //if(this == NULL)
  //{
  //  return;
  //}
  //otherwise we need to build AF for every known VME: Hierarchy not needed here, only dictionary
  if(BuildAFTransformMatrix(m_dictionary, vme, ts, &mRightTransf))
  {
    mafTransfRightLeftConv(&mRightTransf, &mLeftMatrix);
    //correct GTM
    DiMatrixMultiply(&mLeftMatrix, pMat, &mResult);
    DiMatrixCopy(&mResult, pMat);
  }
}


//----------------------------------------------------------------------------
mmoAFSys::mmoAFSys(wxString label) :
mafOp(label), m_DictionaryFName("")
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_RefSys  = NULL;
}

//----------------------------------------------------------------------------
mmoAFSys::~mmoAFSys( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_RefSys);
}

//----------------------------------------------------------------------------
mafOp* mmoAFSys::Copy()   
//----------------------------------------------------------------------------
{
  return new mmoAFSys(m_Label);
}

//----------------------------------------------------------------------------
bool mmoAFSys::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;
  if(mafVMELandmarkCloud::SafeDownCast(vme) == NULL)
    return false;

  for(int i = 0; i < vme->GetNumberOfChildren(); i++)
  {
    if(vme->GetChild(i)->IsA("mafVMEAFRefSys"))
      return false;
  }
  
  return true;
}

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_LOAD_HIERARCHY ,
  ID_LOAD_DICTIONARY,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

//----------------------------------------------------------------------------
void mmoAFSys::OpRun()   
//----------------------------------------------------------------------------
{
  mafString str(m_Input->GetName());
  mafNEW(m_RefSys);
  str += " AF_Frame";
  m_RefSys->SetName(str.GetCStr());
  CreateGui();
}

//----------------------------------------------------------------------------
void mmoAFSys::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->SetListener(this);
  m_Gui->FileOpen(ID_LOAD_DICTIONARY, "Dictionary", &m_DictionaryFName);
  m_Gui->Label("");
  m_Gui->OkCancel();
  ShowGui();
}

//----------------------------------------------------------------------------
void mmoAFSys::OpStop(int result)
//----------------------------------------------------------------------------
{
  DiMatrix mAFTransf;

  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    if(m_RefSys->GetParent())
    {
      mafEventMacro(mafEvent(this, VME_REMOVE, m_RefSys));
    }
    mafEventMacro(mafEvent(this,result));
  }
  else if (result == OP_RUN_OK)
  {
    if(!BuildAFTransformMatrix(m_dictionary, mafVME::SafeDownCast(m_Input), 0, &mAFTransf))
    {
      wxMessageBox("Either dictionary provided or landmarks not sufficient to build anatomical frame.","Alert", wxOK , NULL);
      return;
    }
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
}
//----------------------------------------------------------------------------
void mmoAFSys::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{ 
  switch(maf_event->GetId())
  {
    case wxOK:          
    { 
      OpStop(OP_RUN_OK);
    }
    break;
    case wxCANCEL:
    {    
      OpStop(OP_RUN_CANCEL);
    }
    break;
    case ID_LOAD_DICTIONARY:
    {
      if(m_DictionaryFName != "")
      {
        ReadDictionary(&m_DictionaryFName, m_dictionary);
      }
    }
    break;
    default:
    {
      mafEventMacro(*maf_event); 
    }
    break;
  }
}

//----------------------------------------------------------------------------
void mmoAFSys::OpDo()
//----------------------------------------------------------------------------
{
  DiMatrix mAF;
  wxInt32  nI;
  std::vector<mafTimeStamp> mpStamps;
  vtkMatrix4x4 *mVTK = NULL;

  wxBusyInfo wait("Please wait, working...");

  assert(m_RefSys);
  vtkNEW(mVTK);
  m_RefSys->ReparentTo(m_Input);
  m_RefSys->SetScaleFactor(100.0);
  mafEventMacro(mafEvent(this, VME_ADD, m_RefSys));
  
  //mafProgressBarShowMacro();
  //mafProgressBarSetTextMacro("Creating rigid anatomical frame for entire sequence...");

  mafVME::SafeDownCast(m_Input)->GetTimeStamps(mpStamps);
  for(nI = 0; nI < mpStamps.size(); nI++)
  {
    //mafProgressBarSetValueMacro((100 * nI / m_input->GetNumberOfTimeStamps()));
    GetMatrix(m_dictionary, mafVME::SafeDownCast(m_Input), mpStamps[nI], &mAF);
    mVTK->Identity();
    DiMatrixToVTK(&mAF, mVTK);
    mafMatrix mft(mVTK);
    mft.SetTimeStamp(mpStamps[nI]);
    m_RefSys->SetMatrix(mft);
  }
  //mafProgressBarHideMacro();

  vtkDEL(mVTK);
}
//----------------------------------------------------------------------------
void mmoAFSys::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_RefSys);
  mafEventMacro(mafEvent(this, VME_REMOVE, m_RefSys));
}


//----------------------------------------------------------------------------
static bool _mafBuildPelvisAF    (DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  DiV4d vMiddlePS;
  DiV4d vMiddleAS;

  DiV4dLineComb(&vpLandmarks[0], 0.5f, &vpLandmarks[1], 0.5f, &vMiddleAS);
  DiV4dLineComb(&vpLandmarks[2], 0.5f, &vpLandmarks[3], 0.5f, &vMiddlePS);
  DiMatrixIdentity(mpMatrix);
  mafTransfAFCoords(&vpLandmarks[0], &vpLandmarks[1], &vMiddlePS, &vpLandmarks[1], &vpLandmarks[0], 
    &mpMatrix->vUp, &mpMatrix->vRight, &mpMatrix->vAt);
  mpMatrix->vAt.x = -mpMatrix->vAt.x;
  mpMatrix->vAt.y = -mpMatrix->vAt.y;
  mpMatrix->vAt.z = -mpMatrix->vAt.z;
  DiV4dCopy(&vMiddleAS, &mpMatrix->vPos);
  return true;
}
//----------------------------------------------------------------------------
static bool _mafBuildRightThighAF(DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  //DiMatrixIdentity(mpMatrix);
  //return true;
  DiV4d vMiddleMELE;

  DiV4dLineComb(&vpLandmarks[0], 0.5f, &vpLandmarks[1], 0.5f, &vMiddleMELE);
  DiMatrixIdentity(mpMatrix);
  mafTransfAFCoords(&vpLandmarks[0], &vpLandmarks[1], &vpLandmarks[2], &vMiddleMELE, &vpLandmarks[2], 
    &mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
  mpMatrix->vUp.x = -mpMatrix->vUp.x;
  mpMatrix->vUp.y = -mpMatrix->vUp.y;
  mpMatrix->vUp.z = -mpMatrix->vUp.z;
  DiV4dCopy(&vMiddleMELE, &mpMatrix->vPos);
  return true;
}
//----------------------------------------------------------------------------
static bool _mafBuildLeftThighAF (DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  DiV4d vMiddleMELE;

  DiV4dLineComb(&vpLandmarks[0], 0.5f, &vpLandmarks[1], 0.5f, &vMiddleMELE);
  DiMatrixIdentity(mpMatrix);
  mafTransfAFCoords(&vpLandmarks[1], &vpLandmarks[0], &vpLandmarks[2], &vMiddleMELE, &vpLandmarks[2], 
    &mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
  mpMatrix->vUp.x = -mpMatrix->vUp.x;
  mpMatrix->vUp.y = -mpMatrix->vUp.y;
  mpMatrix->vUp.z = -mpMatrix->vUp.z;
  DiV4dCopy(&vMiddleMELE, &mpMatrix->vPos);
  return true;
}
//----------------------------------------------------------------------------
static bool _mafBuildRightShankAF(DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  DiV4d vMiddleLMMM;
  DiV4dLineComb(&vpLandmarks[2], 0.5f, &vpLandmarks[3], 0.5f, &vMiddleLMMM);
  DiMatrixIdentity(mpMatrix);
  mafTransfAFCoords(&vpLandmarks[2], &vpLandmarks[3], &vpLandmarks[0], &vMiddleLMMM, &vpLandmarks[1], 
    &mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
  mpMatrix->vUp.x = -mpMatrix->vUp.x;
  mpMatrix->vUp.y = -mpMatrix->vUp.y;
  mpMatrix->vUp.z = -mpMatrix->vUp.z;
  DiV4dCopy(&vMiddleLMMM, &mpMatrix->vPos);
  return true;
}
//----------------------------------------------------------------------------
static bool _mafBuildLeftShankAF (DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  DiV4d vMiddleLMMM;

  DiV4dLineComb(&vpLandmarks[2], 0.5f, &vpLandmarks[3], 0.5f, &vMiddleLMMM);
  DiMatrixIdentity(mpMatrix);
  mafTransfAFCoords(&vpLandmarks[3], &vpLandmarks[2], &vpLandmarks[0], &vMiddleLMMM, &vpLandmarks[1], 
    &mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
  mpMatrix->vUp.x = -mpMatrix->vUp.x;
  mpMatrix->vUp.y = -mpMatrix->vUp.y;
  mpMatrix->vUp.z = -mpMatrix->vUp.z;
  DiV4dCopy(&vMiddleLMMM, &mpMatrix->vPos);
  return true;
}

//----------------------------------------------------------------------------
static bool _mafBuildRightFootAF (DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  DiMatrixIdentity(mpMatrix);
  mafTransfAFCoords(&vpLandmarks[3], &vpLandmarks[1], &vpLandmarks[0], &vpLandmarks[2], &vpLandmarks[0], 
    &mpMatrix->vUp, &mpMatrix->vAt, &mpMatrix->vRight);
  DiV4dCopy(&vpLandmarks[0], &mpMatrix->vPos);
  return true;
}
//----------------------------------------------------------------------------
static bool _mafBuildLeftFootAF  (DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  DiMatrixIdentity(mpMatrix);
  mafTransfAFCoords(&vpLandmarks[1], &vpLandmarks[3], &vpLandmarks[0], &vpLandmarks[2], &vpLandmarks[0], 
    &mpMatrix->vUp, &mpMatrix->vAt, &mpMatrix->vRight);
  DiV4dCopy(&vpLandmarks[0], &mpMatrix->vPos);
  return true;
}

//----------------------------------------------------------------------------
static bool _mafBuildUnknownAF (DiV4d const *vpLandmarks, DiMatrix *mpMatrix)
//----------------------------------------------------------------------------
{
  DiV4d    vMiddleMELE;
  double   lineComb = 0.5;
  unsigned prefAxis = 0;//Z-0, Y- 1

  //use two algorithms here depending on data in last landmark
  if(*((wxUint32 *)&vpLandmarks[3].w) == 0xFFFFFFFF)
  {
    //3 landmarks case
    DiV4dLineComb(&vpLandmarks[0], lineComb, &vpLandmarks[1], 1.0 - lineComb, &vMiddleMELE);
    DiMatrixIdentity(mpMatrix);
    switch(prefAxis)
    {
    case 1:
      {
        mafTransfAFCoords(&vpLandmarks[1], &vpLandmarks[0], &vpLandmarks[2], &vMiddleMELE, &vpLandmarks[2], 
          &mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
        break;
      }
    case 0:
      {

        DiV4dSub(&vpLandmarks[1], &vpLandmarks[0], &mpMatrix->vAt);
        DiV4dMakeUnit(&mpMatrix->vAt);
        DiV4dSub(&vMiddleMELE, &vpLandmarks[2], &mpMatrix->vUp);
        DiV4dMakeUnit(&mpMatrix->vUp);
        DiV4dCrossProduct(&mpMatrix->vAt, &mpMatrix->vUp, &mpMatrix->vRight);
        DiV4dMakeUnit(&mpMatrix->vRight);
        //perform correction
        DiV4dCrossProduct(&mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
        mpMatrix->vAt.w = 0.0;
        mpMatrix->vUp.w = 0.0;
        mpMatrix->vRight.w = 0.0;
        break;
      }
    default:
      {
        wxASSERT(false);
      }
    }

    mpMatrix->vUp.x = -mpMatrix->vUp.x;
    mpMatrix->vUp.y = -mpMatrix->vUp.y;
    mpMatrix->vUp.z = -mpMatrix->vUp.z;
    DiV4dCopy(&vMiddleMELE, &mpMatrix->vPos);
    return true;
  }
  else
  {
    //suppose we have 4 points here
    //4 landmarks case
    DiV4dLineComb(&vpLandmarks[0], lineComb, &vpLandmarks[1], 1.0 - lineComb, &vMiddleMELE);
    DiMatrixIdentity(mpMatrix);
    switch(prefAxis)
    {
    case 1:
      {
        mafTransfAFCoords(&vpLandmarks[1], &vpLandmarks[0], &vpLandmarks[2], &vpLandmarks[3], &vpLandmarks[2], 
          &mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
        break;
      }
    case 0:
      {
        DiV4dSub(&vpLandmarks[1], &vpLandmarks[0], &mpMatrix->vAt);
        DiV4dMakeUnit(&mpMatrix->vAt);
        DiV4dSub(&vpLandmarks[3], &vpLandmarks[2], &mpMatrix->vUp);
        DiV4dMakeUnit(&mpMatrix->vUp);
        DiV4dCrossProduct(&mpMatrix->vAt, &mpMatrix->vUp, &mpMatrix->vRight);
        DiV4dMakeUnit(&mpMatrix->vRight);
        //perform correction
        DiV4dCrossProduct(&mpMatrix->vRight, &mpMatrix->vAt, &mpMatrix->vUp);
        mpMatrix->vAt.w = 0.0;
        mpMatrix->vUp.w = 0.0;
        mpMatrix->vRight.w = 0.0;
        break;
      }
    default:
      {
        wxASSERT(false);
      }
    }
    mpMatrix->vUp.x = -mpMatrix->vUp.x;
    mpMatrix->vUp.y = -mpMatrix->vUp.y;
    mpMatrix->vUp.z = -mpMatrix->vUp.z;
    DiV4dCopy(&vMiddleMELE, &mpMatrix->vPos);
    return true;
  }
}
