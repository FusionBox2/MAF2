/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMESurfaceRegParam.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-21 12:26:28 $
  Version:   $Revision: 1.13 $
  Authors:   Roberto Mucci
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

#include "mafVMESurfaceRegParam.h"
#include "mmuIdFactory.h"
#include "mafDataVector.h"
#include "mafMatrixInterpolator.h"
#include "mafDataPipeInterpolator.h"
#include "mafTagArray.h"
#include "mafMatrixVector.h"
#include "mafVMEItemVTK.h"
#include "mafEventSender.h"
#include "mafTransform.h"
#include "mafGUI.h"
#include "mmaMaterial.h"
#include "mafVMEOutputSurface.h"
#include "mafDataPipeCustom.h"

#include "vtkSmartPointer.h"
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
#include "mafVectors.h"

const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MAF_ID_IMP(mafVMESurfaceRegParam::CHANGE_PARAMETER);   // Event rised by change parameter 

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMESurfaceRegParam);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMESurfaceRegParam::mafVMESurfaceRegParam()
//-------------------------------------------------------------------------
{
  m_GeometryType = PARAMETRIC_SPHERE;
	
  m_SphereRadius = 2.0;
  m_SpherePhiRes = 10.0;
  m_SphereTheRes = 10.0;
  m_SphereCenter[0] = m_SphereCenter[1] = m_SphereCenter[2] = 0;
  m_ConeHeight = 5.0;
  m_ConeRadius = 2.0;
  m_ConeCapping = 0;
  m_ConeRes = 20.0;
  m_ConeOrientationAxis = ID_X_AXIS;
  m_CylinderHeight = 5.0;
  m_CylinderRadius = 2.0;
  m_CylinderRes = 20.0;
  m_CylinderOrientationAxis = ID_Y_AXIS;
  m_CubeXLength = 2.0;
  m_CubeYLength = 2.0;
  m_CubeZLength = 2.0;
  m_PlaneXRes = 2.0;
  m_PlaneYRes = 2.0;
  m_PlaneXSize = 40.0;
  m_PlaneYSize = 40.0;
  m_PlaneCenter[0] = m_PlaneCenter[1] = m_PlaneCenter[2] = 0;
  m_PlaneNormal[0] = m_PlaneNormal[1] = m_PlaneNormal[2] = 0;
  m_PlaneOrigin[0] = m_PlaneOrigin[1] = m_PlaneOrigin[2] = 0;
  m_PlanePoint1[0] = 2.0;
  m_PlanePoint1[1] = m_PlanePoint1[2] = 0.0;
  m_PlanePoint2[1] = 3.0;
  m_PlanePoint2[0] = m_PlanePoint2[2] = 0.0;
  m_EllipsoidXLenght = 1.0;
  m_EllipsoidYLenght = 2.0;
  m_EllipsoidZLenght = 3.0;
  m_EllipsoidPhiRes = 10.0;
  m_EllipsoidTheRes = 10.0;
  m_EllipsoidOrientationAxis = ID_X_AXIS;

	mafNEW(m_Transform);
	mafVMEOutputSurface *output=mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);

  // attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInputData(m_PolyData);
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
mafVMESurfaceRegParam::~mafVMESurfaceRegParam()
//-------------------------------------------------------------------------
{
	vtkDEL(m_PolyData);
  mafDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMESurfaceRegParam::GetMaterial()
//-------------------------------------------------------------------------
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute(_R("MaterialAttributes"));
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute(_R("MaterialAttributes"), material);
	}
	return material;
}

