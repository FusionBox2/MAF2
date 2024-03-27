/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphFixed.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphFixed_H_
#define _lhpPipeIntGraphFixed_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphAbstract.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafGUICheckListBox;

//----------------------------------------------------------------------------
// lhpPipeIntGraphFixed :
//----------------------------------------------------------------------------
class lhpPipeIntGraphFixed : public lhpPipeIntGraphAbstract
{
public:
  mafTypeMacro(lhpPipeIntGraphFixed, lhpPipeIntGraphAbstract);
  
  lhpPipeIntGraphFixed();
  virtual     ~lhpPipeIntGraphFixed();

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafNode *node, mafView *view);

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
    ID_LAST
  };
  enum GRAPH_IDS
  {
    GDT_NA   = -1,
    //control
    GDT_LAST = 0,
  };

  virtual const mafString& GetVarTitle(int i) const;
  virtual const mafString& GetVarUnit(int i) const;
  virtual double       GetVarDerivativeCoef(int i) const;
  virtual std::istream& operator>>(std::istream& is);
  virtual std::ostream& operator<<(std::ostream& os) const;

protected:
  virtual void GrabData();
  virtual void UpdateGUIChecks();
  virtual bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts);
  void InvalidateAllVars();
  bool IsValueStored(int ID){return m_vars[ID].second;}
  double GetValue(int ID){return m_vars[ID].first;}
  void SetValue(int ID, double value){m_vars[ID].first = value;m_vars[ID].second = true;}

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI *CreateGui();
  /** Update the properties according to the vme's tags. */
  void UpdateProperty(bool fromTag = false);

  mafGUICheckListBox                    *m_CheckBoxXval;
  mafGUICheckListBox                    *m_CheckBoxYval;
  mafGUICheckListBox                    *m_CheckBoxYder;
  mafTimeStamp                          m_PrevStamp;
  std::vector<std::pair<double, bool> > m_vars;
  int                                   m_InGrabData;
};  
#endif // _lhpPipeIntGraphFixed_H_
