/*=========================================================================

 Program: MAF2
 Module: mafVMEQuadricSurfaceFitting
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "wx/busyinfo.h"
#include "mafVMEQuadricSurfaceFitting.h"
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
#include "mafVMESurface.h"
#include "mafDataPipeCustom.h"
#include "mafVMESurfaceParametric.h"
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafOpAddLandmark.h"
#include "vtkMAFSmartPointer.h"
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
#include "vtkPoints.h"
#include "vtkActor.h"
#include "mafVMEOutputPointSet.h"
#include <vector>
#include "vtkPointSet.h"
#include <boost/numeric/odeint.hpp>


const bool DEBUG_MODE = true;

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
//MAF_ID_IMP(mafVMESurfaceParametric::CHANGE_PARAMETER);   // Event rised by change parameter 

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEQuadricSurfaceFitting);
//------------------------------------------------------------------------------
int sgn(double x)
{
	if (x >= 0)
		return 1;
	else return -1;
}
//------------------------------------------------------------------------------
mafVMEQuadricSurfaceFitting::mafVMEQuadricSurfaceFitting()
//-------------------------------------------------------------------------
{
	m_GeometryType = PARAMETRIC_SPHERE;

  ///suppose ellipsoid centred on (0,0,0) with a,b,c (5,3,1)
  m_PointsZRes = 0.0;
  m_PointsXRes = 0.0;
  m_PointsYRes = 0.0;
  m_Points1Point1[0] = 5.0; m_Points1Point1[1] = 0.0; m_Points1Point1[2] = 0.0;
  m_Points1Point2[0] = 0.0; m_Points1Point2[1] = 3.0; m_Points1Point2[2] = 0.0;
  m_Points1Point3[0] = 0.0; m_Points1Point3[1] = 0.0; m_Points1Point3[2] = 1.0;
  m_Points1Point4[0] = -5.0; m_Points1Point4[1] = 0.0; m_Points1Point4[2] = 0.0;
  m_Points1Point5[0] = 0.0; m_Points1Point5[1] = -3.0; m_Points1Point5[2] = 0.0;
  m_Points1Point6[0] = 0.0; m_Points1Point6[1] = 0.0; m_Points1Point6[2] = -1.0;
  ///suppose ellipsoid centred on (0,0,0) with semi size axes (7,4,2)
  m_Points2Point1[0] = 7.0; m_Points2Point1[1] = 0.0; m_Points2Point1[2] = 0.0;
  m_Points2Point2[0] = 0.0; m_Points2Point2[1] = 4.0; m_Points2Point2[2] = 0.0;
  m_Points2Point3[0] = 0.0; m_Points2Point3[1] = 0.0; m_Points2Point3[2] = 2.0;
  m_Points2Point4[0] = -7.0; m_Points2Point4[1] = 0.0; m_Points2Point4[2] = 0.0;
  m_Points2Point5[0] = 0.0; m_Points2Point5[1] = 0.0; m_Points2Point5[2] = -2.0;
  m_Points2Point6[0] = 0.0; m_Points2Point6[1] = -4.0; m_Points2Point6[2] = 0.0;
  ///suppose ellipsoid centred on (0,0,0) with semi size axes (6,10,4)
  m_Points3Point1[0] = 6.0; m_Points3Point1[1] = 0.0; m_Points3Point1[2] = 0.0;
  m_Points3Point2[0] = 0.0; m_Points3Point2[1] = 10.0; m_Points3Point2[2] = 0.0;
  m_Points3Point3[0] = 0.0; m_Points3Point3[1] = 0.0; m_Points3Point3[2] = 4.0;
  m_Points3Point4[0] = -6.0; m_Points3Point4[1] = 0.0; m_Points3Point4[2] = 0.0;
  m_Points3Point5[0] = 0.0; m_Points3Point5[1] = 0.0; m_Points3Point5[2] = -4.0;
  m_Points3Point6[0] = 0.0; m_Points3Point6[1] = -10.0; m_Points3Point6[2] = 0.0;
  ///suppose ellipsoid centred on (2,3,5) with semi size axes (5,3,1)
  m_Points4Point1[0] = -5.0+2; m_Points4Point1[1] = 0.0+3; m_Points4Point1[2] = 0.0+5;
  m_Points4Point2[0] = 0.0+2; m_Points4Point2[1] = -3.0+3; m_Points4Point2[2] = 0.0+5;
  m_Points4Point3[0] = 0.0+2; m_Points4Point3[1] = 0.0+3; m_Points4Point3[2] = -1.0+5;
  m_Points4Point4[0] = 5.0+2; m_Points4Point4[1] = 0.0+3; m_Points4Point4[2] = 0.0+5;
  m_Points4Point5[0] = 0.0+2; m_Points4Point5[1] = 3.0+3; m_Points4Point5[2] = 0.0+5;
  m_Points4Point6[0] = 0.0+2; m_Points4Point6[1] = 0.0+3; m_Points4Point6[2] = 1.0+5;
  ///suppose ellipsoid centred on (4,4,2) with semi size axes (5,3,1)
  m_Points5Point1[0] = 5.0+4; m_Points5Point1[1] = 0.0+4; m_Points5Point1[2] = 0.0+2;
  m_Points5Point2[0] = 0.0+4; m_Points5Point2[1] =3.0+4; m_Points5Point2[2] = 0.0+2;
  m_Points5Point3[0] = 0.0+4; m_Points5Point3[1] = 0.0+4; m_Points5Point3[2] = 1.0+2;
  m_Points5Point4[0] = -5.0+4; m_Points5Point4[1] = 0.0+4; m_Points5Point4[2] = 0.0+2;
  m_Points5Point5[0] = 0.0+4; m_Points5Point5[1] = -3.0+4; m_Points5Point5[2] = 0.0+2;
  m_Points5Point6[0] = 0.0+4; m_Points5Point6[1] = 0.0+4; m_Points5Point6[2] = -1.0+2;


  m_Points6Point1[0] = 5.0*2.5; m_Points6Point1[1] = 0.0*2.5; m_Points6Point1[2] = 0.0*2.5;
  m_Points6Point2[0] = 0.0*2.5; m_Points6Point2[1] = 3.0*2.5; m_Points6Point2[2] = 0.0*2.5;
  m_Points6Point3[0] = 0.0*2.5; m_Points6Point3[1] = 0.0*2.5; m_Points6Point3[2] = 1.0*2.5;
  m_Points6Point4[0] = -5.0*2.5; m_Points6Point4[1] = 0.0*2.5; m_Points6Point4[2] = 0.0*2.5;
  m_Points6Point5[0] = 0.0*2.5; m_Points6Point5[1] = 3.0*2.5; m_Points6Point5[2] = 0.0*2.5;
  m_Points6Point6[0] = 0.0*2.5; m_Points6Point6[1] = 0.0*2.5; m_Points6Point6[2] = -1.0*2.5;


  ///suppose ellipsoid centred on (1,5,0) with semi size axes (8,6,8)
  m_Points7Point1[0] = 8+1; m_Points7Point1[1] = 0.0+5; m_Points7Point1[2] = 0.0;
  m_Points7Point2[0] = 0.0+1; m_Points7Point2[1] = 6+5; m_Points7Point2[2] = 0.0;
  m_Points7Point3[0] = 0.0+1; m_Points7Point3[1] = 0.0+5; m_Points7Point3[2] = 8.0;
  m_Points7Point4[0] = -8+1; m_Points7Point4[1] = 0.0+5; m_Points7Point4[2] = 0.0;
  m_Points7Point5[0] = 0.0+1; m_Points7Point5[1] = -6.0+5; m_Points7Point5[2] = 0.0;
  m_Points7Point6[0] = 0+1; m_Points7Point6[1] = 0.0+5; m_Points7Point6[2] = -8.0;

  mafNEW(m_Cloud3);

  m_Cloud3->Open();
  m_Cloud3->SetName(_L("parent vertices cloud"));
  m_Cloud3->SetRadius(0.1);
  m_Cloud3->ReparentTo(this);

  mafNEW(m_Cloud);

  m_Cloud->Open();
  m_Cloud->SetName(_L("new landmark cloud"));
  m_Cloud->SetRadius(1.5);
  m_Cloud->ReparentTo(this);

  m_Cloud->AppendLandmark(m_Points1Point1[0], m_Points1Point1[1], m_Points1Point1[2], _R("l1"), false);
  m_Cloud->AppendLandmark(m_Points1Point2[0], m_Points1Point2[1], m_Points1Point2[2], _R("l2"), false);
  m_Cloud->AppendLandmark(m_Points1Point3[0], m_Points1Point3[1], m_Points1Point3[2], _R("l3"), false);
  m_Cloud->AppendLandmark(m_Points1Point4[0], m_Points1Point4[1], m_Points1Point4[2], _R("l4"), false);
  m_Cloud->AppendLandmark(m_Points1Point5[0], m_Points1Point5[1], m_Points1Point5[2], _R("l5"), false);
  m_Cloud->AppendLandmark(m_Points1Point6[0], m_Points1Point6[1], m_Points1Point6[2], _R("l6"), false);

  m_PointsR1 = 30.0;
  m_PointsR2 = 30.0;
  m_PointsR3 = 30.0;
  m_PointsPhiRes = 10.0;
  m_PointsThetaRes = 10.0;

//  ellipseQ = new mafdmEllipsoid(0,0,0,30,30,20);



  rotationMat = Matrix3d::Identity();

  m_SphereRadius = 2.0;
  m_SpherePhiRes = 10.0;
  m_SphereTheRes = 10.0;
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
  m_PointsR1 = 0;
  m_PointsR2 = 0;
  m_PointsR3 = 0;
  
  m_EllipsoidOrientationAxis = ID_X_AXIS;

	mafNEW(m_Transform);
	mafVMEOutputSurface *output = mafVMEOutputSurface::New(); // an output with no data
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);

  // attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	dpipe->SetInput(m_PolyData);
	SetDataPipe(dpipe);
}

//-------------------------------------------------------------------------
mafVMEQuadricSurfaceFitting::~mafVMEQuadricSurfaceFitting()
//-------------------------------------------------------------------------
{
	vtkDEL(m_PolyData);
  mafDEL(m_Transform);
	SetOutput(NULL);
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEQuadricSurfaceFitting::GetMaterial()
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
int mafVMEQuadricSurfaceFitting::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
	  mafVMEQuadricSurfaceFitting *vmeQuadricSurface = mafVMEQuadricSurfaceFitting::SafeDownCast(a);
	  m_Transform->SetMatrix(vmeQuadricSurface->m_Transform->GetMatrix());
	  this->m_GeometryType = vmeQuadricSurface->m_GeometryType;
	  this->m_SphereRadius = vmeQuadricSurface->m_SphereRadius;
	  this->m_SpherePhiRes = vmeQuadricSurface->m_SpherePhiRes;
	  this->m_SphereTheRes = vmeQuadricSurface->m_SphereTheRes;
	  this->m_ConeHeight = vmeQuadricSurface->m_ConeHeight;
	  this->m_ConeRadius = vmeQuadricSurface->m_ConeRadius;
	  this->m_ConeCapping = vmeQuadricSurface->m_ConeCapping;
	  this->m_ConeRes = vmeQuadricSurface->m_ConeRes;
	  this->m_ConeOrientationAxis = vmeQuadricSurface->m_ConeOrientationAxis;
	  this->m_CylinderHeight = vmeQuadricSurface->m_CylinderHeight;
	  this->m_CylinderRadius = vmeQuadricSurface->m_CylinderRadius;
	  this->m_CylinderRes = vmeQuadricSurface->m_CylinderRes;
	  this->m_CylinderOrientationAxis = vmeQuadricSurface->m_CylinderOrientationAxis;
	  this->m_CubeXLength = vmeQuadricSurface->m_CubeXLength;
	  this->m_CubeYLength = vmeQuadricSurface->m_CubeYLength;
	  this->m_CubeZLength = vmeQuadricSurface->m_CubeZLength;
	  this->m_PlaneXRes = vmeQuadricSurface->m_PlaneXRes;
	  this->m_PlaneYRes = vmeQuadricSurface->m_PlaneYRes;

	  this->m_PlaneOrigin[0] = vmeQuadricSurface->m_PlaneOrigin[0];
	  this->m_PlaneOrigin[1] = vmeQuadricSurface->m_PlaneOrigin[1];
	  this->m_PlaneOrigin[2] = vmeQuadricSurface->m_PlaneOrigin[2];

	  this->m_PlanePoint1[0] = vmeQuadricSurface->m_PlanePoint1[0];
	  this->m_PlanePoint1[1] = vmeQuadricSurface->m_PlanePoint1[1];
	  this->m_PlanePoint1[2] = vmeQuadricSurface->m_PlanePoint1[2];

	  this->m_PlanePoint2[0] = vmeQuadricSurface->m_PlanePoint2[0];
	  this->m_PlanePoint2[1] = vmeQuadricSurface->m_PlanePoint2[1];
	  this->m_PlanePoint2[2] = vmeQuadricSurface->m_PlanePoint2[2];

	  this->m_EllipsoidXLenght = vmeQuadricSurface->m_EllipsoidXLenght;
	  this->m_EllipsoidYLenght = vmeQuadricSurface->m_EllipsoidYLenght;
	  this->m_EllipsoidZLenght = vmeQuadricSurface->m_EllipsoidZLenght;
	  this->m_EllipsoidPhiRes = vmeQuadricSurface->m_EllipsoidPhiRes;
	  this->m_EllipsoidTheRes = vmeQuadricSurface->m_EllipsoidTheRes;
	  this->m_EllipsoidOrientationAxis = vmeQuadricSurface->m_EllipsoidOrientationAxis;

	  this->m_Points1Point1[0] = vmeQuadricSurface->m_Points1Point1[0]; this->m_Points1Point1[1] = vmeQuadricSurface->m_Points1Point1[1]; this->m_Points1Point1[2] = vmeQuadricSurface->m_Points1Point1[2];
	  this->m_Points1Point2[0] = vmeQuadricSurface->m_Points1Point2[0]; this->m_Points1Point2[1] = vmeQuadricSurface->m_Points1Point2[1]; this->m_Points1Point2[2] = vmeQuadricSurface->m_Points1Point2[2];
	  this->m_Points1Point3[0] = vmeQuadricSurface->m_Points1Point3[0]; this->m_Points1Point3[1] = vmeQuadricSurface->m_Points1Point3[1]; this->m_Points1Point3[2] = vmeQuadricSurface->m_Points1Point3[2];
	  this->m_Points1Point4[0] = vmeQuadricSurface->m_Points1Point4[0]; this->m_Points1Point4[1] = vmeQuadricSurface->m_Points1Point4[1]; this->m_Points1Point4[2] = vmeQuadricSurface->m_Points1Point4[2];
	  this->m_Points1Point5[0] = vmeQuadricSurface->m_Points1Point5[0]; this->m_Points1Point5[1] = vmeQuadricSurface->m_Points1Point5[1]; this->m_Points1Point5[2] = vmeQuadricSurface->m_Points1Point5[2];
	  this->m_Points1Point6[0] = vmeQuadricSurface->m_Points1Point6[0]; this->m_Points1Point6[1] = vmeQuadricSurface->m_Points1Point6[1]; this->m_Points1Point6[2] = vmeQuadricSurface->m_Points1Point6[2];

	  this->m_PointsR1 = vmeQuadricSurface->m_PointsR1;
	  this->m_PointsR2 = vmeQuadricSurface->m_PointsR2;
	  this->m_PointsR3 = vmeQuadricSurface->m_PointsR3;
	  this->m_PointsXRes = vmeQuadricSurface->m_PointsXRes;
	  this->m_PointsYRes = vmeQuadricSurface->m_PointsYRes;
	  this->m_PointsZRes = vmeQuadricSurface->m_PointsZRes;
	  this->m_PointsPhiRes = vmeQuadricSurface->m_PointsPhiRes;
	  this->m_PointsThetaRes = vmeQuadricSurface->m_PointsThetaRes;
	  this->rotationMat = vmeQuadricSurface->rotationMat;

    mafDataPipeCustom *dpipe = mafDataPipeCustom::SafeDownCast(GetDataPipe());
    if (dpipe)
    {
      dpipe->SetInput(m_PolyData);
    }
    InternalUpdate();
    return MAF_OK;
  }  
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
bool mafVMEQuadricSurfaceFitting::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    if (     
		m_Transform->GetMatrix() == ((mafVMEQuadricSurfaceFitting *)vme)->m_Transform->GetMatrix() &&
		this->m_GeometryType == ((mafVMEQuadricSurfaceFitting *)vme)->m_GeometryType &&
		this->m_SphereRadius == ((mafVMEQuadricSurfaceFitting *)vme)->m_SphereRadius &&
		this->m_SpherePhiRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_SpherePhiRes &&
		this->m_SphereTheRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_SphereTheRes &&
		this->m_ConeHeight == ((mafVMEQuadricSurfaceFitting *)vme)->m_ConeHeight  &&
		this->m_ConeRadius == ((mafVMEQuadricSurfaceFitting *)vme)->m_ConeRadius  &&
		this->m_ConeCapping == ((mafVMEQuadricSurfaceFitting *)vme)->m_ConeCapping  &&
		this->m_ConeRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_ConeRes &&
		this->m_ConeOrientationAxis == ((mafVMEQuadricSurfaceFitting *)vme)->m_ConeOrientationAxis &&
		this->m_CylinderHeight == ((mafVMEQuadricSurfaceFitting *)vme)->m_CylinderHeight  &&
		this->m_CylinderRadius == ((mafVMEQuadricSurfaceFitting *)vme)->m_CylinderRadius  &&
		this->m_CylinderRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_CylinderRes  &&
		this->m_CylinderOrientationAxis == ((mafVMEQuadricSurfaceFitting *)vme)->m_CylinderOrientationAxis  &&
		this->m_CubeXLength == ((mafVMEQuadricSurfaceFitting *)vme)->m_CubeXLength  &&
		this->m_CubeYLength == ((mafVMEQuadricSurfaceFitting *)vme)->m_CubeYLength  &&
		this->m_CubeZLength == ((mafVMEQuadricSurfaceFitting *)vme)->m_CubeZLength  &&
		this->m_PlaneXRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlaneXRes  &&
		this->m_PlaneYRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlaneYRes  &&

		this->m_PlaneOrigin[0] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlaneOrigin[0] &&
		this->m_PlaneOrigin[1] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlaneOrigin[1] &&
		this->m_PlaneOrigin[2] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlaneOrigin[2] &&

		this->m_PlanePoint1[0] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlanePoint1[0] &&
		this->m_PlanePoint1[1] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlanePoint1[1] &&
		this->m_PlanePoint1[2] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlanePoint1[2] &&

		this->m_PlanePoint2[0] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlanePoint2[0] &&
		this->m_PlanePoint2[1] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlanePoint2[1] &&
		this->m_PlanePoint2[2] == ((mafVMEQuadricSurfaceFitting *)vme)->m_PlanePoint2[2] &&

		this->m_EllipsoidXLenght == ((mafVMEQuadricSurfaceFitting *)vme)->m_EllipsoidXLenght &&
		this->m_EllipsoidYLenght == ((mafVMEQuadricSurfaceFitting *)vme)->m_EllipsoidYLenght &&
		this->m_EllipsoidZLenght == ((mafVMEQuadricSurfaceFitting *)vme)->m_EllipsoidZLenght &&
		this->m_EllipsoidPhiRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_EllipsoidPhiRes &&
		this->m_EllipsoidTheRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_EllipsoidTheRes &&
		this->m_EllipsoidOrientationAxis == ((mafVMEQuadricSurfaceFitting *)vme)->m_EllipsoidOrientationAxis &&

		this->m_PointsR1 == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsR1	&&
		this->m_PointsR2 == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsR2	&&
		this->m_PointsR3 == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsR3	&&
		this->m_PointsXRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsXRes	&&
		this->m_PointsYRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsYRes	&&
		this->m_PointsZRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsZRes	&&
		this->m_PointsPhiRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsPhiRes &&
		this->m_PointsThetaRes == ((mafVMEQuadricSurfaceFitting *)vme)->m_PointsThetaRes &&
		this->rotationMat == ((mafVMEQuadricSurfaceFitting *)vme)->rotationMat


      )
    {
      ret = true;
    }
  }
 return ret;
}
//-------------------------------------------------------------------------
mafVMEOutputSurface *mafVMEQuadricSurfaceFitting::GetSurfaceOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputSurface *)GetOutput();
}
//-------------------------------------------------------------------------
void mafVMEQuadricSurfaceFitting::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMEQuadricSurfaceFitting::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
void mafVMEQuadricSurfaceFitting::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
  mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
  kframes.push_back(t);
}
//-------------------------------------------------------------------------
mafGUI* mafVMEQuadricSurfaceFitting::CreateGui()
//-------------------------------------------------------------------------
{
  mafVME::CreateGui();
  if(m_Gui)
  {
    mafString geometryType[8] = {_R("Points_selection1"), _R("Points_selection2"), _R("Points_selection3"), _R("Points_selection4"), _R("Points_selection5"), _R("sphere"),_R("parent vertices") ,_R("Points_selection7")};
    m_Gui->Combo(ID_GEOMETRY_TYPE, _R(""), &m_GeometryType, 8, geometryType);
    m_Gui->Divider(2);
	
	CreateGuiSelectPoints();
    m_Gui->Divider(2);  
    CreateGuiPlane();
	m_Gui->Divider(2);
	CreateGuiCube();
	m_Gui->Divider(2);
	CreateGuiCylinder();
	m_Gui->Divider(2);
	CreateGuiCone();
	m_Gui->Divider(2);
	CreateGuiSphere();
    m_Gui->Divider(2);
    CreateGuiEllipsoid();
	m_Gui->Divider(2);
	CreateGuiParentVerticesResult();

    m_Gui->FitGui();
    
    EnableQuadricSurfaceGui(m_GeometryType);
    m_Gui->Update();
  }

  m_Gui->Divider();

  wxBusyInfo wait("GUI created...");
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEQuadricSurfaceFitting::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_GEOMETRY_TYPE:
      {  
        EnableQuadricSurfaceGui(m_GeometryType);
        m_Gui->Update();
        InternalUpdate();
        m_Gui->FitGui();
      }
		  break;
	  case CHANGE_VALUE_POINTS2:
	  case CHANGE_VALUE_POINTS3:
	  case CHANGE_VALUE_POINTS4:
	  case CHANGE_VALUE_POINTS5:
	  case CHANGE_VALUE_POINTS6:
	  case CHANGE_VALUE_POINTS7:
	  case CHANGE_VALUE_POINTS1:
	  case CHANGE_VALUE_POINTS10:
	  /*case  CHANGE_VALUE_SPHERE:
	  case CHANGE_VALUE_CUBE:
	  case CHANGE_VALUE_CONE:
	  case CHANGE_VALUE_CYLINDER:
	  case CHANGE_VALUE_PLANE:
	  case CHANGE_VALUE_ELLIPSOID:*/
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
void mafVMEQuadricSurfaceFitting::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------
void mafVMEQuadricSurfaceFitting::InternalUpdate()
//-----------------------------------------------------------------------
{
	switch(m_GeometryType)
	{
	case PARAMETRIC_SPHERE://6
	{
							  vtkMAFSmartPointer<vtkSphereSource> surf;
							  surf->SetRadius(m_SphereRadius);
							  surf->SetPhiResolution(m_SpherePhiRes);
							  surf->SetThetaResolution(m_SphereTheRes);
							  surf->Update();
							  m_PolyData->DeepCopy(surf->GetOutput());
							  m_PolyData->Update();
	}
		break;
	case POINTS_SELECTION_Ellipsoid5://cone
		{
										 points->Initialize();
										 points->InsertNextPoint(m_Points5Point1[0], m_Points5Point1[1], m_Points5Point1[2]);
										 m_Cloud->SetLandmark(_R("l1"), m_Points5Point1[0], m_Points5Point1[1], m_Points5Point1[2]);
										 points->InsertNextPoint(m_Points5Point2[0], m_Points5Point2[1], m_Points5Point2[2]);
										 m_Cloud->SetLandmark(_R("l2"), m_Points5Point2[0], m_Points5Point2[1], m_Points5Point2[2]);
										 points->InsertNextPoint(m_Points5Point3[0], m_Points5Point3[1], m_Points5Point3[2]);
										 m_Cloud->SetLandmark(_R("l3"), m_Points5Point3[0], m_Points5Point3[1], m_Points5Point3[2]);
										 points->InsertNextPoint(m_Points5Point4[0], m_Points5Point4[1], m_Points5Point4[2]);
										 m_Cloud->SetLandmark(_R("l4"), m_Points5Point4[0], m_Points5Point4[1], m_Points5Point4[2]);
										 points->InsertNextPoint(m_Points5Point5[0], m_Points5Point5[1], m_Points5Point5[2]);
										 m_Cloud->SetLandmark(_R("l5"), m_Points5Point5[0], m_Points5Point5[1], m_Points5Point5[2]);
										 points->InsertNextPoint(m_Points5Point6[0], m_Points5Point6[1], m_Points5Point6[2]);
										 m_Cloud->SetLandmark(_R("l6"), m_Points5Point6[0], m_Points5Point6[1], m_Points5Point6[2]);
										 m_Cloud->Update();

										 fitEllipsoid(points, &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
										 //ellipseQ->setCenter(m_PointsXRes, m_PointsYRes, m_PointsZRes);
										 //ellipseQ->setSize(m_PointsR1, m_PointsR2, m_PointsR3);
										 //ellipseQ->setResolution(m_PointsPhiRes, m_PointsThetaRes);
										 //m_PolyData->DeepCopy(ellipseQ->getOutput());

										 vtkMAFSmartPointer<vtkSphereSource> surf;
										 surf->SetCenter(m_PointsXRes * (m_PointsR2 / m_PointsR1), m_PointsYRes * 1, m_PointsZRes * (m_PointsR2 / m_PointsR3));
										 surf->SetRadius(m_PointsR2);
										 surf->SetPhiResolution(m_PointsPhiRes);
										 surf->SetThetaResolution(m_PointsThetaRes);
										 surf->Update();
										 vtkMAFSmartPointer<vtkTransform> t;
										 t->Scale(m_PointsR1 / m_PointsR2, 1, m_PointsR3 / m_PointsR2);
										 t->Update();
										 vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
										 ptf->SetTransform(t);
										 ptf->SetInput(surf->GetOutput());
										 ptf->Update();

										 m_PolyData->DeepCopy(ptf->GetOutput());
										 m_PolyData->Update();

										 m_Gui->Update();
	}
	break;
	case POINTS_SELECTION_Ellipsoid4://cylinder
	{
										 points->Initialize();
										 points->InsertNextPoint(m_Points4Point1[0], m_Points4Point1[1], m_Points4Point1[2]);
										 m_Cloud->SetLandmark(_R("l1"), m_Points4Point1[0], m_Points4Point1[1], m_Points4Point1[2]);
										 points->InsertNextPoint(m_Points4Point2[0], m_Points4Point2[1], m_Points4Point2[2]);
										 m_Cloud->SetLandmark(_R("l2"), m_Points4Point2[0], m_Points4Point2[1], m_Points4Point2[2]);
										 points->InsertNextPoint(m_Points4Point3[0], m_Points4Point3[1], m_Points4Point3[2]);
										 m_Cloud->SetLandmark(_R("l3"), m_Points4Point3[0], m_Points4Point3[1], m_Points4Point3[2]);
										 points->InsertNextPoint(m_Points4Point4[0], m_Points4Point4[1], m_Points4Point4[2]);
										 m_Cloud->SetLandmark(_R("l4"), m_Points4Point4[0], m_Points4Point4[1], m_Points4Point4[2]);
										 points->InsertNextPoint(m_Points4Point5[0], m_Points4Point5[1], m_Points4Point5[2]);
										 m_Cloud->SetLandmark(_R("l5"), m_Points4Point5[0], m_Points4Point5[1], m_Points4Point5[2]);
										 points->InsertNextPoint(m_Points4Point6[0], m_Points4Point6[1], m_Points4Point6[2]);
										 m_Cloud->SetLandmark(_R("l6"), m_Points4Point6[0], m_Points4Point6[1], m_Points4Point6[2]);
										 m_Cloud->Update();

										 fitEllipsoid(points, &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
										 //ellipseQ->setCenter(m_PointsXRes, m_PointsYRes, m_PointsZRes);
										 //ellipseQ->setSize(m_PointsR1, m_PointsR2, m_PointsR3);
										 //ellipseQ->setResolution(m_PointsPhiRes, m_PointsThetaRes);
										 //m_PolyData->DeepCopy(ellipseQ->getOutput());

										 vtkMAFSmartPointer<vtkSphereSource> surf;
										 surf->SetCenter(m_PointsXRes * (m_PointsR2 / m_PointsR1), m_PointsYRes * 1, m_PointsZRes * (m_PointsR2 / m_PointsR3));
										 surf->SetRadius(m_PointsR2);
										 surf->SetPhiResolution(m_PointsPhiRes);
										 surf->SetThetaResolution(m_PointsThetaRes);
										 surf->Update();
										 vtkMAFSmartPointer<vtkTransform> t;
										 t->Scale(m_PointsR1 / m_PointsR2, 1, m_PointsR3 / m_PointsR2);
										 t->Update();
										 vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
										 ptf->SetTransform(t);
										 ptf->SetInput(surf->GetOutput());
										 ptf->Update();

										 m_PolyData->DeepCopy(ptf->GetOutput());
										 m_PolyData->Update();

										 m_Gui->Update();

	}
	break;
	case PARENT_POINTS:
	{
						  points->Reset();
						  mafNode* parent = this->GetParent();
						  
						  m_Cloud3->RemoveAllChildren();
						  m_Cloud3->Update();
						  vtkPolyData* data;
						  //vtkPointSet *ptset = vtkPointSet::New();
						  vtkPoints* pts=vtkPoints::New();
						  if (parent->IsMAFType(mafVMESurfaceParametric) || parent->IsMAFType(mafVMESurface) || parent->IsMAFType(mafVMELandmarkCloud))
						  {

							  if (parent->IsMAFType(mafVMESurfaceParametric))
							  {
								  data = ((mafVMESurfaceParametric*)parent)->GetSurfaceOutput()->GetSurfaceData();
								  data->Update();

								  fitEllipsoid(data->GetPoints(), &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
							  
							  }
							  if (parent->IsMAFType(mafVMESurface))
							  {
								  data = ((mafVMESurface*)parent)->GetSurfaceOutput()->GetSurfaceData();
								  data->Update();
								  fitEllipsoid(data->GetPoints(), &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
							  }
							  if (parent->IsMAFType(mafVMELandmarkCloud))
							  {
								 
								  
								  wxBusyInfo wait((_R("mafvme landmarkcloud Name") + ((mafVMELandmarkCloud*)parent)->GetName() + _R(" ")).toWx() );
								  Sleep(2500);

								  std::string str20 = "loop " + boost::lexical_cast<std::string>(((mafVMELandmarkCloud*)parent)->GetNumberOfLandmarks());
								  int nbrLndMarks = ((mafVMELandmarkCloud*)parent)->GetNumberOfLandmarks();
								  for (int i = 0; i<nbrLndMarks; i++)
								  { 
									  /*std::string str2 = "loop " + boost::lexical_cast<std::string>(i)+"//"+str20;
									  wxString mafs2 = str2.c_str();
									  wxBusyInfo wait00(mafs2);
									  Sleep(0500);*/
									  double posPt[3];
									  ((mafVMELandmarkCloud*)parent)->GetPoint(i, posPt, -1);
									  
									  pts->InsertNextPoint(posPt);
									  
								  }

								  

								//  data->SetPoints(pts);
								  
								  
								  if (((mafVMELandmarkCloud*)parent)->GetPointSetOutput()->GetPointSetData())
								  {
									  wxBusyInfo wait1("data ok ");
									  Sleep(2500);
									  data = ((mafVMELandmarkCloud*)parent)->GetPointSetOutput()->GetPointSetData();
									  data->Update();
									  fitEllipsoid(data->GetPoints(), &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
								  }
								  else
								  {
									  wxBusyInfo wait1("data pb ");
									  Sleep(2500);
									  if (pts->GetNumberOfPoints()>0)
										fitEllipsoid(pts, &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
									  else
									  {
										  wxBusyInfo wait1("pts pb ");
										  Sleep(2500);
									  }
								  }


								  vtkDEL(pts);

							  }
							  else
							  {
								  wxBusyInfo wait("input error ");
								  Sleep(2500);
							  }
							 
							 	  
							  
							
								 
							  
							 
							  m_GuiParentVerticesResult->Update();
							  m_Gui->Update();


							  std::string str = "ellipse computed param" + boost::lexical_cast<std::string>(m_PointsR1)+" " + boost::lexical_cast<std::string>(m_PointsR2)+" " + boost::lexical_cast<std::string>(m_PointsR3);
							  wxString mafs = str.c_str();
							  wxBusyInfo wait(mafs);
							  Sleep(2500);

							  vtkMAFSmartPointer<vtkSphereSource> surf;
							  surf->SetCenter(m_PointsXRes * (m_PointsR2 / m_PointsR1), m_PointsYRes * 1, m_PointsZRes * (m_PointsR2 / m_PointsR3));
							  surf->SetRadius(m_PointsR2);
							  surf->SetPhiResolution(m_PointsPhiRes);
							  surf->SetThetaResolution(m_PointsThetaRes);
							  surf->Update();
							  vtkMAFSmartPointer<vtkTransform> t;
							  t->Scale(m_PointsR1 / m_PointsR2, 1, m_PointsR3 / m_PointsR2);
							  t->Update();
							  vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
							  ptf->SetTransform(t);
							  ptf->SetInput(surf->GetOutput());
							  ptf->Update();

							  m_PolyData->DeepCopy(ptf->GetOutput());
							  m_PolyData->Update();

							  m_Gui->Update();


						  }


						 

	
	}
	break;
	case POINTS_SELECTION_Ellipsoid3://cube
	{
										 points->Initialize();
										 points->InsertNextPoint(m_Points3Point1[0], m_Points3Point1[1], m_Points3Point1[2]);
										 m_Cloud->SetLandmark(_R("l1"), m_Points3Point1[0], m_Points3Point1[1], m_Points3Point1[2]);
										 points->InsertNextPoint(m_Points3Point2[0], m_Points3Point2[1], m_Points3Point2[2]);
										 m_Cloud->SetLandmark(_R("l2"), m_Points3Point2[0], m_Points3Point2[1], m_Points3Point2[2]);
										 points->InsertNextPoint(m_Points3Point3[0], m_Points3Point3[1], m_Points3Point3[2]);
										 m_Cloud->SetLandmark(_R("l3"), m_Points3Point3[0], m_Points3Point3[1], m_Points3Point3[2]);
										 points->InsertNextPoint(m_Points3Point4[0], m_Points3Point4[1], m_Points3Point4[2]);
										 m_Cloud->SetLandmark(_R("l4"), m_Points3Point4[0], m_Points3Point4[1], m_Points3Point4[2]);
										 points->InsertNextPoint(m_Points3Point5[0], m_Points3Point5[1], m_Points3Point5[2]);
										 m_Cloud->SetLandmark(_R("l5"), m_Points3Point5[0], m_Points3Point5[1], m_Points3Point5[2]);
										 points->InsertNextPoint(m_Points3Point6[0], m_Points3Point6[1], m_Points3Point6[2]);
										 m_Cloud->SetLandmark(_R("l6"), m_Points3Point6[0], m_Points3Point6[1], m_Points3Point6[2]);
										 m_Cloud->Update();

										 fitEllipsoid(points, &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
										 //ellipseQ->setCenter(m_PointsXRes, m_PointsYRes, m_PointsZRes);
										 //ellipseQ->setSize(m_PointsR1, m_PointsR2, m_PointsR3);
										 //ellipseQ->setResolution(m_PointsPhiRes, m_PointsThetaRes);
										 //m_PolyData->DeepCopy(ellipseQ->getOutput());

										 vtkMAFSmartPointer<vtkSphereSource> surf;
										 surf->SetCenter(m_PointsXRes * (m_PointsR2 / m_PointsR1), m_PointsYRes * 1, m_PointsZRes * (m_PointsR2 / m_PointsR3));
										 surf->SetRadius(m_PointsR2);
										 surf->SetPhiResolution(m_PointsPhiRes);
										 surf->SetThetaResolution(m_PointsThetaRes);
										 surf->Update();
										 vtkMAFSmartPointer<vtkTransform> t;
										 t->Scale(m_PointsR1 / m_PointsR2, 1, m_PointsR3 / m_PointsR2);
										 t->Update();
										 vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
										 ptf->SetTransform(t);
										 ptf->SetInput(surf->GetOutput());
										 ptf->Update();

										 m_PolyData->DeepCopy(ptf->GetOutput());
										 m_PolyData->Update();

										 m_Gui->Update();
	}
	break;
	case POINTS_SELECTION_Ellipsoid2://plane
	{
										 points->Initialize();
										 points->InsertNextPoint(m_Points2Point1[0], m_Points2Point1[1], m_Points2Point1[2]);
										 m_Cloud->SetLandmark(_R("l1"), m_Points2Point1[0], m_Points2Point1[1], m_Points2Point1[2]);
										 points->InsertNextPoint(m_Points2Point2[0], m_Points2Point2[1], m_Points2Point2[2]);
										 m_Cloud->SetLandmark(_R("l2"), m_Points2Point2[0], m_Points2Point2[1], m_Points2Point2[2]);
										 points->InsertNextPoint(m_Points2Point3[0], m_Points2Point3[1], m_Points2Point3[2]);
										 m_Cloud->SetLandmark(_R("l3"), m_Points2Point3[0], m_Points2Point3[1], m_Points2Point3[2]);
										 points->InsertNextPoint(m_Points2Point4[0], m_Points2Point4[1], m_Points2Point4[2]);
										 m_Cloud->SetLandmark(_R("l4"), m_Points2Point4[0], m_Points2Point4[1], m_Points2Point4[2]);
										 points->InsertNextPoint(m_Points2Point5[0], m_Points2Point5[1], m_Points2Point5[2]);
										 m_Cloud->SetLandmark(_R("l5"), m_Points2Point5[0], m_Points2Point5[1], m_Points2Point5[2]);
										 points->InsertNextPoint(m_Points2Point6[0], m_Points2Point6[1], m_Points2Point6[2]);
										 m_Cloud->SetLandmark(_R("l6"), m_Points2Point6[0], m_Points2Point6[1], m_Points2Point6[2]);
										 m_Cloud->Update();

										 fitEllipsoid(points, &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
										 //ellipseQ->setCenter(m_PointsXRes, m_PointsYRes, m_PointsZRes);
										 //ellipseQ->setSize(m_PointsR1, m_PointsR2, m_PointsR3);
										 //ellipseQ->setResolution(m_PointsPhiRes, m_PointsThetaRes);
										 //m_PolyData->DeepCopy(ellipseQ->getOutput());

										 vtkMAFSmartPointer<vtkSphereSource> surf;
										 surf->SetCenter(m_PointsXRes * (m_PointsR2 / m_PointsR1), m_PointsYRes * 1, m_PointsZRes * (m_PointsR2 / m_PointsR3));
										 surf->SetRadius(m_PointsR2);
										 surf->SetPhiResolution(m_PointsPhiRes);
										 surf->SetThetaResolution(m_PointsThetaRes);
										 surf->Update();
										 vtkMAFSmartPointer<vtkTransform> t;
										 t->Scale(m_PointsR1 / m_PointsR2, 1, m_PointsR3 / m_PointsR2);
										 t->Update();
										 vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
										 ptf->SetTransform(t);
										 ptf->SetInput(surf->GetOutput());
										 ptf->Update();

										 m_PolyData->DeepCopy(ptf->GetOutput());
										 m_PolyData->Update();

										 m_Gui->Update();
	}
	break;
	case POINTS_SELECTION_Ellipsoid7://ellipsoid
    {
										 points->Initialize();
										points->InsertNextPoint(m_Points7Point1[0], m_Points7Point1[1], m_Points7Point1[2]);
										m_Cloud->SetLandmark(_R("l1"), m_Points7Point1[0], m_Points7Point1[1], m_Points7Point1[2]);
										points->InsertNextPoint(m_Points7Point2[0], m_Points7Point2[1], m_Points7Point2[2]);
										m_Cloud->SetLandmark(_R("l2"), m_Points7Point2[0], m_Points7Point2[1], m_Points7Point2[2]);
										points->InsertNextPoint(m_Points7Point3[0], m_Points7Point3[1], m_Points7Point3[2]);
										m_Cloud->SetLandmark(_R("l3"), m_Points7Point3[0], m_Points7Point3[1], m_Points7Point3[2]);
										points->InsertNextPoint(m_Points7Point4[0], m_Points7Point4[1], m_Points7Point4[2]);
										m_Cloud->SetLandmark(_R("l4"), m_Points7Point4[0], m_Points7Point4[1], m_Points7Point4[2]);
										points->InsertNextPoint(m_Points7Point5[0], m_Points7Point5[1], m_Points7Point5[2]);
										m_Cloud->SetLandmark(_R("l5"), m_Points7Point5[0], m_Points7Point5[1], m_Points7Point5[2]);
										points->InsertNextPoint(m_Points7Point6[0], m_Points7Point6[1], m_Points7Point6[2]);
										m_Cloud->SetLandmark(_R("l6"), m_Points7Point6[0], m_Points7Point6[1], m_Points7Point6[2]);
										m_Cloud->Update();

										fitEllipsoid(points, &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
										//ellipseQ->setCenter(m_PointsXRes, m_PointsYRes, m_PointsZRes);
										//ellipseQ->setSize(m_PointsR1, m_PointsR2, m_PointsR3);
										//ellipseQ->setResolution(m_PointsPhiRes, m_PointsThetaRes);
										//m_PolyData->DeepCopy(ellipseQ->getOutput());

										vtkMAFSmartPointer<vtkSphereSource> surf;
										surf->SetCenter(m_PointsXRes * (m_PointsR2 / m_PointsR1), m_PointsYRes * 1, m_PointsZRes * (m_PointsR2 / m_PointsR3)); 
										surf->SetRadius(m_PointsR2);
										surf->SetPhiResolution(m_PointsPhiRes);
										surf->SetThetaResolution(m_PointsThetaRes);
										surf->Update();
										vtkMAFSmartPointer<vtkTransform> t;
										t->Scale(m_PointsR1 / m_PointsR2, 1, m_PointsR3 / m_PointsR2);
										t->Update();
										vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
										ptf->SetTransform(t);
										ptf->SetInput(surf->GetOutput());
										ptf->Update();

										m_PolyData->DeepCopy(ptf->GetOutput());
										m_PolyData->Update();

										m_Gui->Update();

    }
    break;

	case POINTS_SELECTION_Ellipsoid1://points
    {
										 points->Initialize();
										points->InsertNextPoint(m_Points1Point1[0], m_Points1Point1[1], m_Points1Point1[2]);
										m_Cloud->SetLandmark(_R("l1"), m_Points1Point1[0], m_Points1Point1[1], m_Points1Point1[2]);
										points->InsertNextPoint(m_Points1Point2[0], m_Points1Point2[1], m_Points1Point2[2]);
										m_Cloud->SetLandmark(_R("l2"), m_Points1Point2[0], m_Points1Point2[1], m_Points1Point2[2]);
										points->InsertNextPoint(m_Points1Point3[0], m_Points1Point3[1], m_Points1Point3[2]);
										m_Cloud->SetLandmark(_R("l3"), m_Points1Point3[0], m_Points1Point3[1], m_Points1Point3[2]);
										points->InsertNextPoint(m_Points1Point4[0], m_Points1Point4[1], m_Points1Point4[2]);
										m_Cloud->SetLandmark(_R("l4"), m_Points1Point4[0], m_Points1Point4[1], m_Points1Point4[2]);
										points->InsertNextPoint(m_Points1Point5[0], m_Points1Point5[1], m_Points1Point5[2]);
										m_Cloud->SetLandmark(_R("l5"), m_Points1Point5[0], m_Points1Point5[1], m_Points1Point5[2]);
										points->InsertNextPoint(m_Points1Point6[0], m_Points1Point6[1], m_Points1Point6[2]);
										m_Cloud->SetLandmark(_R("l6"), m_Points1Point6[0], m_Points1Point6[1], m_Points1Point6[2]);
										m_Cloud->Update();

										fitEllipsoid(points, &m_PointsXRes, &m_PointsYRes, &m_PointsZRes, &m_PointsR1, &m_PointsR2, &m_PointsR3, &rotationMat);
										//ellipseQ->setCenter(m_PointsXRes, m_PointsYRes, m_PointsZRes);
										//ellipseQ->setSize(m_PointsR1, m_PointsR2, m_PointsR3);
										//ellipseQ->setResolution(m_PointsPhiRes, m_PointsThetaRes);
										//m_PolyData->DeepCopy(ellipseQ->getOutput());
										


										vtkMAFSmartPointer<vtkSphereSource> surf;
										surf->SetCenter(m_PointsXRes * (m_PointsR2 / m_PointsR1), m_PointsYRes * 1, m_PointsZRes * (m_PointsR2 / m_PointsR3));
										surf->SetRadius(m_PointsR2);
										surf->SetPhiResolution(m_PointsPhiRes);
										surf->SetThetaResolution(m_PointsThetaRes);
										surf->Update();
										vtkMAFSmartPointer<vtkTransform> t;
										t->Scale(m_PointsR1 / m_PointsR2, 1, m_PointsR3 / m_PointsR2);
										t->Update();
										vtkMAFSmartPointer<vtkTransformPolyDataFilter> ptf;
										ptf->SetTransform(t);
										ptf->SetInput(surf->GetOutput());
										ptf->Update();

										m_PolyData->DeepCopy(ptf->GetOutput());
										m_PolyData->Update();

										m_Gui->Update();

	
    }
    break;
	}

  /*mafEvent ev(this,CAMERA_UPDATE);
  this->ForwardUpEvent(&ev);*/
}
//-----------------------------------------------------------------------
int mafVMEQuadricSurfaceFitting::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
	if (Superclass::InternalStore(parent)==MAF_OK)
	{
    if (
		parent->StoreMatrix(_R("Transform"),&m_Transform->GetMatrix()) == MAF_OK && 
		parent->StoreInteger(_R("Geometry"), 	m_GeometryType) == MAF_OK &&
    parent->StoreDouble(_R("ShereRadius"), m_SphereRadius) == MAF_OK &&
    parent->StoreDouble(_R("SpherePhiRes"),m_SpherePhiRes) == MAF_OK &&
    parent->StoreDouble(_R("SphereThetaRes"),m_SphereTheRes) == MAF_OK &&
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
    parent->StoreVectorN(_R("PlaneOrigin"),m_PlaneOrigin,3) == MAF_OK &&
    parent->StoreVectorN(_R("PlanePoint1"),m_PlanePoint1,3) == MAF_OK &&
    parent->StoreVectorN(_R("PlanePoint2"),m_PlanePoint2,3) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidXLenght"),m_EllipsoidXLenght) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidYLenght"),m_EllipsoidYLenght) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidZLenght"),m_EllipsoidZLenght) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidTheRes"),m_EllipsoidTheRes) == MAF_OK &&
    parent->StoreDouble(_R("EllipsoidPhiRes"),m_EllipsoidPhiRes) == MAF_OK &&
    parent->StoreInteger(_R("EllipsoidOrientationAxis"),m_CylinderOrientationAxis) == MAF_OK &&
	parent->StoreDouble(_R("PointsXLenght"), m_PointsR1) == MAF_OK &&
	parent->StoreDouble(_R("PointsYLenght"), m_PointsR2) == MAF_OK &&
	parent->StoreDouble(_R("PointsZLenght"), m_PointsR3) == MAF_OK &&
	parent->StoreDouble(_R("PointsXCenter"), m_PointsXRes) == MAF_OK &&
	parent->StoreDouble(_R("PointsYCenter"), m_PointsYRes) == MAF_OK &&
	parent->StoreDouble(_R("PointsZCenter"), m_PointsZRes) == MAF_OK &&
	parent->StoreDouble(_R("PointsTheRes"), m_PointsThetaRes) == MAF_OK &&
	parent->StoreDouble(_R("PointsPhiRes"), m_PointsPhiRes) == MAF_OK
	
	//&& parent->StoreMatrix(_R("PointsRotMat"), rotationMat) == MAF_OK
    )
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMEQuadricSurfaceFitting::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalRestore(node)==MAF_OK)
	{
    mafMatrix matrix;
    if (node->RestoreMatrix(_R("Transform"),&matrix)==MAF_OK)
    {
      m_Transform->SetMatrix(matrix); 
      node->RestoreInteger(_R("Geometry"),m_GeometryType);
      node->RestoreDouble(_R("ShereRadius"),m_SphereRadius);
      node->RestoreDouble(_R("SpherePhiRes"),m_SpherePhiRes);
      node->RestoreDouble(_R("SphereThetaRes"),m_SphereTheRes);
      node->RestoreDouble(_R("ConeHieght"),m_ConeHeight);
      node->RestoreDouble(_R("ConeRadius"),m_ConeRadius);
      node->RestoreInteger(_R("ConeCapping"),m_ConeCapping);
      node->RestoreDouble(_R("ConeRes"),m_ConeRes);
      node->RestoreInteger(_R("ConeOrientationAxis"),m_ConeOrientationAxis);
      node->RestoreDouble(_R("CylinderHeight"),m_CylinderHeight);
      node->RestoreDouble(_R("CylinderRadius"),m_CylinderRadius);
      node->RestoreDouble(_R("CylinderRes"),m_CylinderRes);
      node->RestoreInteger(_R("CylinderOrientationAxis"),m_CylinderOrientationAxis);
      node->RestoreDouble(_R("CubeXLength"),m_CubeXLength);
      node->RestoreDouble(_R("CubeYLength"),m_CubeYLength);
      node->RestoreDouble(_R("CubeZLength"),m_CubeZLength);
      node->RestoreDouble(_R("PlaneXRes"),m_PlaneXRes);
      node->RestoreDouble(_R("PlaneYRes"),m_PlaneYRes);
      node->RestoreVectorN(_R("PlaneOrigin"),m_PlaneOrigin,3);
      node->RestoreVectorN(_R("PlanePoint1"),m_PlanePoint1,3);
      node->RestoreVectorN(_R("PlanePoint2"),m_PlanePoint2,3);
      node->RestoreDouble(_R("EllipsoidXLenght"),m_EllipsoidXLenght);
      node->RestoreDouble(_R("EllipsoidYLenght"),m_EllipsoidYLenght);
      node->RestoreDouble(_R("EllipsoidZLenght"),m_EllipsoidZLenght);
      node->RestoreDouble(_R("EllipsoidTheRes"),m_EllipsoidTheRes);
      node->RestoreDouble(_R("EllipsoidPhiRes"),m_EllipsoidPhiRes);
      node->RestoreInteger(_R("EllipsoidOrientationAxis"),m_CylinderOrientationAxis);
	  node->RestoreDouble(_R("PointsXLenght"), m_PointsR1);
	  node->RestoreDouble(_R("PointsYLenght"), m_PointsR2);
	  node->RestoreDouble(_R("PointsZLenght"), m_PointsR3);
	  node->RestoreDouble(_R("PointsXCenter"), m_PointsXRes);
	  node->RestoreDouble(_R("PointsYCenter"), m_PointsYRes);
	  node->RestoreDouble(_R("PointsZCenter"), m_PointsZRes);
	  node->RestoreDouble(_R("PointsTheRes"), m_PointsThetaRes);
	  node->RestoreDouble(_R("PointsPhiRes"), m_PointsPhiRes);
	  //node->RetoreMatrix(_R("PointsRotMat"), rotationMat);
      return MAF_OK;
    }
	}
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMEQuadricSurfaceFitting::SetGeometryType(int parametricSurfaceTypeID)
//-------------------------------------------------------------------------
{
  m_GeometryType = parametricSurfaceTypeID;
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEQuadricSurfaceFitting::SetSphereRadius(double radius)
//-------------------------------------------------------------------------
{
  m_SphereRadius = radius;
  Modified();
}
//-------------------------------------------------------------------------
char** mafVMEQuadricSurfaceFitting::GetIcon()
//-------------------------------------------------------------------------
{
  #include "mafVMEProcedural.xpm"
  return mafVMEProcedural_xpm;
}



void mafVMEQuadricSurfaceFitting::CreateGuiSelectPoints()
{
	m_GuiSelectPoints = new mafGUI(this);
	m_GuiSelectPoints->Label(_R("Points Selection1"));
	
			
		
	m_GuiSelectPoints->Vector(CHANGE_VALUE_POINTS1, _L("l1"), m_Points1Point1);
	m_GuiSelectPoints->Vector(CHANGE_VALUE_POINTS1, _L("l2"), m_Points1Point2);
	m_GuiSelectPoints->Vector(CHANGE_VALUE_POINTS1, _L("l3"), m_Points1Point3); 
	m_GuiSelectPoints->Vector(CHANGE_VALUE_POINTS1, _L("l4"), m_Points1Point4);
	m_GuiSelectPoints->Vector(CHANGE_VALUE_POINTS1, _L("l5"), m_Points1Point5);
	m_GuiSelectPoints->Vector(CHANGE_VALUE_POINTS1, _L("l6"), m_Points1Point6);
	
	m_GuiSelectPoints->Double(CHANGE_VALUE_POINTS1, _L("Phi Res"), &m_PointsPhiRes);
	m_GuiSelectPoints->Double(CHANGE_VALUE_POINTS1, _L("Theta Res"), &m_PointsThetaRes);
	m_Cloud->Update();
	assert(m_Gui);
	m_Gui->AddGui(m_GuiSelectPoints);
	m_Gui->Update();
}
void mafVMEQuadricSurfaceFitting::CreateGuiPlane()
{
	m_GuiPlane = new mafGUI(this);
	
	m_GuiPlane->Label(_R("Points Selection2"));
	m_GuiPlane->Vector(CHANGE_VALUE_POINTS2, _L("l1"), m_Points2Point1);
	m_GuiPlane->Vector(CHANGE_VALUE_POINTS2, _L("l2"), m_Points2Point2);
	m_GuiPlane->Vector(CHANGE_VALUE_POINTS2, _L("l3"), m_Points2Point3);
	m_GuiPlane->Vector(CHANGE_VALUE_POINTS2, _L("l4"), m_Points2Point4);
	m_GuiPlane->Vector(CHANGE_VALUE_POINTS2, _L("l5"), m_Points2Point5);
	m_GuiPlane->Vector(CHANGE_VALUE_POINTS2, _L("l6"), m_Points2Point6);

	m_GuiPlane->Double(CHANGE_VALUE_POINTS2, _L("Phi Res"), &m_PointsPhiRes);
	m_GuiPlane->Double(CHANGE_VALUE_POINTS2, _L("Theta Res"), &m_PointsThetaRes);
	m_Cloud->Update();
	assert(m_Gui);
	m_Gui->AddGui(m_GuiPlane);
	m_Gui->Update();
}

void mafVMEQuadricSurfaceFitting::CreateGuiCube()
{
	  m_GuiCube = new mafGUI(this);
	  m_GuiCube->Label(_R("Points Selection3"));
	  m_GuiCube->Vector(CHANGE_VALUE_POINTS3, _L("l1"), m_Points3Point1);
	  m_GuiCube->Vector(CHANGE_VALUE_POINTS3, _L("l2"), m_Points3Point2);
	  m_GuiCube->Vector(CHANGE_VALUE_POINTS3, _L("l3"), m_Points3Point3);
	  m_GuiCube->Vector(CHANGE_VALUE_POINTS3, _L("l4"), m_Points3Point4);
	  m_GuiCube->Vector(CHANGE_VALUE_POINTS3, _L("l5"), m_Points3Point5);
	  m_GuiCube->Vector(CHANGE_VALUE_POINTS3, _L("l6"), m_Points3Point6);


	  m_GuiCube->Double(CHANGE_VALUE_POINTS3, _L("Phi Res"), &m_PointsPhiRes);
	  m_GuiCube->Double(CHANGE_VALUE_POINTS3, _L("Theta Res"), &m_PointsThetaRes);
	  m_Cloud->Update();
	  assert(m_Gui);
	  m_Gui->AddGui(m_GuiCube);
	  m_Gui->Update();
}

void mafVMEQuadricSurfaceFitting::CreateGuiCylinder()
{
	m_GuiCylinder = new mafGUI(this);
	
	m_GuiCylinder->Label(_R("Points Selection4"));
	m_GuiCylinder->Vector(CHANGE_VALUE_POINTS4, _L("l1"), m_Points4Point1);
	m_GuiCylinder->Vector(CHANGE_VALUE_POINTS4, _L("l2"), m_Points4Point2);
	m_GuiCylinder->Vector(CHANGE_VALUE_POINTS4, _L("l3"), m_Points4Point3);
	m_GuiCylinder->Vector(CHANGE_VALUE_POINTS4, _L("l4"), m_Points4Point4);
	m_GuiCylinder->Vector(CHANGE_VALUE_POINTS4, _L("l5"), m_Points4Point5);
	m_GuiCylinder->Vector(CHANGE_VALUE_POINTS4, _L("l6"), m_Points4Point6);

	m_GuiCylinder->Double(CHANGE_VALUE_POINTS4, _L("Phi Res"), &m_PointsPhiRes);
	m_GuiCylinder->Double(CHANGE_VALUE_POINTS4, _L("Theta Res"), &m_PointsThetaRes);
	m_Cloud->Update();
	assert(m_Gui);
	m_Gui->AddGui(m_GuiCylinder);
	m_Gui->Update();

}

void mafVMEQuadricSurfaceFitting::CreateGuiCone()
{
	m_GuiCone = new mafGUI(this);
	m_GuiCone->Label(_R("Points Selection5"));
	m_GuiCone->Vector(CHANGE_VALUE_POINTS5, _L("l1"), m_Points5Point1);
	m_GuiCone->Vector(CHANGE_VALUE_POINTS5, _L("l2"), m_Points5Point2);
	m_GuiCone->Vector(CHANGE_VALUE_POINTS5, _L("l3"), m_Points5Point3);
	m_GuiCone->Vector(CHANGE_VALUE_POINTS5, _L("l4"), m_Points5Point4);
	m_GuiCone->Vector(CHANGE_VALUE_POINTS5, _L("l5"), m_Points5Point5);
	m_GuiCone->Vector(CHANGE_VALUE_POINTS5, _L("l6"), m_Points5Point6);

	m_GuiCone->Double(CHANGE_VALUE_POINTS5, _L("Phi Res"), &m_PointsPhiRes);
	m_GuiCone->Double(CHANGE_VALUE_POINTS5, _L("Theta Res"), &m_PointsThetaRes);
	m_Cloud->Update();
	assert(m_Gui);
	m_Gui->AddGui(m_GuiCone);
	m_Gui->Update();
}

void mafVMEQuadricSurfaceFitting::CreateGuiSphere()
{
	m_GuiSphere = new mafGUI(this);
	m_GuiSphere->Label(_R("Sphere"));
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _L("Radius"), &m_SphereRadius);
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _L("Phi res"), &m_SpherePhiRes);
	m_GuiSphere->Double(CHANGE_VALUE_SPHERE, _L("Theta res"), &m_SphereTheRes);
	assert(m_Gui);
	m_Gui->AddGui(m_GuiSphere);
}

void mafVMEQuadricSurfaceFitting::CreateGuiEllipsoid()
{
	m_GuiEllipsoid = new mafGUI(this);
	m_GuiEllipsoid->Label(_R("Points Selection7"));
	m_GuiEllipsoid->Vector(CHANGE_VALUE_POINTS7, _L("l1"), m_Points7Point1);
	m_GuiEllipsoid->Vector(CHANGE_VALUE_POINTS7, _L("l2"), m_Points7Point2);
	m_GuiEllipsoid->Vector(CHANGE_VALUE_POINTS7, _L("l3"), m_Points7Point3);
	m_GuiEllipsoid->Vector(CHANGE_VALUE_POINTS7, _L("l4"), m_Points7Point4);
	m_GuiEllipsoid->Vector(CHANGE_VALUE_POINTS7, _L("l5"), m_Points7Point5);
	m_GuiEllipsoid->Vector(CHANGE_VALUE_POINTS7, _L("l6"), m_Points7Point6);

	m_GuiEllipsoid->Double(CHANGE_VALUE_POINTS7, _L("Phi Res"), &m_PointsPhiRes);
	m_GuiEllipsoid->Double(CHANGE_VALUE_POINTS7, _L("Theta Res"), &m_PointsThetaRes);
	m_Cloud->Update();
	assert(m_Gui);
	m_Gui->AddGui(m_GuiEllipsoid);
	m_Gui->Update();
}
void mafVMEQuadricSurfaceFitting::CreateGuiParentVerticesResult()
{
	m_GuiParentVerticesResult = new mafGUI(this);
	m_GuiParentVerticesResult->Label(_R("ParentVerticesResult"));
	m_GuiParentVerticesResult->Double(CHANGE_VALUE_POINTS10, _L("R1"), &m_PointsR1);
	m_GuiParentVerticesResult->Double(CHANGE_VALUE_POINTS10, _L("R2"), &m_PointsR2);
	m_GuiParentVerticesResult->Double(CHANGE_VALUE_POINTS10, _L("R3"), &m_PointsR3);

	m_GuiParentVerticesResult->Double(CHANGE_VALUE_POINTS10, _L("Phi Res"), &m_PointsPhiRes);
	m_GuiParentVerticesResult->Double(CHANGE_VALUE_POINTS10, _L("Theta Res"), &m_PointsThetaRes);
	
	assert(m_Gui);
	m_Gui->AddGui(m_GuiParentVerticesResult);
	m_Gui->Update();
}


void mafVMEQuadricSurfaceFitting::EnableGuiPlane()
{
	m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, false);
	m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, true);
	m_GuiCube->Enable(CHANGE_VALUE_POINTS3, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, false);
	m_GuiCone->Enable(CHANGE_VALUE_POINTS5, false);
 // m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, false);
  m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, false);
}

void mafVMEQuadricSurfaceFitting::EnableGuiCube()
{
	m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, false);
	m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, false);
	m_GuiCube->Enable(CHANGE_VALUE_POINTS3, true);
	m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, false);
	m_GuiCone->Enable(CHANGE_VALUE_POINTS5, false);
 // m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, false);
  m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, false);
}

