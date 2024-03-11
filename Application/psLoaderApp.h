/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: psLoaderApp.h,v $
  Language:  C++
  Date:      $Date: 2008-11-10 14:10:46 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __psLoaderApp_H__
#define __psLoaderApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "lhpBuilderLogic.h" 
#include "mafEvent.h"
#include "mafBaseEventHandler.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class lhpUser;

class psLoaderApp : public wxApp
{
public:
  bool OnInit();
  int  OnExit();


  ////Called when the application is in the idle state
  //virtual void OnIdle(wxIdleEvent& event);  
  //DECLARE_EVENT_TABLE()

protected:
  lhpBuilderLogic *m_Logic;

};
DECLARE_APP(psLoaderApp)
#endif 
