/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDictionary.h,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafDictionary_H__
#define __mafDictionary_H__


#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>
#include "mafString.h"

bool ReadDictionary(mafString *fileName, std::vector<std::pair<mafString, mafString> >&  dictionary);
mafString const *LookupUserName(const mafString& name, std::vector<std::pair<mafString, mafString> >&  dictionary);
mafString const *LookupStdName(const mafString& name, std::vector<std::pair<mafString, mafString> >&  dictionary);


#endif