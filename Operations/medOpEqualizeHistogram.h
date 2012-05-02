/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpEqualizeHistogram.h,v $
Language:  C++
Date:      $Date: 2012-04-06 09:09:53 $
Version:   $Revision: 1.1.2.4 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medOpEqualizeHistogram_H__
#define __medOpEqualizeHistogram_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "medOperationsDefines.h"
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
  medOpEqualizeHistogram(const wxString &label = "Equalize Histogram");
  /** destructor. */
  ~medOpEqualizeHistogram(); 

  /** Precess events coming from other objects */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

  /** RTTI macro */
  mafTypeMacro(medOpEqualizeHistogram, mafOp);

  /*virtual*/ mafOp* Copy();

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode *node);

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

protected:

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);

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
