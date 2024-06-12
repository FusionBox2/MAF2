/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpFusionBoxApp.h,v $
  Language:  C++
  Date:      $Date: 2009-05-08 09:23:51 $
  Version:   $Revision: 1.5.2.1 $
  Authors:   Paolo Quadrani , Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpFusionBoxApp_H__
#define __lhpFusionBoxApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medLogicWithManagers.h"
#include "mafEvent.h"
#include "mafBaseEventHandler.h"
#include "mafDynamicLoader.h"
#include <memory>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class lhpFusionBoxApp : public wxApp
{
public:
  bool OnInit();
  int  OnExit();


  ////Called when the application is in the idle state
  //virtual void OnIdle(wxIdleEvent& event);  
  //DECLARE_EVENT_TABLE()

protected:
  std::unique_ptr<medLogicWithManagers> m_Logic;
  std::vector<std::pair<LibHandle, void(*)() > > m_Plugins;

};
DECLARE_APP(lhpFusionBoxApp)
#endif 
