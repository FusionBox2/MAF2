#pragma once

#include "ftkConfigure.h"

#include <vtkSmartPointer.h>

class mafVME;
class vtkRenderer;
class vtkPolyDataMapper2D;
class vtkCoordinate;
class vtkActor2D;
class vtkLookupTable;
class vtkMatrix4x4;
class vtkPolyData;
class vtkMAFAnnotatedCubeActor;
class vtkMAFOrientationMarkerWidget;
class vtkMAFGlobalAxesHeadActor;

class mafAxes
	//----------------------------------------------------------------------------
	/**
	Create 3D axes representation and insert them in the passed renderer: axes can be global (World Coordinates
	reference system )or local ( VME reference system ).

	Axes representation can be:
	TRIAD: Common 3D refsys featuring 3 orthogonal axes
	CUBE: A rotating cube featuring orientation feedback letters on cube faces
	HEAD: A rotating head

	Some API use cases follows.

	mafAxes(renderer, NULL, TRIAD); => Create a global axis triad 2D actor in the passed renderer
	mafAxes(renderer, vme, TRIAD); => Create a vme local axis triad 2D actor in the passed renderer

	mafAxes(renderer, NULL, CUBE); => Create a 3D global axes cube on a new renderer and on a superimposed layer
	//mafAxes(renderer, vme, CUBE); => BEWARE !!! NOT SUPPORTED !!!

	mafAxes(renderer, NULL, HEAD); => Create a 3D global axes head on a new renderer and on a superimposed layer
	mafAxes(renderer, vme, HEAD); => Create a 3D local axes head on a new renderer and on a superimposed layer

	If a vme is provided, the axes will represent
	the local vme-reference system.

	If vme is NULL, the axes will represent the global
	reference system

	@sa mafAxesTest for example usage

	@todo API improvements needed, see mafAxesTest for api improvement use cases
	*/
{
public:

	enum AXIS_TYPE_ENUM
	{
		TRIAD = 0, // default: 2D actor representing 3 small axes attached to the main render window camera
		CUBE, // 3D actor representing a rotating cube with anatomical directions annotations attached to the main render window camera
		HEAD, // 3D actor representing a rotating head attached to the main render window camera
		NUMBER_OF_AXES_TYPES,
	};

	mafAxes(mafVME* vme);

	/** Constructor note: given renderer ren cannot be NULL */
	mafAxes(vtkRenderer* ren, int axesType);
	virtual ~mafAxes();

	/** Set the axes visibility. */
	void     SetVisibility(bool show);

	/** Force the position of the Axes, if omitted the ABS vme matrix is used instead.
	Works only if a vme was specified in the constructor*/
	void     SetPose(vtkMatrix4x4* abs_pose_matrix = nullptr);

protected:

	mafVME* m_Vme = nullptr;
	vtkRenderer* m_Renderer = nullptr;
	int m_AxesType = TRIAD;

	vtkSmartPointer<vtkCoordinate> m_Coord;
	vtkSmartPointer<vtkActor2D> m_AxesActor2D;
	vtkSmartPointer<vtkMAFAnnotatedCubeActor> m_AnnotatedCubeActor;
	vtkSmartPointer<vtkMAFOrientationMarkerWidget> m_OrientationMarkerWidget;
};
