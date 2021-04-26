/*=========================================================================

 Program: MAF2
 Module: mafGUIDialogLogin
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafGUIDialogLogin_H__
#define __mafGUIDialogLogin_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafGUIDialog.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** mafGUIDialogLogin - a dialog widget with a find VME GUI.
@sa mafGUIDialog
*/
class mafGUIDialogLogin : public mafGUIDialog
{
public:
	mafGUIDialogLogin(const mafString& title, long style = mafCLOSEWINDOW | mafOK | mafCANCEL);
	virtual ~mafGUIDialogLogin();

  void OnEvent(mafEventBase *maf_event);

  /** Set the user credentials.*/
  void SetUserCredentials(mafString &usename, mafString &pwd, int &proxyFlag, mafString &proxyHost, int &proxyPort, int &remember_me);
  
  /** Return the username information.*/
  mafString &GetUser();

  /** Return the password information.*/
  mafString &GetPwd();

  /** Select image type during saving of the views*/
  int GetProxyFlag(){return m_ProxyFlag;};

  /** Select image type during saving of the views*/
  mafString &GetProxyHost();

  /** Get Port in proxy connection*/
  int GetProxyPort();

  /** Set the flag to store user credentials.*/
  void SetRememberUserCredentials(int remember = 1) {m_RememberMe = remember;};

  /** Return information to allow saving or not users' information.*/
  int GetRememberUserCredentials() {return m_RememberMe;};

protected:

  /** Used to enable/disable items according to the current widgets state.*/
  void EnableItems();

  mafGUI *m_Gui; ///< Gui variable used to plug custom widgets.
  mafString m_Username; ///< Current username inserted into the dialog.
  mafString m_Pwd; ///< Current password inserted into the dialog.
  mafString m_UsernameOld; ///< Last username inserted.
  mafString m_PwdOld; ///< Last password inserted.
  mafString   m_ProxyHost;///< Proxy Host
  int         m_ProxyFlag;///< Flag used to for enable proxy. 
  int         m_ProxyPort;///< Proxy Host
  bool m_InformationsInserted; ///< Flag used to check if the user insert some information or not.
  int m_RememberMe;
};
#endif