//-------------------------------------------------------------------------
int mafVMESurfaceRegParam::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMESurfaceRegParam *vmeParametricSurface=mafVMESurfaceRegParam::SafeDownCast(a);
    m_Transform->SetMatrix(vmeParametricSurface->m_Transform->GetMatrix());
    this->m_GeometryType = vmeParametricSurface->m_GeometryType;
    this->m_SphereRadius = vmeParametricSurface->m_SphereRadius;
    this->m_SpherePhiRes = vmeParametricSurface->m_SpherePhiRes;
    this->m_SphereTheRes = vmeParametricSurface->m_SphereTheRes;
    this->m_SphereCenter[0] = vmeParametricSurface->m_SphereCenter[0];
    this->m_SphereCenter[1] = vmeParametricSurface->m_SphereCenter[1];
    this->m_SphereCenter[2] = vmeParametricSurface->m_SphereCenter[2];
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

    this->m_PlaneCenter[0] = vmeParametricSurface->m_PlaneCenter[0];
    this->m_PlaneCenter[1] = vmeParametricSurface->m_PlaneCenter[1];
    this->m_PlaneCenter[2] = vmeParametricSurface->m_PlaneCenter[2];

    this->m_PlaneNormal[0] = vmeParametricSurface->m_PlaneNormal[0];
    this->m_PlaneNormal[1] = vmeParametricSurface->m_PlaneNormal[1];
    this->m_PlaneNormal[2] = vmeParametricSurface->m_PlaneNormal[2];

    this->m_PlaneOrigin[0] = vmeParametricSurface->m_PlaneOrigin[0];
    this->m_PlaneOrigin[1] = vmeParametricSurface->m_PlaneOrigin[1];
    this->m_PlaneOrigin[2] = vmeParametricSurface->m_PlaneOrigin[2];

    this->m_PlanePoint1[0] = vmeParametricSurface->m_PlanePoint1[0];
    this->m_PlanePoint1[1] = vmeParametricSurface->m_PlanePoint1[1];
    this->m_PlanePoint1[2] = vmeParametricSurface->m_PlanePoint1[2];

    this->m_PlanePoint2[0] = vmeParametricSurface->m_PlanePoint2[0];
    this->m_PlanePoint2[1] = vmeParametricSurface->m_PlanePoint2[1];
    this->m_PlanePoint2[2] = vmeParametricSurface->m_PlanePoint2[2];

    this->m_EllipsoidXLenght = vmeParametricSurface->m_EllipsoidXLenght;
    this->m_EllipsoidYLenght = vmeParametricSurface->m_EllipsoidYLenght;
    this->m_EllipsoidZLenght = vmeParametricSurface->m_EllipsoidZLenght;
    this->m_EllipsoidPhiRes = vmeParametricSurface->m_EllipsoidPhiRes;
    this->m_EllipsoidTheRes = vmeParametricSurface->m_EllipsoidTheRes;
    this->m_EllipsoidOrientationAxis = vmeParametricSurface->m_EllipsoidOrientationAxis;

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInputData(m_PolyData);
    }
    InternalUpdate();
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMESurfaceRegParam::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  if (Superclass::Equals(vme))
  {
    if (     
      m_Transform->GetMatrix()==((mafVMESurfaceRegParam *)vme)->m_Transform->GetMatrix() &&
      this->m_GeometryType == ((mafVMESurfaceRegParam *)vme)->m_GeometryType &&
      this->m_SphereRadius == ((mafVMESurfaceRegParam *)vme)->m_SphereRadius &&
      this->m_SpherePhiRes == ((mafVMESurfaceRegParam *)vme)->m_SpherePhiRes &&
      this->m_SphereTheRes == ((mafVMESurfaceRegParam *)vme)->m_SphereTheRes &&
      this->m_SphereCenter[0] == ((mafVMESurfaceRegParam *)vme)->m_SphereCenter[0] &&
      this->m_SphereCenter[1] == ((mafVMESurfaceRegParam *)vme)->m_SphereCenter[1] &&
      this->m_SphereCenter[2] == ((mafVMESurfaceRegParam *)vme)->m_SphereCenter[2] &&
      this->m_ConeHeight == ((mafVMESurfaceRegParam *)vme)->m_ConeHeight  &&
      this->m_ConeRadius == ((mafVMESurfaceRegParam *)vme)->m_ConeRadius  &&
      this->m_ConeCapping == ((mafVMESurfaceRegParam *)vme)->m_ConeCapping  &&
      this->m_ConeRes == ((mafVMESurfaceRegParam *)vme)->m_ConeRes &&
      this->m_ConeOrientationAxis == ((mafVMESurfaceRegParam *)vme)->m_ConeOrientationAxis &&
      this->m_CylinderHeight == ((mafVMESurfaceRegParam *)vme)->m_CylinderHeight  &&
      this->m_CylinderRadius == ((mafVMESurfaceRegParam *)vme)->m_CylinderRadius  &&
      this->m_CylinderRes == ((mafVMESurfaceRegParam *)vme)->m_CylinderRes  &&
      this->m_CylinderOrientationAxis == ((mafVMESurfaceRegParam *)vme)->m_CylinderOrientationAxis  &&
      this->m_CubeXLength == ((mafVMESurfaceRegParam *)vme)->m_CubeXLength  &&
      this->m_CubeYLength == ((mafVMESurfaceRegParam *)vme)->m_CubeYLength  &&
      this->m_CubeZLength == ((mafVMESurfaceRegParam *)vme)->m_CubeZLength  &&
      this->m_PlaneXRes == ((mafVMESurfaceRegParam *)vme)->m_PlaneXRes  &&
      this->m_PlaneYRes == ((mafVMESurfaceRegParam *)vme)->m_PlaneYRes  &&

      this->m_PlaneCenter[0] == ((mafVMESurfaceRegParam *)vme)->m_PlaneCenter[0]  &&
      this->m_PlaneCenter[1] == ((mafVMESurfaceRegParam *)vme)->m_PlaneCenter[1]  &&
      this->m_PlaneCenter[2] == ((mafVMESurfaceRegParam *)vme)->m_PlaneCenter[2]  &&

      this->m_PlaneNormal[0] == ((mafVMESurfaceRegParam *)vme)->m_PlaneNormal[0]  &&
      this->m_PlaneNormal[1] == ((mafVMESurfaceRegParam *)vme)->m_PlaneNormal[1]  &&
      this->m_PlaneNormal[2] == ((mafVMESurfaceRegParam *)vme)->m_PlaneNormal[2]  &&

      this->m_PlaneOrigin[0] == ((mafVMESurfaceRegParam *)vme)->m_PlaneOrigin[0]  &&
      this->m_PlaneOrigin[1] == ((mafVMESurfaceRegParam *)vme)->m_PlaneOrigin[1]  &&
      this->m_PlaneOrigin[2] == ((mafVMESurfaceRegParam *)vme)->m_PlaneOrigin[2]  &&

      this->m_PlanePoint1[0] == ((mafVMESurfaceRegParam *)vme)->m_PlanePoint1[0]  &&
      this->m_PlanePoint1[1] == ((mafVMESurfaceRegParam *)vme)->m_PlanePoint1[1]  &&
      this->m_PlanePoint1[2] == ((mafVMESurfaceRegParam *)vme)->m_PlanePoint1[2]  &&

      this->m_PlanePoint2[0] == ((mafVMESurfaceRegParam *)vme)->m_PlanePoint2[0]  &&
      this->m_PlanePoint2[1] == ((mafVMESurfaceRegParam *)vme)->m_PlanePoint2[1]  &&
      this->m_PlanePoint2[2] == ((mafVMESurfaceRegParam *)vme)->m_PlanePoint2[2] &&

      this->m_EllipsoidXLenght == ((mafVMESurfaceRegParam *)vme)->m_EllipsoidXLenght &&
      this->m_EllipsoidYLenght == ((mafVMESurfaceRegParam *)vme)->m_EllipsoidYLenght &&
      this->m_EllipsoidZLenght == ((mafVMESurfaceRegParam *)vme)->m_EllipsoidZLenght &&
      this->m_EllipsoidPhiRes == ((mafVMESurfaceRegParam *)vme)->m_EllipsoidPhiRes &&
      this->m_EllipsoidTheRes == ((mafVMESurfaceRegParam *)vme)->m_EllipsoidTheRes &&
      this->m_EllipsoidOrientationAxis == ((mafVMESurfaceRegParam *)vme)->m_EllipsoidOrientationAxis
      )
    {
      return true;
    }
  }
 return false;
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMESurfaceRegParam::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMESurfaceRegParam::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
  mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}
