/*=========================================================================

 Program: MAF2
 Module: mafStorable
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafStorable.h"
#include "mafStorageElement.h"

//------------------------------------------------------------------------------
int mafStorable::Store(mafStorageElement *parent)
//------------------------------------------------------------------------------
{
  return InternalStore(parent);
}

//------------------------------------------------------------------------------
int mafStorable::Restore(mafStorageElement *element)
//------------------------------------------------------------------------------
{
  return InternalRestore(element);
}
