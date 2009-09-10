/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVMEFactory.cpp,v $
  Language:  C++
  Date:      $Date: 2009-09-10 08:38:05 $
  Version:   $Revision: 1.13.2.3 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medVMEFactory.h"

#include "mafVersion.h"

#include "medVMEWrappedMeter.h"
#include "medVMEPolylineGraph.h"
#include "medVMEPolylineEditor.h"
#include "medVMEMaps.h"

#include "medVMESurfaceEditor.h"
#include "medVMELabeledVolume.h"

#ifdef MAF_USE_ITK
  #include "mafVMERawMotionData.h"
  #include "medVMEAnalog.h"
#endif

#include "mafVMEVolumeLarge.h"

#include "mafIndent.h"
#include <string>
#include <ostream>

medVMEFactory *medVMEFactory::m_Instance=NULL;

mafCxxTypeMacro(medVMEFactory);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int medVMEFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance = medVMEFactory::New();

    if (m_Instance)
    {
      m_Instance->RegisterFactory(m_Instance);
      return MAF_OK;  
    }
    else
    {
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//------------------------------------------------------------------------
medVMEFactory::medVMEFactory()
//------------------------------------------------------------------------------
{
  //
  // Plug here Nodes in this factory
  //

#ifdef MAF_USE_ITK
  
#endif
  //mafPlugObjectMacro(mmaObject,"Object attributes");

  mafPlugNodeMacro(medVMEWrappedMeter,"Generalized VME Meter with wrapping geometry");
  mafPlugNodeMacro(medVMEPolylineGraph,"VME for Graph and Polyline");
	mafPlugNodeMacro(medVMEPolylineEditor,"VME for Editing Graph and Polyline");
 	mafPlugNodeMacro(medVMESurfaceEditor,"VME for Editing Surface");
  mafPlugNodeMacro(medVMELabeledVolume,"VME representing a label put on a volume");
  mafPlugNodeMacro(medVMEMaps, "VME representing density-distace surface scalars");

#ifdef MAF_USE_ITK
  mafPlugNodeMacro(mafVMERawMotionData,"VME that is a group for RawMotionData");
  mafPlugNodeMacro(medVMEAnalog,"VME rapresenting EMG scalar data");
#endif
  //mafPlugNodeMacro(mafVMEthing,"VME representing a thing");
  
  //TODO: to be committed down
  mafPlugNodeMacro(mafVMEVolumeLarge, "VME storing large volume datasets with one scalar component");
}

//------------------------------------------------------------------------------
const char* medVMEFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for Medical VMEs";
}

//------------------------------------------------------------------------------
mafVME *medVMEFactory::CreateVMEInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafVME::SafeDownCast(Superclass::CreateInstance(type_name));
}