//-------------------------------------------------------------------------
mafGUI* mafVMESurfaceRegParam::CreateGui()
//-------------------------------------------------------------------------
{
  mafVME::CreateGui();
  if(m_Gui)
  {
    mafString geometryType[6] = {_R("Sphere"), _R("Cone"), _R("Cylinder"), _R("Cube"), _R("Plane"), _R("Ellipsoid")};
    m_Gui->Combo(ID_GEOMETRY_TYPE, _R(""), &m_GeometryType, 6, geometryType);
    m_Gui->Divider(2);

    CreateGuiSphere();   
    m_Gui->Divider(2);
    CreateGuiCone();
    m_Gui->Divider(2);
    CreateGuiCylinder();
    m_Gui->Divider(2);
    CreateGuiCube();
    m_Gui->Divider(2);
    CreateGuiPlane();
    m_Gui->Divider(2);
    CreateGuiEllipsoid();
   
    m_Gui->FitGui();
    
    EnableParametricSurfaceGui(m_GeometryType);
    m_Gui->Update();
  }

  m_Gui->Divider();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_GEOMETRY_TYPE:
      {  
        EnableParametricSurfaceGui(m_GeometryType);
        m_Gui->Update();
        InternalUpdate();
        m_Gui->FitGui();
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

//-----------------------------------------------------------------------
void mafVMESurfaceRegParam::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMESurfaceRegParam::InternalUpdate()
//-----------------------------------------------------------------------
{
	switch(m_GeometryType)
	{
	case PARAMETRIC_SPHERE:
		{
      vtkNew<vtkSphereSource> surf;
			surf->SetRadius(m_SphereRadius);
      surf->SetCenter(m_SphereCenter);
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

    switch(m_ConeOrientationAxis)
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

    switch(m_CylinderOrientationAxis)
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
    V3d<double> norm(m_PlaneNormal);
    V3d<double> dir1(1.0, 1.0, 1.0);
    V3d<double> dir2;
    double mx = fabs(norm[0]);
    int mxi   = 0;
    if(fabs(norm[1]) > mx)
    {
      mx = fabs(norm[1]);
      mxi = 1;
    }
    if(fabs(norm[2]) > mx)
    {
      mx = fabs(norm[2]);
      mxi = 2;
    }
    double dot = norm * dir1;
    dir1[mxi] -= dot / norm[mxi];
    dir2 = norm ^ dir1;
    dir1 /= sqrt(dir1.length2());
    dir2 /= sqrt(dir2.length2());
    V3d<double> origin(m_PlaneCenter);
    origin -= 0.5 * m_PlaneXSize * dir1 + 0.5 * m_PlaneYSize * dir2;
    V3d<double> pnt1(origin);
    V3d<double> pnt2(origin);
    pnt1 += m_PlaneXSize * dir1;
    pnt2 += m_PlaneYSize * dir2;
    surf->SetXResolution(m_PlaneXRes);
    surf->SetYResolution(m_PlaneYRes);
    surf->SetOrigin(origin.components);
    surf->SetPoint1(pnt1.components);
    surf->SetPoint2(pnt2.components);
    /*surf->SetOrigin(m_PlaneOrigin);
    surf->SetPoint1(m_PlanePoint1);
    surf->SetPoint2(m_PlanePoint2);*/
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
      surf->SetRadius(m_EllipsoidYLenght);
      surf->SetPhiResolution(m_EllipsoidPhiRes);
      surf->SetThetaResolution(m_EllipsoidTheRes);
      surf->Update();

      vtkNew<vtkTransform> t;

      switch(m_EllipsoidOrientationAxis)
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

      t->Scale(m_EllipsoidXLenght/m_EllipsoidYLenght,1,m_EllipsoidZLenght/m_EllipsoidYLenght);
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
int mafVMESurfaceRegParam::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
	if (Superclass::InternalStore(parent)==MAF_OK)
	{
    if (
		parent->StoreMatrix(_R("Transform"),m_Transform->GetMatrix()) == MAF_OK && 
		parent->StoreInteger(_R("Geometry"), 	m_GeometryType) == MAF_OK &&
    parent->StoreDouble(_R("ShereRadius"), m_SphereRadius) == MAF_OK &&
    parent->StoreDouble(_R("SpherePhiRes"),m_SpherePhiRes) == MAF_OK &&
    parent->StoreDouble(_R("SphereThetaRes"),m_SphereTheRes) == MAF_OK &&
    parent->StoreVectorN(_R("SphereCenter"),m_SphereCenter,3) == MAF_OK &&
    parent->StoreDouble(_R("ConeHieght"),m_ConeHeight) == MAF_OK &&
    parent->StoreDouble(_R("ConeRadius"),m_ConeRadius) == MAF_OK &&
    parent->StoreInteger(_R("ConeCapping"),m_ConeCapping) == MAF_OK &&
    parent->StoreDouble(_R("ConeRes"),m_ConeRes) == MAF_OK &&
    parent->StoreInteger(_R("ConeOrientationAxis"),m_ConeOrientationAxis) == MAF_OK &&
    parent->StoreDouble(_R("CylinderHeight"),m_CylinderHeight) == MAF_OK &&
    parent->StoreDouble(_R("CylinderRadius"),m_CylinderRadius) == MAF_OK &&
    parent->StoreDouble(_R("CylinderRes"),m_CylinderRes) == MAF_OK &&
    parent->StoreInteger(_R("CylinderOrientationAxis"),m_CylinderOrientationAxis) == MAF_OK &&
    parent->StoreDouble(_R("CubeXLength"),m_CubeXLength) == MAF_OK &&
    parent->StoreDouble(_R("CubeYLength"),m_CubeYLength) == MAF_OK &&
    parent->StoreDouble(_R("CubeZLength"),m_CubeZLength) == MAF_OK &&
    parent->StoreDouble(_R("PlaneXRes"),m_PlaneXRes) == MAF_OK &&
    parent->StoreDouble(_R("PlaneYRes"),m_PlaneYRes) == MAF_OK &&
    //parent->StoreVectorN(_R("PlaneOrigin"),m_PlaneOrigin,3) == MAF_OK &&
    parent->StoreVectorN(_R("PlaneCenter"),m_PlaneCenter,3) == MAF_OK &&
    parent->StoreVectorN(_R("PlaneNormal"),m_PlaneNormal,3) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidXLenght"),m_EllipsoidXLenght) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidYLenght"),m_EllipsoidYLenght) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidZLenght"),m_EllipsoidZLenght) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidTheRes"),m_EllipsoidTheRes) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidPhiRes"),m_EllipsoidPhiRes) == MAF_OK &&
    parent->StoreInteger(_R("EllipsoidOrientationAxis"),m_CylinderOrientationAxis) == MAF_OK
    )
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMESurfaceRegParam::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalRestore(node)==MAF_OK)
	{
    mafMatrix matrix;
    if (node[_R("Transform")].RestoreMatrix(matrix) ==MAF_OK)
    {
      m_Transform->SetMatrix(matrix); 
      node.RestoreInteger(_R("Geometry"),m_GeometryType);
      node.RestoreDouble(_R("ShereRadius"),m_SphereRadius);
      node.RestoreDouble(_R("SpherePhiRes"),m_SpherePhiRes);
      node.RestoreDouble(_R("SphereThetaRes"),m_SphereTheRes);
      node.RestoreDouble(_R("ConeHieght"),m_ConeHeight);
      node.RestoreDouble(_R("ConeRadius"),m_ConeRadius);
      node.RestoreInteger(_R("ConeCapping"),m_ConeCapping);
      node.RestoreDouble(_R("ConeRes"),m_ConeRes);
      node.RestoreInteger(_R("ConeOrientationAxis"),m_ConeOrientationAxis);
      node.RestoreDouble(_R("CylinderHeight"),m_CylinderHeight);
      node.RestoreDouble(_R("CylinderRadius"),m_CylinderRadius);
      node.RestoreDouble(_R("CylinderRes"),m_CylinderRes);
      node.RestoreInteger(_R("CylinderOrientationAxis"),m_CylinderOrientationAxis);
      node.RestoreDouble(_R("CubeXLength"),m_CubeXLength);
      node.RestoreDouble(_R("CubeYLength"),m_CubeYLength);
      node.RestoreDouble(_R("CubeZLength"),m_CubeZLength);
      node.RestoreDouble(_R("PlaneXRes"),m_PlaneXRes);
      node.RestoreDouble(_R("PlaneYRes"),m_PlaneYRes);
      //node.RestoreVectorN(_R("PlaneOrigin"),m_PlaneOrigin,3) == MAF_OK && 
      node.RestoreVectorN(_R("PlaneCenter"),m_PlaneCenter,3);
      node.RestoreVectorN(_R("PlaneNormal"),m_PlaneNormal,3);
      node.RestoreDouble(_R("EllipsoidXLenght"),m_EllipsoidXLenght);
      node.RestoreDouble(_R("EllipsoidYLenght"),m_EllipsoidYLenght);
      node.RestoreDouble(_R("EllipsoidZLenght"),m_EllipsoidZLenght);
      node.RestoreDouble(_R("EllipsoidTheRes"),m_EllipsoidTheRes);
      node.RestoreDouble(_R("EllipsoidPhiRes"),m_EllipsoidPhiRes);
      node.RestoreInteger(_R("EllipsoidOrientationAxis"),m_CylinderOrientationAxis);
      return MAF_OK;
    }
	}
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetGeometryType( int parametricSurfaceTypeID )
//-------------------------------------------------------------------------
{
  m_GeometryType = parametricSurfaceTypeID;
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetSphereRadius(double radius)
//-------------------------------------------------------------------------
{
  m_SphereRadius = radius;
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetSphereCenter(const double *center)
//-------------------------------------------------------------------------
{
  m_SphereCenter[0] = center[0];
  m_SphereCenter[1] = center[1];
  m_SphereCenter[2] = center[2];
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetPlaneCenter(const double *center)
//-------------------------------------------------------------------------
{
  m_PlaneCenter[0] = center[0];
  m_PlaneCenter[1] = center[1];
  m_PlaneCenter[2] = center[2];
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetPlaneNormal(const double *normal)
//-------------------------------------------------------------------------
{
  m_PlaneNormal[0] = normal[0];
  m_PlaneNormal[1] = normal[1];
  m_PlaneNormal[2] = normal[2];
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetPlaneOrigin(const double *origin)
//-------------------------------------------------------------------------
{
  m_PlaneOrigin[0] = origin[0];
  m_PlaneOrigin[1] = origin[1];
  m_PlaneOrigin[2] = origin[2];
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetPlanePoint1(const double *point1)
//-------------------------------------------------------------------------
{
  m_PlanePoint1[0] = point1[0];
  m_PlanePoint1[1] = point1[1];
  m_PlanePoint1[2] = point1[2];
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetPlanePoint2(const double *point2)
//-------------------------------------------------------------------------
{
  m_PlanePoint2[0] = point2[0];
  m_PlanePoint2[1] = point2[1];
  m_PlanePoint2[2] = point2[2];
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetEllipsoidXSemiAxis(double axis)
//-------------------------------------------------------------------------
{
  m_EllipsoidXLenght = axis;
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetEllipsoidYSemiAxis(double axis)
//-------------------------------------------------------------------------
{
  m_EllipsoidYLenght = axis;
  Modified();
}
//-------------------------------------------------------------------------
void mafVMESurfaceRegParam::SetEllipsoidZSemiAxis(double axis)
//-------------------------------------------------------------------------
{
  m_EllipsoidZLenght = axis;
  Modified();
}

//-------------------------------------------------------------------------
char** mafVMESurfaceRegParam::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEProcedural.xpm"
  return mafVMEProcedural_xpm;
}

void mafVMESurfaceRegParam::CreateGuiPlane()
{
  m_GuiPlane= new mafGUI(this);
  m_GuiPlane->Label(_R("Plane"));
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_L("X Res"), &m_PlaneXRes);
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_L("Y Res"), &m_PlaneYRes);
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_L("X Size"), &m_PlaneXSize);
  m_GuiPlane->Double(CHANGE_VALUE_PLANE,_L("Y Size"), &m_PlaneYSize);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_L("Center"), m_PlaneCenter);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_L("Normal"), m_PlaneNormal);
  /*m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_L("Origin"), m_PlaneOrigin);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_L("Point 1"), m_PlanePoint1);
  m_GuiPlane->Vector(CHANGE_VALUE_PLANE,_L("Point 2"), m_PlanePoint2);*/
  assert(m_Gui);
  m_Gui->AddGui(m_GuiPlane);
}

void mafVMESurfaceRegParam::CreateGuiCube()
{
  m_GuiCube = new mafGUI(this);
  m_GuiCube->Label(_R("Cube"));
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_L("X Length"), &m_CubeXLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_L("Y Length"), &m_CubeYLength);
  m_GuiCube->Double(CHANGE_VALUE_CUBE,_L("Z Length"), &m_CubeZLength);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCube);
}

void mafVMESurfaceRegParam::CreateGuiCylinder()
{
  m_GuiCylinder = new mafGUI(this);
  m_GuiCylinder->Label(_R("Cylinder"));
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_L("Height"), &m_CylinderHeight);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_L("Radius"), &m_CylinderRadius);
  m_GuiCylinder->Double(CHANGE_VALUE_CYLINDER,_L("Resolution"), &m_CylinderRes);
  mafString orientationArray[3] = {_L("X axis"),_L("Y axis"),_L("Z axis")};
  m_GuiCylinder->Radio(CHANGE_VALUE_CYLINDER,_R("Orientation"), &m_CylinderOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCylinder);

}

