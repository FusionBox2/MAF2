/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeIntGraph.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _mafPipeIntGraph_H_
#define _mafPipeIntGraph_H_

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafPipe.h"
#include "mafMemGraph.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafViewIntGraph;

//----------------------------------------------------------------------------
// mafPipeIntGraph :
//----------------------------------------------------------------------------
class mafPipeIntGraph : public mafPipe
{
public:
               mafPipeIntGraph();
  virtual     ~mafPipeIntGraph();
  void         SetView(mafViewIntGraph  *view){m_View = view;}
  void         GrabData(int nIdx, mafTimeStamp nTimeStamp = -1.);
protected:
  void         StoreValueByIdx(int nObjectOrderID, IDType nVarID, int nGraphIndex, mafTimeStamp nTimeStamp = -1., mafTimeStamp nPrevTimeStamp = -1.);
  void         InvalidateAllVars();
  mafViewIntGraph                       *m_View;
  mafTimeStamp                          m_PrevStamp;
  std::vector<std::pair<double, bool> > m_variables;
};  
#endif // _mafPipeIntGraph_H_
