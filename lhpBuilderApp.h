/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpBuilderApp.h,v $
  Language:  C++
  Date:      $Date: 2006-04-20 08:49:36 $
  Version:   $Revision: 1.1.1.1 $
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
#include "mafLogicWithManagers.h" 

// ============================================================================
class lhpBuilderApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();
protected:
  mafLogicWithManagers *m_Logic;
};
DECLARE_APP(lhpBuilderApp)
#endif //__testViewApp_H__


