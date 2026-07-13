#include "mafDefines.h" 

#include "mafVMESurfaceParametric.h"

#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"

#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkConeSource.h"
#include "vtkCylinderSource.h"
#include "vtkPlaneSource.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

#include <vector>

const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
mafVMESurfaceParametric::mafVMESurfaceParametric()
//-------------------------------------------------------------------------
{
	m_Transform = mafTransform::NewSPtr();
	auto output = mafVMEOutputSurface::NewUPtr(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(std::move(output));

	GetMaterial();

	// attach a data pipe which creates a bridge between VTK and MAF
	auto dpipe = mafDataPipeCustom::NewSPtr();
	dpipe->SetInputData(m_PolyData);
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
mafVMESurfaceParametric::~mafVMESurfaceParametric()
//-------------------------------------------------------------------------
{
	SetOutput(nullptr);
}

//-------------------------------------------------------------------------
std::shared_ptr<mmaMaterial> mafVMESurfaceParametric::GetMaterial()
//-------------------------------------------------------------------------
{
	auto material = mmaMaterial::SafeDownCast(GetAttribute(mmaMaterial::GetAttributeName()));
	if (!material)
	{
		material = mmaMaterial::NewSPtr();
		SetAttribute(material);
	}
	return material;
}

//-------------------------------------------------------------------------
int mafVMESurfaceParametric::DeepCopy(mafNode* a)
//-------------------------------------------------------------------------
{
	if (Superclass::DeepCopy(a) == MAF_OK)
	{
		auto vmeParametricSurface = mafVMESurfaceParametric::SafeDownCast(a);
		m_Transform->SetMatrix(vmeParametricSurface->m_Transform->GetMatrix());
		this->m_GeometryType = vmeParametricSurface->m_GeometryType;
		this->m_SphereRadius = vmeParametricSurface->m_SphereRadius;
		this->m_SpherePhiRes = vmeParametricSurface->m_SpherePhiRes;
		this->m_SphereTheRes = vmeParametricSurface->m_SphereTheRes;
		this->m_ConeHeight = vmeParametricSurface->m_ConeHeight;
		this->m_ConeRadius = vmeParametricSurface->m_ConeRadius;
		this->m_ConeCapping = vmeParametricSurface->m_ConeCapping;
		this->m_ConeRes = vmeParametricSurface->m_ConeRes;
		this->m_ConeOrientationAxis = vmeParametricSurface->m_ConeOrientationAxis;
		this->m_CylinderHeight = vmeParametricSurface->m_CylinderHeight;
		this->m_CylinderRadius = vmeParametricSurface->m_CylinderRadius;
		this->m_CylinderRes = vmeParametricSurface->m_CylinderRes;
		this->m_CylinderOrientationAxis = vmeParametricSurface->m_CylinderOrientationAxis;
		this->m_CubeXLength = vmeParametricSurface->m_CubeXLength;
		this->m_CubeYLength = vmeParametricSurface->m_CubeYLength;
		this->m_CubeZLength = vmeParametricSurface->m_CubeZLength;
		this->m_PlaneXRes = vmeParametricSurface->m_PlaneXRes;
		this->m_PlaneYRes = vmeParametricSurface->m_PlaneYRes;

		this->m_PlaneOrigin[0] = vmeParametricSurface->m_PlaneOrigin[0];
		this->m_PlaneOrigin[1] = vmeParametricSurface->m_PlaneOrigin[1];
		this->m_PlaneOrigin[2] = vmeParametricSurface->m_PlaneOrigin[2];

		this->m_PlanePoint1[0] = vmeParametricSurface->m_PlanePoint1[0];
		this->m_PlanePoint1[1] = vmeParametricSurface->m_PlanePoint1[1];
		this->m_PlanePoint1[2] = vmeParametricSurface->m_PlanePoint1[2];

		this->m_PlanePoint2[0] = vmeParametricSurface->m_PlanePoint2[0];
		this->m_PlanePoint2[1] = vmeParametricSurface->m_PlanePoint2[1];
		this->m_PlanePoint2[2] = vmeParametricSurface->m_PlanePoint2[2];

		this->m_EllipsoidXLength = vmeParametricSurface->m_EllipsoidXLength;
		this->m_EllipsoidYLength = vmeParametricSurface->m_EllipsoidYLength;
		this->m_EllipsoidZLenght = vmeParametricSurface->m_EllipsoidZLenght;
		this->m_EllipsoidPhiRes = vmeParametricSurface->m_EllipsoidPhiRes;
		this->m_EllipsoidTheRes = vmeParametricSurface->m_EllipsoidTheRes;
		this->m_EllipsoidOrientationAxis = vmeParametricSurface->m_EllipsoidOrientationAxis;

		if (auto dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe()))
		{
			dpipe->SetInputData(m_PolyData);
		}
		InternalUpdate();
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMESurfaceParametric::Equals(mafVME* vme)
//-------------------------------------------------------------------------
{
	bool ret = false;
	if (Superclass::Equals(vme))
	{
		if (auto vmeParametricSurface = mafVMESurfaceParametric::SafeDownCast(vme); vmeParametricSurface &&
			m_Transform->GetMatrix() == vmeParametricSurface->m_Transform->GetMatrix() &&
			this->m_GeometryType == vmeParametricSurface->m_GeometryType &&
			this->m_SphereRadius == vmeParametricSurface->m_SphereRadius &&
			this->m_SpherePhiRes == vmeParametricSurface->m_SpherePhiRes &&
			this->m_SphereTheRes == vmeParametricSurface->m_SphereTheRes &&
			this->m_ConeHeight == vmeParametricSurface->m_ConeHeight &&
			this->m_ConeRadius == vmeParametricSurface->m_ConeRadius &&
			this->m_ConeCapping == vmeParametricSurface->m_ConeCapping &&
			this->m_ConeRes == vmeParametricSurface->m_ConeRes &&
			this->m_ConeOrientationAxis == vmeParametricSurface->m_ConeOrientationAxis &&
			this->m_CylinderHeight == vmeParametricSurface->m_CylinderHeight &&
			this->m_CylinderRadius == vmeParametricSurface->m_CylinderRadius &&
			this->m_CylinderRes == vmeParametricSurface->m_CylinderRes &&
			this->m_CylinderOrientationAxis == vmeParametricSurface->m_CylinderOrientationAxis &&
			this->m_CubeXLength == vmeParametricSurface->m_CubeXLength &&
			this->m_CubeYLength == vmeParametricSurface->m_CubeYLength &&
			this->m_CubeZLength == vmeParametricSurface->m_CubeZLength &&
			this->m_PlaneXRes == vmeParametricSurface->m_PlaneXRes &&
			this->m_PlaneYRes == vmeParametricSurface->m_PlaneYRes &&

			this->m_PlaneOrigin[0] == vmeParametricSurface->m_PlaneOrigin[0] &&
			this->m_PlaneOrigin[1] == vmeParametricSurface->m_PlaneOrigin[1] &&
			this->m_PlaneOrigin[2] == vmeParametricSurface->m_PlaneOrigin[2] &&

			this->m_PlanePoint1[0] == vmeParametricSurface->m_PlanePoint1[0] &&
			this->m_PlanePoint1[1] == vmeParametricSurface->m_PlanePoint1[1] &&
			this->m_PlanePoint1[2] == vmeParametricSurface->m_PlanePoint1[2] &&

			this->m_PlanePoint2[0] == vmeParametricSurface->m_PlanePoint2[0] &&
			this->m_PlanePoint2[1] == vmeParametricSurface->m_PlanePoint2[1] &&
			this->m_PlanePoint2[2] == vmeParametricSurface->m_PlanePoint2[2] &&

			this->m_EllipsoidXLength == vmeParametricSurface->m_EllipsoidXLength &&
			this->m_EllipsoidYLength == vmeParametricSurface->m_EllipsoidYLength &&
			this->m_EllipsoidZLenght == vmeParametricSurface->m_EllipsoidZLenght &&
			this->m_EllipsoidPhiRes == vmeParametricSurface->m_EllipsoidPhiRes &&
			this->m_EllipsoidTheRes == vmeParametricSurface->m_EllipsoidTheRes &&
			this->m_EllipsoidOrientationAxis == vmeParametricSurface->m_EllipsoidOrientationAxis
			)
		{
			ret = true;
		}
	}
	return ret;
}
//-------------------------------------------------------------------------
mafVMEOutputSurface* mafVMESurfaceParametric::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputSurface*)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetMatrix(const mafMatrix& mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMESurfaceParametric::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::GetLocalTimeStamps(std::vector<mafTimeStamp>& kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
//-------------------------------------------------------------------------
mafGUI* mafVMESurfaceParametric::CreateGui()
//-------------------------------------------------------------------------
{
	auto gui = mafVME::CreateGui();
	if (gui)
	{
		mafString geometryType[6] = { _R("Sphere"), _R("Cone"), _R("Cylinder"), _R("Cube"), _R("Plane"), _R("Ellipsoid") };
		gui->Combo(ID_GEOMETRY_TYPE, _R(""), &m_GeometryType, 6, geometryType);
		gui->Divider(2);

		CreateGuiSphere(gui);
		gui->Divider(2);
		CreateGuiCone(gui);
		gui->Divider(2);
		CreateGuiCylinder(gui);
		gui->Divider(2);
		CreateGuiCube(gui);
		gui->Divider(2);
		CreateGuiPlane(gui);
		gui->Divider(2);
		CreateGuiEllipsoid(gui);

		gui->FitGui();

		EnableParametricSurfaceGui(m_GeometryType);
		gui->Update();
	}

	gui->Divider();
	return gui;
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::OnEvent(mafEventBase* maf_event)
//-------------------------------------------------------------------------
{
	// events to be sent up or down in the tree are simply forwarded
	if (mafEvent* e = mafEvent::SafeDownCast(maf_event))
	{
		auto gui = AccessGUI();
		switch (e->GetId())
		{
		case ID_GEOMETRY_TYPE:
		{
			EnableParametricSurfaceGui(m_GeometryType);
			UpdateGUI();
			InternalUpdate();
			gui->FitGui();
		}

		case CHANGE_VALUE_SPHERE:
		case CHANGE_VALUE_CUBE:
		case CHANGE_VALUE_CONE:
		case CHANGE_VALUE_CYLINDER:
		case CHANGE_VALUE_PLANE:
		case CHANGE_VALUE_ELLIPSOID:
		{
			InternalUpdate();
			e->SetId(CAMERA_UPDATE);
			ForwardUpEvent(e);
		}
		break;

		default:
			mafVME::OnEvent(maf_event);
		}
	}

	else
	{
		Superclass::OnEvent(maf_event);
	}
}

std::vector<std::unique_ptr<IProperty>> mafVMESurfaceParametric::getProperties()
{
	auto result = Superclass::getProperties();

	std::vector<base::String> geometryNames = {	_R("Sphere"), _R("Cone"), _R("Cylinder"), _R("Cube"), _R("Plane"), _R("Ellipsoid") };
	result.push_back(makeProperty(_R("Geometry"), [this]() {return m_GeometryType; }, [this](int v) {m_GeometryType = v; Modified(); InternalUpdate();  m_valuesChanged.emit(); },
	//result.push_back(makeProperty(_R("Geometry"), *this, &mafVMESurfaceParametric::GetGeometryType, &mafVMESurfaceParametric::SetGeometryType,
		{
			{_R("visual_name"), base::String(_R(""))},
			{_R("enum_type"), base::String(_R("combo"))},
			{_R("entries"), geometryNames}
		}));
	result.push_back(makeProperty(_R("ShereRadius"), [this]() {return m_SphereRadius; }, [this](double v) {m_SphereRadius = v; Modified(); InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Radius")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_SPHERE; })}
		}));
	result.push_back(makeProperty(_R("SpherePhiRes"), [this]() {return m_SpherePhiRes; }, [this](double v) {m_SpherePhiRes = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Phi res")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_SPHERE; })}
		}));
	result.push_back(makeProperty(_R("SphereThetaRes"), [this]() {return m_SphereTheRes; }, [this](double v) {m_SphereTheRes = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Theta res")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_SPHERE; })}
		}));

	result.push_back(makeProperty(_R("ConeHieght"), [this]() {return m_ConeHeight; }, [this](double v) {m_ConeHeight = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Height")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CONE; })}
		}));
	result.push_back(makeProperty(_R("ConeRadius"), [this]() {return m_ConeRadius; }, [this](double v) {m_ConeRadius = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Radius")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CONE; })}
		}));
	result.push_back(makeProperty(_R("ConeRes"), [this]() {return m_ConeRes; }, [this](double v) {m_ConeRes = v; InternalUpdate();  m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Resolution")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CONE; })}
		}));
	result.push_back(makeProperty(_R("ConeCapping"), [this]() {return m_ConeCapping; }, [this](int v) {m_ConeCapping = v; InternalUpdate();  m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Cap")) },
			{ _R("boolean"), true },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CONE; })}
		}));
		std::vector<base::String> oriNames = { _L("X axis"),_L("Y axis"),_L("Z axis") };
	result.push_back(makeProperty(_R("ConeOrientationAxis"), [this]() {return m_ConeOrientationAxis; }, [this](int v) {m_ConeOrientationAxis = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Orientation")) },
			{_R("enum_type"), base::String(_R("radio"))},
			{_R("entries"), oriNames},
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CONE; })}
		}));

	result.push_back(makeProperty(_R("CylinderHeight"), [this]() {return m_CylinderHeight; }, [this](double v) {m_CylinderHeight = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Height")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CYLINDER; })}
		}));
	result.push_back(makeProperty(_R("CylinderRadius"), [this]() {return m_CylinderRadius; }, [this](double v) {m_CylinderRadius = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Radius")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CYLINDER; })}
		}));
	result.push_back(makeProperty(_R("CylinderRes"), [this]() {return m_CylinderRes; }, [this](double v) {m_CylinderRes = v; InternalUpdate();  m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Resolution")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CYLINDER; })}
		}));
	result.push_back(makeProperty(_R("CylinderOrientationAxis"), [this]() {return m_CylinderOrientationAxis; }, [this](int v) {m_CylinderOrientationAxis = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Orientation")) },
			{_R("enum_type"), base::String(_R("radio"))},
			{_R("entries"), oriNames},
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CYLINDER; })}
		}));

	result.push_back(makeProperty(_R("CubeXLength"), [this]() {return m_CubeXLength; }, [this](double v) {m_CubeXLength = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("X Length")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CUBE; })}
		}));
	result.push_back(makeProperty(_R("CubeYLength"), [this]() {return m_CubeYLength; }, [this](double v) {m_CubeYLength = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Y Length")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CUBE; })}
		}));
	result.push_back(makeProperty(_R("CubeZLength"), [this]() {return m_CubeZLength; }, [this](double v) {m_CubeZLength = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Z Length")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_CUBE; })}
		}));

	result.push_back(makeProperty(_R("PlaneXRes"), [this]() {return m_PlaneXRes; }, [this](double v) {m_PlaneXRes = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("X Res")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_PLANE; })}
		}));
	result.push_back(makeProperty(_R("PlaneYRes"), [this]() {return m_PlaneYRes; }, [this](double v) {m_PlaneYRes = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Y Res")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_PLANE; })}
		}));
	result.push_back(makeProperty(_R("PlaneOrigin"), [this]() {return m_PlaneOrigin; }, [this](const std::array<double, 3>& v) {m_PlaneOrigin = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Origin")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_PLANE; })}
		}));
	result.push_back(makeProperty(_R("PlanePoint1"), [this]() {return m_PlanePoint1; }, [this](const std::array<double, 3>& v) {m_PlanePoint1 = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Point 1")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_PLANE; })}
		}));
	result.push_back(makeProperty(_R("PlanePoint2"), [this]() {return m_PlanePoint2; }, [this](const std::array<double, 3>& v) {m_PlanePoint2 = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Point 2")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_PLANE; })}
		}));

	result.push_back(makeProperty(_R("EllipsoidXLenght"), [this]() {return m_EllipsoidXLength; }, [this](double v) {m_EllipsoidXLength = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("X Length")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_ELLIPSOID; })}
		}));
	result.push_back(makeProperty(_R("EllipsoidYLenght"), [this]() {return m_EllipsoidYLength; }, [this](double v) {m_EllipsoidYLength = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Y Length")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_ELLIPSOID; })}
		}));
	result.push_back(makeProperty(_R("EllipsoidZLenght"), [this]() {return m_EllipsoidZLenght; }, [this](double v) {m_EllipsoidZLenght = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Z Length")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_ELLIPSOID; })}
		}));
	result.push_back(makeProperty(_R("EllipsoidPhiRes"), [this]() {return m_EllipsoidPhiRes; }, [this](double v) {m_EllipsoidPhiRes = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Phi res")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_ELLIPSOID; })}
		}));
	result.push_back(makeProperty(_R("EllipsoidTheRes"), [this]() {return m_EllipsoidTheRes; }, [this](double v) {m_EllipsoidTheRes = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Theta res")) },
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_ELLIPSOID; })}
		}));
	result.push_back(makeProperty(_R("EllipsoidOrientationAxis"), [this]() {return m_EllipsoidOrientationAxis; }, [this](int v) {m_EllipsoidOrientationAxis = v; InternalUpdate(); m_valuesChanged.emit(); },
		{
			{_R("visual_name"), base::String(_R("Orientation")) },
			{_R("enum_type"), base::String(_R("radio"))},
			{_R("entries"), oriNames},
			{_R("enabled"), std::function<bool()>([this]() {return m_GeometryType == PARAMETRIC_ELLIPSOID; })}
		}));
	return result;
}

