/*=========================================================================

 Program: MAF2
 Module: mafOpCreateOsteometricBoard
 Authors: TJ
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateOsteometricBoard_H__
#define __mafOpCreateOsteometricBoard_H__

#include "mafOp.h"
#include "mafVMEOsteometricBoard.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
//class mafVMEPlane;
class mafGUI;
class mafEvent;
//class mafVMEOsteometricBoard;
//----------------------------------------------------------------------------
// mafOpCreateVolume :
//----------------------------------------------------------------------------
/** Operation used to create a mafVolumeGray containing the vtkStructuredPoint
dataset filled with a constant scalar value. This is useful for testing purposes.*/
class MAF_EXPORT mafOpCreateOsteometricBoard : public mafOp
{
public:
	mafOpCreateOsteometricBoard(const mafString& label = _R("Create OsteometricBoard"));
	~mafOpCreateOsteometricBoard();

	mafTypeMacro(mafOpCreateOsteometricBoard, mafOp);

  /** Receive events coming from the user interface.*/
  //void OnEvent(mafEventBase *maf_event);

  /** Return a copy of the operation.*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Builds operation's interface. */
  void OpRun();
  void OpDo();






private:
	mafVMEOsteometricBoard *m_osteometricBoard;

};
#endif
