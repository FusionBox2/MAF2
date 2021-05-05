/*=========================================================================

 Program: MAF2
 Module: mafVMECenterLine
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
#include "mafVMECenterLine.h"
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

#include "mafDataPipeCustom.h"
#include "mafRWI.h"
#include "mafGUIDialogPreview.h"
#include <math.h> 
#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafOpAddLandmark.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkAppendPolyData.h"
#include "vtkCellArray.h"
#include "vtkBitArray.h"
#include "vtkPointData.h"
#include "vtkLineSource.h"
#include "vtkTriangleFilter.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPoints.h"
#include "vtkActor.h"
#include "mafVMEOutputMuscleWrapping.h"
#include "mafVMESurface.h"

#include "mafPointCloud.hpp"
#include <vector>
#include <algorithm>
#include "mafVMEMeter.h"
#include "mafVMEPlane.h"
#include "unsupported/Eigen/Polynomials"
//const bool DEBUG_MODE = true;
using namespace Eigen;

mafCxxTypeMacro(mafVMECenterLine);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMECenterLine::mafVMECenterLine()
//-------------------------------------------------------------------------
{
	centerTemp = new double[3];
	centerTemp[0] = 0; centerTemp[01] = 0; centerTemp[02] = 0;


	mafString gLength = _R("");


  
  rotationMat = Eigen::Matrix3d::Identity();

 

	mafNEW(m_Transform);
	mafVMEOutputPolyline* output = mafVMEOutputPolyline::New();
	
	output->SetTransform(m_Transform); // force my transform in the output
	SetOutput(output);

	GetMaterial();

	vtkNEW(m_PolyData);
	DependsOnLinkedNodeOn();
  // attach a data pipe which creates a bridge between VTK and MAF
	mafDataPipeCustom *dpipe = mafDataPipeCustom::New();
	//m_PolyData->DeepCopy(m_Goniometer->GetOutput());
	m_PolyData->Update();
	dpipe->SetInput(m_PolyData);
	
	SetDataPipe(dpipe);


	mafNEW(m_CloudPath1);
	m_CloudPath1->Open();
	m_CloudPath1->SetName(_L("path1"));
	m_CloudPath1->SetRadius(1.5);
	m_CloudPath1->ReparentTo(this);
	m_CloudPath1->AppendLandmark(0, 0, 0, _R("first"), false);
	m_CloudPath1->AppendLandmark(0, 0, 0, _R("last"), false);
	
	m_CloudPath1->AppendLandmark(0, 0, 0, _R("mm"), false);


}

//-------------------------------------------------------------------------
mafVMECenterLine::~mafVMECenterLine()
//-------------------------------------------------------------------------
{
	vtkDEL(m_PolyData);
    mafDEL(m_Transform);
	SetOutput(NULL);
	
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMECenterLine::GetMaterial()
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
Eigen::Matrix3d mafVMECenterLine::principalAxesLCS(int nbr, vector<vector<double>>& vertex, double Circumf, double CrSec_Ratio, std::vector<std::vector<double>>& Cross_Sec_PGD_LCS)
{

	//string s650 = "principalAxesLCS start... ";
	//wxBusyInfo wait650(s650.c_str());
	//Sleep(1500);
	Eigen::Matrix3d V_N, V_N0;
	Eigen::RowVector3d mean = getOrigin(nbr, vertex);
	int sz = nbr;




	double Perp_MidLn = 1;
	Eigen::MatrixX3d centeredVertex, vertex1;
	centeredVertex.resize(sz, 3);
	for (int i = 0; i < sz; i++)
	{

		centeredVertex(i, 0) = vertex[i][0] - mean(0);
		centeredVertex(i, 1) = vertex[i][1] - mean(1);
		centeredVertex(i, 2) = vertex[i][2] - mean(2);

		vertex1.resize(i + 1, 3);
		vertex1(i, 0) = (vertex)[i][0];
		vertex1(i, 1) = (vertex)[i][1];
		vertex1(i, 2) = (vertex)[i][2];
	}



	double J_X = 0, J_Y = 0, J_Z = 0, J_XY = 0, J_XZ = 0, J_YZ = 0;

	for (int i = 0; i < sz; i++)
	{
		//string s551 = "centered... " + std::to_string(centeredVertex(i, 0)) + " " + std::to_string(centeredVertex(i, 1)) + " " + std::to_string(centeredVertex(i, 2));
		//wxBusyInfo wait551(s551.c_str());
		//Sleep(100);
		J_X = J_X + centeredVertex(i, 1)*centeredVertex(i, 1) + centeredVertex(i, 2)*centeredVertex(i, 2);
		J_Y = J_Y + centeredVertex(i, 0)*centeredVertex(i, 0) + centeredVertex(i, 2)*centeredVertex(i, 2);
		J_Z = J_Z + centeredVertex(i, 0)*centeredVertex(i, 0) + centeredVertex(i, 1)*centeredVertex(i, 1);

		J_XY = J_XY - centeredVertex(i, 0)*centeredVertex(i, 1);
		J_XZ = J_XZ - centeredVertex(i, 0)*centeredVertex(i, 2);
		J_YZ = J_YZ - centeredVertex(i, 1)*centeredVertex(i, 2);

	}


	Eigen::Vector3d G_Orig = mean.transpose();
	Eigen::Matrix3d Inertia_T;
	Inertia_T << J_X, J_XY, J_XZ, J_XY, J_Y, J_YZ, J_XZ, J_YZ, J_Z;

	Eigen::EigenSolver<Eigen::Matrix3d> eig(Inertia_T);
	Eigen::Vector3cd values = eig.eigenvalues();
	Eigen::Matrix3cd vectors = eig.eigenvectors();


	Eigen::Vector3d valuesd = values.real();
	Eigen::Matrix3d vectorsd = vectors.real();



	Eigen::Vector3d lam;
	Eigen::Vector3i indlam;
	lam << 1 / valuesd(0), 1 / valuesd(1), 1 / valuesd(2);
	lam = lam / lam.norm();

	
	vectorsd.col(0) = vectorsd.col(0) / vectorsd.col(0).norm();
	vectorsd.col(01) = vectorsd.col(01) / vectorsd.col(01).norm();
	vectorsd.col(02) = vectorsd.col(02) / vectorsd.col(02).norm();

	
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 3; j++)
	{
		V_N(i, j) = vectorsd(i, j);

	}

	V_N.col(2) = V_N.col(0).cross(V_N.col(01));
	V_N.col(02) = V_N.col(02) / V_N.col(02).norm();
	double min = lam(0), max = lam(0);
	indlam(0) = 10; indlam(1) = 10; indlam(2) = 10;

	for (int i = 0; i < 3; i++)
	{
		if (lam(i) <= min)
		{
			min = lam(i);
			indlam(0) = i;
		}

		if (lam(i) >= max)
		{
			max = lam(i);
			indlam(2) = i;
		}

	}
	for (int i = 0; i < 3; i++)
	{
		if ((i != indlam(0)) && (i != indlam(2)))
			indlam(1) = i;		

	}
	
//	string lamst = "lam " + std::to_string(lam(0)) + " " + std::to_string(lam(1)) + " " + std::to_string(lam(2)) + " " + std::to_string(indlam(0)) + " " + std::to_string(indlam(1)) + " " + std::to_string(indlam(2));
//	wxBusyInfo waitslam(lamst.c_str());
//	Sleep(3500);


	if (V_N(1, indlam(2))>0)
	{
		
		for (int j = 0; j < 3; j++)
		{
				V_N(j, indlam(2)) = -V_N(j, indlam(2));
		}
	}
	//string s3 = "VN0 " + std::to_string(V_N(0, 0)) + " " + std::to_string(V_N(0, 1)) + " " + std::to_string(V_N(0, 2)) + " " + std::to_string(V_N(1, 0)) + " " + std::to_string(V_N(1, 1)) + " " + std::to_string(V_N(1, 2))
	//		+ " " + std::to_string(V_N(2, 0)) + " " + std::to_string(V_N(2, 1)) + " " + std::to_string(V_N(2, 2));
	//wxBusyInfo waits3(s3.c_str());
	//Sleep(3500);

	//ofstream VNfile;
	//VNfile.open("VN.txt");

	//VNfile << J_X<< " "<<J_XY<<" "<< J_XZ<< "\n"<<J_XY<<" "<<J_Y<<" "<< J_YZ<< "\n "<<J_XZ<< " "<<J_YZ<<" "<< J_Z<<"\n \n"<<""
	//		<< V_N(0, 0) << " "<< V_N(0, 1) << " " << V_N(0, 2) << "\n "
	//		<< V_N(1, 0) << " " << V_N(1, 1) << " " << V_N(1, 2) << "\n "
	//		<<V_N(2, 0) << " " << V_N(2, 1) << " " << V_N(2, 2) << "\n ";
	//VNfile << "\n \n";
	V_N0.col(0) = V_N.col(indlam(2));
	V_N0.col(1) = V_N.col(indlam(1)).cross(V_N.col(indlam(2)));
	V_N0.col(2) = V_N.col(indlam(1));
	
	if (V_N0(0,2)<0)
	{
		for (int j = 0; j < 3; j++)
		{
			V_N0(j, 2) = -V_N0(j, 2);
		}
			
		V_N0.col(1) = V_N0.col(2).cross(V_N0.col(0));
	}
	V_N0.col(01) = V_N0.col(01) / V_N0.col(01).norm();
		
	//string s651 = "principalAxesLCS end... ";
	//wxBusyInfo wait651(s651.c_str());
	//Sleep(1500);
	//VNfile.close();
	return V_N0;
}
Eigen::Matrix3d mafVMECenterLine::principalAxesLCSRib(int nbr, vector<vector<double>>& vertex, double Circumf, double CrSec_Ratio, std::vector<std::vector<double>>& Cross_Sec_PGD_LCS, Eigen::Vector3d Perp_MidLn)
{


	//string s551 = "principalAxesLCSRib... "; 
	//wxBusyInfo wait551(s551.c_str());
	//Sleep(1500);

	Eigen::Matrix3d V_N;
	Eigen::RowVector3d mean = getOrigin(nbr, vertex);
	int sz = nbr;





	Eigen::MatrixX3d centeredVertex, vertex1;
	centeredVertex.resize(sz, 3);
	vertex1.resize(sz, 3);
	for (int i = 0; i < sz; i++)
	{

		centeredVertex(i, 0) = vertex[i][0] - mean(0);
		centeredVertex(i, 1) = vertex[i][1] - mean(1);
		centeredVertex(i, 2) = vertex[i][2] - mean(2);

		
		vertex1(i, 0) = (vertex)[i][0];
		vertex1(i, 1) = (vertex)[i][1];
		vertex1(i, 2) = (vertex)[i][2];
	}



	double J_X = 0, J_Y = 0, J_Z = 0, J_XY = 0, J_XZ = 0, J_YZ = 0;

	for (int i = 0; i < sz; i++)
	{
		//string s551 = "centered... " + std::to_string(centeredVertex(i, 0)) + " " + std::to_string(centeredVertex(i, 1)) + " " + std::to_string(centeredVertex(i, 2));
		//wxBusyInfo wait551(s551.c_str());
		//Sleep(100);
		J_X = J_X + centeredVertex(i, 1)*centeredVertex(i, 1) + centeredVertex(i, 2)*centeredVertex(i, 2);
		J_Y = J_Y + centeredVertex(i, 0)*centeredVertex(i, 0) + centeredVertex(i, 2)*centeredVertex(i, 2);
		J_Z = J_Z + centeredVertex(i, 0)*centeredVertex(i, 0) + centeredVertex(i, 1)*centeredVertex(i, 1);

		J_XY = J_XY - centeredVertex(i, 0)*centeredVertex(i, 1);
		J_XZ = J_XZ - centeredVertex(i, 0)*centeredVertex(i, 2);
		J_YZ = J_YZ - centeredVertex(i, 1)*centeredVertex(i, 2);

	}


	Eigen::Vector3d G_Orig = mean.transpose();
	Eigen::Matrix3d Inertia_T;
	Inertia_T << J_X, J_XY, J_XZ, J_XY, J_Y, J_YZ, J_XZ, J_YZ, J_Z;


	Eigen::EigenSolver<Eigen::Matrix3d> eig(Inertia_T);
	Eigen::Vector3cd values = eig.eigenvalues();
	Eigen::Matrix3cd vectors = eig.eigenvectors();
	//Eigen::Vector3d valuesd = values.cwiseAbs2();
	//Eigen::Matrix3d vectorsd = vectors.cwiseAbs2();

	Eigen::Vector3d valuesd = values.real();
	Eigen::Matrix3d vectorsd = vectors.real();



	Eigen::Vector3d lam;
	Eigen::Vector3i indlam;
	lam << 1 / valuesd(0), 1 / valuesd(1), 1 / valuesd(2);
	lam = lam / lam.norm();

//	string s = "lam " + std::to_string(lam(0)) + " "+std::to_string(lam(1)) + " "+std::to_string(lam(2));
//	wxBusyInfo wait750(s.c_str());
//	Sleep(1500);





	vectorsd.col(0) = vectorsd.col(0) / vectorsd.col(0).norm();
	vectorsd.col(01) = vectorsd.col(01) / vectorsd.col(01).norm();
	vectorsd.col(02) = vectorsd.col(02) / vectorsd.col(02).norm();

	//string s2 = "vectorsd " + std::to_string(vectorsd(0, 0)) + " " + std::to_string(vectorsd(0, 1)) + " " + std::to_string(vectorsd(0, 2)) + " " + std::to_string(vectorsd(1, 0)) + " " + std::to_string(vectorsd(1, 1)) + " " + std::to_string(vectorsd(1,2))
	//	+ " " + std::to_string(vectorsd(2, 0)) + " " + std::to_string(vectorsd(2, 1)) + " " + std::to_string(vectorsd(2,2));
	//wxBusyInfo wait950(s2.c_str());
	//Sleep(3500);

	vectorsd.col(2) = vectorsd.col(0).cross(vectorsd.col(01));
	vectorsd.col(02) = vectorsd.col(02) / vectorsd.col(02).norm();

	double min = lam(0), max = lam(0);
	indlam(0) = 0; indlam(1) = 0; indlam(2) = 0;

	for (int i = 1; i < 3; i++)
	{
		if (lam(i) < min)
		{
			min = lam(i);
			indlam(0) = i;
		}

		if (lam(i) > max)
		{
			max = lam(i);
			indlam(2) = i;
		}

	}
	for (int i = 0; i < 3; i++)
	{
		if ((i == indlam(0)) || (i == indlam(2)))
			;
		else
			indlam(1) = i;

	}



	for (int i = 0; i < 3; i++)
	{
		V_N(i, 0) = vectorsd(i, indlam(2));
		V_N.col(1) = vectorsd.col(indlam(1)).cross(vectorsd.col(indlam(2)));
		V_N(i, 2) = vectorsd(i, indlam(1));
	}




	if (V_N(0, 2) < 0)
	{
		for (int i = 0; i < 3; i++)
		{
			V_N(i, 2) = -V_N(i, 2);

		}
		V_N.col(0) = V_N.col(1).cross(V_N.col(2));
	}
	if (V_N(1, 0) < 0)
	{
		for (int i = 0; i < 3; i++)
		{
			V_N(i, 0) = -V_N(i, 0);

		}
		V_N.col(1) = V_N.col(2).cross(V_N.col(0));
	}
	for (int i = 0; i < 3; i++)
	{
		if (V_N.col(1).transpose()*Perp_MidLn < 0)
		{
			V_N(i, 1) = -V_N(i, 1);

		}
		V_N.col(2) = V_N.col(0).cross(V_N.col(1));
	}

	//ofstream fd;
	//fd.open(" vnRib.txt");
	//fd << J_X << " " << J_XY << " " << J_XZ << "\n"
	//	<< J_XY << " " << J_Y << " " << J_YZ << "\n"
	//	<< J_XZ << " " << J_YZ << " " << J_Z << "\n"
	//	<< std::to_string(V_N(0, 0)) << " " << std::to_string(V_N(0, 1)) << " " << std::to_string(V_N(0, 2)) << "\n "
	//	<< std::to_string(V_N(1, 0)) << " " << std::to_string(V_N(1, 1)) << " " << std::to_string(V_N(1, 2)) << " \n"
	//	<< std::to_string(V_N(2, 0)) << " " << std::to_string(V_N(2, 1)) << " " << std::to_string(V_N(2, 2)) << "\n"
	//	<< Perp_MidLn(0) << " " << Perp_MidLn(1) << " " << Perp_MidLn(2) << "\n"
	//	<< mean(0) << " " << mean(1) << " " << mean(2);

	//fd.close();


	

	Eigen::Matrix3Xd aa;
	aa.resize(3, sz);
	//Eigen::Vector3f bbb;
	for (int i = 0; i < sz; i++)
		for (int j = 0; j < 3; j++)
		{

			aa(j, i) = G_Orig(j);
		}


	Eigen::Matrix3Xd	Loc_XYZ_Vertx = V_N.transpose()*(vertex1.transpose() - aa);
	//ofstream fdLoc_XYZ_Vertx;
	//fdLoc_XYZ_Vertx.open(" fdLoc_XYZ_Vertx.txt");

	for (int i = 0; i < sz; i++)
	{
		Loc_XYZ_Vertx(1, i) = 0;
		//fdLoc_XYZ_Vertx << Loc_XYZ_Vertx(0, i) << " " << Loc_XYZ_Vertx(1, i) << " " << Loc_XYZ_Vertx(2, i)<<"\n";
	}
	
	//fdLoc_XYZ_Vertx.close();

	Eigen::VectorXd S;

	Eigen::Matrix3Xd P = Loc_XYZ_Vertx.cwiseProduct(Loc_XYZ_Vertx);



	S = P.colwise().sum();
	Eigen::VectorXd Dist_Loc_XYZ_Vertx = S.cwiseSqrt();

	Eigen::Matrix3Xd Dir_Loc_XYZ_Vertx;

	//for (int i = 0; i < 3; i++)
	//{
	//	bbb(i) = Dist_Loc_XYZ_Vertx(i);
	//}
	Dir_Loc_XYZ_Vertx.resize(3,sz);



	
	for (int j = 0; j < sz; j++)
	{
		for (int i = 0; i < 3; i++)
		{
			
			Dir_Loc_XYZ_Vertx(i, j) = Loc_XYZ_Vertx(i, j) / Dist_Loc_XYZ_Vertx(j);

			
		}
		
	}
	//ofstream fdir0;
	//fdir0.open(" fdir0.txt");
	//for (int j = 0; j < sz; j++)
	//{
	//	fdir0 << Loc_XYZ_Vertx(0, j) << " " << Loc_XYZ_Vertx(1, j) << " " << Loc_XYZ_Vertx(2, j) << " " << Dist_Loc_XYZ_Vertx(j) << "\n";
	//}
	//fdir0.close();
	//string s752020 = "direction ok";
	//wxBusyInfo wait7520(s752020.c_str());
	//Sleep(1000);
	double Ang_Step_5 = 10 * 3.1416 / 180;
	Eigen::RowVectorXf Ang_Scan, X_cos, Z_sin;
	
	for (int i = 0; i <= (2 * 3.1416) / Ang_Step_5; i++)
	{
		Ang_Scan.conservativeResize(i + 1);
		X_cos.conservativeResize(i + 1);
		Z_sin.conservativeResize(i + 1);
		Ang_Scan(i) = i*Ang_Step_5;
		X_cos(i) = std::cos(Ang_Scan(i));
		Z_sin(i) = std::sin(Ang_Scan(i));


		
	}

	//string s752021 = "init x z ok";
	//wxBusyInfo wait7521(s752021.c_str());
	//Sleep(1000);


	Eigen::Vector3f ff;
	ff(0) = X_cos(0); ff(1) = 0; ff(2) = Z_sin(0);

	Eigen::MatrixX3f m;
	m.resize(sz,3);
	for (int k = 0; k < sz; k++)
	{
		
		m(k, 0) = ff(0);
		m(k, 1) = ff(1);
		m(k, 2) = ff(2);

	}

	Eigen::MatrixX3d Nm;
	Eigen::RowVectorXd Prj_dir;
	Nm.resize(sz,3);
	for (int i = 0; i < sz; i++)
	{
		
		for (int j = 0; j < 3; j++)
		{
			Nm(i, j) = Dir_Loc_XYZ_Vertx(j, i)*m(i,j);
		}
	}

	Prj_dir.resize(sz);
	for (int i = 0; i < sz; i++)
	{
		double sum = 0;
		for (int k = 0; k < 3; k++)
			sum = sum + Nm(i,k);
		Prj_dir(i) =  sum;
	}


	double Ang_cmp = Ang_Step_5 / 3;

	Eigen::VectorXd  Ind_ang;
	Ind_ang.resize(sz);
	for (int i = 0; i < sz; i++)
	{
		Ind_ang(i) = std::acos(Prj_dir(i)) <  9*Ang_cmp;
	}
	//ofstream fdang;
	//fdang.open(" fdang0.txt");
	//for (int i = 0; i < sz; i++)
	//{
	//	fdang << Prj_dir(i)<<" "<<Ind_ang(i) << "\n";
	//}
	//fdang.close();

	//ofstream fdir;
	//fdir.open(" fdir.txt");
	//for (int i = 0; i < sz; i++)
	//{
	//	fdir << Dir_Loc_XYZ_Vertx(0, i) << " " << Dir_Loc_XYZ_Vertx(1, i) << " " << Dir_Loc_XYZ_Vertx(2, i)<<"\n";
	//}
	//fdir.close();

	//string s752 = "Ang_cmp= " + std::to_string(Ang_cmp);
	//wxBusyInfo wait752(s752.c_str());
	//Sleep(2500);
	Eigen::MatrixX3d Loc_XYZ_Vertx_prj, Loc_XYZ_Vertx_prjPow2;

	//Loc_XYZ_Vertx_prj.resize(sz, 3);
	//Loc_XYZ_Vertx_prjPow2.resize(sz, 3);

	//ofstream fdLoc_XYZ_Vertx_prjPow2;
//	fdLoc_XYZ_Vertx_prjPow2.open(" Loc_XYZ_Vertx_prjPow2_0.txt");
	for (int i = 0; i < sz; i++)
	{
		if (Ind_ang(i)>0)
		{


			//string s752q = "Loc if ok ";
			//wxBusyInfo wait752q(s752q.c_str());
			//Sleep(1000);
			Loc_XYZ_Vertx_prj.conservativeResize(Loc_XYZ_Vertx_prj.rows()+1, 3);
			Loc_XYZ_Vertx_prj(Loc_XYZ_Vertx_prj.rows()-1, 0) = Loc_XYZ_Vertx(0, i);
			Loc_XYZ_Vertx_prj(Loc_XYZ_Vertx_prj.rows() - 1, 1) = Loc_XYZ_Vertx(1, i);
			Loc_XYZ_Vertx_prj(Loc_XYZ_Vertx_prj.rows() - 1, 2) = Loc_XYZ_Vertx(2, i);

			Loc_XYZ_Vertx_prjPow2.conservativeResize(Loc_XYZ_Vertx_prjPow2.rows()+1, 3);
			Loc_XYZ_Vertx_prjPow2(Loc_XYZ_Vertx_prjPow2.rows() - 1, 0) = Loc_XYZ_Vertx(0, i)*Loc_XYZ_Vertx(0, i);
			Loc_XYZ_Vertx_prjPow2(Loc_XYZ_Vertx_prjPow2.rows() - 1, 1) = Loc_XYZ_Vertx(1, i)*Loc_XYZ_Vertx(1, i);
			Loc_XYZ_Vertx_prjPow2(Loc_XYZ_Vertx_prjPow2.rows() - 1, 2) = Loc_XYZ_Vertx(2, i)*Loc_XYZ_Vertx(2, i);
		//	fdLoc_XYZ_Vertx_prjPow2 << Loc_XYZ_Vertx_prjPow2(Loc_XYZ_Vertx_prjPow2.rows() - 1, 0) << " " << Loc_XYZ_Vertx_prjPow2(Loc_XYZ_Vertx_prjPow2.rows() - 1, 1) << " " << Loc_XYZ_Vertx_prjPow2(Loc_XYZ_Vertx_prjPow2.rows() - 1, 2) << "\n ";
		}
		else
		{

			;
			//string s752q = "Loc if not ok ";
			//wxBusyInfo wait752q(s752q.c_str());
			//Sleep(1000);
		}
	
	}


	

	//string s752z = "Loc_XYZ_Vertx_prjPow2 ";
	//wxBusyInfo wait752z(s752z.c_str());
	//Sleep(1000);
	
//	fdLoc_XYZ_Vertx_prjPow2.close();

	//Eigen::Vector3d::Index   maxIndex;
	Eigen::VectorXd Loc_XYZ_Vertx_prjPow2Sum = Loc_XYZ_Vertx_prjPow2.rowwise().sum();
	//Loc_XYZ_Vertx_prjPow2.rowwise().sum().maxCoeff(&maxIndex);
	double Max_dist = std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(0));
	int ind_Max_Dist = 0;

	
	for (int i = 1; i <Loc_XYZ_Vertx_prjPow2Sum.rows(); i++)
	{
		if (std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(i))>Max_dist)
		{

			Max_dist = std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(i));
			ind_Max_Dist = i;

		}
	}

	Eigen::Vector3d Curr_point_0;
	Curr_point_0(0) = Loc_XYZ_Vertx_prj(ind_Max_Dist, 0);
	Curr_point_0(1) = Loc_XYZ_Vertx_prj(ind_Max_Dist, 1);
	Curr_point_0(2) = Loc_XYZ_Vertx_prj(ind_Max_Dist, 2);




	Eigen::MatrixX3d Test_Pnt;
	Test_Pnt.resize(1, 3);
	Test_Pnt(0, 0) = Curr_point_0(0);
	Test_Pnt(0, 1) = Curr_point_0(1);
	Test_Pnt(0, 2) = Curr_point_0(2);

	//wxBusyInfo wait75f("test init pts ...fff");
	//Sleep(1000);
	double S_trg = 0;// Cross_Sec_Area
	Circumf= 0; //current Circumference
	// global Perp_MidLn Cross_Sec_PGD_LCS ICR_MHA_Yes S_trg CrSec_Ratio Circumf



	
	for (int ii = 1; ii < Ang_Scan.cols(); ii++)
	{
		
		
		ff(0) = X_cos(ii); ff(1) = 0; ff(2) = Z_sin(ii);
		//Eigen::MatrixX3f m;
		for (int k = 0; k < sz; k++)
		{
			//m.conservativeResize(k + 1, 3);
			m(k, 0) = ff(0);
			m(k, 1) = ff(1);
			m(k, 2) = ff(2);

		}


		

		//Eigen::Matrix3Xd Nm;
		//Eigen::RowVector3d Prj_dir;
		for (int j = 0; j < 3; j++)
		for (int i = 0; i < sz; i++)
		{
			Nm(i, j) = Dir_Loc_XYZ_Vertx(j, i)*m(i,j);
		}

	

		

		for (int i = 0; i < sz; i++)
		{
			double sum = 0;
			for (int k = 0; k < 3; k++)
				sum = sum + Nm(i,k);
			Prj_dir(i) = sum;
		}

		
		
		
		double Ang_cmp = Ang_Step_5 / 3;

		
		for (int i = 0; i < sz; i++)
			Ind_ang(i) = std::acos(Prj_dir(i)) <  Ang_cmp;


		Eigen::MatrixX3d Loc_XYZ_Vertx_prjj;
		Eigen::MatrixX3d Loc_XYZ_Vertx_prjjPow2;
		
		for (int i = 0; i < sz; i++)
		{

			if (Ind_ang(i)>0)
			{

				Loc_XYZ_Vertx_prjj.conservativeResize(Loc_XYZ_Vertx_prjj.rows()+1,3);
				Loc_XYZ_Vertx_prjj(Loc_XYZ_Vertx_prjj.rows() - 1, 0) = Loc_XYZ_Vertx(0, i);
				Loc_XYZ_Vertx_prjj(Loc_XYZ_Vertx_prjj.rows() - 1, 1) = Loc_XYZ_Vertx(1, i);
				Loc_XYZ_Vertx_prjj(Loc_XYZ_Vertx_prjj.rows() - 1, 2) = Loc_XYZ_Vertx(2, i);


				Loc_XYZ_Vertx_prjjPow2.conservativeResize(Loc_XYZ_Vertx_prjjPow2.rows()+1,3);
				Loc_XYZ_Vertx_prjjPow2(Loc_XYZ_Vertx_prjjPow2.rows() -1, 0) = Loc_XYZ_Vertx(0, i)*Loc_XYZ_Vertx(0, i);
				Loc_XYZ_Vertx_prjjPow2(Loc_XYZ_Vertx_prjjPow2.rows() -1, 1) = Loc_XYZ_Vertx(1, i)*Loc_XYZ_Vertx(1, i);
				Loc_XYZ_Vertx_prjjPow2(Loc_XYZ_Vertx_prjjPow2.rows() -1, 2) = Loc_XYZ_Vertx(2, i)*Loc_XYZ_Vertx(2, i);

			}
		}


		
	//	ofstream fdPrj_Loc;
	//	string sdh = " Prj_Loc" + std::to_string(ii) + ".txt";
	//	fdPrj_Loc.open(sdh);

		int rtr = Loc_XYZ_Vertx_prjj.rows();
		//string ereùeùe = "Prj_Loc ...fff " + std::to_string(rtr);
		//wxBusyInfo wait75fd(ereùeùe.c_str());
		//Sleep(1000);
		
		//Eigen::RowVectorXd
		double Max_dist = 0;
		Eigen::VectorXd Loc_XYZ_Vertx_prjPow2Sum2;
		if (rtr > 0)
		{
			Loc_XYZ_Vertx_prjPow2Sum2 = Loc_XYZ_Vertx_prjjPow2.rowwise().sum();
			Max_dist=	std::sqrt(Loc_XYZ_Vertx_prjPow2Sum2(0));
		}


		 
		int ind_Max_Dist = 0;

		//string qsq = "before lloop" ;
		//wxBusyInfo wait75qsq(qsq.c_str());
		//Sleep(1000);
		for (int i = 1; i <Loc_XYZ_Vertx_prjPow2Sum2.rows(); i++)
		{
			if (std::sqrt(Loc_XYZ_Vertx_prjPow2Sum2(i))>Max_dist)
			{

				Max_dist = std::sqrt(Loc_XYZ_Vertx_prjPow2Sum2(i));
				ind_Max_Dist = i;

			}
		}
		//wxBusyInfo wait7d("qsqsdqPrj_Loc ...fff");
		//Sleep(1000);
	//	fdPrj_Loc << "max dist... "<< std::to_string(Max_dist) <<" " << std::to_string(ind_Max_Dist) << "size "<< std::to_string(Loc_XYZ_Vertx_prjj.rows()) <<" " << std::to_string(Loc_XYZ_Vertx_prjj.cols())<<"\n";

	//	for (int u = 0; u<rtr; u++)
	//		fdPrj_Loc << Loc_XYZ_Vertx_prjj(u, 0) << " " << Loc_XYZ_Vertx_prjj(u, 1) << " " << Loc_XYZ_Vertx_prjj(u, 2) << "\n";
	//	fdPrj_Loc.close();

		
		//Eigen::RowVectorXd
		Eigen::Vector3d Curr_point_1;
		if (rtr > 0)

		{
			Curr_point_1(0) = Loc_XYZ_Vertx_prjj(ind_Max_Dist, 0);
			Curr_point_1(1) = Loc_XYZ_Vertx_prjj(ind_Max_Dist, 1);
			Curr_point_1(2) = Loc_XYZ_Vertx_prjj(ind_Max_Dist, 2);


			double a = Curr_point_0.norm();
			double b = Curr_point_1.norm();
			double c = (Curr_point_0 - Curr_point_1).norm();

			double p = 0.5*(a + b + c);
			S_trg = S_trg + sqrt(p*(p - a)*(p - b)*(p - c));
			(Circumf) = (Circumf)+c; // current Circumference
			Test_Pnt.conservativeResize(Test_Pnt.rows() + 1, 3);
			Test_Pnt(Test_Pnt.rows() - 1, 0) = Curr_point_1(0);
			Test_Pnt(Test_Pnt.rows() - 1, 1) = Curr_point_1(1);
			Test_Pnt(Test_Pnt.rows() - 1, 2) = Curr_point_1(2);
			Curr_point_0 = Curr_point_1;

			//wxBusyInfo wait75dfvdd("ffsdsfsdsdf...fff");
			//Sleep(1000);
		}

//
//		ofstream fdPrj_dir;
//		string sdh1 = " Prj_dir" + std::to_string(ii) + ".txt";
//		fdPrj_dir.open(sdh1);
//		for (int u = 0; u<sz; u++)
//			fdPrj_dir << Prj_dir(u) << "\n";
//		fdPrj_dir.close();
		
	}


	//string s7503 = "loop end... " ;
	//wxBusyInfo wait7503(s7503.c_str());
	//Sleep(1500);
	
	double szTstPts;
	//double CrSec_Ratio;
	szTstPts = Test_Pnt.rows();

	//ofstream fdRIB2;
	//fdRIB2.open(" tstpts.txt");
	//for (int u = 0; u<szTstPts; u++)
	//	fdRIB2 << Test_Pnt(u, 0) << " " << Test_Pnt(u, 1) << " " << Test_Pnt(u, 2) << "\n";
	//fdRIB2.close();
	
	ellipse_struct* ellipse_t;
	
	if (szTstPts > 5)
	{
	
		

		 ellipse_t = fitellipse(Test_Pnt, 0,2);


		
		if (ellipse_t->a > 0)
		{

			double a = std::max(ellipse_t->a, ellipse_t->b);
			double b = std::min(ellipse_t->a, ellipse_t->b);
			double X0 = ellipse_t->X0;
			double Y0 = ellipse_t->Y0;
			// the ellipse
			Eigen::VectorXd theta_r, ellipse_x_r, ellipse_y_r, zer;
			for (int i = 0; i <= 39; i++)
			{
				theta_r.conservativeResize(i + 1); 
				ellipse_x_r.conservativeResize(i + 1);
				ellipse_y_r.conservativeResize(i + 1);
				zer.conservativeResize(i + 1);
				theta_r(i) =(i* 2 * 3.1416) / 39;
				ellipse_x_r(i) = X0 + a * cos(theta_r(i));
				ellipse_y_r(i) = Y0 + b * std::sin(theta_r(i));
				zer(i) = 0;

				

			}

			
			Test_Pnt.resize(40, 3);
			for (int t = 0; t < ellipse_y_r.rows(); t++)
			{
				Test_Pnt(t, 0) = ellipse_x_r(t);
				Test_Pnt(t, 1) = zer(t);
				Test_Pnt(t, 2) = ellipse_y_r(t);
			}
			
			S_trg = 3.1416*a*b;

			Circumf = 3.1416*(3 * (a + b) - sqrt((3 * a + b)*(a + 3 * b))); //current Circumference
			CrSec_Ratio = b / a;



		}

		
	}



	Eigen::MatrixX3d or;
	or.resize(Test_Pnt.rows(),3);
	for (int u = 0; u < Test_Pnt.rows();u++)
	{
		or(u, 0) = mean(0);
		or(u, 1) = mean(1);
		or(u, 2) = mean(2);
	}

	Eigen::Matrix3Xd Test_Pnt2;
	Test_Pnt2 = V_N*Test_Pnt.transpose() +or.transpose();


	if (S_trg > 0) // NEW LCS 

	{

		
		Eigen::MatrixX3d vertex10 = Test_Pnt2.transpose();

	
		sz = vertex10.rows();

		
		mean = vertex10.colwise().mean();
		

		for (int i = 0; i < sz; i++)
		{
			centeredVertex.conservativeResize(i + 1, 3);
			centeredVertex(i, 0) = vertex10(i, 0) - mean(0);
			centeredVertex(i, 1) = vertex10(i, 1) - mean(1);
			centeredVertex(i, 2) = vertex10(i, 2) - mean(2);
		}

	
		
		double J_X = 0, J_Y = 0, J_Z = 0, J_XY = 0, J_XZ = 0, J_YZ = 0;

		for (int i = 0; i < sz; i++)
		{

			J_X = J_X + centeredVertex(i, 1)*centeredVertex(i, 1) + centeredVertex(i, 2)*centeredVertex(i, 2);
			J_Y = J_Y + centeredVertex(i, 0)*centeredVertex(i, 0) + centeredVertex(i, 2)*centeredVertex(i, 2);
			J_Z = J_Z + centeredVertex(i, 0)*centeredVertex(i, 0) + centeredVertex(i, 1)*centeredVertex(i, 1);

			J_XY = J_XY - centeredVertex(i, 0)*centeredVertex(i, 1);
			J_XZ = J_XZ - centeredVertex(i, 0)*centeredVertex(i, 2);
			J_YZ = J_YZ - centeredVertex(i, 1)*centeredVertex(i, 2);

		}

				
		G_Orig = mean.transpose();
		Inertia_T << J_X, J_XY, J_XZ, J_XY, J_Y, J_YZ, J_XZ, J_YZ, J_Z;


	

		//Inertia_T << J_X, J_XY, J_XZ, J_XY, J_Y, J_YZ, J_XZ, J_YZ, J_Z;
		Eigen::EigenSolver<Eigen::Matrix3d> eig(Inertia_T);
		Eigen::Vector3cd values = eig.eigenvalues();
		Eigen::Matrix3cd vectors = eig.eigenvectors();

		Eigen::Vector3d valuesd = values.real();
		Eigen::Matrix3d vectorsd = vectors.real();

		lam << 1 / valuesd(0), 1 / valuesd(1), 1 / valuesd(2);
		lam = lam / lam.norm();

		
		
		if (vectorsd.col(0).transpose()*V_N.col(0) < 0)
		{
			vectorsd(0, 0) = -vectorsd(0, 0);
			vectorsd(1, 0) = -vectorsd(1, 0);
			vectorsd(2, 0) = -vectorsd(2, 0);
		}

		if (vectorsd(1, 0) < 0)
		{
			vectorsd(0, 0) = -vectorsd(0, 0);
			vectorsd(1, 0) = -vectorsd(1, 0);
			vectorsd(2, 0) = -vectorsd(2, 0);
		}
		
		if (vectorsd.col(2).transpose()*V_N.col(1) < 0)
		{
			vectorsd(0, 2) = -vectorsd(0, 2);
			vectorsd(1, 2) = -vectorsd(1, 2);
			vectorsd(2, 2) = -vectorsd(2, 2);
		}

		


		V_N.col(0) = vectorsd.col(0) / vectorsd.col(0).norm();
		V_N.col(1) = vectorsd.col(2) / vectorsd.col(2).norm();
		V_N.col(2) = vectorsd.col(1) / vectorsd.col(1).norm();
		//lam_Out << [lam(1) lam(3) lam(2)]';
	
	
		
		if ((Test_Pnt2.rows() < 6) || (ellipse_t->a == 0))
		{
			CrSec_Ratio = lam(2) / lam(1);
		}



		V_N.col(2) = V_N.col(0).cross(V_N.col(1));

		
	}
	
	
	bool ICR_MHA_Yes =  true;
	//Eigen::MatrixXd Cross_Sec_PGD_LCS;
	double fi, R_Theta;


	
	

	
	//if (ICR_MHA_Yes)
	{
		Eigen::Matrix3d R_out_Trx_N = V_N;
		Eigen::Vector3d Tr_out_Trx_N = G_Orig;

		

		Eigen::Vector4d uu = extract(R_out_Trx_N);
		double fi = uu(3);
		Eigen::Vector3d u;
		u(0) = uu(0);
		u(1) = uu(1);
		u(2) = uu(2);

	
		Eigen::Matrix3d R_Theta = theta2r(u*fi); //Back transf
		

		double de = fi * 180 / 3.1416;
		Eigen::RowVectorXd k;
		int nbrrow = Cross_Sec_PGD_LCS.size();
		
		k.resize(7);
		k(0) = de;
		k(1) = u(0);
		k(2) = u(1);
		k(3) = u(2);
		k(4) = Tr_out_Trx_N(0);
		k(5) = Tr_out_Trx_N(1);
		k(6) = Tr_out_Trx_N(2);

		Cross_Sec_PGD_LCS.push_back({k(0),k(1),k(2),k(3),k(4),k(5),k(6)});
		/*Cross_Sec_PGD_LCS[nbrrow+1][0] = k(0);
		Cross_Sec_PGD_LCS[nbrrow+1][1] = k(1);
		Cross_Sec_PGD_LCS[nbrrow+1][2] = k(2);
		Cross_Sec_PGD_LCS[nbrrow+1][3] = k(3);
		Cross_Sec_PGD_LCS[nbrrow+1][4] = k(4);
		Cross_Sec_PGD_LCS[nbrrow+1][5] = k(5);
		Cross_Sec_PGD_LCS[nbrrow+1][6] = k(6);*/
		//wxBusyInfo wait1650("init ok");
		//Sleep(2500);

		
		
	}
	
	//std::string eeevee = "PA Rib " + std::to_string(V_N(0, 0)) + " " + std::to_string(V_N(0, 1)) + " " + std::to_string(V_N(0, 2)) + "\n"+std::to_string(V_N(1, 0)) + " " + std::to_string(V_N(1, 1)) + " " + std::to_string(V_N(1, 2))
	//	+"\n"+std::to_string(V_N(2, 0)) + " " + std::to_string(V_N(2, 1)) + " " + std::to_string(V_N(2, 2));
	//wxBusyInfo wait10eeveee45(eeevee.c_str());
	//Sleep(1500);

	
