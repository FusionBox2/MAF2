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

#define ADD_PREDEF(title, arr_name, boneID) predefinedScripts.push_back(PredefinedScripts(_R(title), std::vector<mafString>(&arr_name[0], &arr_name[0] + sizeof(arr_name)/sizeof(arr_name[0])), mafVMEAFRefSys::ID_AFS_##boneID))
void lhpOpKinectAFs::InitPredefined()
{
  /*mafString _S01[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Pelvis")};
  mafString _S02[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Spine")};
  mafString _S03[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Thorax")};
  mafString _S04[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Head")};
  mafString _S05[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftShoulder")};
  mafString _S06[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftElbow")};
  mafString _S07[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftWrist")};
  mafString _S08[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftHand")};
  mafString _S09[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightShoulder")};
  mafString _S10[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightElbow")};
  mafString _S11[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightWrist")};
  mafString _S12[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightHand")};
  mafString _S13[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftHip")};
  mafString _S14[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftKnee")};
  mafString _S15[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftAnkle")};
  mafString _S16[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftFoot")};
  mafString _S17[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightHip")};
  mafString _S18[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightKnee")};
  mafString _S19[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightAnkle")};
  mafString _S20[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightFoot")};*/

  mafString _F01[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Pelvis")};
  mafString _F02[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Spine")};
  mafString _F03[] = {_R("SUBV Y THORAX SPINE"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Thorax")};
  mafString _F04[] = {_R("SUBV Y THORAX SPINE"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Head")};
  mafString _F05[] = {_R("SUBV Y LEFTELBOW LEFTSHOULDER"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftShoulder")};
  mafString _F06[] = {_R("SUBV Y LEFTWRIST LEFTELBOW"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftElbow")};
  mafString _F07[] = {_R("SUBV Y LEFTHAND LEFTWRIST"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftWrist")};
  mafString _F08[] = {_R("SUBV Y LEFTHAND LEFTWRIST"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftHand")};
  mafString _F09[] = {_R("SUBV Y RIGHTSHOULDER RIGHTELBOW"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightShoulder")};
  mafString _F10[] = {_R("SUBV Y RIGHTELBOW RIGHTWRIST"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightElbow")};
  mafString _F11[] = {_R("SUBV Y RIGHTWRIST RIGHTHAND"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightWrist")};
  mafString _F12[] = {_R("SUBV Y RIGHTWRIST RIGHTHAND"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightHand")};
  mafString _F13[] = {_R("SUBV Y LEFTKNEE LEFTHIP"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftHip")};
  mafString _F14[] = {_R("SUBV Y LEFTANKLE LEFTKNEE"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftKnee")};
  mafString _F15[] = {_R("SUBV X LEFTANKLE LEFTFOOT"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftAnkle")};
  mafString _F16[] = {_R("SUBV X LEFTANKLE LEFTFOOT"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftFoot")};
  mafString _F17[] = {_R("SUBV Y RIGHTHIP RIGHTKNEE"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightHip")};
  mafString _F18[] = {_R("SUBV Y RIGHTKNEE RIGHTANKLE"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightKnee")};
  mafString _F19[] = {_R("SUBV X RIGHTFOOT RIGHTANKLE"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightAnkle")};
  mafString _F20[] = {_R("SUBV X RIGHTFOOT RIGHTANKLE"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightFoot")};

  mafString _A01[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Pelvis")};
  mafString _A02[] = {_R("SUBV Y SPINE PELVIS"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Spine")};
  mafString _A03[] = {_R("SUBV Y THORAX SPINE"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Thorax")};
  mafString _A04[] = {_R("SUBV Y THORAX SPINE"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P Head")};
  mafString _A05[] = {_R("SUBV Y LEFTELBOW LEFTSHOULDER"), _R("NRML Y"), _R("SUBV Y1 THORAX SPINE"), _R("NRML Y1"), _R("SUBV Z1 RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z1"), _R("CROSS X Y1 Z1"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("ASSV P LeftShoulder")};
  mafString _A06[] = {_R("SUBV Y LEFTWRIST LEFTELBOW"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftElbow")};
  mafString _A07[] = {_R("SUBV Y LEFTHAND LEFTWRIST"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftWrist")};
  mafString _A08[] = {_R("SUBV Y LEFTHAND LEFTWRIST"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftHand")};
  mafString _A09[] = {_R("SUBV Y RIGHTSHOULDER RIGHTELBOW"), _R("NRML Y"), _R("SUBV Y1 THORAX SPINE"), _R("NRML Y1"), _R("SUBV Z1 RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z1"), _R("CROSS X Z1 Y1"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("ASSV P RightShoulder")};
  mafString _A10[] = {_R("SUBV Y RIGHTELBOW RIGHTWRIST"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightElbow")};
  mafString _A11[] = {_R("SUBV Y RIGHTWRIST RIGHTHAND"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightWrist")};
  mafString _A12[] = {_R("SUBV Y RIGHTWRIST RIGHTHAND"), _R("NRML Y"), _R("SUBV Z RIGHTSHOULDER LEFTSHOULDER"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightHand")};
  mafString _A13[] = {_R("SUBV Y LEFTKNEE LEFTHIP"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftHip")};
  mafString _A14[] = {_R("SUBV Y LEFTANKLE LEFTKNEE"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftKnee")};
  mafString _A15[] = {_R("SUBV X LEFTANKLE LEFTFOOT"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftAnkle")};
  mafString _A16[] = {_R("SUBV X LEFTANKLE LEFTFOOT"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P LeftFoot")};
  mafString _A17[] = {_R("SUBV Y RIGHTHIP RIGHTKNEE"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightHip")};
  mafString _A18[] = {_R("SUBV Y RIGHTKNEE RIGHTANKLE"), _R("NRML Y"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightKnee")};
  mafString _A19[] = {_R("SUBV X RIGHTFOOT RIGHTANKLE"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightAnkle")};
  mafString _A20[] = {_R("SUBV X RIGHTFOOT RIGHTANKLE"), _R("NRML X"), _R("SUBV Z RIGHTHIP LEFTHIP"), _R("NRML Z"), _R("CROSS Y Z X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P RightFoot")};

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
  mafString refs_names[] = {_R("Flex"), _R("Abd")};
  std::vector<PredefinedScripts>& predefinedScripts = m_predefinedScripts[m_TypeOfRefs];
  for(unsigned nm = 0; nm < predefinedScripts.size(); nm++)
  {
    mafVMEAFRefSys *refsys;
    mafNEW(refsys);
    mafString str(m_Input->GetName());
    str += _R("_");
    str += predefinedScripts[nm].m_Name;
    str += _R("_");
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
  mafString refs_names[] = {_R("Flexion"), _R("Abduction")};
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Combo(ID_TYPEOFREFS, _R("Type"),&m_TypeOfRefs, 2, refs_names);
  m_Gui->OkCancel();
  ShowGui();
}
void lhpOpKinectAFs::SetTypeOfRefs(int i)
{
  mafString refs_names[] = {_R("Flex"), _R("Abd")};
  m_TypeOfRefs = i;
  if(m_RefSys.empty())
    return;
  std::vector<PredefinedScripts>& predefinedScripts = m_predefinedScripts[m_TypeOfRefs];
  for(unsigned nm = 0; nm < predefinedScripts.size(); nm++)
  {
    mafVMEAFRefSys *refsys = m_RefSys[nm];
    mafString str(m_Input->GetName());
    str += _R("_");
    str += predefinedScripts[nm].m_Name;
    str += _R("_");
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