//-----------------------------------------------------------------------
void mafVMESurfaceParametric::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::InternalUpdate()
//-----------------------------------------------------------------------
{
	switch (m_GeometryType)
	{
	case PARAMETRIC_SPHERE:
	{
		vtkNew<vtkSphereSource> surf;
		surf->SetRadius(m_SphereRadius);
		surf->SetPhiResolution(m_SpherePhiRes);
		surf->SetThetaResolution(m_SphereTheRes);
		surf->Update();
		m_PolyData->DeepCopy(surf->GetOutput());
	}
	break;
	case PARAMETRIC_CONE:
	{
		vtkNew<vtkConeSource> surf;
		surf->SetHeight(m_ConeHeight);
		surf->SetRadius(m_ConeRadius);
		surf->SetCapping(m_ConeCapping);
		surf->SetResolution(m_ConeRes);
		surf->Update();

		vtkNew<vtkTransform> t;

		switch (m_ConeOrientationAxis)
		{
		case ID_X_AXIS:
			//do nothing
			break;
		case ID_Y_AXIS:
			t->RotateZ(90);
			break;
		case ID_Z_AXIS:
			t->RotateY(-90);
			break;
		default:
			break;
		}

		t->Update();

		vtkNew<vtkTransformPolyDataFilter> ptf;
		ptf->SetTransform(t);
		ptf->SetInputConnection(surf->GetOutputPort());
		ptf->Update();

		m_PolyData->DeepCopy(ptf->GetOutput());
	}
	break;
	case PARAMETRIC_CYLINDER:
	{
		vtkNew<vtkCylinderSource> surf;
		surf->SetHeight(m_CylinderHeight);
		surf->SetRadius(m_CylinderRadius);
		surf->SetResolution(m_CylinderRes);
		surf->Update();

		vtkNew<vtkTransform> t;

		switch (m_CylinderOrientationAxis)
		{
		case ID_X_AXIS:
			t->RotateZ(90);
			break;
		case ID_Y_AXIS:
			//do nothing
			break;
		case ID_Z_AXIS:
			t->RotateX(-90);
			break;
		default:
			break;
		}

		t->Update();

		vtkNew<vtkTransformPolyDataFilter> ptf;
		ptf->SetTransform(t);
		ptf->SetInputConnection(surf->GetOutputPort());
		ptf->Update();


		m_PolyData->DeepCopy(ptf->GetOutput());

	}
	break;
	case PARAMETRIC_CUBE:
	{
		vtkNew<vtkCubeSource> surf;
		surf->SetXLength(m_CubeXLength);
		surf->SetYLength(m_CubeYLength);
		surf->SetZLength(m_CubeZLength);
		surf->Update();
		m_PolyData->DeepCopy(surf->GetOutput());
	}
	break;

	case PARAMETRIC_PLANE:
	{
		vtkNew<vtkPlaneSource> surf;
		surf->SetXResolution(m_PlaneXRes);
		surf->SetYResolution(m_PlaneYRes);
		surf->SetOrigin(m_PlaneOrigin.data());
		surf->SetPoint1(m_PlanePoint1.data());
		surf->SetPoint2(m_PlanePoint2.data());
		surf->Update();
		vtkNew<vtkTriangleFilter> triangle;
		triangle->SetInputConnection(surf->GetOutputPort());
		triangle->Update();
		m_PolyData->DeepCopy(triangle->GetOutput());
	}
	break;

	case PARAMETRIC_ELLIPSOID:
	{
		vtkNew<vtkSphereSource> surf;
		surf->SetRadius(m_EllipsoidYLength);
		surf->SetPhiResolution(m_EllipsoidPhiRes);
		surf->SetThetaResolution(m_EllipsoidTheRes);
		surf->Update();

		vtkNew<vtkTransform> t;

		switch (m_EllipsoidOrientationAxis)
		{
		case ID_X_AXIS:
			//do nothing
			break;
		case ID_Y_AXIS:
			t->RotateZ(90);
			break;
		case ID_Z_AXIS:
			t->RotateY(-90);
			break;
		default:
			break;
		}

		t->Scale(m_EllipsoidXLength / m_EllipsoidYLength, 1, m_EllipsoidZLenght / m_EllipsoidYLength);
		t->Update();

		vtkNew<vtkTransformPolyDataFilter> ptf;
		ptf->SetTransform(t);
		ptf->SetInputConnection(surf->GetOutputPort());
		ptf->Update();

		m_PolyData->DeepCopy(ptf->GetOutput());
	}
	break;
	}

	/*mafEvent ev(this,CAMERA_UPDATE);
	this->ForwardUpEvent(&ev);*/
}
//-----------------------------------------------------------------------
void mafVMESurfaceParametric::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{
	Superclass::InternalStore(parent);
	parent[_R("Transform")].SetValue(m_Transform->GetMatrix());
	parent[_R("Geometry")].SetValue(m_GeometryType);
	parent[_R("ShereRadius")].SetValue(m_SphereRadius);
	parent[_R("SpherePhiRes")].SetValue(m_SpherePhiRes);
	parent[_R("SphereThetaRes")].SetValue(m_SphereTheRes);
	parent[_R("ConeHieght")].SetValue(m_ConeHeight);
	parent[_R("ConeRadius")].SetValue(m_ConeRadius);
	parent[_R("ConeCapping")].SetValue(m_ConeCapping);
	parent[_R("ConeRes")].SetValue(m_ConeRes);
	parent[_R("ConeOrientationAxis")].SetValue(m_ConeOrientationAxis);
	parent[_R("CylinderHeight")].SetValue(m_CylinderHeight);
	parent[_R("CylinderRadius")].SetValue(m_CylinderRadius);
	parent[_R("CylinderRes")].SetValue(m_CylinderRes);
	parent[_R("CylinderOrientationAxis")].SetValue(m_CylinderOrientationAxis);
	parent[_R("CubeXLength")].SetValue(m_CubeXLength);
	parent[_R("CubeYLength")].SetValue(m_CubeYLength);
	parent[_R("CubeZLength")].SetValue(m_CubeZLength);
	parent[_R("PlaneXRes")].SetValue(m_PlaneXRes);
	parent[_R("PlaneYRes")].SetValue(m_PlaneYRes);
	parent[_R("PlaneOrigin")].SetValue(mafToString(m_PlaneOrigin.data(), 3));
	parent[_R("PlanePoint1")].SetValue(mafToString(m_PlanePoint1.data(), 3));
	parent[_R("PlanePoint2")].SetValue(mafToString(m_PlanePoint2.data(), 3));
	parent[_R("EllipsoidXLenght")].SetValue(m_EllipsoidXLength);
	parent[_R("EllipsoidYLenght")].SetValue(m_EllipsoidYLength);
	parent[_R("EllipsoidZLenght")].SetValue(m_EllipsoidZLenght);
	parent[_R("EllipsoidTheRes")].SetValue(m_EllipsoidTheRes);
	parent[_R("EllipsoidPhiRes")].SetValue(m_EllipsoidPhiRes);
	parent[_R("EllipsoidOrientationAxis")].SetValue(m_CylinderOrientationAxis);
}

