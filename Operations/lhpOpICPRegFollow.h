/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpICPRegFollow.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 10:29:46 $
  Version:   $Revision: 1.3 $
  Authors:   Stefania Paperini, Stefano Perticoni, porting Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpICPRegFollow_H__
#define __lhpOpICPRegFollow_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafEvent;
class mafNode;
class mafVME;
class mafVMESurface;
class mafString;

//----------------------------------------------------------------------------
// lhpOpICPRegFollow :
//----------------------------------------------------------------------------
class lhpOpICPRegFollow: public mafOp
{
public:
  mafTypeMacro(lhpOpICPRegFollow, mafOp)
	lhpOpICPRegFollow(const mafString& label = _R(""));
	~lhpOpICPRegFollow() override; 
	void   OnEvent(mafEventBase *maf_event) override;
	mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme) override;

	/** Builds operation's interface. */
	void OpRun() override;

	/** Execute the operation. */
	void OpDo() override;

protected:
	/** Create the gui */
  virtual void CreateGui();

	/** Allow to choose the target surface for the registration. */
	void OnChooseTarget();

  /** Allow to choose the target surface for the registration. */
  void OnChooseSource();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result) override;

	mafVME*					m_Target;				// m_input, inherited from mafOp, is used as source
	mafVME*					m_Source;				// m_input, inherited from mafOp, is used as source
	mafVMESurface*					m_Registered;   // the output: a copy of m_input registered on m_target
	mafString					m_InputName;
	mafString					m_TargetName;
  mafString					m_SourceName;
	mafString					m_ReportFilename;
	double						m_Convergence;
};
#endif
