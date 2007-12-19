/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandler.h,v $
  Language:  C++
  Date:      $Date: 2007-12-19 16:18:59 $
  Version:   $Revision: 1.3 $
  Authors:   Stefano Perticoni - Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpTagHandler_h
#define __lhpTagHandler_h

//----------------------------------------------------------------------------
// forward references:
//----------------------------------------------------------------------------
class mafVME;

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafObject.h"
#include "mafString.h"
#include "lhpUser.h"

class MAF_EXPORT lhpTagHandlerInputOutputParametersCargo : public mafObject
{
public:
  mafTypeMacro(lhpTagHandlerInputOutputParametersCargo, mafObject);
  
  /** Set the input vme */
  void SetInputVme(mafVME* vme) {m_InputVme = vme;};
  mafVME *GetInputVme(){return m_InputVme;};

  /** Set the input vme */
  void SetInputUser(mafUser* user) {m_InputUser = user;};
  mafUser *GetInputUser(){return m_InputUser;};
  
  /** Called by the tag handler code: fill the auto string to be written in XML*/
  void SetTagHandlerGeneratedString(mafString tagHandlerGeneratedString) {m_TagHandlerGeneratedString = tagHandlerGeneratedString;};
  mafString GetTagHandlerGeneratedString() {return m_TagHandlerGeneratedString;};

private:
  mafVME* m_InputVme;
  mafUser* m_InputUser;
  mafString m_TagHandlerGeneratedString;
  lhpTagHandlerInputOutputParametersCargo();

};

class MAF_EXPORT lhpTagHandler : public mafObject
{
public:
  mafTypeMacro(lhpTagHandler,mafObject);

  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo){mafLogMessage("Please write handling code for this tag!");};  
  const char *GetTagName(){return m_TagName;};

protected:
  virtual void ExtractTagName();
  mafString m_TagName;
  lhpTagHandler();
};
#endif