void mafVMEQuadricSurfaceFitting::EnableGuiCylinder()
{
	m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, false);
	m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, false);
	m_GuiCube->Enable(CHANGE_VALUE_POINTS3, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, true);
  m_GuiCone->Enable(CHANGE_VALUE_POINTS5, false);
 // m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, false);
  m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, false);
}

void mafVMEQuadricSurfaceFitting::EnableGuiCone()
{
	m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, false);
	m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, false);
	m_GuiCube->Enable(CHANGE_VALUE_POINTS3, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, false);
	m_GuiCone->Enable(CHANGE_VALUE_POINTS5, true);
  //m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, false);
  m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, false);
}

void  mafVMEQuadricSurfaceFitting::EnableGuiSelectPoints()
{
	m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, true);
	m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, false);
	m_GuiCube->Enable(CHANGE_VALUE_POINTS3, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, false);
	m_GuiCone->Enable(CHANGE_VALUE_POINTS5, false);
//	m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, false);
	m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, false);
	
}

void mafVMEQuadricSurfaceFitting::EnableGuiSphere()
{
	m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, false);
	m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, false);
  m_GuiCube->Enable(CHANGE_VALUE_POINTS3, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, false);
  m_GuiCone->Enable(CHANGE_VALUE_POINTS5, false);
