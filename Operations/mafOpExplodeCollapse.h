/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpExplodeCollapse.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:49:23 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafOpExplodeCollapse_H__
#define __mafOpExplodeCollapse_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpExplodeCollapse :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpExplodeCollapse: public mafOp
{
public:
  mafOpExplodeCollapse(const wxString &label = "ExplodeCollapse");
 ~mafOpExplodeCollapse(); 
  
  mafTypeMacro(mafOpExplodeCollapse, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();
};
#endif
