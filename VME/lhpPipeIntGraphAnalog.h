/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphAnalog.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphAnalog_H_
#define _lhpPipeIntGraphAnalog_H_

#include "lhpDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphAbstract.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafGUICheckListBox;
class medVMEAnalog;

//----------------------------------------------------------------------------
// lhpPipeIntGraphAnalog :
//----------------------------------------------------------------------------
class lhpPipeIntGraphAnalog : public lhpPipeIntGraphAbstract
{
public:
  mafTypeMacro(lhpPipeIntGraphAnalog, lhpPipeIntGraphAbstract);
  
  lhpPipeIntGraphAnalog();
  virtual     ~lhpPipeIntGraphAnalog();

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Answer events coming from the Gui and from cloud */
  void OnEvent(mafEventBase *maf_event);

  /** Change the visibility of the bounding box actor representing the selection for the vme. */
  virtual void Select(bool select); 

  /** Set the scalar visibility. */
  //void SetScalarVisibility(int visibility){m_ScalarVisibility = visibility;};

  /** IDs for the GUI */
  enum PIPE_INT_GRAPH
  {
    ID_CHECK_BOXXVAL = Superclass::ID_LAST,
    ID_CHECK_BOXYVAL,
    ID_CHECK_BOXYDER,
    ID_LIMITED,
    ID_START,
    ID_END,
    ID_LAST
  };

  virtual const char   *GetVarTitle(int i) const;
  virtual const char   *GetVarUnit(int i) const;
  virtual double       GetVarDerivativeCoef(int i) const;

protected:
  virtual void GrabData();
  virtual void UpdateGUIChecks();
  virtual void StoreValueByIdx(int nVarID, int nGraphIndex, mafTimeStamp ts, mafTimeStamp prevts);

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI *CreateGui();
  /** Update the properties according to the vme's tags. */
  void UpdateProperty(bool fromTag = false);

  mafGUICheckListBox                    *m_CheckBoxXval;
  mafGUICheckListBox                    *m_CheckBoxYval;
  mafGUICheckListBox                    *m_CheckBoxYder;
  mafTimeStamp                          m_PrevStamp;

  int                                   m_NumberOfSignals;
  int                                   m_TimeStamp;
  std::vector<mafString>                m_Names;
  medVMEAnalog                          *m_EmgPlot;
  mafTimeStamp                          m_Start;
  mafTimeStamp                          m_End;
  int                                   m_Limited;

};  
#endif // _lhpPipeIntGraphAnalog_H_