//  m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, true);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, true);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, false);
  m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, false);
}

void mafVMEQuadricSurfaceFitting::EnableGuiEllipsoid()
{
  m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, false);
  m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, false);
  m_GuiCube->Enable(CHANGE_VALUE_POINTS3, false);
  m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, false);
  m_GuiCone->Enable(CHANGE_VALUE_POINTS5, false);
//  m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, false);
  m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
  m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, true);
  m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, false);
}
void mafVMEQuadricSurfaceFitting::EnableGuiParentVerticesResult()
{
	m_GuiSelectPoints->Enable(CHANGE_VALUE_POINTS1, false);
	m_GuiPlane->Enable(CHANGE_VALUE_POINTS2, false);
	m_GuiCube->Enable(CHANGE_VALUE_POINTS3, false);
	m_GuiCylinder->Enable(CHANGE_VALUE_POINTS4, false);
	m_GuiCone->Enable(CHANGE_VALUE_POINTS5, false);
	//  m_GuiSphere->Enable(CHANGE_VALUE_POINTS6, false);
	m_GuiSphere->Enable(CHANGE_VALUE_SPHERE, false);
	m_GuiEllipsoid->Enable(CHANGE_VALUE_POINTS7, false);
	m_GuiParentVerticesResult->Enable(CHANGE_VALUE_POINTS10, true);
}
void mafVMEQuadricSurfaceFitting::EnableQuadricSurfaceGui(int surfaceTypeID)
{
  switch(surfaceTypeID)
  {

  case POINTS_SELECTION_Ellipsoid1:
	EnableGuiSelectPoints();
	if (DEBUG_MODE)
	{
		std::ostringstream stringStream;
		stringStream << "enabling Points selection gui" << std::endl;
		mafLogMessage(_M(stringStream.str().c_str()));
	}
	break;
  case PARAMETRIC_SPHERE:
       EnableGuiSphere();
       
       if (DEBUG_MODE)
         {
           std::ostringstream stringStream;
           stringStream << "enabling Sphere gui" << std::endl;
		   mafLogMessage(_M(stringStream.str().c_str()));
         }
    break;
  
  case POINTS_SELECTION_Ellipsoid5:
      EnableGuiCone();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Cone gui" << std::endl;
		mafLogMessage(_M(stringStream.str().c_str()));
      }

    break;
    
  case POINTS_SELECTION_Ellipsoid4:
      EnableGuiCylinder();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Cylinder gui" << std::endl;
		mafLogMessage(_M(stringStream.str().c_str()));
      }

    break;
  
  case POINTS_SELECTION_Ellipsoid3:
      EnableGuiCube();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Cube gui" << std::endl;
		mafLogMessage(_M(stringStream.str().c_str()));
      }

    break;

  case POINTS_SELECTION_Ellipsoid2:
      EnableGuiPlane();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Plane gui" << std::endl;
		mafLogMessage(_M(stringStream.str().c_str()));
      }

    break;

  case POINTS_SELECTION_Ellipsoid7:
      EnableGuiEllipsoid();
      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "enabling Ellipsoid gui" << std::endl;
		mafLogMessage(_M(stringStream.str().c_str()));
      }

      break;
  case PARENT_POINTS:
	  this->EnableGuiParentVerticesResult();

	  if (DEBUG_MODE)
	  {
		  std::ostringstream stringStream;
		  stringStream << "enabling parent points" << std::endl;
		  mafLogMessage(_M(stringStream.str().c_str()));
	  }
    default:
      break;
  }
}