//-----------------------------------------------------------------------
void mafVMESurfaceParametric::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
	Superclass::InternalRestore(node);
	m_Transform->SetMatrix(node[_R("Transform")].As<mafMatrix>());
	m_GeometryType = node[_R("Geometry")].As<int>();
	m_SphereRadius = node[_R("ShereRadius")].As<double>();
	m_SpherePhiRes = node[_R("SpherePhiRes")].As<double>();
	m_SphereTheRes = node[_R("SphereThetaRes")].As<double>();
	m_ConeHeight = node[_R("ConeHieght")].As<double>();
	m_ConeRadius = node[_R("ConeRadius")].As<double>();
	m_ConeCapping = node[_R("ConeCapping")].As<int>();
	m_ConeRes = node[_R("ConeRes")].As<double>();
	m_ConeOrientationAxis = node[_R("ConeOrientationAxis")].As<int>();
	m_CylinderHeight = node[_R("CylinderHeight")].As<double>();
	m_CylinderRadius = node[_R("CylinderRadius")].As<double>();
	m_CylinderRes = node[_R("CylinderRes")].As<double>();
	m_CylinderOrientationAxis = node[_R("CylinderOrientationAxis")].As<int>();
	m_CubeXLength = node[_R("CubeXLength")].As<double>();
	m_CubeYLength = node[_R("CubeYLength")].As<double>();
	m_CubeZLength = node[_R("CubeZLength")].As<double>();
	m_PlaneXRes = node[_R("PlaneXRes")].As<double>();
	m_PlaneYRes = node[_R("PlaneYRes")].As<double>();
	mafParseVector(node[_R("PlaneOrigin")].As<mafString>(), m_PlaneOrigin.data(), 3);
	mafParseVector(node[_R("PlanePoint1")].As<mafString>(), m_PlanePoint1.data(), 3);
	mafParseVector(node[_R("PlanePoint2")].As<mafString>(), m_PlanePoint2.data(), 3);
	m_EllipsoidXLength = node[_R("EllipsoidXLenght")].As<double>();
	m_EllipsoidYLength = node[_R("EllipsoidYLenght")].As<double>();
	m_EllipsoidZLenght = node[_R("EllipsoidZLenght")].As<double>();
	m_EllipsoidTheRes = node[_R("EllipsoidTheRes")].As<double>();
	m_EllipsoidPhiRes = node[_R("EllipsoidPhiRes")].As<double>();
	m_CylinderOrientationAxis = node[_R("EllipsoidOrientationAxis")].As<int>();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetGeometryType(int parametricSurfaceTypeID)
