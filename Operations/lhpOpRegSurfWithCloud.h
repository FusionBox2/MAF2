/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpRegSurfWithCloud.h,v $
  Language:  C++
  Date:      $Date: 2007-07-09 16:58:31 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani      - porting Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpRegSurfWithCloud_H__
#define __lhpOpRegSurfWithCloud_H__

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
class mafEvent;
//----------------------------------------------------------------------------
// lhpOpRegSurfWithCloud :
//----------------------------------------------------------------------------
/** */
class lhpOpRegSurfWithCloud: public mafOp
{
public:
  lhpOpRegSurfWithCloud(const mafString& label = _L("Register Surface with Cloud"));
 ~lhpOpRegSurfWithCloud(); 
  virtual void OnEvent(mafEventBase *maf_event);
  
  mafTypeMacro(lhpOpRegSurfWithCloud, mafOp);
  
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);   

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

  static bool ClosedCloudAccept(mafNode* node) {return (node != NULL);}// && node->IsA("mafVMELandmarkCloud") && !((mafVMELandmarkCloud*)node)->IsOpen());};

protected:
  /** Check the correctness of the vme's type. */
  void OnChooseVme(mafNode *vme);
  void OpStop(int result);

  //mafVMELandmarkCloud*                         m_Source;
  mafVME*                                      m_Source;
  mafVME                                       *m_Resultat;
  mafString                                    m_SourceName;
  int                                          m_MultiTime;
  std::vector<std::pair<wxString, wxString> >  m_LMDict;
  mafString                                    m_ScriptFName;

 };
#endif