//	ofstream fdRIB;
//	fdRIB.open(" Rib.txt");


//	fdRIB << V_N(0, 0) << " " << V_N(0, 1) << " " << V_N(0, 2) << "\n " << V_N(1, 0) << " " << V_N(1, 1) << " " << V_N(1, 2) << "\n " << V_N(2, 0) << " " << V_N(2, 1) << " " << V_N(2, 2)<<"\n ";
//	fdRIB << "Cross_Sec_PGD_LCS size " << Cross_Sec_PGD_LCS[0][0] << " " << Cross_Sec_PGD_LCS[0][1] << " " << Cross_Sec_PGD_LCS[0][2] << " " << Cross_Sec_PGD_LCS[0][3] << " " << Cross_Sec_PGD_LCS[0][4] << " " << Cross_Sec_PGD_LCS[0][5] <<" "<< Cross_Sec_PGD_LCS[0][6]<<"\n";
//	fdRIB << "szTstPts S_trg circumf " << szTstPts<< " " << S_trg << " " << Circumf << "\n";
//	fdRIB.close();
	

	//string s650 = "Ribfunction end... ";
	//wxBusyInfo wait650(s650.c_str());
	//Sleep(1500);
	return V_N;
}
double mafVMECenterLine::Max_Eigen_v(int N,int NDIM, Eigen::Matrix3d A, double EPS)
{

	//string s650 = "Max_Eigen_v start... ";
	//wxBusyInfo wait650(s650.c_str());
	//Sleep(1500);
	int Ier = 1001;
	double R, S, E_lam;
	Eigen::Vector3d E_vec = Eigen::Vector3d::Zero(); 
	
	E_vec(0)= std::sqrt(N + .0);
	E_vec(1) = std::sqrt(N + .0);
	E_vec(2) = std::sqrt(N + .0);
	
	int K_iter = 0;
	 S = 1. + EPS;
	////////
	while (S > EPS && K_iter <= Ier)
	{
		K_iter = K_iter + 1;
		Eigen::Vector3d Wrk = E_vec;
		E_vec = A*Wrk;
		R = E_vec.norm();
		if (R <= EPS)
		{	
			Ier = 1;		
			return E_lam;
		}
		R = 1. / R;
		E_vec = E_vec*R;
		Wrk = Wrk - E_vec;
	    S = Wrk.norm();
	}
	if (S <= EPS)
		Ier = 0; 

	E_lam = R;
	
	E_vec = E_vec.transpose();

	//string s651 = "Max_Eigen_v end... ";
	//wxBusyInfo wait651(s651.c_str());
	//Sleep(1500);
	return E_lam;

}
Eigen::Vector4d mafVMECenterLine::extract(Eigen::Matrix3d r)
{
	//Extracts the  unit  vector  u of the screw axis and the rotation angle fi
	//  from a rotation matrix stored in the 3 * 3 left - upper submatrix of a matrix R.
	//EXTRACT performs the inverse operation of ROTAT.

	//[u, angle] = extract(R)

	//string s650 = "extract start... ";
	//wxBusyInfo wait650(s650.c_str());
	//Sleep(1500);
	double const_5 = 0.5;
	Eigen::Vector4d result;
	Eigen::Vector3d u;
	u(0)=u(1)=u(2)= 0.0;
	double a = const_5*(r(2, 1) - r(1, 2));
	double b = const_5*(r(0, 2) - r(2, 0));
	double c = const_5*(r(1, 0) - r(0, 1));

	double s = sqrt(a*a + b*b + c*c);
	double co = const_5*(r(0, 0) + r(1, 1) + r(2, 2) - 1);


	 

	double fi = std::atan2(s, co);
	double deg = fi * 180 / 3.1416;
	Eigen::Matrix3d eye;
	eye << 1, 0, 0, 0, 1, 0, 0, 0, 1;
	if (deg > 135);
	{
		Eigen::Matrix3d R = r;
		Eigen::Matrix3d b_ttt = 0.5*(R + R.transpose()) - cos(fi) * eye;
		Eigen::Vector3d b1 = b_ttt.col(0); 
		Eigen::Vector3d b2 = b_ttt.col(1);
		Eigen::Vector3d b3 = b_ttt.col(2);
		Eigen::Vector3d btmp;
		btmp << b1.transpose()*b1, b2.transpose()*b2, b3.transpose()*b3;


		double bmax = btmp.maxCoeff();
		int i;
		for (int j = 0; j < 3; j++)
		{
			if (btmp(j)==bmax)
				i = j;
		}

		Eigen::Vector3d n  = b_ttt.col(i) / sqrt(bmax);
		if (sign(R(2, 1) - R(1, 2)) != sign(n(0, 0)))
		{
			n = n*(-1);
		}

	}

	co = std::min(1.0, std::max(-1.0, co));
	double v = 1 - co;

	
	

	if ((s) > 0.1)
	{
		u(0) = a / s;
		u(1) = b / s;
		u(2) = c / s;

	}
	else if ((fi!= 0) & (co > 0))
		{
		double t = 1 / v;
		u(0) = sign(r(2, 1) - r(1, 2))*sqrt(abs((r(0, 0) - co)*t));
		u(1) = sign(r(0, 2) - r(2, 0))*sqrt(abs((r(1, 1) - co)*t));
		u(2) = sign(r(1, 0) - r(0, 1))*sqrt(abs((r(2, 2) - co)*t));
		}
	else if ((fi != 0) & (co < 0))
	{
		double t = 1 / v;
		u(0) = sqrt(abs((r(0, 0) - co)*t));
		u(1) = sqrt(abs((r(1, 1) - co)*t));
		u(2) = sqrt(abs((r(2, 2) - co)*t));


		double x;
		if ((u(0) >= u(1)) & (u(0) >= u(2)))
			x = 0;
		else if ((u(1) >= u(0)) & (u(1) >= u(2)))
			x = 1;
		else
			x = 2;

		double y = std::fmod(x, 2) + 1;
		double z = std::fmod(x + 1, 3) + 1;
		if ((r(2, 1) - r(1, 2)) >= 0)
			s = 1;
		else
			s = -1;

		u(0) = u(0)*s;
		u(1) = u(1)*sign(r(1, 0) + r(0, 1))*s;
		u(2) = u(2)*sign(r(2, 1) + r(0, 2))*s;

	}
	else
	{
		u(0) = u(1) = u(2) = 0;
	}
	

	result(0) = u(0);
	result(1) = u(1);
	result(2) = u(2);
	result(3) = fi;

	
//	string s651 = "extract end... ";
//	wxBusyInfo wait651(s651.c_str());
//	Sleep(1500);

	return result;



}


