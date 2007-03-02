/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.h,v $
  Language:  C++
  Date:      $Date: 2007-03-02 16:22:53 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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

// ============================================================================
class lhpBuilderApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();
protected:
  lhpBuilderLogic *m_Logic;
};
DECLARE_APP(lhpBuilderApp)
#endif //__testViewApp_H__
