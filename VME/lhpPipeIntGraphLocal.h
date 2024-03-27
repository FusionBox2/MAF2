/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphLocal.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphLocal_H_
#define _lhpPipeIntGraphLocal_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphGlobal.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafVME;
//----------------------------------------------------------------------------
// lhpPipeIntGraphLocal :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphLocal :
//----------------------------------------------------------------------------
class lhpPipeIntGraphLocal : public lhpPipeIntGraphGlobal
{
public:
  mafTypeMacro(lhpPipeIntGraphLocal, lhpPipeIntGraphGlobal);

  lhpPipeIntGraphLocal();
  virtual     ~lhpPipeIntGraphLocal();

  /** IDs for the GUI */
  enum PIPE_INT_GRAPH_OVPGES
  {
    ID_PARENT = Superclass::ID_LAST,
    ID_RESETPARENT,
    ID_LAST
  };

  virtual void Create(mafNode *node, mafView *view);

  enum GRAPH_IDS
  {
  //LTM
  GDT_LAST   = Superclass::GDT_LAST,
  /*GDT_LTM_POSX   = Superclass::GDT_LAST,
  GDT_LTM_POSY   ,
  GDT_LTM_POSZ   ,
  GDT_LTM_ROTX   ,
  GDT_LTM_ROTY   ,
  GDT_LTM_ROTZ   ,

  //control
  GDT_LAST*/
  };

  virtual const mafString& GetVarTitle(int i) const;
  virtual const mafString& GetVarUnit(int i) const;
  virtual double       GetVarDerivativeCoef(int i) const;
  virtual std::istream& operator>>(std::istream& is);
  virtual std::ostream& operator<<(std::ostream& os) const;

  /** Answer events coming from the Gui and from cloud */
  void OnEvent(mafEventBase *maf_event);

  mafVME *GetProximal()const{ return m_Proximal; }
  void SetProximal(mafVME *proximal);

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI *CreateGui();
  virtual bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts);
  mafVME *m_Proximal;
  mafString m_ProximalName;
};
#endif // _lhpPipeIntGraphLocal_H_
