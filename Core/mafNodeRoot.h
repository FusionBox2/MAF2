/*=========================================================================

 Program: MAF2
 Module: mafNodeRoot
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafNodeRoot_h
#define __mafNodeRoot_h

#include "mafNode.h"
#include "ftk/Core/Root.h"

/** mafNodeRoot - a VME is the root of a tree of VME nodes. 
  This node is a specialized VME acting as root of the tree. It inherits
  root behavior from mafNodeRoot and also is responsible for generating 
  Ids fore VME items added to the tree.
  @sa mafNodeRoot mafNode */
class MAF_EXPORT mafNodeRoot : public mafNode, public model::data::Root
{
public:
  mafTypeMacro(mafNodeRoot,mafNode);

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;// const;

  /** allow only a NULL parent */
  bool CanReparentTo(mafNode *parent) override {return parent == nullptr;}

  void OnEvent(mafEventBase *e) override;

  void RenewIds(Node *node) override;

  /** return an xpm-icon that can be used to represent this node */
  static const char ** GetIcon();   //SIL. 11-4-2005:  
protected:
  mafNodeRoot() {}
  ~mafNodeRoot() override {}

private:
  mafNodeRoot(const mafNodeRoot&); // Not implemented
  void operator=(const mafNodeRoot&); // Not implemented
};

#endif
