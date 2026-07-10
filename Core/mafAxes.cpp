
#include "mafAxes.h"

#include "mafVME.h"
#include "mafAbsMatrixPipe.h"

#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkViewport.h"
#include "vtkCamera.h"
#include "vtkAxes.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"
#include "vtkCoordinate.h"
#include "vtkMAFLocalAxisCoordinate.h"
#include "vtkMAFGlobalAxisCoordinate.h"
#include "vtkRenderWindow.h"
#include "vtkMAFOrientationMarkerWidget.h"
#include "vtkMAFAnnotatedCubeActor.h"
#include "vtkMAFGlobalAxesHeadActor.h"

mafAxes::mafAxes(mafVME* vme)
{
	m_AxesType = TRIAD;
	m_Vme = vme;

	vtkNew<vtkAxes> triadAxes;
	triadAxes->SetScaleFactor(1);

	if (m_Vme)
	{
		m_Vme->GetOutput()->Update();
		vtkNew<vtkMAFLocalAxisCoordinate> localAxisCoord;
		m_Coord = localAxisCoord;
		localAxisCoord->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
		localAxisCoord->SetDataSet(m_Vme->GetOutput()->GetVTKData());
	}
	else
	{
		m_Coord = vtkNew<vtkMAFGlobalAxisCoordinate>();
	}

	vtkNew<vtkLookupTable> axesLUT;
	axesLUT->SetNumberOfTableValues(3);
	axesLUT->SetTableValue(0, 1, 0, 0, 1);
	axesLUT->SetTableValue(1, 0, 1, 0, 1);
	axesLUT->SetTableValue(2, 0, 0, 1, 1);

	vtkNew<vtkPolyDataMapper2D> axesMapper2D;
	axesMapper2D->SetInputConnection(triadAxes->GetOutputPort());
	axesMapper2D->SetScalarModeToUsePointData();

	axesMapper2D->SetTransformCoordinate(m_Coord);
	axesMapper2D->SetLookupTable(axesLUT);

	axesMapper2D->SetScalarRange(0, 0.5);
	axesMapper2D->ScalarVisibilityOn();

	m_AxesActor2D = vtkNew<vtkActor2D>();
	m_AxesActor2D->SetMapper(axesMapper2D);
	m_AxesActor2D->GetProperty()->SetLineWidth(2);
	m_AxesActor2D->VisibilityOff();
	m_AxesActor2D->PickableOff();
}