//-------------------------------------------------------------------------
{
	m_GeometryType = parametricSurfaceTypeID;
	Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceParametric::SetSphereRadius(double radius)
//-------------------------------------------------------------------------
{
	m_SphereRadius = radius;
	Modified();
}
//-------------------------------------------------------------------------
const char** mafVMESurfaceParametric::GetIcon()
//-------------------------------------------------------------------------
{
#include "mafVMEProcedural.xpm"
	return mafVMEProcedural_xpm;
}

void mafVMESurfaceParametric::CreateGuiPlane(mafGUI* gui)
{
	m_GuiPlane = new mafGUI(this);
	m_GuiPlane->Label(_R("Plane"));
	m_GuiPlane->Double(CHANGE_VALUE_PLANE, _L("X Res"), &m_PlaneXRes);
	m_GuiPlane->Double(CHANGE_VALUE_PLANE, _L("Y Res"), &m_PlaneYRes);
	m_GuiPlane->Vector(CHANGE_VALUE_PLANE, _L("Origin"), m_PlaneOrigin.data());
	m_GuiPlane->Vector(CHANGE_VALUE_PLANE, _L("Point 1"), m_PlanePoint1.data());
	m_GuiPlane->Vector(CHANGE_VALUE_PLANE, _L("Point 2"), m_PlanePoint2.data());
	assert(gui);
	gui->AddGui(m_GuiPlane);
}

void mafVMESurfaceParametric::CreateGuiCube(mafGUI* gui)
{
	m_GuiCube = new mafGUI(this);
	m_GuiCube->Label(_R("Cube"));
	m_GuiCube->Double(CHANGE_VALUE_CUBE, _L("X Length"), &m_CubeXLength);
	m_GuiCube->Double(CHANGE_VALUE_CUBE, _L("Y Length"), &m_CubeYLength);
	m_GuiCube->Double(CHANGE_VALUE_CUBE, _L("Z Length"), &m_CubeZLength);
	assert(gui);
	gui->AddGui(m_GuiCube);
}

void mafVMESurfaceParametric::CreateGuiCylinder(mafGUI* gui)
{
	m_GuiCylinder = new mafGUI(this);
	m_GuiCylinder->Label(_R("Cylinder"));
	m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER, _L("Height"), &m_CylinderHeight);
	m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER, _L("Radius"), &m_CylinderRadius);
	m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER, _L("Resolution"), &m_CylinderRes);
	mafString orientationArray[3] = { _L("X axis"),_L("Y axis"),_L("Z axis") };
	m_GuiCylinder->Radio(CHANGE_VALUE_CYLINDER, _R("Orientation"), &m_CylinderOrientationAxis, 3, orientationArray);
	assert(gui);
	gui->AddGui(m_GuiCylinder);

}