//void SetInputPointsCloud(vtkSmartPointer<vtkPoints> points)

void mafVMEQuadricSurfaceFitting::fitEllipsoid(vtkMAFSmartPointer<vtkPoints> points, double* xres, double* yres, double* zres, double*r1, double* r2, double* r3, Matrix3d* rotationMatrix)
{

	//%%% see "Turner, 1999. An algorithm for fitting an ellipsoid to data"  eq 4 & 5
	//A_tmp = [(x.^2+y.^2-2*z.^2) (x.^2-2*y.^2+z.^2) 4*x.*y 2*x.*z 2*y.*z x y z ones(length(x),1)]; %%% size(Pnts_xyz,1)*9 matrix
	//%%% 9*1 vector:
	//abcd = A_tmp \ (x.^2+y.^2+z.^2); %%% If A is a rectangular m-by-n matrix with m ~= n, and B is a matrix with m rows,
	//%%% then A\B returns a least-squares solution to the system of equations A*x= B.

	//A_QvF = [ %%% Symmetric Quadratic form matrix (see eq. 5, "Turner, 1999...")
	// (1.-abcd(1)-abcd(2)) -2*abcd(3)             -abcd(4)
	//-2*abcd(3)           (1.-abcd(1)+2*abcd(2)) -abcd(5)
	// -abcd(4)            -abcd(5)               (1.+2*abcd(1)-abcd(2))
	// ];

	// v(1) = A_QvF(1,1); v(2) = A_QvF(2,2); v(3) = A_QvF(3,3);
	// v(4) = A_QvF(1,2); v(5) = A_QvF(1,3); v(6) = A_QvF(2,3);
	// v(7:9) = -abcd(6:8)/2; v(10) = -abcd(9);

	// v_out_v = v';


	const int N = points->GetNumberOfPoints();
	MatrixXd A(N, 9);
	VectorXd b(N);

	for (int i = 0; i < N; i++)
	{
		double* p = points->GetPoint(i);
		double x = p[0];
		double y = p[1];
		double z = p[2];
		A(i, 0) = x * x + y * y - 2 * z * z;
		A(i, 1) = x * x - 2 * y * y + z * z;
		A(i, 2) = 4 * x * y;
		A(i, 3) = 2 * x * z;
		A(i, 4) = 2 * y * z;
		A(i, 5) = x;
		A(i, 6) = y;
		A(i, 7) = z;
		A(i, 8) = 1;

		b(i) = x * x + y * y + z * z;
	}

	VectorXd abcd = A.colPivHouseholderQr().solve(b);




	double v[10];
	v[0] = 1.0 - abcd(0) - abcd(1);
	v[1] = 1.0 - abcd(0) + 2 * abcd(1);
	v[2] = 1.0 + 2 * abcd(0) - abcd(1);
	v[3] = -2 * abcd(2);
	v[4] = -abcd(3);

	v[5] = -abcd(4);
	v[6] = -abcd(5) / 2;
	v[7] = -abcd(6) / 2;
	v[8] = -abcd(7) / 2;
	v[9] = -abcd(8);

	//mafLogMessage("v: %f %f %f %f %f %f %f %f %f %f", v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9]);


	Matrix4d Alg;
	Alg << v[0], v[3], v[4], v[6],
		v[3], v[1], v[5], v[7],
		v[4], v[5], v[2], v[8],
		v[6], v[7], v[8], v[9];

	for (int i = 0; i<10; i++)
	{
		cout << v[i] << endl;
	}
	cout << endl;

	Vector3d center, center2;
	Matrix3d Asmall = Alg.block<3, 3>(0, 0);
	Vector3d vsmall;
	vsmall << v[6], v[7], v[8];
	center = -Asmall.colPivHouseholderQr().solve(vsmall);
	//center2 = -Asmall.inverse() * vsmall;


	*xres = center(0);
	*yres = center(1);
	*zres = center(2);
	Matrix4d T = Matrix4d::Identity();
	T(3, 0) = center(0);
	T(3, 1) = center(1);
	T(3, 2) = center(2);
	Matrix4d R = T * Alg * T.transpose();

	//    mafLogMessage("R1 %f %f %f", R(0,0), R(0,1), R(0,2));
	//    mafLogMessage("R2 %f %f %f", R(1, 0), R(1, 1), R(1, 2));
	//    mafLogMessage("R3 %f %f %f", R(2, 0), R(2, 1), R(2, 2));

	Matrix3d temp = R.block<3, 3>(0, 0) / -R(3, 3);
	VectorXd eigenvalues = temp.eigenvalues().real();
	*r1 = sgn(eigenvalues(0)) * sqrt(1.0 / abs(eigenvalues(0)));
	*r2 = sgn(eigenvalues(1)) * sqrt(1.0 / abs(eigenvalues(1)));
	*r3 = sgn(eigenvalues(2)) * sqrt(1.0 / abs(eigenvalues(2)));

	//mafLogMessage("Radii: %f %f %f", r1,r2,r3) ;


	Eigen::EigenSolver<MatrixXd> es(temp);

	Matrix3d V = es.pseudoEigenvectors();
	cout << "input " << endl;
	cout << temp << endl;
	cout << "evecs" << endl;
	cout << V << endl;


	// test for right/left hand sided coordinate system
	Matrix3d Vtemp;
	Vtemp.col(0) = V.col(1).cross(V.col(2));
	Vtemp.col(1) = V.col(0).cross(V.col(2));
	Vtemp.col(2) = V.col(0).cross(V.col(1));
	// fix the coordinate system in this case
	if (Vtemp.norm() > 0.1)
	{
		V.col(2) = Vtemp.col(2);
	}



	*rotationMatrix = V;
}


