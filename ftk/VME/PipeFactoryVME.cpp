#include "ftk/VME/PipeFactoryVME.h"

#include "ftk/Core/PipeFactory.h"
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


#include "medPipeVolumeMIP.h"
#include "medPipeVolumeDRR.h"
#include "medPipeVolumeVR.h"
#include "medPipeTrajectories.h"

#include "medPipeWrappedMeter.h"
#include "medPipeSurfaceEditor.h"
#include "medVisualPipeSlicerSlice.h"
#include "medVisualPipePolylineGraph.h"



#include "medPipeDensityDistance.h"

#ifdef MAF_USE_ITK  
#include "medPipeGraph.h"
#endif

#include "medPipePolylineGraphEditor.h"

//TODO: commit down to openMAF
#include "mafPipeMeshSlice_BES.h"
#include "mafPipePolylineSlice_BES.h"
#include "mafPipeSurfaceSlice_BES.h"
#include "mafPipeVolumeSlice_BES.h"
///////

#include "medPipeCompoundVolume.h"
#include "medPipeCompoundVolumeFixedScalars.h"
#include "medPipeVectorFieldGlyphs.h"
#include "medPipeVectorFieldSurface.h"
#include "medPipeVectorFieldMapWithArrows.h"
#include "medPipeVectorFieldSlice.h"
#include "medPipeTensorFieldGlyphs.h"
#include "medPipeTensorFieldSurface.h"
#include "medPipeTensorFieldSlice.h"
#include "medPipeVolumeSliceBlend.h"
#include "medVisualPipeCollisionDetection.h"
#include "medPipeVolumeSliceNotInterpolated.h"

#include "medPipeRayCast.h"

int mafPipeFactoryVME::Initialize()
{
  PipeFactory::Initialize();

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

  //
  // Plug here Pipes in this factory
  //
  mafPlugPipeMacro(medPipeVolumeMIP, "Pipe for render vtk volumes with MIP ray cast method.");
  mafPlugPipeMacro(medPipeVolumeDRR, "Pipe for render vtk volumes with XRay cast method.");

  mafPlugPipeMacro(medPipeVolumeVR, "Pipe for render vtk volumes with Volume Rendere cast method.");

  mafPlugPipeMacro(medPipeDensityDistance, "Pipe for visualize the value of scalars on a surface.");

  mafPlugPipeMacro(medPipeTrajectories, "Pipe to render animated Landmark trajectories in a time interval.");
  mafPlugPipeMacro(medPipePolylineGraphEditor, "Pipe to Visualize Polyline/Graph in way to edit them.");

  mafPlugPipeMacro(medPipeWrappedMeter, "Pipe to Visualize Wrapped Meter");
  mafPlugPipeMacro(medPipeSurfaceEditor, "Pipe to Visualize Surface in way to edit them.");

  mafPlugPipeMacro(medVisualPipeSlicerSlice, "Pipe to Visualize Slicer as borders cutted from a plane.");

  mafPlugPipeMacro(medVisualPipePolylineGraph, "Pipe to Visualize Polyline and Graph.");


#ifdef MAF_USE_ITK  

  mafPlugPipeMacro(medPipeGraph, "Pipe to plot scalar graphics.");

#endif

  mafPlugPipeMacro(medPipeCompoundVolume, "Compound pipe for rendering volumes.");
  mafPlugPipeMacro(medPipeCompoundVolumeIsosurface, "Compound Pipe for render vtk volumes as a iso-surface");
  mafPlugPipeMacro(medPipeCompoundVolumeMIP, "Compound pipe for render vtk volumes with MIP ray cast method.");
  mafPlugPipeMacro(medPipeCompoundVolumeDRR, "Compound pipe for render vtk volumes with XRay cast method.");
  mafPlugPipeMacro(medPipeCompoundVolumeVR, "Compound pipe for render vtk volumes with Volume Rendere cast method.");
  mafPlugPipeMacro(medPipeVectorFieldGlyphs, "Pipe for rendering of vector fields using various glyphs.");
  mafPlugPipeMacro(medPipeVectorFieldSurface, "Pipe for rendering of vector fields using color mapping on the object surface.");
  mafPlugPipeMacro(medPipeVectorFieldMapWithArrows, "Pipe for rendering of vector and scalar fields using colored arrows mapping on the object surface.");
  mafPlugPipeMacro(medPipeVectorFieldSlice, "Pipe for rendering of vector fields using color mapping on the slice of the object.");
  mafPlugPipeMacro(medPipeTensorFieldGlyphs, "Pipe for rendering of tensor fields using glyphs.");
  mafPlugPipeMacro(medPipeTensorFieldSurface, "Pipe for rendering of tensor fields using color mapping on the object surface.");
  mafPlugPipeMacro(medPipeTensorFieldSlice, "Pipe for rendering of tensor fields using color mapping on the slice of the object.");
  mafPlugPipeMacro(medPipeVolumeSliceBlend, "Pipe for rendering volume with 2 slices with opacity.");
  mafPlugPipeMacro(medVisualPipeCollisionDetection, "Pipe for visualization of collision between 2 surfaces.");


  //BES: 16.4.2008 - these pipes are to be committed down (without _BES suffix) to openMAF in the future
  mafPlugPipeMacro(mafPipeVolumeSlice_BES, "BES: mafPipeVolumeSlice_BES.");
  mafPlugPipeMacro(mafPipeSurfaceSlice_BES, "BES: mafPipeSurfaceSlice.");
  mafPlugPipeMacro(mafPipePolylineSlice_BES, "BES: mafPipePolylineSlice_BES.");
  mafPlugPipeMacro(mafPipeMeshSlice_BES, "BES: mafPipeMeshSlice_BES.");

  mafPlugPipeMacro(medPipeVolumeSliceNotInterpolated, "Pipe for not interpolated and not resampled volume visualization");

  mafPlugPipeMacro(medPipeRayCast, "Pipe for RayCast Volume rendering of bone-blood-muscle");

	return MAF_OK;
}
