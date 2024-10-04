#include "ftk/VME/VMEFactory.h"

#include "ftk/Core/NodeFactory.h"
#include "mafNodeLayout.h"
#include "mafVMEExternalData.h"
#include "mafVMEGeneric.h"
#include "mafVMEGizmo.h"
#include "mafVMEGroup.h"
#include "mafVMEImage.h"
#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEMeter.h"
#include "mafVMEInfoText.h"
#include "mafVMEMesh.h"
#include "mafVMEPointSet.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMEProber.h"
#include "mafVMERefSys.h"
#include "mafVMEEllipsoid.h"
#include "mafVMEQuadricSurfaceFitting.h"
#include "mafVMEHyperboloid.h"
#include "mafVMECylinder.h"
#include "mafVMELineSeg.h"
#include "mafVMEMuscleWrapping.h"
#include "mafVMEHyperboloid2S.h"
#include "mafVMEPlane.h"
#include "mafVMEGravityLine.h"
#include "mafVMECenterLine.h"
#include "mafVMEOsteometricBoard.h"
#include "mafVMERoot.h"

#ifdef MAF_USE_ITK
  #include "mafVMEScalarMatrix.h"
  #include "mafVMEItemScalarMatrix.h"
#endif

#include "mafVMEScalar.h"
#include "mafVMESlicer.h"
#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafQuadraticSurface.hpp"
#include "mafVMEVector.h"
#include "mafVMEVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"

#include <string>

#include "medVMEWrappedMeter.h"
#include "medVMEPolylineGraph.h"
#include "medVMEPolylineEditor.h"
#include "medVMEMaps.h"

#include "medVMESurfaceEditor.h"
#include "medVMELabeledVolume.h"
#include "medVMESegmentationVolume.h"

#ifdef MAF_USE_ITK
#include "mafVMERawMotionData.h"
#include "medVMEAnalog.h"
#include "medVMEStent.h"
#endif

#include "mafVMEVolumeLarge.h"


int VMEFactory::Initialize()
{
  NodeFactory::Initialize();
  
  mafPlugNodeMacro(mafVMERoot,"root for VME tree");
  mafPlugNodeMacro(mafVMEExternalData,"VME used for referencing MAF external data.");
  mafPlugNodeMacro(mafVMEGroup,"VME used for composing assemblies");
  mafPlugNodeMacro(mafVMEGeneric,"Generic VME storing VTK datasets");
  mafPlugNodeMacro(mafVMEImage,"Generic VME storing vtkImageData datasets");
  mafPlugNodeMacro(mafVMEMesh,"Generic VME storing vtkUnstructuredGrid datasets");
  mafPlugNodeMacro(mafVMESurface,"Generic VME storing vtkPolyData surfaces datasets");
  mafPlugNodeMacro(mafVMESurfaceParametric,"Generic VME storing vtkPolyData parametric surfaces");
  mafPlugNodeMacro(mafVMEVolumeGray,"Generic VME storing vtk volume datasets with one scalar component");
  mafPlugNodeMacro(mafVMEVolumeRGB,"Generic VME storing vtk volume datasets with 3 RGB scalar components");
  mafPlugNodeMacro(mafVMEPolyline,"Generic VME storing polyline datasets");
  mafPlugNodeMacro(mafVMEPolylineSpline,"Generic VME outputting a spline");
  mafPlugNodeMacro(mafVMEMeter,"Generic VME generating meter");
  mafPlugNodeMacro(mafVMEInfoText,"Generic VME for text information in gui");
  mafPlugNodeMacro(mafVMELandmark,"VME representing a marker");
  mafPlugNodeMacro(mafVMELandmarkCloud,"VME representing a cloud of mafVMELandmark");
  mafPlugNodeMacro(mafVMEProber,"VME representing a prober");
  //mafPlugNodeMacro(mafVMEGizmo,"VME representing a tool");
  mafPlugNodeMacro(mafVMEPointSet,"VME representing a set of points");
  mafPlugNodeMacro(mafVMERefSys,"VME representing a reference system");
  mafPlugNodeMacro(mafVMEScalar,"VME representing time varying scalar values");
  mafPlugNodeMacro(mafVMEEllipsoid, "VME representing Ellipsoid");
  mafPlugNodeMacro(mafVMEQuadricSurfaceFitting, "VME representing quadric fitting");
  mafPlugNodeMacro(mafVMECylinder, "VME representing Cylinder");
  mafPlugNodeMacro(mafVMEHyperboloid, "VME representing Hyperboloid");
  mafPlugNodeMacro(mafVMEPlane, "VME representing Plane");
  mafPlugNodeMacro(mafVMEOsteometricBoard, "VME representing mafVMEOsteometricBoard");
  mafPlugNodeMacro(mafVMEGravityLine, "VME representing GravityLine");
  mafPlugNodeMacro(mafVMEHyperboloid2S, "VME representing Hyperboloid2S");
  mafPlugNodeMacro(mafVMELineSeg, "VME representing LineSeg");
  mafPlugNodeMacro(mafVMEMuscleWrapping, "VME representing Muscle Wrapping");
  mafPlugNodeMacro(mafVMECenterLine, "VME representing CenterLine");
#ifdef MAF_USE_ITK
  mafPlugNodeMacro(mafVMEScalarMatrix,"VME representing time varying scalar values");
#endif
  mafPlugNodeMacro(mafVMESlicer,"VME representing a slice of a volume");
  mafPlugNodeMacro(mafVMEVector,"VME representing aa applyed vector");

	mafPlugNodeMacro(medVMEWrappedMeter, "Generalized VME Meter with wrapping geometry");
  mafPlugNodeMacro(medVMEPolylineGraph, "VME for Graph and Polyline");
  mafPlugNodeMacro(medVMEPolylineEditor, "VME for Editing Graph and Polyline");
  mafPlugNodeMacro(medVMESurfaceEditor, "VME for Editing Surface");
  mafPlugNodeMacro(medVMELabeledVolume, "VME representing a label put on a volume");
  mafPlugNodeMacro(medVMEMaps, "VME representing density-distace surface scalars");
  mafPlugNodeMacro(medVMESegmentationVolume, "VME for Segmented Volume");

#ifdef MAF_USE_ITK
  mafPlugNodeMacro(mafVMERawMotionData, "VME that is a group for RawMotionData");
  mafPlugNodeMacro(medVMEAnalog, "VME rapresenting EMG scalar data");
  mafPlugNodeMacro(medVMEStent, "VME representing stent structure");
#endif
  
  //TODO: to be committed down
  mafPlugNodeMacro(mafVMEVolumeLarge, "VME storing large volume datasets with one scalar component");

	return MAF_OK;
}
