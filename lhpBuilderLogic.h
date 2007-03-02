/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderLogic.h,v $
  Language:  C++
  Date:      $Date: 2007-03-02 15:53:37 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __lhpBuilderLogic_H__
#define __lhpBuilderLogic_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpBuilderLogic :
//----------------------------------------------------------------------------
/**
*/
class lhpBuilderLogic: public mafLogicWithManagers
{
public:
               lhpBuilderLogic();
	virtual     ~lhpBuilderLogic(); 

protected:
  /** Respond to a VME_ADDED evt. propagate evt. to SideBar,ViewManager,ecc.. */
  virtual void VmeAdded(mafNode *vme);
};
#endif