mafAxes::mafAxes(vtkRenderer* ren, int axesType)
{
	m_AxesType = axesType;

	m_Renderer = ren;

	assert(m_Renderer);

	if (m_AxesType == TRIAD)
	{
		vtkNew<vtkAxes> triadAxes;
		triadAxes->SetScaleFactor(1);

		if (m_Vme)
		{
			m_Vme->GetOutput()->Update();
			vtkNew<vtkMAFLocalAxisCoordinate> localAxisCoord;
			m_Coord = localAxisCoord;
			localAxisCoord->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
			localAxisCoord->SetDataSet(m_Vme->GetOutput()->GetVTKData());
		}
		else
		{
			m_Coord = vtkNew<vtkMAFGlobalAxisCoordinate>();
		}

		vtkNew<vtkLookupTable> axesLUT;
		axesLUT->SetNumberOfTableValues(3);
		axesLUT->SetTableValue(0, 1, 0, 0, 1);
		axesLUT->SetTableValue(1, 0, 1, 0, 1);
		axesLUT->SetTableValue(2, 0, 0, 1, 1);

		vtkNew<vtkPolyDataMapper2D> axesMapper2D;
		axesMapper2D->SetInputConnection(triadAxes->GetOutputPort());
		axesMapper2D->SetScalarModeToUsePointData();

		axesMapper2D->SetTransformCoordinate(m_Coord);
		axesMapper2D->SetLookupTable(axesLUT);

		axesMapper2D->SetScalarRange(0, 0.5);
		axesMapper2D->ScalarVisibilityOn();

		m_AxesActor2D = vtkNew<vtkActor2D>();
		m_AxesActor2D->SetMapper(axesMapper2D);
		m_AxesActor2D->GetProperty()->SetLineWidth(2);
		m_AxesActor2D->VisibilityOff();
		m_AxesActor2D->PickableOff();
		m_Renderer->AddViewProp(m_AxesActor2D);
	}
	else if (m_AxesType == CUBE)
	{
		m_OrientationMarkerWidget = vtkMAFOrientationMarkerWidget::New();
		m_AnnotatedCubeActor = vtkMAFAnnotatedCubeActor::New();

		m_AnnotatedCubeActor->SetFaceTextScale(0.5);
		m_OrientationMarkerWidget->SetOrientationMarker(m_AnnotatedCubeActor);
		m_OrientationMarkerWidget->SetInteractor(m_Renderer->GetRenderWindow()->GetInteractor());
		m_OrientationMarkerWidget->SetEnabled(1);
		m_OrientationMarkerWidget->SetInteractive(0);
		m_OrientationMarkerWidget->SetViewport(0., 0., 0.4, 0.4);
	}
	else if (m_AxesType == HEAD)
	{
		vtkNew<vtkMAFGlobalAxesHeadActor> globalAxesHeadActor ;
		m_OrientationMarkerWidget = vtkNew<vtkMAFOrientationMarkerWidget>();
		if (m_Vme)
		{
			globalAxesHeadActor->SetInitialPose(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
		}

		wxString headABSFileName = globalAxesHeadActor->GetHeadABSFileName().c_str();

		wxString cwd = wxGetCwd().c_str();
		std::ostringstream stringStream;
		stringStream << "Head ABS file name: " << headABSFileName.c_str() << std::endl;

		mafLogMessage(_M(stringStream.str().c_str()));
		bool exists = wxFileExists(headABSFileName.c_str());
		assert(exists);

		if (exists)
		{
			// continue
		}
		else
		{
			wxString tmp = "Head VTK file not found: ";
			tmp.Append(headABSFileName.c_str());
			tmp.Append(" cannot create 3d head marker");
			wxMessageBox(tmp);
			return;
		}

		m_OrientationMarkerWidget->SetOrientationMarker(globalAxesHeadActor);
		m_OrientationMarkerWidget->SetInteractor(m_Renderer->GetRenderWindow()->GetInteractor());
		m_OrientationMarkerWidget->SetEnabled(1);
		m_OrientationMarkerWidget->SetInteractive(0);
		m_OrientationMarkerWidget->SetViewport(0.75, 0., 1, 0.25);
	}

}

mafAxes::~mafAxes()
{
	if (m_OrientationMarkerWidget != nullptr)
	{
		m_OrientationMarkerWidget->SetInteractor(nullptr);
	}
	if (m_Renderer)
	{
		m_Renderer->RemoveViewProp(m_AxesActor2D);
	}
}

void mafAxes::SetVisibility(bool show)
{
	if (m_AxesType == TRIAD)
	{
		m_AxesActor2D->SetVisibility(show);
	}
	else if (m_AxesType == CUBE)
	{
		m_AnnotatedCubeActor->SetVisibility(show);
	}
}

/*void mafAxes::SetPose(vtkMatrix4x4* abs_pose_matrix)
{
	// WARNING - I am assuming that if m_Vme != NULL --> m_Coord ISA vtkMAFLocalAxisCoordinate
	if (!m_Vme)
	{
		return;
	}
	assert(m_Coord);
	auto coord = vtkMAFLocalAxisCoordinate::SafeDownCast(m_Coord);
	if (abs_pose_matrix)
	{
		coord->SetMatrix(abs_pose_matrix);
	}
	else
	{
		coord->SetMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrix().GetVTKMatrix());
	}
	coord->Modified();
}*/
