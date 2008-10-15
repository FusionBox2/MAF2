/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.h,v $
  Language:  C++
  Date:      $Date: 2008-10-15 15:34:28 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani , Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpBuilderApp_H__
#define __lhpBuilderApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "lhpBuilderLogic.h" 

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class lhpUser;

class lhpBuilderApp : public wxApp
{
public:
  bool OnInit();
  int  OnExit();

  /** Used with proxy authentication*/
  void SetProxyURL(mafString proxyURL) {m_ProxyURL = proxyURL;};
  mafString GetProxyURL() {return m_ProxyURL;};

  /** Used with proxy authentication*/
  void SetProxyPort(mafString proxyPort) {m_ProxyPort = proxyPort;};
  mafString GetProxyPort() {return m_ProxyPort;};

  ////Called when the application is in the idle state
  //virtual void OnIdle(wxIdleEvent& event);  
  //DECLARE_EVENT_TABLE()

protected:
  lhpBuilderLogic *m_Logic;
  static lhpUser  m_User;
  mafString m_ProxyURL;
  mafString m_ProxyPort;
};
DECLARE_APP(lhpBuilderApp)
#endif 
