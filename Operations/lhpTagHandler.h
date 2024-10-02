/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandler.h,v $
  Language:  C++
  Date:      $Date: 2009-03-31 16:43:23 $
  Version:   $Revision: 1.4.2.2 $
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
#include "ftk/Base/Object.h"
#include "ftk/Base/String.h"
#include "mafUser.h"

class MAF_EXPORT lhpTagHandlerInputOutputParametersCargo : public mafObject
{
public:
  mafTypeMacro(lhpTagHandlerInputOutputParametersCargo, mafObject);
  
  /** Set the input vme */
  void SetInputVme(mafVME* vme) {m_InputVme = vme;};
  mafVME *GetInputVme(){return m_InputVme;};

  /** Set the input user */
  void SetInputUser(mafUser* user) {m_InputUser = user;};
  mafUser *GetInputUser(){return m_InputUser;};

	/** Set the input storage */
	void SetInputMSF(const char *msfFile) {m_InputMSF = _R(msfFile);};
	const char *GetInputMSF(){return m_InputMSF.GetCStr();};
  
  /** Called by the tag handler code: fill the auto string to be written in XML*/
  void SetTagHandlerGeneratedString(mafString tagHandlerGeneratedString) {m_TagHandlerGeneratedString = tagHandlerGeneratedString;};
  mafString GetTagHandlerGeneratedString() {return m_TagHandlerGeneratedString;};

private:
  mafVME* m_InputVme;
  mafUser* m_InputUser;
	mafString m_InputMSF;
  mafString m_TagHandlerGeneratedString;
  lhpTagHandlerInputOutputParametersCargo();

};

class MAF_EXPORT lhpTagHandler : public mafObject
{
public:
  mafTypeMacro(lhpTagHandler,mafObject);

  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo){mafLogMessage(_M("Please write handling code for this tag!"));}
  const char *GetTagName(){return m_TagName.GetCStr();};
  
    /** Set python.exe interpreter used at runtime*/
  void SetPythonExe(const char *pythonExe){m_PythonExe = _R(pythonExe);};
  
  /** Get python.exe interpreter used at runtime*/
  mafString &GetPythonExe(){return m_PythonExe;};

  /** Set pythonw.exe interpreter used at runtime*/
  void SetPythonwExe(const char *pythonwExe){m_PythonwExe = _R(pythonwExe);};

	/** Get pythonw.exe interpreter used at runtime*/
  mafString &GetPythonwExe() {return m_PythonwExe;};

  
protected:
  virtual void ExtractTagName();
  mafString m_TagName;

  mafString   m_PythonExe;///< python.exe full path
  mafString   m_PythonwExe;///< pythonw.exe full path

	mafString m_VMEUploaderDownloaderDir;

  lhpTagHandler();
};
#endif