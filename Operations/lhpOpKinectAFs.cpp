/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpKinectAFs.cpp,v $
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

#include "lhpOpKinectAFs.h"
#include "wx/busyinfo.h"
#include "wx/textfile.h"

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"
#include "mafDictionary.h"

#include "mafSmartPointer.h"

#include "mafVMEAFRefSys.h"
#include "mafVMELandmarkCloud.h"

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

#define ADD_PREDEF(title, arr_name, boneID) predefinedScripts.push_back(PredefinedScripts(title, std::vector<mafString>(&arr_name[0], &arr_name[0] + sizeof(arr_name)/sizeof(arr_name[0])), mafVMEAFRefSys::ID_AFS_##boneID))
void lhpOpKinectAFs::InitPredefined()
{
  /*mafString _S01[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Pelvis"};
  mafString _S02[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Spine"};
  mafString _S03[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Thorax"};
  mafString _S04[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Head"};
  mafString _S05[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftShoulder"};
  mafString _S06[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftElbow"};
  mafString _S07[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftWrist"};
  mafString _S08[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftHand"};
  mafString _S09[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightShoulder"};
  mafString _S10[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightElbow"};
  mafString _S11[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightWrist"};
  mafString _S12[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightHand"};
  mafString _S13[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftHip"};
  mafString _S14[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftKnee"};
  mafString _S15[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftAnkle"};
  mafString _S16[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftFoot"};
  mafString _S17[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightHip"};
  mafString _S18[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightKnee"};
  mafString _S19[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightAnkle"};
  mafString _S20[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightFoot"};*/

  mafString _F01[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Pelvis"};
  mafString _F02[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Spine"};
  mafString _F03[] = {"SUBV Y THORAX SPINE", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Thorax"};
  mafString _F04[] = {"SUBV Y THORAX SPINE", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Head"};
  mafString _F05[] = {"SUBV Y LEFTELBOW LEFTSHOULDER", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftShoulder"};
  mafString _F06[] = {"SUBV Y LEFTWRIST LEFTELBOW", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftElbow"};
  mafString _F07[] = {"SUBV Y LEFTHAND LEFTWRIST", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftWrist"};
  mafString _F08[] = {"SUBV Y LEFTHAND LEFTWRIST", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftHand"};
  mafString _F09[] = {"SUBV Y RIGHTSHOULDER RIGHTELBOW", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightShoulder"};
  mafString _F10[] = {"SUBV Y RIGHTELBOW RIGHTWRIST", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightElbow"};
  mafString _F11[] = {"SUBV Y RIGHTWRIST RIGHTHAND", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightWrist"};
  mafString _F12[] = {"SUBV Y RIGHTWRIST RIGHTHAND", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightHand"};
  mafString _F13[] = {"SUBV Y LEFTKNEE LEFTHIP", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftHip"};
  mafString _F14[] = {"SUBV Y LEFTANKLE LEFTKNEE", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftKnee"};
  mafString _F15[] = {"SUBV X LEFTANKLE LEFTFOOT", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P LeftAnkle"};
  mafString _F16[] = {"SUBV X LEFTANKLE LEFTFOOT", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P LeftFoot"};
  mafString _F17[] = {"SUBV Y RIGHTHIP RIGHTKNEE", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightHip"};
  mafString _F18[] = {"SUBV Y RIGHTKNEE RIGHTANKLE", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightKnee"};
  mafString _F19[] = {"SUBV X RIGHTFOOT RIGHTANKLE", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P RightAnkle"};
  mafString _F20[] = {"SUBV X RIGHTFOOT RIGHTANKLE", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P RightFoot"};

  mafString _A01[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Pelvis"};
  mafString _A02[] = {"SUBV Y SPINE PELVIS", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Spine"};
  mafString _A03[] = {"SUBV Y THORAX SPINE", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Thorax"};
  mafString _A04[] = {"SUBV Y THORAX SPINE", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P Head"};
  mafString _A05[] = {"SUBV Y LEFTELBOW LEFTSHOULDER", "NRML Y", "SUBV Y1 THORAX SPINE", "NRML Y1", "SUBV Z1 RIGHTSHOULDER LEFTSHOULDER", "NRML Z1", "CROSS X Y1 Z1", "NRML X", "CROSS Z X Y", "NRML Z", "CROSS X Y Z", "NRML X", "ASSV P LeftShoulder"};
  mafString _A06[] = {"SUBV Y LEFTWRIST LEFTELBOW", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftElbow"};
  mafString _A07[] = {"SUBV Y LEFTHAND LEFTWRIST", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftWrist"};
  mafString _A08[] = {"SUBV Y LEFTHAND LEFTWRIST", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftHand"};
  mafString _A09[] = {"SUBV Y RIGHTSHOULDER RIGHTELBOW", "NRML Y", "SUBV Y1 THORAX SPINE", "NRML Y1", "SUBV Z1 RIGHTSHOULDER LEFTSHOULDER", "NRML Z1", "CROSS X Z1 Y1", "NRML X", "CROSS Z X Y", "NRML Z", "CROSS X Y Z", "NRML X", "ASSV P RightShoulder"};
  mafString _A10[] = {"SUBV Y RIGHTELBOW RIGHTWRIST", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightElbow"};
  mafString _A11[] = {"SUBV Y RIGHTWRIST RIGHTHAND", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightWrist"};
  mafString _A12[] = {"SUBV Y RIGHTWRIST RIGHTHAND", "NRML Y", "SUBV Z RIGHTSHOULDER LEFTSHOULDER", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightHand"};
  mafString _A13[] = {"SUBV Y LEFTKNEE LEFTHIP", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftHip"};
  mafString _A14[] = {"SUBV Y LEFTANKLE LEFTKNEE", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P LeftKnee"};
  mafString _A15[] = {"SUBV X LEFTANKLE LEFTFOOT", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P LeftAnkle"};
  mafString _A16[] = {"SUBV X LEFTANKLE LEFTFOOT", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P LeftFoot"};
  mafString _A17[] = {"SUBV Y RIGHTHIP RIGHTKNEE", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightHip"};
  mafString _A18[] = {"SUBV Y RIGHTKNEE RIGHTANKLE", "NRML Y", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS X Y Z", "NRML X", "CROSS Z X Y", "NRML Z", "ASSV P RightKnee"};
  mafString _A19[] = {"SUBV X RIGHTFOOT RIGHTANKLE", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P RightAnkle"};
  mafString _A20[] = {"SUBV X RIGHTFOOT RIGHTANKLE", "NRML X", "SUBV Z RIGHTHIP LEFTHIP", "NRML Z", "CROSS Y Z X", "NRML Y", "CROSS Z X Y", "NRML Z", "ASSV P RightFoot"};

  {
    std::vector<PredefinedScripts> predefinedScripts;
    ADD_PREDEF("Pelvis",         _F01, NOTDEFINED);
    ADD_PREDEF("Spine",          _F02, NOTDEFINED);
    ADD_PREDEF("Thorax",         _F03, NOTDEFINED);
    ADD_PREDEF("Head",           _F04, NOTDEFINED);
    ADD_PREDEF("LeftShoulder",   _F05, NOTDEFINED);
    ADD_PREDEF("LeftElbow",      _F06, NOTDEFINED);
    ADD_PREDEF("LeftWrist",      _F07, NOTDEFINED);
    ADD_PREDEF("LeftHand",       _F08, NOTDEFINED);
    ADD_PREDEF("RightShoulder",  _F09, NOTDEFINED);
    ADD_PREDEF("RightElbow",     _F10, NOTDEFINED);
    ADD_PREDEF("RightWrist",     _F11, NOTDEFINED);
    ADD_PREDEF("RightHand",      _F12, NOTDEFINED);
    ADD_PREDEF("LeftHip",        _F13, NOTDEFINED);
    ADD_PREDEF("LeftKnee",       _F14, NOTDEFINED);
    ADD_PREDEF("LeftAnkle",      _F15, NOTDEFINED);
    ADD_PREDEF("LeftFoot",       _F16, NOTDEFINED);
    ADD_PREDEF("RightHip",       _F17, NOTDEFINED);
    ADD_PREDEF("RightKnee",      _F18, NOTDEFINED);
    ADD_PREDEF("RightAnkle",     _F19, NOTDEFINED);
    ADD_PREDEF("RightFoot",      _F20, NOTDEFINED);
    m_predefinedScripts.push_back(predefinedScripts);
  }
  {
    std::vector<PredefinedScripts> predefinedScripts;
    ADD_PREDEF("Pelvis",         _A01, NOTDEFINED);
    ADD_PREDEF("Spine",          _A02, NOTDEFINED);
    ADD_PREDEF("Thorax",         _A03, NOTDEFINED);
    ADD_PREDEF("Head",           _A04, NOTDEFINED);
    ADD_PREDEF("LeftShoulder",   _A05, NOTDEFINED);
    ADD_PREDEF("LeftElbow",      _A06, NOTDEFINED);
    ADD_PREDEF("LeftWrist",      _A07, NOTDEFINED);
    ADD_PREDEF("LeftHand",       _A08, NOTDEFINED);
    ADD_PREDEF("RightShoulder",  _A09, NOTDEFINED);
    ADD_PREDEF("RightElbow",     _A10, NOTDEFINED);
    ADD_PREDEF("RightWrist",     _A11, NOTDEFINED);
    ADD_PREDEF("RightHand",      _A12, NOTDEFINED);
    ADD_PREDEF("LeftHip",        _A13, NOTDEFINED);
    ADD_PREDEF("LeftKnee",       _A14, NOTDEFINED);
    ADD_PREDEF("LeftAnkle",      _A15, NOTDEFINED);
    ADD_PREDEF("LeftFoot",       _A16, NOTDEFINED);
    ADD_PREDEF("RightHip",       _A17, NOTDEFINED);
    ADD_PREDEF("RightKnee",      _A18, NOTDEFINED);
    ADD_PREDEF("RightAnkle",     _A19, NOTDEFINED);
    ADD_PREDEF("RightFoot",      _A20, NOTDEFINED);
    m_predefinedScripts.push_back(predefinedScripts);
  }
}

/*Pelvis
Spine 
Thorax 
Head 
LeftShoulder
LeftElbow
LeftWrist 
LeftHand 
RightShoulder 
RightElbow 
RightWrist 
RightHand 
LeftHip 
LeftKnee 
LeftAnkle 
LeftFoot
RightHip 
RightKnee 
RightAnkle 
RightFoot
*/

mafCxxTypeMacro(lhpOpKinectAFs)

//----------------------------------------------------------------------------
lhpOpKinectAFs::lhpOpKinectAFs(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType   = OPTYPE_OP;
  m_Canundo  = true;
  InitPredefined();
  m_TypeOfRefs = 0;
}

//----------------------------------------------------------------------------
lhpOpKinectAFs::~lhpOpKinectAFs()
//----------------------------------------------------------------------------
{
  for(std::vector<mafVMEAFRefSys*>::iterator it = m_RefSys.begin(); it != m_RefSys.end(); ++it)
  {
    mafDEL(*it);
  }
}

//----------------------------------------------------------------------------
mafOp* lhpOpKinectAFs::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpKinectAFs(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpKinectAFs::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;
  /*if(mafVMELandmarkCloud::SafeDownCast(vme) == NULL)
    return false;*/

  /*for(int i = 0; i < vme->GetNumberOfChildren(); i++)
  {
    if(vme->GetChild(i)->IsA("mafVMEAFRefSys"))
      return false;
  }*/
  
  return true;
}

//----------------------------------------------------------------------------
// widget id's
//----------------------------------------------------------------------------
enum 
{
  ID_DEFAULT = MINID,
  ID_TYPEOFREFS,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

//----------------------------------------------------------------------------
void lhpOpKinectAFs::OpRun()   
//----------------------------------------------------------------------------
{
  mafString refs_names[] = {"Flex", "Abd"};
  std::vector<PredefinedScripts>& predefinedScripts = m_predefinedScripts[m_TypeOfRefs];
  for(unsigned nm = 0; nm < predefinedScripts.size(); nm++)
  {
    mafVMEAFRefSys *refsys;
    mafNEW(refsys);
    mafString str(m_Input->GetName());
    str += "_";
    str += predefinedScripts[nm].m_Name;
    str += "_";
    str += refs_names[m_TypeOfRefs];
    refsys->SetName(str);
    refsys->SetScriptText(predefinedScripts[nm].m_Script);
    refsys->SetBoneID(predefinedScripts[nm].m_BoneID);
    m_RefSys.push_back(refsys);
  }
  if(!m_TestMode)
    CreateGui();
}

//----------------------------------------------------------------------------
void lhpOpKinectAFs::CreateGui()
//----------------------------------------------------------------------------
{
  mafString refs_names[] = {"Flexion", "Abduction"};
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Combo(ID_TYPEOFREFS, "Type",&m_TypeOfRefs, 2, refs_names);
  m_Gui->OkCancel();
  ShowGui();
}
void lhpOpKinectAFs::SetTypeOfRefs(int i)
{
  mafString refs_names[] = {"Flex", "Abd"};
  m_TypeOfRefs = i;
  if(m_RefSys.empty())
    return;
  std::vector<PredefinedScripts>& predefinedScripts = m_predefinedScripts[m_TypeOfRefs];
  for(unsigned nm = 0; nm < predefinedScripts.size(); nm++)
  {
    mafVMEAFRefSys *refsys = m_RefSys[nm];
    mafString str(m_Input->GetName());
    str += "_";
    str += predefinedScripts[nm].m_Name;
    str += "_";
    str += refs_names[m_TypeOfRefs];
    refsys->SetName(str);
    refsys->SetScriptText(predefinedScripts[nm].m_Script);
    refsys->SetBoneID(predefinedScripts[nm].m_BoneID);
    m_RefSys.push_back(refsys);
  }

}

//----------------------------------------------------------------------------
void lhpOpKinectAFs::OnEvent(mafEventBase *maf_event) 
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
    case ID_TYPEOFREFS:
      SetTypeOfRefs(m_TypeOfRefs);
      break;
    default:
      mafEventMacro(*maf_event); 
      break;
  }
}

//----------------------------------------------------------------------------
void lhpOpKinectAFs::OpDo()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Please wait, working...");

  for(std::vector<mafVMEAFRefSys*>::iterator it = m_RefSys.begin(); it != m_RefSys.end(); ++it)
  {
    assert(*it);
    (*it)->ReparentTo(m_Input);
    (*it)->SetScaleFactor(100.0);
    (*it)->SetActive(1);
  }
}
//----------------------------------------------------------------------------
void lhpOpKinectAFs::OpUndo()
//----------------------------------------------------------------------------
{
  for(std::vector<mafVMEAFRefSys*>::iterator it = m_RefSys.begin(); it != m_RefSys.end(); ++it)
  {
    assert(*it);
    (*it)->ReparentTo(NULL);
    (*it)->SetScaleFactor(100.0);
    (*it)->SetActive(1);
  }
}

