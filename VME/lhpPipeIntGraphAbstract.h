/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphAbstract.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphAbstract_H_
#define _lhpPipeIntGraphAbstract_H_

#include "lhpDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "mafPipe.h"
#include "mafGraphIDDesc.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafViewIntGraph;
class mafGUICheckListBox;

//----------------------------------------------------------------------------
// lhpPipeIntGraphAbstract :
//----------------------------------------------------------------------------
class lhpPipeIntGraphAbstract : public mafPipe
{
public:
  static bool s_WholeRange;
  mafAbstractTypeMacro(lhpPipeIntGraphAbstract, mafPipe);
  
  lhpPipeIntGraphAbstract();
  virtual     ~lhpPipeIntGraphAbstract();

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafNode *node, mafView *view);

  /** Answer events coming from the Gui and from cloud */
  void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_INT_GRAPH_ABSTRACT
  {
    ID_WHOLE_RANGE = Superclass::ID_LAST,
    ID_LAST
  };

  virtual const mafString& GetVarTitle(int i) const = 0;
  virtual const mafString& GetVarUnit(int i) const = 0;
  virtual double     GetVarDerivativeCoef(int i) const = 0;
  virtual std::istream& operator>>(std::istream& is);
  virtual std::ostream& operator<<(std::ostream& os) const ;
  int                GetForcedWholeRange(){return m_ForcedWholeRange;}
  void               SetForcedWholeRange(int forced);
  void               SetSmoothParam(double param);

protected:
  virtual void GrabData() = 0;

  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI *CreateGui();

  mafGraphDataImpl                      *m_Graph;
  int                                   m_ForcedWholeRange;
  int                                   m_WholeRange;
};  
#endif // _lhpPipeIntGraphAbstract_H_
