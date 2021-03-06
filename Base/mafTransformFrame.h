/*=========================================================================

 Program: MAF2
 Module: mafTransformFrame
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafTransformFrame_h
#define __mafTransformFrame_h

#include "mafTransformBase.h"

class vtkMatrix4x4;

/** mafTransformFrame - class for multi frame homogeneous transformations.
  mafTransformFrame provides functionalities for converting homogeneous transformations,
  between different frames.
  The idea of this class is to represent by itself a transformation, obtained by changing
  the an input transformation for its original coordinate system to a target coordinate 
  system. Coordinate systems are by default the world coords system. Reference systems can 
  be expressed as a mafTransformBase.

  @sa mafTransformBase mafTransform
 
  @todo
  - Modifica GetMTime per considerare anche l'MTime di InputFrame e TargetFrame
 */
class MAF_EXPORT mafTransformFrame : public mafTransformBase
{
 public:
   mafTransformFrame();
  ~mafTransformFrame();
	
  mafTypeMacro(mafTransformFrame,mafTransformBase);

  //virtual void Print(std::ostream& os, const int tabs=0) const;

  /** set the matrix to be transformed */
  void SetInput(mafTransformBase *frame);
  void SetInput(mafMatrix *frame);
  mafTransformBase *GetInput() {return m_Input;}

  /**
  Set/Get the input reference system, i.e. the reference system of the 
  input matrix.*/
  void SetInputFrame(mafMatrix *frame);
  void SetInputFrame(mafTransformBase *frame);
  mafTransformBase *GetInputFrame() {return m_InputFrame;}

  /**
  Set/Get the output reference system, i.e. the reference system of the output
  matrix or the target reference system for point transformation.*/
  void SetTargetFrame(mafMatrix *frame);
  void SetTargetFrame(mafTransformBase *frame);
  mafTransformBase *GetTargetFrame() {return m_TargetFrame;}

  /** 
    Return current modification time, taking inro consideration also
    Input, InputFrame and TargetFrame. */
  virtual unsigned long GetMTime();

protected:
  void InternalUpdate();

  mafTransformBase  *m_Input;
  mafTransformBase  *m_InputFrame;
  mafTransformBase  *m_TargetFrame;

private:
  mafTransformFrame(const mafTransformFrame&);  // Not implemented.
  void operator=(const mafTransformFrame&);  // Not implemented.
};

#endif
