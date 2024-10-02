/*=========================================================================

 Program: MAF2
 Module: mafNodeLayout
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafNodeLayout_h
#define __mafNodeLayout_h

#include "mafNode.h"
#include "mafRoot.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------
class mmaApplicationLayout;

/** mafNodeLayout - a VME is the node to store application layout list. 
  @sa mafNodeLayout mafNode */
class MAF_EXPORT mafNodeLayout : public mafNode
{
public:
  mafTypeMacro(mafNodeLayout,mafNode);

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;// const;

  /** Return the application layout attribute.*/
  std::shared_ptr<mmaApplicationLayout> GetLayout();

protected:
  mafNodeLayout();
  ~mafNodeLayout() override {}

private:
  mafNodeLayout(const mafNodeLayout&); // Not implemented
  void operator=(const mafNodeLayout&); // Not implemented
};
#endif
