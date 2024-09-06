/*=========================================================================

 Program: MAF2Medical
 Module: medOpImporterC3D
 Authors: Paolo Quadrani - porting Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpImporterC3D_H__
#define __medOpImporterC3D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafNode;
class mafEvent;
class mafEventListener;
//----------------------------------------------------------------------------
// medOpImporterC3D :
//----------------------------------------------------------------------------
/**
class name: medOpImporterC3D
Import C3D file inside a landmark cloud. C3D is a standard format file
for movement analysis data. http://www.c3d.org/
*/
class MED_OPERATION_EXPORT medOpImporterC3D: public mafOp
{
public:
  mafTypeMacro(medOpImporterC3D, mafOp)
  /** constructor */
  medOpImporterC3D(const mafString& label=_R(""));
  /** destructor */
 ~medOpImporterC3D() override;
  /** retrieve the copy of the object */
  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme) override {return true;};

	/** Builds operation's interface. */
  void OpRun() override;

	/** Execute the operation. */
  void OpDo() override;

	/** Makes the undo for the operation. */
  void OpUndo() override;

protected:
  mafVME  *m_Vme; 
	mafString m_File;
	mafString m_FileDir;
	mafString m_Dict; 
	mafString m_DictDir;
	int m_DictionaryAvailable;
};
#endif