void mafVMESurfaceRegParam::CreateGuiCone()
{
  m_GuiCone = new mafGUI(this);
  m_GuiCone->Label(_R("Cone"));
  m_GuiCone->Double(CHANGE_VALUE_CONE,_L("Height"), &m_ConeHeight);
  m_GuiCone->Double(CHANGE_VALUE_CONE,_L("Radius"), &m_ConeRadius);
  m_GuiCone->Double(CHANGE_VALUE_CONE,_L("Resolution"), &m_ConeRes);
  m_GuiCone->Bool(CHANGE_VALUE_CONE,_R("Cap"), &m_ConeCapping); // Open or closed cone
  mafString orientationArray[3] = {_L("X axis"),_L("Y axis"),_L("Z axis")};
  m_GuiCone->Radio(CHANGE_VALUE_CONE,_R("Orientation"), &m_ConeOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiCone);
}

void mafVMESurfaceRegParam::CreateGuiSphere()
{
  m_GuiSphere = new mafGUI(this);
  m_GuiSphere->Label(_R("Sphere"));
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_L("Radius"), &m_SphereRadius);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_L("Phi res"), &m_SpherePhiRes);
  m_GuiSphere->Double(CHANGE_VALUE_SPHERE,_L("Theta res"), &m_SphereTheRes);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiSphere);
}

