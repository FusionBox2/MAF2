/*=========================================================================

 Program: MAF2
 Module: lhpOpImporterPLY
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __lhpOpImporterPLY_H__
#define __lhpOpImporterPLY_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafString.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafVMESurface;
class mafEvent;

#ifdef MAF_EXPORTS
#include "mafDllMacros.h"
EXPORT_STL_VECTOR(MAF_EXPORT,mafString);
EXPORT_STL_VECTOR(MAF_EXPORT,mafVMESurface*);
EXPORT_STL_VECTOR(MAF_EXPORT,int);
#endif

//----------------------------------------------------------------------------
// lhpOpImporterPLY :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT lhpOpImporterPLY: public mafOp
{
public:
	lhpOpImporterPLY(const mafString& label = _R("PLYImporter"));
	~lhpOpImporterPLY() override; 
	
  mafTypeMacro(lhpOpImporterPLY, mafOp);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node) override;

	/** Set the filename for the .stl to import */
  void SetFileName(const mafString& file_name);

  /** Builds operation's interface. */
	void OpRun() override;

  /** Makes the undo for the operation. */
  void OpUndo() override;

  /** Execute the operation. */
  void OpDo() override;

  /** Import the file. */
	void ImportPLY();

  /** Used to retrieve imported data. It is useful when the operation is executed from code.*/
  void GetImportedPLY(std::vector<mafVMESurface*> &importedPLY);

protected:

  std::vector<mafVMESurface*> m_ImportedPLYs;
  std::vector<mafString>	    m_Files;
	mafString	                  m_FileDir;
};
#endif
