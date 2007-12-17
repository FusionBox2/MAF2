/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandler.h,v $
  Language:  C++
  Date:      $Date: 2007-12-17 09:15:47 $
  Version:   $Revision: 1.1 $
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

class MAF_EXPORT lhpTagHandler : public mafObject
{
public:
  mafTypeMacro(lhpTagHandler,mafObject);

  lhpTagHandler();
  virtual void FillVMETag(mafVME *vme) {mafLogMessage("Please write handling code for this tag!");};  
  const char *GetTagName(){return m_TagName;};

protected:
  virtual void ExtractTagName();
  mafString m_TagName;
};
#endif