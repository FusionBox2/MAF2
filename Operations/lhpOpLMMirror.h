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
	lhpOpLMMirror(const mafString& label = _R("Surface Mirror"));
	~lhpOpLMMirror() override;
	void OnEvent(mafEventBase *maf_event) override;
	mafOp* Copy() override;

	mafTypeMacro(lhpOpLMMirror, mafOp);

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* node) override;   


	/** Builds operation's interface. */
  void OpRun() override;

	/** Execute the operation. */
  void OpDo() override;

	/** Makes the undo for the operation. */
  void OpUndo() override;

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  void OpStop(int result) override;

	int		m_MirrorX;
	int		m_MirrorY;
	int		m_MirrorZ;
};
#endif
