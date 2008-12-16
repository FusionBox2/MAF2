/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: psLoaderPlusApp.h,v $
  Language:  C++
  Date:      $Date: 2008-12-16 15:09:38 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __psLoaderPlusApp_H__
#define __psLoaderPlusApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "lhpBuilderLogic.h" 
#include "mafEvent.h"
#include "mafObserver.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class lhpUser;

class psLoaderPlusApp : public wxApp
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
DECLARE_APP(psLoaderPlusApp)
#endif 
