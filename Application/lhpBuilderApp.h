/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.h,v $
  Language:  C++
  Date:      $Date: 2008-03-26 13:28:19 $
  Version:   $Revision: 1.2 $
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
