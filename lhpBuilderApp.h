/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.h,v $
  Language:  C++
  Date:      $Date: 2007-07-23 10:52:01 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani , Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testViewApp_H__
#define __testViewApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "lhpBuilderLogic.h" 

class lhpBuilderApp : public wxApp
{
public:
  bool OnInit();
  int  OnExit();
protected:
  lhpBuilderLogic *m_Logic;
};
DECLARE_APP(lhpBuilderApp)
#endif 
