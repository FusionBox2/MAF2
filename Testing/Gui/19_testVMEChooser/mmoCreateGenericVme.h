/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateGenericVme.h,v $
  Language:  C++
  Date:      $Date: 2007-08-13 08:13:38 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmoCreateGenericVme_H__
#define __mmoCreateGenericVme_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafNodeGeneric;
// ============================================================================
class mmoCreateGenericVme : public mafOp
// ============================================================================
{
public:
                mmoCreateGenericVme(wxString label);
               ~mmoCreateGenericVme();
	virtual void OnEvent(mafEvent& e);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return vme != NULL;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafNodeGeneric  *m_vme; 
};
#endif // __mmoCreateGenericVme_H__