void mafVMESurfaceParametric::CreateGuiCone(mafGUI* gui)
{
	m_GuiCone = new mafGUI(this);
	m_GuiCone->Label(_R("Cone"));
	m_GuiCone->Double(CHANGE_VALUE_CONE, _L("Height"), &m_ConeHeight);
	m_GuiCone->Double(CHANGE_VALUE_CONE, _L("Radius"), &m_ConeRadius);
	m_GuiCone->Double(CHANGE_VALUE_CONE, _L("Resolution"), &m_ConeRes);
	m_GuiCone->Bool(CHANGE_VALUE_CONE, _R("Cap"), &m_ConeCapping); // Open or closed cone
	mafString orientationArray[3] = { _L("X axis"),_L("Y axis"),_L("Z axis") };
	m_GuiCone->Radio(CHANGE_VALUE_CONE, _R("Orientation"), &m_ConeOrientationAxis, 3, orientationArray);
	assert(gui);
	gui->AddGui(m_GuiCone);
}

void mafVMESurfaceParametric::CreateGuiSphere(mafGUI* gui)
{
	m_GuiSphere = new mafGUI(this);
	m_GuiSphere->Label(_R("Sphere"));
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _L("Radius"), &m_SphereRadius);
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _L("Phi res"), &m_SpherePhiRes);
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _L("Theta res"), &m_SphereTheRes);
	assert(gui);
	gui->AddGui(m_GuiSphere);
}

