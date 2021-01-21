/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpAFSys.cpp,v $
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

#include "lhpOpAFSys.h"
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

#define ADD_PREDEF(title, arr_name, boneID) m_predefinedScripts.push_back(PredefinedScripts(_R(title), std::vector<mafString>(&arr_name[0], &arr_name[0] + sizeof(arr_name)/sizeof(arr_name[0])), mafVMEAFRefSys::ID_AFS_##boneID))
void lhpOpAFSys::InitPredefined()
{
  mafString _IPE[] = {_R("ASSV PN0 RIAS"), _R("ASSV PN1 LIAS"), _R("ASSV PN2 RIPS"), _R("ASSV PN3 LIPS"), _R("DEFVI RIAC"), _R("DEFVI LIAC"), _R("LNCMB MIDDLEA 0.5 PN0 0.5 PN1"), _R("LNCMB MIDDLEP 0.5 PN2 0.5 PN3"), _R("ASSV P1 PN0"), _R("ASSV P2 PN1"), _R("ASSV P3 MIDDLEP"), _R("ASSV P4 PN1"), _R("ASSV P5 PN0"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X Y1"), _R("ASSV Y X1"), _R("LNCMB Z -1 Z1 0 Y1"), _R("ASSV P MIDDLEA")};
  mafString _LFT[] = {_R("ASSV PN0 LFCC"), _R("ASSV PN1 LFM5"), _R("ASSV PN2 LFM2"), _R("ASSV PN3 LFM1"), _R("ASSV P1 PN3"), _R("ASSV P2 PN1"), _R("ASSV P3 PN0"), _R("ASSV P4 PN2"), _R("ASSV P5 PN0"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X Z1"), _R("ASSV Y X1"), _R("ASSV Z Y1"), _R("ASSV P PN0")};
  mafString _LSH_FAX[] = {_R("ASSV PN0 LFAX"), _R("ASSV PN1 LTTC"), _R("ASSV PN2 LTAM"), _R("ASSV PN3 LFAL"), _R("LNCMB MIDDLE 0.5 PN2 0.5 PN3"), _R("ASSV P1 PN2"), _R("ASSV P2 PN3"), _R("ASSV P3 PN0"), _R("ASSV P4 MIDDLE"), _R("ASSV P5 PN1"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _LSH_FNE[] = {_R("ASSV PN0 LFNE"), _R("ASSV PN1 LTTC"), _R("ASSV PN2 LTAM"), _R("ASSV PN3 LFAL"), _R("LNCMB MIDDLE 0.5 PN2 0.5 PN3"), _R("ASSV P1 PN2"), _R("ASSV P2 PN3"), _R("ASSV P3 PN0"), _R("ASSV P4 MIDDLE"), _R("ASSV P5 PN1"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _LSH_ISB[] = {_R("ASSV PN0 LTLR"), _R("ASSV PN1 LTMR"), _R("ASSV PN2 LFAL"), _R("ASSV PN3 LTAM"), _R("LNCMB MIDDLEUP 0.5 PN0 0.5 PN1"), _R("LNCMB MIDDLEDN 0.5 PN2 0.5 PN3"), _R("SUBV Z PN3 PN2"), _R("NRML Z"), _R("SUBV A MIDDLEUP MIDDLEDN"), _R("NRML A"), _R("CROSS X A Z"), _R("NRML X"), _R("CROSS Y Z X"), _R("NRML Y"), _R("ASSV P MIDDLEDN")};
  mafString _LTH[] = {_R("ASSV PN0 LFME"), _R("ASSV PN1 LFLE"), _R("ASSV PN2 LFCH"), _R("LNCMB MIDDLE 0.5 PN0 0.5 PN1"), _R("ASSV P1 PN0"), _R("ASSV P2 PN1"), _R("ASSV P3 PN2"), _R("ASSV P4 MIDDLE"), _R("ASSV P5 PN2"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _RFT[] = {_R("ASSV PN0 RFCC"), _R("ASSV PN1 RFM1"), _R("ASSV PN2 RFM2"), _R("ASSV PN3 RFM5"), _R("ASSV P1 PN3"), _R("ASSV P2 PN1"), _R("ASSV P3 PN0"), _R("ASSV P4 PN2"), _R("ASSV P5 PN0"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X Z1"), _R("ASSV Y X1"), _R("ASSV Z Y1"), _R("ASSV P PN0")};
  mafString _RSH_FAX[] = {_R("ASSV PN0 RFAX"), _R("ASSV PN1 RTTC"), _R("ASSV PN2 RFAL"), _R("ASSV PN3 RTAM"), _R("LNCMB MIDDLE 0.5 PN2 0.5 PN3"), _R("ASSV P1 PN2"), _R("ASSV P2 PN3"), _R("ASSV P3 PN0"), _R("ASSV P4 MIDDLE"), _R("ASSV P5 PN1"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _RSH_FNE[] = {_R("ASSV PN0 RFNE"), _R("ASSV PN1 RTTC"), _R("ASSV PN2 RFAL"), _R("ASSV PN3 RTAM"), _R("LNCMB MIDDLE 0.5 PN2 0.5 PN3"), _R("ASSV P1 PN2"), _R("ASSV P2 PN3"), _R("ASSV P3 PN0"), _R("ASSV P4 MIDDLE"), _R("ASSV P5 PN1"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _RSH_ISB[] = {_R("ASSV PN0 RTLR"), _R("ASSV PN1 RTMR"), _R("ASSV PN2 RFAL"), _R("ASSV PN3 RTAM"), _R("LNCMB MIDDLEUP 0.5 PN0 0.5 PN1"), _R("LNCMB MIDDLEDN 0.5 PN2 0.5 PN3"), _R("SUBV Z PN2 PN3"), _R("NRML Z"), _R("SUBV A MIDDLEUP MIDDLEDN"), _R("NRML A"), _R("CROSS X A Z"), _R("NRML X"), _R("CROSS Y Z X"), _R("NRML Y"), _R("ASSV P MIDDLEDN")};
  mafString _RTH[] = {_R("ASSV PN0 RFLE"), _R("ASSV PN1 RFME"), _R("ASSV PN2 RFCH"), _R("LNCMB MIDDLE 0.5 PN0 0.5 PN1"), _R("ASSV P1 PN0"), _R("ASSV P2 PN1"), _R("ASSV P3 PN2"), _R("ASSV P4 MIDDLE"), _R("ASSV P5 PN2"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _RHFT[] = {_R("ASSV PN0 RFCC"), _R("ASSV PN1 RFPT"), _R("ASSV PN2 RFST"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV X MIDDLE PN0"), _R("NRML X"), _R("SUBV A PN1 PN2"), _R("NRML A"), _R("CROSS Y A X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P MIDDLE")};
  mafString _LHFT[] = {_R("ASSV PN0 LFCC"), _R("ASSV PN1 LFPT"), _R("ASSV PN2 LFST"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV X MIDDLE PN0"), _R("NRML X"), _R("SUBV A PN2 PN1"), _R("NRML A"), _R("CROSS Y A X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P MIDDLE")};
  mafString _RFFT[] = {_R("ASSV PN0 RFM2"), _R("ASSV PN1 RFMT"), _R("ASSV PN2 RFNT"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV X PN0 MIDDLE "), _R("NRML X"), _R("SUBV A PN1 PN2"), _R("NRML A"), _R("CROSS Y A X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P MIDDLE")};
  mafString _LFFT[] = {_R("ASSV PN0 LFM2"), _R("ASSV PN1 LFMT"), _R("ASSV PN2 LFNT"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV X PN0 MIDDLE "), _R("NRML X"), _R("SUBV A PN2 PN1"), _R("NRML A"), _R("CROSS Y A X"), _R("NRML Y"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P MIDDLE")};

  mafString _TRX[]   = {_R("ASSV PN0 MSXS"), _R("ASSV PN1 MTV8"), _R("ASSV PN2 MSJN"), _R("ASSV PN3 MTV2"), _R("LNCMB MIDDLEDN 0.5 PN0 0.5 PN1"), _R("LNCMB MIDDLEUP 0.5 PN2 0.5 PN3"), _R("SUBV Y MIDDLEUP MIDDLEDN"), _R("NRML Y"), _R("SUBV A PN2 PN3"), _R("NRML A"), _R("CROSS Z A Y"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("ASSV P PN2")};
  mafString _RCLV[]  = {_R("ASSV PN0 MSXS"), _R("ASSV PN1 MTV8"), _R("ASSV PN2 MSJN"), _R("ASSV PN3 MTV2"), _R("ASSV PN4 RCAS"), _R("ASSV PN5 RCAJ"), _R("LNCMB MIDDLEDN 0.5 PN0 0.5 PN1"), _R("LNCMB MIDDLEUP 0.5 PN2 0.5 PN3"), _R("SUBV YT MIDDLEUP MIDDLEDN"), _R("NRML YT"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Z PN5 PN4"), _R("NRML Z"), _R("CROSS X YT Z"), _R("NRML X"), _R("CROSS Y Z X"), _R("NRML Y"), _R("ASSV P PN4")};
  mafString _LCLV[]  = {_R("ASSV PN0 MSXS"), _R("ASSV PN1 MTV8"), _R("ASSV PN2 MSJN"), _R("ASSV PN3 MTV2"), _R("ASSV PN4 LCAS"), _R("ASSV PN5 LCAJ"), _R("LNCMB MIDDLEDN 0.5 PN0 0.5 PN1"), _R("LNCMB MIDDLEUP 0.5 PN2 0.5 PN3"), _R("SUBV YT MIDDLEUP MIDDLEDN"), _R("NRML YT"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Z PN4 PN5"), _R("NRML Z"), _R("CROSS X YT Z"), _R("NRML X"), _R("CROSS Y Z X"), _R("NRML Y"), _R("ASSV P PN4")};
  mafString _RSCP[]  = {_R("ASSV PN0 RSAA"), _R("ASSV PN1 RSRS"), _R("ASSV PN2 RSIA"), _R("SUBV Z PN0 PN1"), _R("NRML Z"), _R("SUBV A PN1 PN2"), _R("NRML A"), _R("CROSS X A Z"), _R("NRML X"), _R("CROSS Y Z X"), _R("NRML Y"), _R("ASSV P PN0")};
  mafString _LSCP[]  = {_R("ASSV PN0 LSAA"), _R("ASSV PN1 LSRS"), _R("ASSV PN2 LSIA"), _R("SUBV Z PN1 PN0"), _R("NRML Z"), _R("SUBV A PN1 PN2"), _R("NRML A"), _R("CROSS X A Z"), _R("NRML X"), _R("CROSS Y Z X"), _R("NRML Y"), _R("ASSV P PN0")};
  mafString _RHUM1[] = {_R("ASSV PN0 RHCH"), _R("ASSV PN1 RHLE"), _R("ASSV PN2 RHME"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Y PN0 MIDDLE"), _R("NRML Y"), _R("SUBV A PN1 PN2"), _R("NRML A"), _R("CROSS X Y A"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P PN0")};
  mafString _LHUM1[] = {_R("ASSV PN0 LHCH"), _R("ASSV PN1 LHLE"), _R("ASSV PN2 LHME"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Y PN0 MIDDLE"), _R("NRML Y"), _R("SUBV A PN2 PN1"), _R("NRML A"), _R("CROSS X Y A"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P PN0")};
  mafString _RHUM2[] = {_R("ASSV PN0 RHCH"), _R("ASSV PN1 RHLE"), _R("ASSV PN2 RHME"), _R("ASSV PN4 RUSP"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Y PN0 MIDDLE"), _R("NRML Y"), _R("SUBV YF PN4 MIDDLE"), _R("NRML YF"), _R("CROSS Z Y YF"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("ASSV P PN0")};
  mafString _LHUM2[] = {_R("ASSV PN0 LHCH"), _R("ASSV PN1 LHLE"), _R("ASSV PN2 LHME"), _R("ASSV PN4 LUSP"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Y PN0 MIDDLE"), _R("NRML Y"), _R("SUBV YF PN4 MIDDLE"), _R("NRML YF"), _R("CROSS Z Y YF"), _R("NRML Z"), _R("CROSS X Y Z"), _R("NRML X"), _R("ASSV P PN0")};
  mafString _RFRA[]  = {_R("ASSV PN0 RUSP"), _R("ASSV PN1 RHLE"), _R("ASSV PN2 RHME"), _R("ASSV PN3 RRSP"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Y PN0 MIDDLE"), _R("NRML Y"), _R("SUBV A PN3 PN0"), _R("NRML A"), _R("CROSS X Y A"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P PN0")};
  mafString _LFRA[]  = {_R("ASSV PN0 LUSP"), _R("ASSV PN1 LHLE"), _R("ASSV PN2 LHME"), _R("ASSV PN3 LRSP"), _R("LNCMB MIDDLE 0.5 PN1 0.5 PN2"), _R("SUBV Y PN0 MIDDLE"), _R("NRML Y"), _R("SUBV A PN0 PN3"), _R("NRML A"), _R("CROSS X Y A"), _R("NRML X"), _R("CROSS Z X Y"), _R("NRML Z"), _R("ASSV P PN0")};

  mafString _3PNT_Y[] = {_R("ASSV PN0 PNT1"), _R("ASSV PN1 PNT2"), _R("ASSV PN2 PNT3"), _R("DEFSI 0 t 1"), _R("SUBS s 1 t"), _R("LNCMB MIDDLE t PN0 s PN1"), _R("ASSV P1 PN1"), _R("ASSV P2 PN0"), _R("ASSV P3 PN2"), _R("ASSV P4 MIDDLE"), _R("ASSV P5 PN2"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _3PNT_Z[] = {_R("ASSV PN0 PNT1"), _R("ASSV PN1 PNT2"), _R("ASSV PN2 PNT3"), _R("DEFSI 0 t 1"), _R("SUBS s 1 t"), _R("LNCMB MIDDLE t PN0 s PN1"), _R("SUBV Z PN1 PN0"), _R("NRML Z"), _R("SUBV Y1 MIDDLE PN2"), _R("NRML Y1"), _R("CROSS X Z Y1"), _R("NRML X"), _R("CROSS Y1 X Z"), _R("LNCMB Y -1 Y1 0 Y1"), _R("ASSV P MIDDLE")};
  mafString _4PNT_Y[] = {_R("ASSV PN0 PNT1"), _R("ASSV PN1 PNT2"), _R("ASSV PN2 PNT3"), _R("ASSV PN3 PNT4"), _R("DEFSI 0 t 1"), _R("SUBS s 1 t"), _R("LNCMB MIDDLE t PN0 s PN1"), _R("ASSV P1 PN1"), _R("ASSV P2 PN0"), _R("ASSV P3 PN2"), _R("ASSV P4 PN3"), _R("ASSV P5 PN2"), _R("LNCMB A 1 P2 -1 P1"), _R("NRML A"), _R("LNCMB B 1 P3 -1 P2"), _R("NRML B"), _R("CROSS X1 A B"), _R("NRML X1"), _R("LNCMB R 1 P5 -1 P4"), _R("NRML R"), _R("CROSS Y1 X1 R"), _R("NRML Y1"), _R("CROSS Z1 X1 Y1"), _R("NRML Z1"), _R("ASSV X X1"), _R("LNCMB Y -1 Z1 0 Y1"), _R("ASSV Z Y1"), _R("ASSV P MIDDLE")};
  mafString _4PNT_Z[] = {_R("ASSV PN0 PNT1"), _R("ASSV PN1 PNT2"), _R("ASSV PN2 PNT3"), _R("ASSV PN3 PNT4"), _R("DEFSI 0 t 1"), _R("SUBS s 1 t"), _R("LNCMB MIDDLE t PN0 s PN1"), _R("SUBV Z PN1 PN0"), _R("NRML Z"), _R("SUBV Y1 PN3 PN2"), _R("NRML Y1"), _R("CROSS X Z Y1"), _R("NRML X"), _R("CROSS Y1 X Z"), _R("LNCMB Y -1 Y1 0 Y1"), _R("ASSV P MIDDLE")};

  ADD_PREDEF("Pelvis",     _IPE,     PELVIS);
  ADD_PREDEF("RThigh",     _RTH,     RTHIGH);
  ADD_PREDEF("LThigh",     _LTH,     LTHIGH);
  ADD_PREDEF("RShank_FAX", _RSH_FAX, RSHANK);
  ADD_PREDEF("RShank_FNE", _RSH_FNE, RSHANK);
  ADD_PREDEF("RShank_ISB", _RSH_ISB, NOTDEFINED);
  ADD_PREDEF("LShank_FAX", _LSH_FAX, LSHANK);
  ADD_PREDEF("LShank_FNE", _LSH_FNE, LSHANK);
  ADD_PREDEF("LShank_ISB", _LSH_ISB, NOTDEFINED);
  ADD_PREDEF("RFoot",     _RFT,      RFOOT);
  ADD_PREDEF("LFoot",     _LFT,      LFOOT);
  ADD_PREDEF("RHindfoot", _RHFT,     NOTDEFINED);
  ADD_PREDEF("LHindfoot", _LHFT,     NOTDEFINED);
  ADD_PREDEF("RForefoot", _RFFT,     NOTDEFINED);
  ADD_PREDEF("LForefoot", _LFFT,     NOTDEFINED);

  ADD_PREDEF("Thorax",  _TRX,    NOTDEFINED);
  ADD_PREDEF("RClavicle", _RCLV, NOTDEFINED);
  ADD_PREDEF("LClavicle", _LCLV, NOTDEFINED);
  ADD_PREDEF("RScapula", _RSCP,  NOTDEFINED);
  ADD_PREDEF("LScapula", _LSCP,  NOTDEFINED);
  ADD_PREDEF("RHumerus1",_RHUM1, NOTDEFINED);
  ADD_PREDEF("LHumerus1",_LHUM1, NOTDEFINED);
  ADD_PREDEF("RHumerus2",_RHUM2, NOTDEFINED);
  ADD_PREDEF("LHumerus2",_LHUM2, NOTDEFINED);
  ADD_PREDEF("RForearm", _RFRA,  NOTDEFINED);
  ADD_PREDEF("LForearm", _LFRA,  NOTDEFINED);

  ADD_PREDEF("3PNT_Y", _3PNT_Y, NOTDEFINED);
  ADD_PREDEF("3PNT_Z", _3PNT_Z, NOTDEFINED);
  ADD_PREDEF("4PNT_Y", _4PNT_Y, NOTDEFINED);
  ADD_PREDEF("4PNT_Z", _4PNT_Z, NOTDEFINED);
}

mafCxxTypeMacro(lhpOpAFSys)

//----------------------------------------------------------------------------
lhpOpAFSys::lhpOpAFSys(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType   = OPTYPE_OP;
  m_Canundo  = true;
  m_RefSys   = NULL;
  InitPredefined();
  m_Radio    = m_predefinedScripts.size();
}

//----------------------------------------------------------------------------
lhpOpAFSys::~lhpOpAFSys()
//----------------------------------------------------------------------------
{
  mafDEL(m_RefSys);
}

//----------------------------------------------------------------------------
mafOp* lhpOpAFSys::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpAFSys(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpAFSys::Accept(mafNode* vme)
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
  ID_RADIO_SCRIPT,
  ID_LOAD_DICTIONARY,
  ID_LOAD_SCRIPT,
  ID_LAST,
  ID_FORCED_DWORD = 0x7fffffff
};

//----------------------------------------------------------------------------
void lhpOpAFSys::OpRun()   
//----------------------------------------------------------------------------
{
  mafString strBase(m_Input->GetName());
  mafNEW(m_RefSys);
  strBase += _R("_AF_Frame");
  mafString str = strBase;
  unsigned ind = 0;
  unsigned i;
  do
  {
    for(i = 0; i < m_Input->GetNumberOfChildren(); i++)
    {
      mafNode *node = m_Input->GetChild(i);
      if(node->GetName() == str)
      {
        str = strBase + mafString::Format(_R("_%u"), ind);
        ind++;
        break;
      }
    }
  }
  while(i < m_Input->GetNumberOfChildren() && ind != UINT_MAX);
  m_RefSys->SetName(str);
  for(unsigned nm = 0; nm < m_predefinedScripts.size(); nm++)
  {
    if(stricmp(m_predefinedScripts[nm].m_Name.GetCStr(), m_Input->GetName().GetCStr()) == 0)
    {
      m_Radio = nm;
      m_RefSys->SetScriptText(m_predefinedScripts[m_Radio].m_Script);
      m_RefSys->SetBoneID(m_predefinedScripts[m_Radio].m_BoneID);
      break;
    }
  }
  CreateGui();
}

//----------------------------------------------------------------------------
void lhpOpAFSys::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  std::vector<mafString> list;
  for(unsigned i = 0; i < m_predefinedScripts.size(); i++)
  {
    list.push_back(m_predefinedScripts[i].m_Name);
  }
  list.push_back(_R("Custom"));
  m_Gui->Radio(ID_RADIO_SCRIPT, _R(""),&m_Radio, list.size(), &list[0]);
  m_Gui->FileOpen(ID_LOAD_SCRIPT, _R("Script"), &m_ScriptFName);
  m_Gui->Label(_R(""));

  m_Gui->Enable(ID_LOAD_SCRIPT, m_Radio == m_predefinedScripts.size());
  m_Gui->OkCancel();
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpAFSys::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
    HideGui();
    if(m_RefSys->GetParent())
    {
      m_RefSys->ReparentTo(NULL);
    }
    mafEventMacro(mafEvent(this,result));
  }
  else if (result == OP_RUN_OK)
  {
    if(m_Radio == m_predefinedScripts.size() && m_ScriptFName.IsEmpty())
    {
      wxMessageBox("Method is not specified","Alert", wxOK , NULL);
      return;
    }
    HideGui();
    mafEventMacro(mafEvent(this,result));
  }
}

bool lhpOpAFSys::ReadScript(const mafString& filename, std::vector<mafString>& output)
{
  FILE *fp = fopen(filename.GetCStr(), "rt");
  if(fp == NULL)
  {
    return false;
  }

  int const maxStrLen = 1000;
  char      sLine[maxStrLen];
  char      *pRet;

  while(true)
  {
    pRet = fgets(sLine, maxStrLen, fp);
    if(pRet == NULL)
      break;
    output.push_back(_R(pRet));
  }
  fclose(fp);
  return true;
}


//----------------------------------------------------------------------------
void lhpOpAFSys::OnEvent(mafEventBase *maf_event) 
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
    case ID_LOAD_SCRIPT:
      {
        std::vector<mafString> tmp;
        if(!m_ScriptFName.IsEmpty() && ReadScript(m_ScriptFName, tmp))
        {
          m_RefSys->SetScriptText(tmp);
          m_RefSys->SetBoneID(mafVMEAFRefSys::ID_AFS_NOTDEFINED);
        }
      }
      break;
    case ID_RADIO_SCRIPT:
      {
        m_Gui->Enable(ID_LOAD_SCRIPT, m_Radio == m_predefinedScripts.size());
        if(m_Radio != m_predefinedScripts.size())
        {
          m_RefSys->SetScriptText(m_predefinedScripts[m_Radio].m_Script);
          m_RefSys->SetBoneID(m_predefinedScripts[m_Radio].m_BoneID);
        }
        else
        {
          std::vector<mafString> tmp;
          if(!m_ScriptFName.IsEmpty() && ReadScript(m_ScriptFName, tmp))
          {
            m_RefSys->SetScriptText(tmp);
            m_RefSys->SetBoneID(mafVMEAFRefSys::ID_AFS_NOTDEFINED);
          }
        }
        m_Gui->Update();
      }
      break;
    default:
      mafEventMacro(*maf_event); 
      break;
  }
}

//----------------------------------------------------------------------------
void lhpOpAFSys::OpDo()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Please wait, working...");

  assert(m_RefSys);
  m_RefSys->ReparentTo(m_Input);
  m_RefSys->SetScaleFactor(100.0);
  m_RefSys->SetActive(1);
}
//----------------------------------------------------------------------------
void lhpOpAFSys::OpUndo()
//----------------------------------------------------------------------------
{
  assert(m_RefSys);
  m_RefSys->ReparentTo(NULL);
}

