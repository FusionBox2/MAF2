/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-10-22 12:00:35 $
  Version:   $Revision: 1.5 $
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
#include "medLogicWithManagers.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class lhpGUINetworkConnectionSettings;
class lhpUser;

//----------------------------------------------------------------------------
// lhpBuilderLogic :
//----------------------------------------------------------------------------
/**
*/
class lhpBuilderLogic: public medLogicWithManagers
{
public:
               lhpBuilderLogic();
	virtual     ~lhpBuilderLogic(); 

	virtual void OnEvent(mafEventBase *maf_event);

  /** Configure the application.
  At this point are plugged all the managers, the side-bar docking panel. 
  Are plugged also all the setting to the dialogs interface. */
  virtual void Configure();

  /** Get user credentials */
  void GetCredentials();

protected:
  /** Respond to a VME_ADDED evt. propagate evt. to SideBar,ViewManager,ecc.. */
  virtual void VmeAdded(mafNode *vme);

  lhpGUINetworkConnectionSettings *m_NetworkConnectionSettings;
  lhpUser  *m_User;
};
#endif