Eigen::Matrix3d mafVMECenterLine::theta2r(Eigen::Vector3d th)
{

	//string s650 = "theta2r start... ";
	//wxBusyInfo wait650(s650.c_str());
	//Sleep(1500);
	Eigen::Matrix3d R;
	double fi = sqrt(th.transpose()*th);
	if (fi <= 0.0000001)
	{	

	
		R <<1,0,0,0,1,0,0,0,1;


		return R;
	}



	Eigen::Matrix3d eye;
	eye << 1, 0, 0, 0, 1, 0, 0, 0, 1;
	double sinc = std::sin(fi) / fi;
	double cosc = (1 - std::cos(fi)) / (fi *fi);

	Eigen::Matrix3d A;
	A<< 0, - th(2), th(1), th(2), 0, -th(0), -th(1), th(0), 0;
	R = cos(fi)*eye + sinc*A + cosc*(th*th.transpose());
	//string s651 = "theta2r end... ";
	//wxBusyInfo wait651(s651.c_str());
	//Sleep(1500);

	return R;

}
ellipse_struct* mafVMECenterLine::fitellipse(Eigen::MatrixX3d pts, int axe1, int axe2)
{

	//string s650 = "fitellipse start... ";
	//wxBusyInfo wait650(s650.c_str());
	//Sleep(1500);
	//ofstream  fdRIB2;
	//fdRIB2.open(" elllipsefitting.txt");

	
	
	Eigen::VectorXd xx, yy, x2, y2;
	
	Eigen::MatrixXd XX;
	double* center = new double[3];
	double orientation_tolerance = 1e-3;
	double mean_x = 0, mean_y = 0;
	for (int i = 0; i < pts.rows(); i++)
	{
		mean_x = mean_x + pts(i,axe1);
		mean_y = mean_y + pts(i,axe2);
	}
	mean_x = mean_x / pts.rows();
	mean_y = mean_y / pts.rows();
	//fdRIB2 << mean_x << " " << mean_y << "\n";
	//std::vector<double> x, y;

	for (int i = 0; i < pts.rows(); i++)
	{

		xx.conservativeResize(i + 1);
		yy.conservativeResize(i + 1);
		x2.conservativeResize(i + 1);
		y2.conservativeResize(i + 1);
		xx[i] = pts(i,axe1) - mean_x;
		yy[i] = pts(i,axe2) - mean_y;
		x2(i) = xx(i) * xx(i);
		y2(i) = yy(i) * yy(i);
	}
	
	
	for (int i = 0; i < xx.rows(); i++)
	{
		XX.conservativeResize(i + 1, 5);
		XX(i, 0) = x2(i);
		XX(i, 1) = xx(i)*yy(i);
		XX(i, 2) = y2(i);
		XX(i, 3) = xx(i);
		XX(i, 4) = yy(i);
	}
	
	Eigen::MatrixXd vv=(XX.transpose())*XX;
	
	Eigen::RowVectorXd b;
	b = XX.colwise().sum();
	
	Eigen::VectorXd a = b*vv.inverse();
	//for (int i = 0; i<a.rows();i++)
	//{
	//	fdRIB2 << a(i)  << "\n";
		
	//}



	


	double X0=0,X0_in=0, Y0=0,Y0_in=0, F=0, aa=0, bb=0, cc=0, dd=0, ee=0, cos_phi=0, sin_phi=0, orientation_rad=0, long_axis=0, short_axis=0;
	if (min(fabs(a(1) / a(0)), fabs(a(1) / a(2))) > orientation_tolerance)
	{

	
		
		orientation_rad = 0.5 * atan(a(1) / (a(2) - a(0)));

		



		cos_phi = cos(orientation_rad);
		sin_phi = sin(orientation_rad);
		aa = a(0)*cos_phi*cos_phi - a(1)*cos_phi*sin_phi + a(2)*sin_phi*sin_phi;
		bb = 0;
		cc = a(0)*sin_phi*sin_phi + a(1)*cos_phi*sin_phi + a(2)*cos_phi*cos_phi;
		dd = a(3)*cos_phi - a(4)*sin_phi;
		ee=a(3)*sin_phi + a(4)*cos_phi;
	
		mean_x = cos_phi*mean_x - sin_phi*mean_y;
		mean_y=sin_phi*mean_x + cos_phi*mean_y;
	}

	else
	{

	
		
		orientation_rad = 0;
		cos_phi = cos(orientation_rad);
		sin_phi = sin(orientation_rad);
	}



	ellipse_struct* ellip = new ellipse_struct();
	



	if (aa*cc > 0)
	{

	
		if (aa<0)
		{
			aa = -aa;
			cc = -cc;
			dd = -dd;
			ee = -ee;
		}
		X0 = mean_x - dd / 2 / aa;
		Y0 = mean_y - ee / 2 / cc;
		F = 1 + (dd*dd) / (4 * aa) + (ee*ee) / (4 * cc);
		aa = sqrt(F / aa);
		bb= sqrt(F / cc);
		long_axis = 2 * max(aa, bb);
		short_axis = 2 * min(aa, bb);

		
		
		X0_in = X0*cos_phi+Y0*sin_phi;
		Y0_in = -sin_phi*X0+cos_phi*Y0;



		
	}
	else
	{

		//no ellipse

		//return NULL;
	}
	ellip->a = aa;
	ellip->b = bb;
	ellip->X0 = X0;
	ellip->Y0 = Y0;
	ellip->phi = orientation_rad;
	ellip->X0_in = X0_in;
	ellip->Y0_in = Y0_in;
	ellip->long_axis = long_axis;
	ellip->short_axis = short_axis;

	//fdRIB2 << orientation_rad<<" "<<cos_phi << " " << sin_phi << " " << aa << " " << bb << " " << X0 << " " << Y0 << " " << orientation_rad << " " << X0_in << " " << Y0_in << " " << long_axis << " " << short_axis << "\n";
	
	//fdRIB2.close();

//	string s651 = "fitellipse end... ";
//	wxBusyInfo wait651(s651.c_str());
//	Sleep(1500);
	return ellip;
}

