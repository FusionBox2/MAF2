/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRegisterLMScripted.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:14:48 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani      - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpRegisterLMScripted_H__
#define __lhpOpRegisterLMScripted_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEGroup.h"
#include "mafVMEInfoText.h"
#include "mafVMESurface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafEvent;
class vtkPoints;
class mafGUIDialog;

//----------------------------------------------------------------------------
// lhpOpRegisterLMScripted :
//----------------------------------------------------------------------------
/** */
class lhpOpRegisterLMScripted: public mafOp
{
public:
  lhpOpRegisterLMScripted(wxString label = _("Register Landmark Cloud"));
 ~lhpOpRegisterLMScripted(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(lhpOpRegisterLMScripted, mafOp);
  
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  static bool ClosedCloudAccept(mafNode* node) {if(node != NULL && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen())return true;return false;}

protected:
  bool ReadLMDictionary(mafString *fileName);
  void ParseString(wxString &pFirstLine, wxString &sOne, wxString &sTwo);

  /** Method called to extract matching point between source and target.*/
  int ExtractMatchingPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, double time = -1);
  
  /** Register the source  on the target  according 
  to the registration method selected: rigid, similar or affine. */
  double RegisterPoints(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMEGenericAbstract *reg, double currTime = -1);

  /** Check the correctness of the vme's type. */
  void OnChooseTargetVme(mafNode *vme);

  bool RegistrationProcedure();

  bool ProcessNode(mafVMELandmarkCloud *src, mafVMELandmarkCloud *trg, mafVMELandmarkCloud *registered);

  mafVME*      m_Source;
  mafVME*      m_Target;
  mafVME*      m_Registered;

  mafString   m_SourceName;
  mafString   m_TargetName;
  
  vtkPoints *m_PointsSource;
  vtkPoints *m_PointsTarget;
  
  int m_RegistrationMode; 
  int m_MultiTime;           

  std::vector<std::pair<wxString, wxString> >  m_LMDict;
  mafString                                    m_ListFName;
 };
#endif