void mafVMESurfaceParametric::CreateGuiEllipsoid(mafGUI* gui)
{
	m_GuiEllipsoid = new mafGUI(this);
	m_GuiEllipsoid->Label(_R("Ellipsoid"));
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _L("X Length"), &m_EllipsoidXLength);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _L("Y Length"), &m_EllipsoidYLength);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _L("Z Length"), &m_EllipsoidZLenght);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _L("Phi res"), &m_EllipsoidPhiRes);
	m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID, _L("Theta res"), &m_EllipsoidTheRes);
	mafString orientationArray[3] = { _L("X axis"),_L("Y axis"),_L("Z axis") };
	m_GuiEllipsoid->Radio(CHANGE_VALUE_ELLIPSOID, _R("Orientation"), &m_EllipsoidOrientationAxis, 3, orientationArray);
	assert(gui);
	gui->AddGui(m_GuiEllipsoid);
}

void mafVMESurfaceParametric::EnableGuiPlane()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, true);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiCube()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, true);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiCylinder()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, true);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiCone()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, true);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiSphere()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, true);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceParametric::EnableGuiEllipsoid()
{
	m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
	m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
	m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, true);
}

void mafVMESurfaceParametric::EnableParametricSurfaceGui(int surfaceTypeID)
{
	switch (surfaceTypeID)
	{
	case PARAMETRIC_SPHERE:
		EnableGuiSphere();

		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Sphere gui" << std::endl;
			mafLogMessage(_M(stringStream.str().c_str()));
		}
		break;

	case PARAMETRIC_CONE:
		EnableGuiCone();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Cone gui" << std::endl;
			mafLogMessage(_M(stringStream.str().c_str()));
		}

		break;

	case PARAMETRIC_CYLINDER:
		EnableGuiCylinder();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Cylinder gui" << std::endl;
			mafLogMessage(_M(stringStream.str().c_str()));
		}

		break;

	case PARAMETRIC_CUBE:
		EnableGuiCube();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Cube gui" << std::endl;
			mafLogMessage(_M(stringStream.str().c_str()));
		}

		break;

	case PARAMETRIC_PLANE:
		EnableGuiPlane();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Plane gui" << std::endl;
			mafLogMessage(_M(stringStream.str().c_str()));
		}

		break;

	case PARAMETRIC_ELLIPSOID:
		EnableGuiEllipsoid();
		if (DEBUG_MODE)
		{
			std::ostringstream stringStream;
			stringStream << "enabling Ellipsoid gui" << std::endl;
			mafLogMessage(_M(stringStream.str().c_str()));
		}

		break;

	default:
		break;
	}
}