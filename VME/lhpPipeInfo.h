/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeInfo.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 22:11:37 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef _lhpPipeInfo_H_
#define _lhpPipeInfo_H_

#include "mafDefines.h"

//----------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeInfo :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// lhpPipeInfo :
//----------------------------------------------------------------------------
class lhpPipeInfo : public mafPipe
{
public:
  mafTypeMacro(lhpPipeInfo, mafPipe);

  lhpPipeInfo();
  ~lhpPipeInfo() override;

  void Create(mafNode *node, mafView *view) override;

  void OnEvent(mafEventBase *maf_event) override;

  const mafString& GetPageText(){return m_PageText;}

protected:
  void FillPage();
  mafString m_PageText;
};
#endif // _lhpPipeInfo_H_
