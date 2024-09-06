/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphOVPGES.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeIntGraphOVPGES_H_
#define _lhpPipeIntGraphOVPGES_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "lhpPipeIntGraphLocal.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphOVPGES :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeIntGraphOVPGES :
//----------------------------------------------------------------------------
class lhpPipeIntGraphOVPGES : public lhpPipeIntGraphLocal
{
public:
  mafTypeMacro(lhpPipeIntGraphOVPGES, lhpPipeIntGraphLocal);

  lhpPipeIntGraphOVPGES();
  ~lhpPipeIntGraphOVPGES() override;

  /** IDs for the GUI */
  enum PIPE_INT_GRAPH_OVPGES
  {
    ID_REF_STAMP = Superclass::ID_LAST,
    ID_LAST
  };

  enum GRAPH_IDS
  {
  //OVP
  GDT_OVP_POSX    = Superclass::GDT_LAST,
  GDT_OVP_POSY   ,
  GDT_OVP_POSZ   ,
  GDT_OVP_ROTX   ,
  GDT_OVP_ROTY   ,
  GDT_OVP_ROTZ   ,

  //GES
  GDT_GES_POSX   ,
  GDT_GES_POSY   ,
  GDT_GES_POSZ   ,
  GDT_GES_ROTX   ,
  GDT_GES_ROTY   ,
  GDT_GES_ROTZ   ,

  //control
  GDT_LAST
  };
  /** Answer events coming from the Gui and from cloud */
  void OnEvent(mafEventBase *maf_event) override;

  const mafString& GetVarTitle(int i) const override;
  const mafString& GetVarUnit(int i) const override;
  double       GetVarDerivativeCoef(int i) const override;

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  mafGUI *CreateGui() override;
  bool StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts) override;
  mafTimeStamp m_RefStamp;
};
#endif // _lhpPipeIntGraphOVPGES_H_
