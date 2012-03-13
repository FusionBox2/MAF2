/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIBaseDlg.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __testRWIBaseDlg_H__
#define __testRWIBaseDlg_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "wx/dialog.h"
#include "mafGUIDialog.h"

#include "mafRWIBase.h"
#include "mafRWI.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
//----------------------------------------------------------------------------
// testRWIBaseDlg :
//----------------------------------------------------------------------------
/**
sample showing the usage of RWIBase
*/
class testRWIBaseDlg : public mafGUIDialog
{
public:
	testRWIBaseDlg (const wxString& title);
	virtual ~testRWIBaseDlg (); 
protected:

  vtkConeSource *CS;
  vtkPolyDataMapper *PDM;
  vtkActor *A;
  vtkRenderer *R;
  vtkRenderWindow *RW; 
  mafRWIBase *RWI;
};
#endif