//-------------------------------------------------------------------------
int mafVMECenterLine::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
	

  if (Superclass::DeepCopy(a)==MAF_OK)
  {
	  mafVMECenterLine *vmeCenterLine = mafVMECenterLine::SafeDownCast(a);
	  m_Transform->SetMatrix(vmeCenterLine->m_Transform->GetMatrix());
	
	 
	  
	  this->rotationMat = vmeCenterLine->rotationMat;



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
bool mafVMECenterLine::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    if (     
		m_Transform->GetMatrix() == ((mafVMECenterLine *)vme)->m_Transform->GetMatrix() &&
		

		this->rotationMat == ((mafVMECenterLine *)vme)->rotationMat


      )
    {
      ret = true;
    }
  }
 return ret;
}

mafVMEOutputPolyline *mafVMECenterLine::GetPolylineOutput()
//-------------------------------------------------------------------------
{
	return (mafVMEOutputPolyline *)GetOutput();
	//return (mafVMEOutputPolyline *)m_Goniometer->GetOutput();
}
//-------------------------------------------------------------------------
void mafVMECenterLine::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
	m_Transform->SetMatrix(mat);
	Modified();
}
//-------------------------------------------------------------------------
bool mafVMECenterLine::IsAnimated()
//-------------------------------------------------------------------------
{
	return false;
}
//-------------------------------------------------------------------------
int mafVMECenterLine::sign(double a)
{
	if (a > 0)
		return 1;
	else
		return -1;

}
//-------------------------------------------------------------------------
mafGUI* mafVMECenterLine::CreateGui()
//-------------------------------------------------------------------------
{
	m_Gui=mafVME::CreateGui();
  if(m_Gui)
  {
   // mafString geometryType[2] =// {"Points_selection1", "Points_selection2", "Points_selection3", "Points_selection4", "Points_selection5", "sphere", "Points_selection7"};
	//{ "Points_selection1", "sphere" };
   // m_Gui->Combo(ID_GEOMETRY_TYPE, "", &m_GeometryType, 2, geometryType);
   // m_Gui->Divider(2);
	
	//CreateGuiSelectPoints();
	//CreateGuiResult();
	//m_Gui->Divider(2);
	//mafString ss = " ";
	//gLength = ss.Append(std::to_string(distToPlan).c_str());
	//m_Gui->Label(_("distance: "), &gLength, true);
    //m_Gui->Divider(2);
   // CreateGuiSphere();
 
   m_Gui->Button(ID_Surface_LINK, &m_SurfaceName, _L("Surface"), _L("Select Surace"));



   m_Gui->Double(CHANGE_VALUE_CenterLine, _L("Delt_1"), &Delt_1);
   m_Gui->Double(CHANGE_VALUE_CenterLine, _L("Mult_01"), &Mult_01);
   m_Gui->Double(CHANGE_VALUE_CenterLine, _L("D_1mm"), &D_1mm);
   m_Gui->Double(CHANGE_VALUE_CenterLine, _L("D_50mm"), &D_50mm);
   m_Gui->Double(CHANGE_VALUE_CenterLine, _L("Fi_stp_5"), &Fi_stp_5);
    m_Gui->FitGui();
    
  //  EnableQuadricSurfaceGui(m_GeometryType);
    m_Gui->Update();

	
  }

  m_Gui->Divider();

  //wxBusyInfo wait("GUI created...");
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMECenterLine::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{


  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {


	  mafString ee;
	  if (e->GetVme() == NULL)
		 ee = _R("VME NOT found");
	  else
	  {
		  ee = e->GetVme()->GetName();
		  InternalUpdate();
	  }
	  
	  
	 
    switch(e->GetId())
    {
//		EnableQuadricSurfaceGui(m_GeometryType);
		m_Gui->Update();
		m_Gui->FitGui();
		//////////////////



	case ID_Surface_LINK:
	{

						  mafID button_id = e->GetId();
						  mafString title = _L("Choose surface vme link");
						  e->SetId(VME_CHOOSE);
						  e->SetArg((long)&mafVMEPlane::VMEAccept);
						  e->SetString(&title);
						  ForwardUpEvent(e);
						  mafNode *n = e->GetVme();
						  if (n != NULL)
						  {

							  SetSurfaceLink(_R("SurfaceVME"), n);
							  m_SurfaceName = n->GetName();
							  surface = (mafVMESurface*)n;

							  m_Gui->Update();

						  }
	}
		break;

	  
	case CHANGE_VALUE_CenterLine:
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

 
  //InternalUpdate();
}
void mafVMECenterLine::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
	kframes.clear(); // no timestamps
	mafTimeStamp t = m_Transform->GetMatrix().GetTimeStamp();
	kframes.push_back(t);
}
//-----------------------------------------------------------------------
void mafVMECenterLine::InternalPreUpdate()
//-----------------------------------------------------------------------
{
	InternalInitialize();
}
//-----------------------------------------------------------------------

void mafVMECenterLine::Update()
{
	InternalUpdate();

}
void mafVMECenterLine::InternalUpdate()
//-----------------------------------------------------------------------
{
	

	UpdateLinks();
	mafTimeStamp currTs = GetTimeStamp();
	double xyzr[3];
	double m_Point1[3];
	double pPoint[3];
	Eigen::Vector3d G_Orig_D;
	Eigen::Matrix3Xd result2;
	if (surface != NULL)
	{
		
		const int nbr = surface->GetSurfaceOutput()->GetVTKData()->GetNumberOfPoints();
		double* coord;
		
		coord = new double[3];
		int pt = 0;

		
		mafMatrix tm;
		mafTransform *m_TmpTransform1;
		mafNEW(m_TmpTransform1);
		tm = surface->GetOutput()->GetAbsTransform()->GetMatrix();
		m_TmpTransform1->SetMatrix(tm);
		Eigen::Matrix3Xd vert ;
		Eigen::MatrixX3d vertT ;
		std::vector<std::vector<double>>coordd(nbr, vector<double>(3));
		(vert).resize(3, nbr);
		(vertT).resize(nbr, 3);
		if (nbr > 0)
		{


			double mmean[3];
			mmean[0] = mmean[1] = mmean[2] = 0;

			


			for (int i = 0; i < nbr; i++)
			
			{
				coord = surface->GetSurfaceOutput()->GetVTKData()->GetPoint(i);
				m_TmpTransform1->TransformPoint(coord, coord);



				vert(0, i) = coord[0];
				vert(1, i) = coord[1];
				vert(2, i) = coord[2];
				
				vertT(i, 0) = coord[0];
				vertT(i, 1) = coord[1];
				vertT(i, 2) = coord[2];

			
				(coordd)[i][0] = coord[0];
				(coordd)[i][1] = coord[1];
				(coordd)[i][2] = coord[2];
				mmean[0] = mmean[0] + coord[0] / nbr;
				mmean[1] = mmean[1] + coord[1] / nbr;
				mmean[2] = mmean[2] + coord[2] / nbr;


			}

			
			//principal axes computation
			double CrSec_Ratio = 0;
			double Circumf = 0;
			Eigen::Matrix3d PAxes;
			
			//	Eigen::MatrixXd Cross_Sec_PGD_LCS;
			std::vector<std::vector<double>> Cross_Sec_PGD_LCS(0, vector<double>(7));
			if (nbr > 3)
			{
				PAxes = principalAxesLCS(nbr, coordd, CrSec_Ratio, Circumf, Cross_Sec_PGD_LCS);
		
				G_Orig_D = getOrigin(nbr, coordd);
			}
			else
			{
				wxBusyInfo wai5tsds1ueefqqcu20("error2 size==0");
				Sleep(1500);
			}



			int cols = vert.cols();
			Eigen::Matrix3Xd  af;
			af.resize(3, cols);
			for (int i = 0; i < cols; i++)
			{

				af(0, i) = G_Orig_D(0);
				af(1, i) = G_Orig_D(1);
				af(2, i) = G_Orig_D(2);
			}
			
			Eigen::Matrix3Xd D = PAxes.transpose()*(vert - af);

			//double Delt_1 = 5;///5
			//double Mult_01 = 0.05;
			//double	D_1mm = 1.5;
			//double	D_50mm = 25;
			//double	Fi_stp_5 = 3;

			Eigen::VectorXd X_ind, X_ind_not;
			Eigen::Matrix3Xd D_Lft;
			Eigen::Matrix3Xd D_rgt;
			int nbrD = 0;
			int nbrG = 0;

			


			
			for (int i = 0; i < D.cols(); i++)
		
			{
				if (D(0, i) < 0 + Delt_1)
				{				
					nbrG++;
					D_Lft.conservativeResize(3, nbrG);
					D_Lft(2, nbrG-1) = D(2, i);
					D_Lft(1, nbrG-1) = D(1, i);
					D_Lft(0, nbrG-1) = D(0, i);
				
				}					
				if (D(0, i) > 0 - Delt_1)
				{
					nbrD++;
					D_rgt.conservativeResize(3,nbrD);
					D_rgt(2, nbrD-1) = D(2, i);
					D_rgt(1, nbrD-1) = D(1, i);
					D_rgt(0, nbrD-1) = D(0, i);
				}				
			}
		

			int N_D_Lft = D_Lft.cols();
			int N_D_rgt = D_rgt.cols();
	
			int ind_Z = 2;

			Eigen::VectorXd vwlft;
			vwlft.resize(D_Lft.cols());
			for (int i = 0; i < D_Lft.cols(); i++)
			{
				vwlft(i) =  D_Lft(ind_Z, i);
			}
			Eigen::VectorXd vwrgt;

			vwrgt.resize(D_rgt.cols());
			for (int i = 0; i < D_rgt.cols(); i++)
			{	
				vwrgt(i)= D_rgt(2,i);
			}
		



		
			
			double D_min_lcs = vwlft(0);
			double D_max_Lft = vwlft(0);
			double D_max_rgt = vwrgt(0);


			int I_Lft = 0;
			int I_rgt = 0;

	
			for (int i = 1; i < vwlft.rows(); i++)
			{

		;
				if (vwlft(i) < D_min_lcs)
				{
					D_min_lcs = vwlft(i);
				}

				if (vwlft(i) > D_max_Lft)
				{
					D_max_Lft = vwlft(i);
					I_Lft = i;
				}
			}


			for (int i = 1; i < vwrgt.rows(); i++)
			{
			
				if (vwrgt(i) > D_max_rgt)
				{
					D_max_rgt = vwrgt(i);
					I_rgt = i;
				}
			}

			
			double H_Z = (D_max_Lft - D_min_lcs);
			Eigen::Vector3d lft, rgt;
			lft(0) = D_Lft(0, I_Lft);
			lft(1) = 0;
			lft(2) = D_Lft(ind_Z, I_Lft) - Mult_01*H_Z;
			rgt(0) = D_rgt(0, I_rgt);
			rgt(1) = 0;
			rgt(2) = D_rgt(ind_Z, I_rgt) - Mult_01*H_Z;
		

			Eigen::Vector3d Ori_XYZ;
			Ori_XYZ << 0, 0, 0 + 0.5*(lft(2) + rgt(2));


		
			double mxd, mnd;
			mxd = D(0, 0);
			mnd = D(0, 0);
			for (int i = 1; i <D.cols(); i++)
			{
				if (D(1, i)>mxd)
					mxd = D(0, i);

				if (D(1, i) < mnd)
					mnd = D(0, i);
			}

			bool Femur_tst_yes = 0;
			if (Femur_tst_yes)
			{
				Ori_XYZ(2) = 15.*(mxd - mnd);
				Fi_stp_5 = 0.2;
				D_1mm =8;
				D_50mm = 35;
			}
		


			Eigen::Vector3d V_lft = lft - Ori_XYZ;
			V_lft = V_lft / V_lft.norm();
			Eigen::Vector3d V_rgt = rgt - Ori_XYZ;
			V_rgt = V_rgt / V_rgt.norm();

			
			/////
			
			Eigen::Vector3d u;
			u << 0, 0, -1;
			double Ang_lft = std::acos(V_lft.transpose()*u);
			double Ang_rgt = std::acos(V_rgt.transpose()*u);

			double del_Fi = Fi_stp_5*3.1416 / 180;
			double c_fi = std::cos(del_Fi);
			double s_fi = std::sin(del_Fi);
			
			Eigen::Matrix3d Rot_Y;
			Rot_Y << c_fi, 0, -s_fi, 0, 1, 0, s_fi, 0, c_fi;
			int I_lft_tot = round((Ang_lft) / del_Fi) + 1;
			int I_lft_rgt = round((Ang_lft + Ang_rgt) / del_Fi) + 1;
			
			Eigen::Vector3d V_lft_curr = V_lft;
			int Ind_D_Lft, Ind_D_rgt;

			Eigen::Vector3d Perp_MidLn;
			Eigen::Matrix3Xd Mid_Ln;
			
			Eigen::Vector3d Pnt_mn, Pnt_mn_prp;
			Eigen::Matrix3Xd Mid_Ln_Perp;
			Eigen::Matrix3d R_V_N;
			Eigen::VectorXd S_trg_Area;
			Eigen::Matrix3Xd Twist_X_ax;
			Eigen::VectorXd Rib_Long_Pos;
			Rib_Long_Pos.resize(I_lft_rgt);
			S_trg_Area.resize(I_lft_rgt);
			Rib_Long_Pos(0) = 0;
			Eigen::VectorXd All_CrSec_Ratio;
			Eigen::VectorXd All_Circumf;
			double S_trg = 0;
			All_CrSec_Ratio.resize(I_lft_rgt);
			All_Circumf.resize(I_lft_rgt);
			Mid_Ln.resize(3, 0);
			Mid_Ln_Perp.resize(3, 0);

			
		
			for (int I_lft = 0; I_lft <I_lft_rgt; I_lft++)//4
			
			{
				Eigen::Vector3d v, m;
				v << 0, -1, 0;
				
				Eigen::Vector3d Perp_V_lft_curr = V_lft_curr.cross(v);

				
				
				
				if (I_lft+1 <= I_lft_tot)
				{
					Eigen::Matrix3Xd ooDD;
					ooDD.resize(3, N_D_Lft);
					for (int i = 0; i < N_D_Lft; i++)
					{

						ooDD(0, i) = Ori_XYZ(0);
						ooDD(1, i) = Ori_XYZ(1);
						ooDD(2, i) = Ori_XYZ(2);
					}
					

					
					Eigen::RowVectorXd mplo;
					mplo= Perp_V_lft_curr.transpose()*(D_Lft - ooDD);
					
					
					
					Pnt_mn(0) = Pnt_mn(1) = Pnt_mn(2) = 0;
					int nnbr = 0;
					for (int kk = 0; kk < mplo.cols(); kk++)
					{
						
						if (std::fabs(mplo(kk)) < D_1mm)
						{

							nnbr++;
							Pnt_mn(0) = Pnt_mn(0)+ D_Lft(0, kk);
							Pnt_mn(1) = Pnt_mn(1)+ D_Lft(1, kk);
							Pnt_mn(2) = Pnt_mn(2)+ D_Lft(2, kk);

						}
					}

					
					if (nnbr>0)
					{
						Pnt_mn(0) = Pnt_mn(0) / nnbr;
						Pnt_mn(1) = Pnt_mn(1) / nnbr;
						Pnt_mn(2) = Pnt_mn(2) / nnbr;
						
					}

					

				
					if (I_lft == 0)
						Pnt_mn_prp = Pnt_mn;
					


					if (I_lft > 0)
					{

						
						Eigen::Matrix3Xd mm;
						Eigen::Vector3d m;
						m(0) = Mid_Ln(0, I_lft - 1);
						m(1) = Mid_Ln(1, I_lft - 1);
						m(2) = Mid_Ln(2, I_lft - 1);
						Perp_MidLn = (Pnt_mn - m) / (Pnt_mn - m).norm();

						
						mm.resize(3, N_D_Lft);
						for (int k = 0; k < N_D_Lft; k++)
						{
							mm(0, k) = Pnt_mn(0);
							mm(1, k) = Pnt_mn(1);
							mm(2, k) = Pnt_mn(2);
						}

						Eigen::VectorXd lmp = Perp_MidLn.transpose()*(D_Lft - mm);
						Eigen::VectorXd Ind_Perp_MidLn;
						Ind_Perp_MidLn.resize(lmp.rows());


						
						for (int k = 0; k < lmp.rows(); k++)
						{

							Ind_Perp_MidLn(k) = std::fabs(lmp(k)) < D_1mm;

						}

						Eigen::Matrix3Xd lmp2 = (D_Lft - mm).array()*(D_Lft - mm).array();
						Eigen::VectorXd sum;
						sum.resize(lmp2.cols());

						
						for (int kk = 0; kk < lmp2.cols(); kk++)
						{

							sum(kk) = sqrt((lmp2(0, kk) + lmp2(1, kk) + lmp2(2, kk)) / 3) < D_50mm;
						}
						int lml = 0;
						for (int kk = 0; kk < lmp2.cols(); kk++)
						{
							Ind_Perp_MidLn(kk) = Ind_Perp_MidLn(kk) * sum(kk);
							lml = lml  + Ind_Perp_MidLn(kk);
						}
				
						Eigen::MatrixX3d newDLFt;

						std::vector<std::vector<double>>newDLFt1;

						int sznewDLFt = 0;
						for (int kk = 0; kk < Ind_Perp_MidLn.rows(); kk++)
						{

							if (Ind_Perp_MidLn(kk)>0)

							{
								sznewDLFt++;
							}
						}
						
						newDLFt.resize(sznewDLFt, 3);
						int uf = 0;

						
						
						
						for (int kk = 0; kk <Ind_Perp_MidLn.rows(); kk++)
						{
							if (Ind_Perp_MidLn(kk)>0)

							{
							
								newDLFt(uf, 0) = D_Lft(0, kk);
								newDLFt(uf, 1) = D_Lft(1, kk);
								newDLFt(uf, 2) = D_Lft(2, kk);
								
								uf++;
								newDLFt1.push_back({ D_Lft(0, kk), D_Lft(1, kk), D_Lft(2, kk) });

							}

						}
					
						if (sznewDLFt > 3)
						{
							Eigen::Matrix3d R_V_N = principalAxesLCSRib(sznewDLFt, newDLFt1, CrSec_Ratio, Circumf, Cross_Sec_PGD_LCS, Perp_MidLn);

							
							Eigen::Vector3d R_G_Orig = getOrigin(newDLFt1.size(), newDLFt1);
							
							Eigen::Vector3d  Pnt_mnt_prp = R_G_Orig;

							
						}
						else
						{

							wxBusyInfo wai5tsds1ueefqqcu20("error0 size==0");
							Sleep(1500);
						}
					}//if (I_lft > 0)


					

				}//if (I_lft <= I_lft_tot)
				else
				{
					
					Eigen::Matrix3Xd ooDD;
					ooDD.resize(3, N_D_rgt);
					for (int i = 0; i < N_D_rgt; i++)
					{

						ooDD(0, i) = Ori_XYZ(0);
						ooDD(1, i) = Ori_XYZ(1);
						ooDD(2, i) = Ori_XYZ(2);
					}
					Eigen::VectorXd mplo = Perp_V_lft_curr.transpose()*(D_rgt - ooDD);
				

					
					int nnbr = 0;
					Pnt_mn(0) = Pnt_mn(1) = Pnt_mn(2) = 0;
					for (int kk = 0; kk < mplo.rows(); kk++)
					{

						
						if (std::fabs(mplo(kk)) < D_1mm)
						{
							nnbr++;
							Pnt_mn(0) = Pnt_mn(0) + D_rgt(0, kk);
							Pnt_mn(1) = Pnt_mn(1) + D_rgt(1, kk);
							Pnt_mn(2) = Pnt_mn(2) + D_rgt(2, kk);

						}
					}

					
					if (nnbr > 0)
					{
						Pnt_mn(0) = Pnt_mn(0) / nnbr;
						Pnt_mn(1) = Pnt_mn(1) / nnbr;
						Pnt_mn(2) = Pnt_mn(2) / nnbr;

				

					}

					
					
					if (I_lft > 0)
					{
						Eigen::Matrix3Xd mm;
						Eigen::Vector3d m;

					
						m(0) = Mid_Ln(0, I_lft - 1);
						m(1) = Mid_Ln(1, I_lft - 1);
						m(2) = Mid_Ln(2, I_lft - 1);
						Perp_MidLn = (Pnt_mn - m) / (Pnt_mn - m).norm();


						mm.resize(3, N_D_rgt);
						for (int k = 0; k < N_D_rgt; k++)
						{
							mm(0, k) = Pnt_mn(0);
							mm(1, k) = Pnt_mn(1);
							mm(2, k) = Pnt_mn(2);
						}

						

						Eigen::VectorXd lmp = Perp_MidLn.transpose()*(D_rgt - mm);
						Eigen::VectorXd Ind_Perp_MidLn;
						Ind_Perp_MidLn.resize(lmp.rows());
						for (int k = 0; k < lmp.rows(); k++)
						{

							Ind_Perp_MidLn(k) = std::fabs(lmp(k)) < D_1mm;

						}



						Eigen::Matrix3Xd lmp2 = (D_rgt - mm).array()*(D_rgt - mm).array();
						lmp2.resize(3, mm.cols());
						Eigen::VectorXd sum;
						sum.resize(lmp2.cols());
						for (int kk = 0; kk < lmp2.cols(); kk++)
						{
							lmp2(0, kk) = lmp2(1, kk) = lmp2(2, kk) = 0;
							sum(kk) = sqrt((lmp2(0, kk) + lmp2(1, kk) + lmp2(2, kk)) / 3) < D_50mm;
						}

						for (int u = 0; u < Ind_Perp_MidLn.rows(); u++)
						{
							Ind_Perp_MidLn(u) = Ind_Perp_MidLn(u) * sum(u);
						}


						Eigen::MatrixX3d newDLFt;
						std::vector<std::vector<double>>newDLFt1;
						int uf = 0;

						//string hhhh = "loop3 ";
						//wxBusyInfo waitaqqcuh20(hhhh.c_str());
						///Sleep(1500);

						int sznewDLFT = 0;
						for (int kk = 0; kk <Ind_Perp_MidLn.rows(); kk++)
						{
							//string f = std::to_string(Ind_Perp_MidLn(kk));

							//wxBusyInfo wai5tsds1ueefqqcu20(f.c_str());
							//Sleep(500);
							if (Ind_Perp_MidLn(kk)> 0)

							{
								sznewDLFT++;
					
								uf++;
							}

						}
						newDLFt.resize(sznewDLFT, 3);
						for (int kk = 0; kk <Ind_Perp_MidLn.rows(); kk++)
						{
							
							if (Ind_Perp_MidLn(kk)> 0)

							{						
								
								newDLFt((newDLFt).cols() - 1, 0) = D_rgt(0, kk);
								newDLFt((newDLFt).cols() - 1, 1) = D_rgt(1, kk);
								newDLFt((newDLFt).cols() - 1, 2) = D_rgt(2, kk);

								newDLFt1.push_back({ D_rgt(0, kk), D_rgt(1, kk), D_rgt(2, kk) });
								
							}

						}

					
						if (uf > 3)
						{
							R_V_N = principalAxesLCSRib(uf, newDLFt1, CrSec_Ratio, Circumf, Cross_Sec_PGD_LCS, Perp_MidLn);

							Eigen::Vector3d R_G_Orig = getOrigin(uf, newDLFt1);

							Eigen::Vector3d  Pnt_mnt_prp = R_G_Orig;
//
					
						}
						else
						{
							wxBusyInfo wai5tsds1ueefqqcu20("error1 size==0");
							Sleep(1500);

						}

					}

				


				}//if (I_lft <= I_lft_tot)else
				
				


				///////////


			
				Mid_Ln.conservativeResize(3, Mid_Ln.cols() + 1);
				Mid_Ln(0, Mid_Ln.cols() - 1) = Pnt_mn(0);
				Mid_Ln(1, Mid_Ln.cols() - 1) = Pnt_mn(1);
				Mid_Ln(2, Mid_Ln.cols() - 1) = Pnt_mn(2);

				Mid_Ln_Perp.conservativeResize(3, Mid_Ln_Perp.cols() + 1);
				Mid_Ln_Perp(0, Mid_Ln_Perp.cols() - 1) = Pnt_mn_prp(0);
				Mid_Ln_Perp(1, Mid_Ln_Perp.cols() - 1) = Pnt_mn_prp(1);
				Mid_Ln_Perp(2, Mid_Ln_Perp.cols() - 1) = Pnt_mn_prp(2);
				


				if (I_lft > 0)
				{
					Twist_X_ax.conservativeResize(3, Twist_X_ax.cols() + 1);
					Twist_X_ax(0, Twist_X_ax.cols() - 1) = R_V_N(0, 0);
					Twist_X_ax(1, Twist_X_ax.cols() - 1) = R_V_N(1, 0);
					Twist_X_ax(2, Twist_X_ax.cols() - 1) = R_V_N(2, 0);


					//S_trg_Area.resize(S_trg_Area.rows() + 1);
					//S_trg_Area(S_trg_Area.rows()-1) = S_trg;
					S_trg_Area(I_lft - 1) = S_trg;
					Eigen::Vector3d fg;
					fg(0) = Pnt_mn_prp(0) - Mid_Ln_Perp(0, I_lft - 1);
					fg(1) = Pnt_mn_prp(1) - Mid_Ln_Perp(1, I_lft - 1);
					fg(2) = Pnt_mn_prp(2) - Mid_Ln_Perp(2, I_lft - 1);
					
					Rib_Long_Pos(I_lft) = Rib_Long_Pos(I_lft - 1) + fg.norm();
					
					//All_CrSec_Ratio.resize(All_CrSec_Ratio.rows() + 1);
					//All_CrSec_Ratio(All_CrSec_Ratio.rows() - 1) = CrSec_Ratio;
					All_CrSec_Ratio(I_lft - 1) = CrSec_Ratio;
				
					//All_Circumf.resize(All_Circumf.rows() + 1);
					//All_Circumf(All_Circumf.rows() - 1) = Circumf;
					All_Circumf(I_lft - 1) = Circumf;

				}
				V_lft_curr = Rot_Y*V_lft_curr;
				
			}////for (int I_lft = 0; I_lft < 1; I_lft++)
			
			
			

			/*
			
			Eigen::Vector3d D_min;
			Eigen::Vector3d D_max;

			for (int j = 0; j < vert.cols(); j++)
			for (int i = 0; i < 3; i++)
			{
				if (D_min(i) <(vert)(i, j))
					D_min(i) = (vert)(i, j);

				if (D_max(i) > (vert)(i, j))
					D_max(i) = (vert)(i, j);
			}
		
			Eigen::Vector3d Bone_D_size_XYZ = D_max - D_min;



			/////////////
			/////////if ICR_MHA_Yes
			int Ind_Call = 2;
			double epsiAxe = 1.;
			int Mult_Vert_epsiAxe = 1;
			double Angle_filtr_T = epsiAxe*Mult_Vert_epsiAxe;
			
			Eigen::VectorXd ind_Cross_Sec_PGD_LCS;
			ind_Cross_Sec_PGD_LCS.resize(Cross_Sec_PGD_LCS.size());
			for (int i = 0; i < Cross_Sec_PGD_LCS.size(); i++)
			{

				ind_Cross_Sec_PGD_LCS(i) = i;
			}
			double Mult_ind_Cross = .3;


			for (int i = 0; i < Cross_Sec_PGD_LCS.size(); i++)
			{
				ind_Cross_Sec_PGD_LCS(i) = std::round(Mult_ind_Cross*Cross_Sec_PGD_LCS.size());
			}


			Eigen::MatrixX3d fgg;
			fgg.resize(Cross_Sec_PGD_LCS.size(), 3);


			for (int u = 0; u < Cross_Sec_PGD_LCS.size(); u++)
			{

				fgg(u, 0) = Cross_Sec_PGD_LCS[u][0];
				fgg(u, 1) = Cross_Sec_PGD_LCS[u][1];
				fgg(u, 2) = Cross_Sec_PGD_LCS[u][2];
			}
		

			std::vector<std::vector<double>> Rm_in_PGD_T;
			
			for (int u = 0; u < Cross_Sec_PGD_LCS.size(); u++)
			{
				
				Rm_in_PGD_T.push_back({ fgg(u, 0)*((Cross_Sec_PGD_LCS)[u][1]), fgg(u, 1)*((Cross_Sec_PGD_LCS)[u][2]), fgg(u, 2)*((Cross_Sec_PGD_LCS)[u][3]), (Cross_Sec_PGD_LCS)[u][4], (Cross_Sec_PGD_LCS)[u][5], (Cross_Sec_PGD_LCS)[u][6] });
				
			}
			
			std::vector<std::vector<double>> cr_pos_T(3, vector<double>(3)), disp_sq_T(3, vector<double>(3)), E_MHA_T(3, vector<double>(3)), disp_ang_T(3, vector<double>(3));
			//Rm_in_PGD_T.size();
			//Rm_in_PGD_T[0].size();
	
			///////double yu = ShV_ICR_MHA(Ind_Call, Cross_Sec_PGD_LCS.size(), Rm_in_PGD_T, Angle_filtr_T, cr_pos_T, disp_sq_T, E_MHA_T, disp_ang_T);

			string s = "ShV_ICR_MHA end  "+std::to_string(yu);

			Eigen::Matrix2Xd ICR_MHA_rez_T_CT;

			//	ICR_MHA_rez_T_CT.row(0) = cr_pos_T.row(cr_pos_T.rows() - 1), disp_sq_T.row(cr_pos_T.rows() - 1);
			//	ICR_MHA_rez_T_CT.row(1) = E_MHA_T.row(E_MHA_T.rows() - 1), disp_ang_T.row(disp_ang_T.rows()-1);


			//////////////////////
			//Eigen::Matrix3Xd TwsitD;
			Eigen::Matrix3Xd smTw;
			smTw.resize(3, Twist_X_ax.cols());
			for (int u = 0; u < Twist_X_ax.cols(); u++)
			{
				//TwsitD.resize(3, u+1);

				//TwsitD(0, u) = Twist_X_ax(0, u)*Twist_X_ax(0, u);
				smTw(0, u) = std::sqrt(Twist_X_ax(0, u)*Twist_X_ax(0, u) + Twist_X_ax(1, u)*Twist_X_ax(1, u) + Twist_X_ax(2, u)*Twist_X_ax(2, u));
				smTw(1, u) = std::sqrt(Twist_X_ax(0, u)*Twist_X_ax(0, u) + Twist_X_ax(1, u)*Twist_X_ax(1, u) + Twist_X_ax(2, u)*Twist_X_ax(2, u));
				smTw(2, u) = std::sqrt(Twist_X_ax(0, u)*Twist_X_ax(0, u) + Twist_X_ax(1, u)*Twist_X_ax(1, u) + Twist_X_ax(2, u)*Twist_X_ax(2, u));
			}

			for (int u = 0; u < Twist_X_ax.cols(); u++)
			{
				Twist_X_ax(0, u) = Twist_X_ax(0, u) / smTw(0, u);
				Twist_X_ax(1, u) = Twist_X_ax(1, u) / smTw(1, u);
				Twist_X_ax(2, u) = Twist_X_ax(2, u) / smTw(2, u);
			}

			
			
			Eigen::Matrix3Xd ff;
			ff.resize(3, I_lft_rgt );
			ff(0, 0) = ff(1, 0) = ff(2, 0) = 0;
			for (int u = 1; u < Mid_Ln_Perp .cols(); u++)
			{
			
				ff(0, u) = Mid_Ln_Perp(0, u) - Mid_Ln_Perp(0, u - 1);
				ff(1, u) = Mid_Ln_Perp(1, u) - Mid_Ln_Perp(1, u - 1);
				ff(2, u) = Mid_Ln_Perp(2, u) - Mid_Ln_Perp(2, u - 1);
			}

			
			Eigen::Matrix3Xd smMid_Ln;

			smMid_Ln.resize(3, ff.cols());
			for (int u = 0; u < ff.cols(); u++)
			{
				//TwsitD.resize(3, u+1);
				//smMid_Ln.resize(3, u + 1);
				//TwsitD(0, u) = Twist_X_ax(0, u)*Twist_X_ax(0, u);
				smMid_Ln(0, u) = std::sqrt(ff(0, u)*ff(0, u) + ff(1, u)*ff(1, u) + ff(2, u)*ff(2, u));
				smMid_Ln(1, u) = std::sqrt(ff(0, u)*ff(0, u) + ff(1, u)*ff(1, u) + ff(2, u)*ff(2, u));
				smMid_Ln(2, u) = std::sqrt(ff(0, u)*ff(0, u) + ff(1, u)*ff(1, u) + ff(2, u)*ff(2, u));
			}

			
			Eigen::Matrix3Xd N_Mid_Ln_Perp;// , gl3;

			N_Mid_Ln_Perp.resize(3, ff.cols());
			for (int u = 0; u < ff.cols(); u++)
			{
				N_Mid_Ln_Perp(0, u) = ff(0, u) / (smMid_Ln(0, u) + 0.001);
				N_Mid_Ln_Perp(1, u) = ff(1, u) / (smMid_Ln(1, u) + 0.001);
				N_Mid_Ln_Perp(2, u) = ff(2, u) / (smMid_Ln(2, u) + 0.001);
			}



			Eigen::Matrix3Xd gl3, gl4;
			Eigen::Vector3d gl1, gl2, gl;

			gl4.resize(3, N_Mid_Ln_Perp.cols());

			//gl1.resize(3, Twist_X_ax.cols() - 2);
			//gl2.resize(3, Twist_X_ax.cols() - 2);
			
			for (int u = 0; u < Twist_X_ax.cols()- 1; u++)
			{

				gl1(0) = Twist_X_ax(0, u);
				gl1(1) = Twist_X_ax(1, u);
				gl1(2) = Twist_X_ax(2, u);

				gl2(0) = Twist_X_ax(0, u + 1);
				gl2(1) = Twist_X_ax(1, u + 1);
				gl2(2) = Twist_X_ax(2, u + 1);

				gl = gl1.cross(gl2);

				gl4(0, u) = gl(0);
				gl4(1, u) = gl(01);
				gl4(2, u) = gl(02);
			}

			
			Eigen::VectorXd sds, AngTwist_X_ax;

			sds.resize(N_Mid_Ln_Perp.cols() - 1);
			AngTwist_X_ax.resize(N_Mid_Ln_Perp.cols() - 1);
			gl3.resize(3, N_Mid_Ln_Perp.cols() );
			
			for (int u = 0; u < N_Mid_Ln_Perp.cols() - 1; u++)
			{


				gl3(0, u) = N_Mid_Ln_Perp(0, u)*gl4(0, u);
				gl3(1, u) = N_Mid_Ln_Perp(1, u)*gl4(1, u);
				gl3(2, u) = N_Mid_Ln_Perp(2, u)*gl4(2, u);

				sds(u) = gl3(0, u) + gl3(1, u) + gl3(2, u);
				AngTwist_X_ax(u) = 180 / 3.1416*std::asin(sds(u));
			}



			int I_Strt = 1;
			std::vector<double> Sum_AngTwist_X_ax, Dst_AngTwist_X_ax;
			//Dst_AngTwist_X_ax.resize(AngTwist_X_ax.cols());
			//Sum_AngTwist_X_ax.resize(AngTwist_X_ax.cols());

			Sum_AngTwist_X_ax.push_back(0);
			Dst_AngTwist_X_ax.push_back(0);
			std::vector<double> Dst_AngTwist_X_ax1, Sum_AngTwist_X_ax1, p_PGD1, p_PGD2;

			for (int i = I_Strt; i < AngTwist_X_ax.cols() - 1; I_Strt++)
			{

				
				Sum_AngTwist_X_ax.push_back(Sum_AngTwist_X_ax.at(i - I_Strt) + AngTwist_X_ax(i - 1));
				Dst_AngTwist_X_ax.push_back(Dst_AngTwist_X_ax.at(i - I_Strt) + (Mid_Ln_Perp.col(i) - Mid_Ln_Perp.col(i - 1)).norm());
				Sum_AngTwist_X_ax1.push_back(Sum_AngTwist_X_ax.at(i - I_Strt) + AngTwist_X_ax(i - 1));
				Dst_AngTwist_X_ax1.push_back(Dst_AngTwist_X_ax.at(i - I_Strt) + (Mid_Ln_Perp.col(i) - Mid_Ln_Perp.col(i - 1)).norm());
			}

			int Poly_Pow = 3;


			Eigen::VectorXd p_PGD = polyfit(Dst_AngTwist_X_ax1, Sum_AngTwist_X_ax1, Poly_Pow);

			for (int k = 0; k < p_PGD.size(); k++)
			{
				p_PGD1.push_back(p_PGD[k]);
			}
			Eigen::VectorXd PGD_Gl_11 = polyeval(p_PGD1, Dst_AngTwist_X_ax1);
			/////////


			
			std::vector<double> Dst_AngTwist_X_ax_N, Sum_AngTwist_X_ax_N;
			for (int i = 0; i < Dst_AngTwist_X_ax.size(); i++)
			{

				

				//if ((Dst_AngTwist_X_ax.at(i) > 0.3*Dst_AngTwist_X_ax.at(Dst_AngTwist_X_ax.size() - 1)))
				{
					Dst_AngTwist_X_ax_N.push_back(Dst_AngTwist_X_ax.at(i));
					Sum_AngTwist_X_ax_N.push_back(Sum_AngTwist_X_ax.at(i));
				}
			}
			if (Dst_AngTwist_X_ax_N.size()>=3)
			{	
			p_PGD = polyfit(Dst_AngTwist_X_ax_N, Sum_AngTwist_X_ax_N, Poly_Pow);
			

		}
			for (int k = 0; k < p_PGD.size(); k++)
			{
				p_PGD2.push_back(p_PGD[k]);
			}

			Eigen::VectorXd PGD_Gl_11_N = polyeval(p_PGD2, Dst_AngTwist_X_ax_N);

			
			///global ind_tst

			std::vector<double> Curr_Max_Range;
			bool Lst_Path_Yes = true;
			if (Lst_Path_Yes)
			{
				Curr_Max_Range.push_back(PGD_Gl_11_N.maxCoeff() - PGD_Gl_11_N.minCoeff());
			}
			
			//Bending Ang Plane YZ_Total
			Eigen::Matrix3Xd sum1;
			sum1.resize(3, I_lft_rgt);
			for (int j = 1; j < Mid_Ln_Perp.cols(); j++)
			{
				sum1(0,j) = (Mid_Ln_Perp(0, j ) - Mid_Ln_Perp(0, j-1 )) + (Mid_Ln_Perp(1, j ) - Mid_Ln_Perp(1, j-1 )) + (Mid_Ln_Perp(2, j ) - Mid_Ln_Perp(2,j-1 ));
				sum1(0,j) = sqrt(sum1(j)*sum1(j));

				sum1(2, j) = sum1(1, j) = sum1(0, j);

			}

			

			for (int i = 0; i < 3;i++)
			for (int j = 1; j < Mid_Ln_Perp.cols(); j++)
			{
				N_Mid_Ln_Perp(i,j) = (Mid_Ln_Perp(i, j) - Mid_Ln_Perp(i, j - 1)) / sum1(i, j);
			}
			Eigen::Matrix3Xd  Z_direct;

			Z_direct.resize(3, I_lft_rgt);
			for (int j = 0; j < I_lft_rgt-1; j++)
			{
				Z_direct(0, j) = 0;
				Z_direct(1, j) = 0;
				Z_direct(2, j) = 1;
			}

			
			Eigen::Vector3d uu,vv,ww;
			double var;
			Eigen::VectorXd AngBand_Z_ax;
			AngBand_Z_ax.resize(N_Mid_Ln_Perp.cols() - 1);
			
			for (int i = 0; i < N_Mid_Ln_Perp.cols()-1; i++)
			{
				uu(0) = N_Mid_Ln_Perp(0, i);
				uu(1) = N_Mid_Ln_Perp(1, i);
				uu(2) = N_Mid_Ln_Perp(2, i);

				vv(0)=N_Mid_Ln_Perp(0, i + 1);
				vv(1) = N_Mid_Ln_Perp(1, i + 1);
				vv(2) = N_Mid_Ln_Perp(2, i + 1);

				ww=uu.cross(vv);

				var=std::asin(Z_direct(0)*ww(0) + Z_direct(1)*ww(1) + Z_direct(2)*ww(2));

				AngBand_Z_ax(i) = 180 / 3.1416 *asin(var);
			}
			
			
			I_Strt = 1;
			std::vector<double> TMP_Dst_AngTwist_X_ax;
			for (int i = I_Strt; i < Dst_AngTwist_X_ax.size()-1; i++)
				TMP_Dst_AngTwist_X_ax .push_back(Dst_AngTwist_X_ax.at(i));


			std::vector<double> Sum_AngBand_Z_ax;
			Sum_AngBand_Z_ax.push_back(0);
			I_Strt = 1;
			for (int i = I_Strt; i < AngBand_Z_ax.size() ;i++)
				Sum_AngBand_Z_ax.push_back( Sum_AngBand_Z_ax.at(i - I_Strt ) + AngBand_Z_ax(i - 1));
			double min_Sum_AngBand_Z_ax, Ang_Left, Ang_Right, Angle_Z_Bend;

			if (Lst_Path_Yes)
			{

				min_Sum_AngBand_Z_ax = Sum_AngBand_Z_ax.at(0);
				for (int i = 1; i < Sum_AngBand_Z_ax.size(); i++)
				{
					if (Sum_AngBand_Z_ax.at(i) < min_Sum_AngBand_Z_ax)
						min_Sum_AngBand_Z_ax = Sum_AngBand_Z_ax.at(i);
				}
				
				Ang_Left = Sum_AngBand_Z_ax.at(0) - min_Sum_AngBand_Z_ax;
				Ang_Right = Sum_AngBand_Z_ax.back() - min_Sum_AngBand_Z_ax;
				Angle_Z_Bend = (Ang_Left + Ang_Right) / 2;

				Curr_Max_Range.push_back(Angle_Z_Bend);

				//Bending Ang Plane XZ

				int Bend_Y_ax_Yes = 2;
			

				Eigen::Matrix3Xd XZ_Mid_Ln_Perp;
				Eigen::Matrix3Xd AngBand_Y_ax;

				for (int ax_ind = 0; ax_ind<Bend_Y_ax_Yes; ax_ind++)
				{
					if (Bend_Y_ax_Yes == 2)
					{
					

						int Ind_XYZ = ax_ind;
						
						XZ_Mid_Ln_Perp.resize(Mid_Ln_Perp.rows(), Mid_Ln_Perp.cols());
						
						
						for (int ii = 0; ii <Mid_Ln_Perp.rows(); ii++)
						for (int jj = 0; jj < Mid_Ln_Perp.cols(); jj++)
						{
							XZ_Mid_Ln_Perp(ii,jj) = Mid_Ln_Perp(ii,jj);
						}

						for (int j = 0; j< XZ_Mid_Ln_Perp.cols();j++)
							XZ_Mid_Ln_Perp(Ind_XYZ, j) = 0;
						
						
						
					

						Eigen::Matrix3Xd sum1;

						sum1.resize(3, I_lft_rgt);
						for (int j = 1; j < XZ_Mid_Ln_Perp.cols(); j++)
						{
							sum1(0, j) = (XZ_Mid_Ln_Perp(0, j) - XZ_Mid_Ln_Perp(0, j - 1)) + (XZ_Mid_Ln_Perp(1, j) - XZ_Mid_Ln_Perp(1, j - 1)) + (XZ_Mid_Ln_Perp(2, j) - XZ_Mid_Ln_Perp(2, j - 1));
							sum1(0, j) = sqrt(sum1(j)*sum1(j));

							sum1(2, j) = sum1(1, j) = sum1(0, j);

						}
					

						for (int i = 0; i < 3; i++)
						for (int j = 1; j < Mid_Ln_Perp.cols(); j++)
						{
							N_Mid_Ln_Perp(i, j) = (Mid_Ln_Perp(i, j) - Mid_Ln_Perp(i, j - 1)) / sum1(i, j);
						}


					
						Eigen::Vector3d u, v, w;
						double var;
						AngBand_Y_ax.resize(3,N_Mid_Ln_Perp.cols());
						for (int i = 0; i < N_Mid_Ln_Perp.cols() - 1; i++)
						{
							u(0) = N_Mid_Ln_Perp(0, i);
							u(1) = N_Mid_Ln_Perp(1, i);
							u(2) = N_Mid_Ln_Perp(2, i);

							v(0) = N_Mid_Ln_Perp(0, i + 1);
							v(1) = N_Mid_Ln_Perp(1, i + 1);
							v(2) = N_Mid_Ln_Perp(2, i + 1);

							w = u.cross(v);

							

							AngBand_Y_ax(0,i) = 180 / 3.1416 *asin(w(0));
							AngBand_Y_ax(1, i) = 180 / 3.1416 *asin(w(1));
							AngBand_Y_ax(2, i) = 180 / 3.1416 *asin(w(2));
								
						}
						AngBand_Y_ax(N_Mid_Ln_Perp.cols() - 1) = 0;
						
						
						
						//Y_ax_cross = cross(N_Mid_Ln_Perp(:, 1 : I_lft_rgt - 2), N_Mid_Ln_Perp(:, 2 : I_lft_rgt - 1));
						//AngBand_Y_ax = 180 / pi*asin(Y_ax_cross(Ind_XYZ, :));
					}
					I_Strt = 1;
					std::vector<double>  Sum_AngBand_Y_ax;
					Sum_AngBand_Y_ax.push_back( 0);

					for (int i = I_Strt; i < AngBand_Y_ax.cols() ;i++)
					{
						Sum_AngBand_Y_ax.push_back(Sum_AngBand_Y_ax.at(i - I_Strt ) + AngBand_Y_ax(i - 1));
					}
					if (Lst_Path_Yes)
					{
						//Curr_Max_Range = [Curr_Max_Range(max(Sum_AngBand_Y_ax) - min(Sum_AngBand_Y_ax))];
					}

					
				
					//Out_Plots_Cell = [Out_Plots_Cell; ' *** figure(220); Norm Dist vs Norm ' ax_XYZ ' Bend Ang;'];
					//Out_Plots_Cell = [Out_Plots_Cell; 'num2str([Dst_AngTwist_X_ax; Sum_AngBand_' ax_XYZ '_ax])'];

					//Out_Plots_Cell = [Out_Plots_Cell; num2str([Dst_AngTwist_X_ax(i_Plot_strt:end) - Dst_AngTwist_X_ax(i_Plot_strt); Sum_AngBand_Y_ax(i_Plot_strt:end) - Sum_AngBand_Y_ax(i_Plot_strt)])];
					//Out_Plots_Cell = [Out_Plots_Cell; [' ** ' ax_XYZ ' Band Ang total = ' num2str([Sum_AngBand_Y_ax(end) - Sum_AngBand_Y_ax(i_Plot_strt)]) '°']];

				
				}
			
			
			
		}
	
		*/
		result2 = PAxes*Mid_Ln;

		}
	}
	else
		{
			string s = "vertices extraction problem";
			wxBusyInfo wait12(s.c_str());
			Sleep(500);
		}


	

		
			
			//
		
		
		
		
		vtkPoints* pts = vtkPoints::New();
		vtkPolyData *polydata = vtkPolyData::New(); 
		int num = result2.cols();
		int pointId[2];
		double* f;
		f = new double[3];

	

		

		pts->SetNumberOfPoints(num);
		vtkMAFSmartPointer<vtkCellArray> cellArray;

		for (int ij = 0; ij< num; ij++)
			{

			f[0] = result2(0, ij) + G_Orig_D(0);
			f[1] = result2(1, ij) + G_Orig_D(1);
			f[2] = result2(2, ij) + G_Orig_D(2);
			if (ij==0)
			{
								
				
				m_CloudPath1->SetLandmark(_R("first"), f[0], f[1], f[2], currTs);
				m_CloudPath1->Update();
			}
			else{

				if (ij == num - 1)
				{
					
					m_CloudPath1->SetLandmark(_R("last"), f[0], f[1], f[2], currTs);
					m_CloudPath1->Update();
				}
				else
				{
					
					if (ij < 10)
					{
						//string a = std::to_string(ij);
						//	wxBusyInfo wait(a.c_str());
						//	Sleep(1500);
						//m_CloudPath1->AppendLandmark(f[0], f[1], f[2], "mm", false);
						//m_CloudPath1->Update();
					}
				}
			}

			
				pts->SetPoint(ij, f);

				if (ij > 0)
				{
					pointId[0] = ij - 1;
					pointId[1] = ij;
					cellArray->InsertNextCell(2, pointId);
				}					


			}

		



		m_PolyData->SetPoints(pts);
		m_PolyData->SetLines(cellArray);
		m_PolyData->Update();
			////

			
  mafEvent ev(this,CAMERA_UPDATE);
  this->ForwardUpEvent(&ev);
  
}

Eigen::VectorXd mafVMECenterLine::polyeval(std::vector<double>& coeffs, std::vector<double>& x)
{
	Eigen::VectorXd output;
	output.resize(x.size());
	double result;
	for (int j = 0; j < x.size(); j++)
	{	result = 0.0;
		for (int i = 0; i < coeffs.size(); i++)
		{
			result += coeffs[i] * pow(x[j], i);

		}
		output(j) = result;
	}
	return output;
}
double mafVMECenterLine::ShV_ICR_MHA(int Ind_Call, int nbr,std::vector<std::vector<double>>& Rm_in_PGD, double Angle_filtr, std::vector<std::vector<double>>& cr_pos, std::vector<std::vector<double>>& disp_sq, std::vector<std::vector<double>>& E_MHA, std::vector<std::vector<double>>& disp_ang)
{

	
	Eigen::Matrix3d eye = Eigen::Matrix3d::Identity();
	int Mtr_Tr_s[2];

//	wxBusyInfo wait("ShV_ICR_MHA  init");
//	Sleep(1500);

	Mtr_Tr_s[0] = Rm_in_PGD.size();
	Mtr_Tr_s[1] = Rm_in_PGD[0].size();
	int k_frames;
	Eigen::Matrix3d M_1, M_2;
	Eigen::Vector3d Transl_k1, Transl_k2;
	

//	Mtr_Tr_s[2] = d3;

	Eigen::MatrixXd Qv_hist,u_hist;

	/*if (Ind_Call == 1)
	{
		k_frames = Mtr_Tr_s[2];

		for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			M_1(i, j) = Rm_in_PGD[i][j][0];
		}


		
		Transl_k1(0) = Rm_in_PGD[0][3][0];
		Transl_k1(01) = Rm_in_PGD[1][3][0];
		Transl_k1(02) = Rm_in_PGD[2][3][0];
	}
	else*/
	
		k_frames = Mtr_Tr_s[0];

		std::vector<double> x;


		
		for (int j = 0; j < 3; j++)
			x.push_back( 3.1416*Rm_in_PGD[0][j] / 180);


		M_1 = ov_mr_ShV(x);

		Transl_k1(0) = Rm_in_PGD[0][3];
		Transl_k1(1) = Rm_in_PGD[0][4];
		Transl_k1(2) = Rm_in_PGD[0][5];
	
		double Eps_PC = 1.e-12, Gr_Rad = atan(1.) / 45.;
		Eigen::Vector3d Qv_t = Eigen::Vector3d::Zero();
		Eigen::Matrix3d Qt_t = Eigen::Matrix3d::Zero();
		Eigen::Matrix3d R_MHA = Eigen::Matrix3d::Zero();
		int k_fr_rez = 0;
//		string ii = "k frames " + std::to_string(k_frames) ;
//		wxBusyInfo wait4(ii.c_str());
//		Sleep(1500);
		for (int k = 1; k < k_frames;k++)
		{
			/*if (Ind_Call == 1)
			{
				for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
				{
					M_1(i, j) = Rm_in_PGD[i][j][k];
				}
				Transl_k2(0) = Rm_in_PGD[0][3][k];
				Transl_k2(01) = Rm_in_PGD[01][3][k];
				Transl_k2(02) = Rm_in_PGD[02][3][k];
			}*/
			//else
			//{
				std::vector<double> x1;
				//string ii = "k_fr_rez " + std::to_string(k_fr_rez);
				//wxBusyInfo wait4(ii.c_str());
				//Sleep(1500);
				for (int j = 0; j < 3; j++)
				{

					
					x1.push_back(3.1416*Rm_in_PGD[k][j] / 180);
				}
					
				
					
					
				M_2 = ov_mr_ShV(x1);
//				string iii = "ov_mr_ShV ends..." + std::to_string(k);
//				wxBusyInfo wait4i(iii.c_str());
//				Sleep(1500);
				
				Transl_k2(0) = Rm_in_PGD[k][3];
				Transl_k2(1) = Rm_in_PGD[k][4];
				Transl_k2(2) = Rm_in_PGD[k][5];

//				string iii2 = "Transl_k2 init ok..." + std::to_string(k);
//				wxBusyInfo wait4i2(iii2.c_str());
//				Sleep(1500);
			//}
		
			Eigen::Matrix3d R = M_2*M_1.transpose();
//			string iii2R = "R ok..." + std::to_string(k);
//			wxBusyInfo wait4iR(iii2R.c_str());
//			Sleep(1500);
			double	cq = 0.5*(R(0, 0) + R(1, 1) + R(2, 2) - 1.); 
			double	qc = 0.;
			
//			string iti = "pt inserted " + std::to_string(M_2(0)) + " " + std::to_string(M_2(1)) + " " + std::to_string(M_2(2)) ;
//			wxBusyInfo wait4j(iti.c_str());
//			Sleep(1500);
			if (abs(cq) < 1.)
			{ 
				qc = acos(cq) / Gr_Rad;
			}

			if (qc >= Angle_filtr)
			{
				k_fr_rez = k_fr_rez + 1;
				Eigen::Vector3d v = Transl_k2 - R*Transl_k1;
				Eigen::Matrix3d Q  = R - eye;
				Eigen::Vector3d Qv = -Q.transpose()*v;
				Eigen::Matrix3d Qt = Q*Q.transpose();
				Eigen::Vector4d uu = extract(R);
				
				Eigen::Vector3d u;
				u(0) = uu(0); 
				u(1) = uu(1);
				u(2) = uu(2);
				double teta_out = uu(2);
				double t_t = .5 / (1. - cos(teta_out));
				//////////
				string tyt = "Qv... " + std::to_string(k_fr_rez);
				wxBusyInfo wait4053(tyt.c_str());
				Sleep(1500);
				Qv_hist.conservativeResize(3, k_fr_rez+1);
				u_hist.conservativeResize(3, k_fr_rez+1);
				for (int y = 0; y <3 ;y++)
				{
					Qv_hist(y,k_fr_rez) = t_t*Qv(y);
					u_hist(y,k_fr_rez) = u(y);
				}
				R_MHA = R_MHA + eye - u*u.transpose();
				Qv_t = Qv_t + Qv;
				Qt_t = Qt_t + Qt;
				////////
				Eigen::Matrix3d R_inv;// cr_pos, disp_sq, disp_ang;
				wxBusyInfo wait4("ShV_ICR_MH4");
				Sleep(1500);
				if (k_fr_rez > 0)
				{
					Eigen::RowVector3d C_22, pp;
					R_inv = Qt_t.inverse();
					pp(0)=cr_pos[k_fr_rez - 1][0] = (R_inv*Qv_t).transpose()(0);
					pp(1)=cr_pos[k_fr_rez - 1][1] = (R_inv*Qv_t).transpose()(1);
					pp(2)=cr_pos[k_fr_rez - 1][2] = (R_inv*Qv_t).transpose()(2);

					wxBusyInfo waitfg4("cr_pos ok");
					Sleep(1500);
					double disp_tmp = 0;
					
					double C_2,C_1;
					
					for (int k_3 = 0; k_3 <k_fr_rez; k_3++)
					{
						//pp(0) = cr_pos[0][k_fr_rez - 1];
						//pp(1) = cr_pos[1][k_fr_rez - 1];
						//pp(2) = cr_pos[2][k_fr_rez - 1];
						string rt = "pp ok " + std::to_string(k_3) + " " + std::to_string(Qv_hist.cols()) + " " + std::to_string(u_hist.cols());
						wxBusyInfo wait412(rt.c_str());
						Sleep(1500);
						C_1 = (pp - Qv_hist.col(k_3).transpose())*u_hist.col(k_3);
						C_22 = pp - Qv_hist.col(k_3).transpose();
						C_2 = C_22*C_22.transpose();
						disp_tmp = disp_tmp + (C_2-C_1*C_1);


					}
					for (int k = 0; disp_sq[0].size();k++)
						disp_sq[k_fr_rez - 1][k] = std::sqrt(disp_tmp/k_fr_rez);


					wxBusyInfo wait4qq12("disp_sq ok");
					Sleep(1500);
					R_inv = R_MHA.inverse();
					double E_ang=Max_Eigen_v(3,3,R_inv,Eps_PC * 1.e5);

					double E_ang_k_fr_rez = std::sqrt(E_ang / k_fr_rez);
					if (std::abs(E_ang_k_fr_rez) > 1)
						E_ang_k_fr_rez = 1;
					wxBusyInfo wait4qqsq12("disp_ang ...");
					Sleep(1500);
					for (int k = 0; disp_ang[0].size(); k++)
						disp_ang[k_fr_rez - 1][k] = std::asin(E_ang_k_fr_rez / Gr_Rad);

					wxBusyInfo wait5("ShV_ICR_MHA5");
					Sleep(1500);

					
				}
				
				M_1 = M_2;
				Transl_k1 = Transl_k2;

			}
			//k_fr_rez = k_fr_rez - 1;
		}
/////
/////
/////
		k_fr_rez = k_fr_rez - 1;
		return k_fr_rez;

}
Eigen::RowVectorXd mafVMECenterLine::deg2rad(Eigen::RowVectorXd a)
{
	Eigen::RowVectorXd r;

	for (int i = 0; i < a.rows(); i++)
	{
		r.resize(i + 1);
		r(i) = a(i) - 3.1416 / 180;
	}

	return r;
}
/*Eigen::Matrix3d  mafVMECenterLine::ov_mr_ShV(Eigen::RowVector3d v_ort)

{
	Eigen::Matrix3d mr;
	
	if (v_ort(0) == 0 && v_ort(1) == 0 && v_ort(2)==0)
	{
		mr = Eigen::Matrix3d::Identity();
	}
	else
	{
		double tt = std::sqrt(v_ort*v_ort.transpose());
		Eigen::Matrix3d A;
		Eigen::Matrix3d eye = Eigen::Matrix3d::Identity();
		A <<0, - v_ort(2)  ,v_ort(1);v_ort(2) ,  0 ,-v_ort(0);-v_ort(1),v_ort(0),0;
		mr = cos(tt)*eye + (sin(tt) / tt)*A + ((1-std::cos(tt)) / (tt*tt))*(v_ort.transpose()*v_ort);
	}
	return mr;
	
}*/
//-----------------------------------------------------------------------
int mafVMECenterLine::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
//	Superclass::InternalStore(parent);
	if (Superclass::InternalStore(parent)==MAF_OK)
	{
		if (
			parent->StoreMatrix(_R("Transform"), &m_Transform->GetMatrix()) == MAF_OK 
			//&& parent->StoreInteger("surface",&m_SurfaceName) == MAF_OK
			)
			
		{
 
			
			return MAF_OK;
		}
		return MAF_OK;
	}
	return MAF_ERROR;
}

//-----------------------------------------------------------------------
int mafVMECenterLine::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
	if (Superclass::InternalRestore(node)==MAF_OK)
	{
    mafMatrix matrix;
	if (node->RestoreMatrix(_R("Transform"), &matrix) == MAF_OK 
		//&& node->RestoreInteger("surface", &m_SurfaceName) == MAF_OK
			
		)
	  {
		  m_Transform->SetMatrix(matrix);
		 return MAF_OK;
	  }
	else{
	
			wxBusyInfo wait400("pb restoring ...");
			Sleep(2500);
	}
	}
  return MAF_ERROR;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//char** mafVMECenterLine::GetIcon()
//-------------------------------------------------------------------------
//{
//  #include "mafVMEProcedural.xpm"
//  return mafVMEProcedural_xpm;
//}








void mafVMECenterLine::SetTimeStamp(mafTimeStamp t)
//-------------------------------------------------------------------------
{
	
	Superclass::SetTimeStamp(t);
	
	this->InternalUpdate();
	
}



mafVMESurface *mafVMECenterLine::GetSurfaceVME()
//-------------------------------------------------------------------------
{
	return mafVMESurface::SafeDownCast(GetLink(_R("SurfaceVME")));
}
void mafVMECenterLine::UpdateLinks()
//-------------------------------------------------------------------------
{
	mafID sub_id = -1;
	mafNode* nd = NULL;



	surface = GetSurfaceVME();
	if (surface && surface->IsMAFType(mafVMESurface))
	{
		nd = GetLink(_R("SurfaceVME"));
		m_SurfaceName = (nd != NULL) ? ((mafVMESurface *)surface)->GetName() : _L("none");
	}
	else
		m_SurfaceName = surface ? surface->GetName() : _L("none");
		

}

void mafVMECenterLine::SetSurfaceLink(const mafString& link_name, mafNode *n)
//-------------------------------------------------------------------------
{	
	if (n->IsMAFType(mafVMESurface))
		SetLink(link_name, n);
	else
		SetLink(link_name, NULL);
}

Eigen::Matrix3d mafVMECenterLine::ov_mr_ShV(std::vector<double>& v_ort)
{
	Eigen::RowVector3d v_ort1;
	v_ort1(0)= v_ort[0];
	v_ort1(1) = v_ort[1];
	v_ort1(2) = v_ort[2];
	Eigen::Matrix3d mr;
	if ((v_ort[0] == 0) && (v_ort[1] == 0) && (v_ort[2] == 0))
		mr<<1,0,0,0,1,0,0,0,1 ;
	else
	{
		double tt = std::sqrt(v_ort[0] * v_ort[0] + v_ort[1] * v_ort[1] + v_ort[2] * v_ort[2]);
		Eigen::Matrix3d A;
		Eigen::Matrix3d id;
		id << 1, 0, 0, 0, 1, 0, 0, 0, 1;
		A<<0, -v_ort[2], v_ort[1],		v_ort[2],   0, - v_ort[0],-v_ort[1],   v_ort[0],   0;
		mr = cos(tt)*id + (sin(tt) / tt)*A + ((1 - cos(tt)) / tt*tt)*(v_ort1.transpose()*v_ort1);
	}	
	return mr;
}
Eigen::VectorXd mafVMECenterLine::polyfit(std::vector<double>& xvals, std::vector<double>& yvals, int order)
{
	assert(xvals.size() == yvals.size());
	assert(order >= 1 && order <= xvals.size() - 1);

	wxBusyInfo wait30("assertion ok");
	Sleep(1500);
	Eigen::MatrixXd A(xvals.size(), order + 1);


	for (int i = 0; i < xvals.size(); i++) {
		A(i, 0) = 1.0;
	}

	for (int j = 0; j < xvals.size(); j++) {
		for (int i = 0; i < order; i++) {
			A(j, i + 1) = A(j, i) * xvals[j];
		}
	}
	

	
	Eigen::VectorXd yyvals;
	yyvals.resize(yvals.size());
	for (int j = 0; j < yvals.size(); j++) {

		yyvals(j) = yvals[j];
	}

	auto Q = A.householderQr();
	auto result = Q.solve(yyvals);
	return result;
}
Eigen::Vector3d mafVMECenterLine::getOrigin(int sz,vector<vector<double>>& vertex)
{
	
	Eigen::RowVector3d mean;
	

	
	mean(0) = mean(1) = mean(2) = 0;
	for (int i = 0; i < sz; i++)
	{
		mean(0) = mean(0)+vertex[i][0] / sz;
		mean(1) = mean(1) + vertex[i][1] / sz;
		mean(2) = mean(2) + vertex[i][2] / sz;

	
	}
	


	const Eigen::Vector3d G_Orig = mean.transpose();
	
	return G_Orig;


}