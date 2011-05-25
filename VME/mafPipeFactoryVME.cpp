/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeFactoryVME.cpp,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:52:18 $
  Version:   $Revision: 1.20.2.3 $
  Authors:   Marco Petrone
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


#include "mafPipeFactoryVME.h"
#include "mafVersion.h"
#include "mafIndent.h"

#include "mafPipe.h"
#include "mafPipeSurface.h"
#include "mafPipeSurfaceSlice.h"
#include "mafPipePolylineSlice.h"
#include "mafPipeSurfaceTextured.h"

#include "mafPipeVolumeSlice.h"
#include "mafPipeVolumeProjected.h"
#include "mafPipeMeter.h"
#include "mafPipeGizmo.h"
#include "mafPipeLandmarkCloud.h"
#include "mafPipePointSet.h"
#include "mafPipePolyline.h"
#include "mafPipeIsosurface.h"
#ifdef WIN32
#include "mafPipeIsosurfaceGPU.h"
#endif
#include "mafPipeImage3D.h"
#include "mafPipeVector.h"
#include "mafPipeMesh.h"
#include "mafPipeMeshSlice.h"
#include "mafPipeScalar.h"
#include "mafVisualPipeVolumeRayCasting.h"

#ifdef MAF_USE_ITK
  #include "mafPipeScalarMatrix.h"
#endif

#include <string>
#include <ostream>

//mafPipeFactoryVME *mafPipeFactoryVME::m_Instance=NULL;

bool mafPipeFactoryVME::m_Initialized=false;

mafCxxTypeMacro(mafPipeFactoryVME);

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafPipeFactoryVME::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    //m_Instance=mafPipeFactoryVME::New();

    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return MAF_OK;  
    }
    else
    {
      m_Initialized=false;
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//------------------------------------------------------------------------
mafPipeFactoryVME::mafPipeFactoryVME()
//------------------------------------------------------------------------------
{
  //m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  mafPlugPipeMacro(mafPipeSurface,"Pipe to render vtk polydata as surface rendering");
  mafPlugPipeMacro(mafPipeSurfaceSlice,"Pipe for render vtk surface as an arbitrary slice");
  mafPlugPipeMacro(mafPipeSurfaceTextured,"Pipe for render vtk polydata as textured surface rendering");
	mafPlugPipeMacro(mafPipePolylineSlice,"Pipe for render vtk polyline as an arbitrary slice");
  mafPlugPipeMacro(mafPipeGizmo,"Pipe to render gizmos");
  mafPlugPipeMacro(mafPipeMeter,"Pipe for mafVMEMeter");
  mafPlugPipeMacro(mafPipeVolumeSlice,"Pipe for render vtk volumes as an arbitrary slice");
  mafPlugPipeMacro(mafPipeVolumeProjected,"Pipe for render vtk volumes as a projected image along X,Y or Z axes");
  mafPlugPipeMacro(mafPipePointSet,"Pipe for render vtk pointset");
  mafPlugPipeMacro(mafPipePolyline,"Pipe for render polylines");
  mafPlugPipeMacro(mafPipeVector,"Pipe to render vectors ");
  mafPlugPipeMacro(mafPipeLandmarkCloud,"Pipe for render landmark clouds");
  mafPlugPipeMacro(mafPipeIsosurface,"Pipe for render vtk volumes as a iso-surface");
#ifdef WIN32
  mafPlugPipeMacro(mafPipeIsosurfaceGPU,"Pipe for render vtk volumes as a iso-surface through a GPU");
#endif
  mafPlugPipeMacro(mafPipeImage3D,"Pipe for render images");
	mafPlugPipeMacro(mafPipeMesh,"Pipe for render Mesh");
	mafPlugPipeMacro(mafPipeMeshSlice,"Pipe for render a slice of a Mesh");
  mafPlugPipeMacro(mafPipeScalar,"Pipe for render scalar data");
  mafPlugPipeMacro(mafVisualPipeVolumeRayCasting,"Pipe for render volume data with Ray Cast method.");

#ifdef MAF_USE_ITK
  mafPlugPipeMacro(mafPipeScalarMatrix,"Pipe for render matrix scalar data");
#endif
}

//------------------------------------------------------------------------------
const char* mafPipeFactoryVME::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafPipeFactoryVME::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF Pipes of VME library";
}
//------------------------------------------------------------------------------
mafPipeFactoryVME* mafPipeFactoryVME::GetInstance()
//------------------------------------------------------------------------------
{
  static mafPipeFactoryVME &istance = *(mafPipeFactoryVME::New());
  Initialize();
  return &istance;
}
