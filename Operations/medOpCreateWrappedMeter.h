#pragma once

#include "mafOp.h"

/** 
  class name: medOpCreateWrappedMeter
  Operation that create a medVMEWrappedMeter for wrap muscle action lines.
*/
class MED_OPERATION_EXPORT medOpCreateWrappedMeter: public mafOp
{
public:
  /** constructor */
  medOpCreateWrappedMeter(const mafString& label = _R("CreateWrappedMeter"));
  /** destructor */
  ~medOpCreateWrappedMeter() override; 

  /** RTTI macro*/
  mafTypeMacroN(medOpCreateWrappedMeter);

  /** clone the object and retrieve a copy*/
  mafOp* Copy() override;

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;
  /** Builds operation's interface. */
  void OpRun() override;
};
