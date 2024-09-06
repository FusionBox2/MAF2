/*=========================================================================

 Program: MAF2Medical
 Module: medOpEqualizeHistogram
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpEqualizeHistogram_H__
#define __medOpEqualizeHistogram_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVolumeGray;
class mafGUIDialog;
class mafGUIHistogramWidget;

/** 
class name : medOpEqualizeHistogram
*/
class MED_OPERATION_EXPORT medOpEqualizeHistogram: public mafOp
{
public:

  /** constructor. */
  medOpEqualizeHistogram(const mafString& label = _R("Equalize Histogram"));
  /** destructor. */
  ~medOpEqualizeHistogram() override; 

  /** Precess events coming from other objects */
  /*virtual*/ void OnEvent(mafEventBase *maf_event) override;

  /** RTTI macro */
  mafTypeMacro(medOpEqualizeHistogram, mafOp);

  /*virtual*/ mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode *node) override;

  /** Builds operation's interface. */
  /*virtual*/ void OpRun() override;

  /** Execute the operation. */
  /*virtual*/ void OpDo() override;

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo() override;

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result) override;

  /** Perform the equalization of the histogram */
  void Algorithm();

  /** Create the operation gui */
  void CreateGui();

  void CreateHistogramDialog();
  void DeleteHistogramDialog();

  double m_Alpha;
  double m_Beta;
  int m_Radius[3];

  mafVMEVolumeGray *m_VolumeInput;
  mafVMEVolumeGray *m_VolumeOutput;

  mafGUIDialog *m_Dialog;
  mafGUIHistogramWidget *m_Histogram;


};
#endif
