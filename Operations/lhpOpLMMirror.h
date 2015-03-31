/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpLMMirror.h,v $
  Language:  C++
  Date:      $Date: 2007-12-28 12:55:08 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani - porting  Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpLMMirror_H__
#define __lhpOpLMMirror_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"


//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafEvent;
class vtkMEDPolyDataMirror;

//----------------------------------------------------------------------------
// lhpOpLMMirror :
//----------------------------------------------------------------------------
/** */
class lhpOpLMMirror: public mafOp
{
public:
	lhpOpLMMirror(wxString label = "Surface Mirror");
	~lhpOpLMMirror(); 
	virtual void OnEvent(mafEventBase *maf_event);
	mafOp* Copy();

	mafTypeMacro(lhpOpLMMirror, mafOp);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node);   


	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result);

	int		m_MirrorX;
	int		m_MirrorY;
	int		m_MirrorZ;
};
#endif
