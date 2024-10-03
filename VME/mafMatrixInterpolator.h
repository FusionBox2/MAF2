/*=========================================================================

 Program: MAF2
 Module: mafMatrixInterpolator
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafMatrixInterpolator_h
#define __mafMatrixInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafMatrixPipe.h"
#include "ftk/Base/RegisteringPointer.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafMatrix;

/** Class Name: mafMatrixInterpolator.
  mafMatrixInterpolator is used for computing the position of VME in terms 
  of matrix that is an element of vector of items.
  mafMatrixInterpolator is a matrix pipe producing as output a matrix interpolated
  against the ones stored in the array associated to the VME itself (only for VMEs
  with matrix array like mafVMEGenericAbstract). 

*/
class MAF_EXPORT mafMatrixInterpolator:public mafMatrixPipe
{
public:
  /** type macro for RTTI and instance creation.*/
  mafTypeMacro(mafMatrixInterpolator,mafMatrixPipe);

  /** This matrix pipe accepts only VME's with internal matrix Array. */
  bool Accept(mafVME *vme) override;

  /**  Get the output of the interpolator item. */
  std::shared_ptr<mafMatrix> GetCurrentItem() {return m_CurrentItem;}

  /**
  Set the current time. Overidden to allow the output not to change when 
  not necessary, e.g. constant pose (in time) being interpolated should not
  produce a change in the output data when time is changed.*/
  void SetTimeStamp(mafTimeStamp time) override;

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  MTimeType GetMTime() override;

  /** Force update of the pipe.*/
  void Update() override;

protected:
  /** constructor.*/
  mafMatrixInterpolator();
  /** destructor.*/
  ~mafMatrixInterpolator() override;

  //virtual void PreExecute(){};
  //virtual void Execute(){};

  /**
  This function is called internally to updated the output data according
  interpolator rules. It should be reimplemented in sub-classes.*/
  virtual void InternalItemUpdate();

  /** update the output matrix. */
  void InternalUpdate() override;

  /** Set the current matrix of the pipe. */
  void SetCurrentItem(std::shared_ptr<mafMatrix>data);

  /** request the update of the current matrix of the pipe. */
  void UpdateCurrentItem(std::shared_ptr<mafMatrix>item);

  std::shared_ptr<mafMatrix>    m_CurrentItem; ///< the item currently selected for the current time

  std::shared_ptr<mafMatrix>    m_OldItem;     ///< the item previously selected

  mafTimeStamp  m_OldTimeStamp; ///< previous time
private:
  /** copy constructor not implemented. */
  mafMatrixInterpolator(const mafMatrixInterpolator&);
  /** assignment operator not implemeted. */
  void operator=(const mafMatrixInterpolator&); // Not implemented
  
};

#endif /* __mafMatrixInterpolator_h */
 