void mafVMESurfaceRegParam::CreateGuiEllipsoid()
{
  m_GuiEllipsoid = new mafGUI(this);
  m_GuiEllipsoid->Label(_R("Ellipsoid"));
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_L("X Length"), &m_EllipsoidXLenght);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_L("Y Length"), &m_EllipsoidYLenght);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_L("Z Length"), &m_EllipsoidZLenght);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_L("Phi res"), &m_EllipsoidPhiRes);
  m_GuiEllipsoid->Double(CHANGE_VALUE_ELLIPSOID,_L("Theta res"), &m_EllipsoidTheRes);
  mafString orientationArray[3] = {_L("X axis"),_L("Y axis"),_L("Z axis")};
  m_GuiEllipsoid->Radio(CHANGE_VALUE_ELLIPSOID,_R("Orientation"), &m_EllipsoidOrientationAxis, 3,orientationArray);
  assert(m_Gui);
  m_Gui->AddGui(m_GuiEllipsoid);
}

void mafVMESurfaceRegParam::EnableGuiPlane()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, true);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceRegParam::EnableGuiCube()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, true);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceRegParam::EnableGuiCylinder()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, true);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceRegParam::EnableGuiCone()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, true);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceRegParam::EnableGuiSphere()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, true);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, false);
}

void mafVMESurfaceRegParam::EnableGuiEllipsoid()
{
  m_GuiPlane->Enable(CHANGE_VALUE_PLANE, false);
  m_GuiCube->Enable(CHANGE_VALUE_CUBE, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_CYLINDER, false);
  m_GuiCone->Enable(CHANGE_VALUE_CONE, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_ELLIPSOID, true);
}

void mafVMESurfaceRegParam::EnableParametricSurfaceGui( int surfaceTypeID )
{
  switch(surfaceTypeID)
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
