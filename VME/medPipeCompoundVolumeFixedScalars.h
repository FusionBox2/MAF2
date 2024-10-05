/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeCompoundVolumeFixedScalars.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:01:46 $ 
  Version: $Revision: 1.1.2.5 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeCompoundVolumeFixedScalars_h__
#define medPipeCompoundVolumeFixedScalars_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "medPipeCompoundVolume.h"

/** Scalar visual pipe is fixed and cannot be changed. Vector visual pipe
and tensor visual pipe can be changed but only one change is available.
This class serves for a backward compatibility. */
class MED_VME_EXPORT medPipeCompoundVolumeFixedScalars : public medPipeCompoundVolume
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeCompoundVolumeFixedScalars, medPipeCompoundVolume);

public:
  /** constructor */
  medPipeCompoundVolumeFixedScalars() {
  }

  /** destructor */
  ~medPipeCompoundVolumeFixedScalars () override
  {
  }

public:
  /** Returns the currently constructed scalar visual pipe. */
  virtual std::shared_ptr<mafPipe> GetCurrentScalarVisualPipe();

protected:  
  /** Called from CreatePageGroups to create groups for scalars. */
  /*virtual*/ bool CreateScalarPageGroups() override;

  /** Called from CreatePageGroups to create groups for vectors. */
  /*virtual*/ bool CreateVectorPageGroups() override;

  /** Called from CreatePageGroups to create groups for tensors. */
  /*virtual*/ bool CreateTensorPageGroups() override;  

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultScalarVisualPipe() {
    return NULL;  //no visual pipe available
  }

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultVectorVisualPipe() {
    return "medPipeVectorFieldGlyphs";  //no visual pipe available
  }

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultTensorVisualPipe() {
    return NULL;  //no visual pipe available
  }
};

/** Override for Isosurface */
class MED_VME_EXPORT medPipeCompoundVolumeIsosurface : public medPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeCompoundVolumeIsosurface, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() override
  {
    return "mafPipeIsosurface";  //no visual pipe available
  }
};

/** Override for MIP */
class MED_VME_EXPORT medPipeCompoundVolumeDRR : public medPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeCompoundVolumeDRR, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() override
  {
    return "medPipeVolumeDRR";  //no visual pipe available
  }
};

/** Override for MIP */
class MED_VME_EXPORT medPipeCompoundVolumeMIP : public medPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeCompoundVolumeMIP, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() override
  {
    return "medPipeVolumeMIP";  //no visual pipe available
  }
};

/** Override for VR */
class MED_VME_EXPORT medPipeCompoundVolumeVR : public medPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeCompoundVolumeVR, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() override
  {
    return "medPipeVolumeVR";  //no visual pipe available
  }
};

#endif // medPipeCompoundVolumeFixedScalars_h__