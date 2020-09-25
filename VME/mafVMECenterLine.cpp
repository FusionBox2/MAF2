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

#include "mafVMEMeter.h"
#include "mafVMEPlane.h"
#include "../../Eigen/unsupported/Eigen/Polynomials"
const bool DEBUG_MODE = true;
using namespace Eigen;

mafCxxTypeMacro(mafVMECenterLine);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafVMECenterLine::mafVMECenterLine()
//-------------------------------------------------------------------------
{
	centerTemp = new double[3];
	centerTemp[0] = 0; centerTemp[01] = 0; centerTemp[02] = 0;


	mafString gLength = "";


	





  


  //vtkNEW(m_Goniometer);

  //m_Goniometer->Update();



  
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
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}
//-------------------------------------------------------------------------
Eigen::Matrix3d mafVMECenterLine::principalAxes(int nbr, vector<vector<double>>& vertex, double Circumf, double CrSec_Ratio, std::vector<std::vector<double>>& Cross_Sec_PGD_LCS)
{
	
	


	
	Eigen::Matrix3d V_N;
	Eigen::RowVector3d mean = getOrigin(nbr, vertex);
	int sz = nbr;
	
	string s551 = "sz " + std::to_string(sz);
	Sleep(1500);


	double Perp_MidLn=1;
	Eigen::MatrixX3d centeredVertex,vertex1;
	centeredVertex.resize(sz, 3);
	for (int i = 0; i < sz; i++)
	{
		
		centeredVertex(i, 0)=vertex[i][0] - mean(0);
		centeredVertex(i, 1) = vertex[i][1] - mean(1);
		centeredVertex(i, 2) = vertex[i][2] - mean(2);
		
		vertex1.resize(i + 1, 3);
		vertex1(i, 0) = (vertex)[i][0];
		vertex1(i, 1) = (vertex)[i][1] ;
		vertex1(i, 2) = (vertex)[i][2] ;
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



	Eigen::Vector3d valuesd = values.cwiseAbs2();
	Eigen::Matrix3d vectorsd = vectors.cwiseAbs2();


	string s55v0 = "values... " + std::to_string(valuesd(0)) + "   " + std::to_string(valuesd(1)) + "    " + std::to_string(valuesd(2));
	wxBusyInfo wait550v0(s55v0.c_str());
	Sleep(2500);
	Eigen::Vector3d lam;
	Eigen::Vector3i indlam;
	lam << 1 / valuesd(0), 1 / valuesd(1), 1 / valuesd(2);
	lam = lam / lam.norm();
	/////
	//lam(0) = 0.98694; lam(1) =0.1193 ; lam(2) =0.10821 ;
	
	/////
	string s550 = "lam... " + std::to_string(lam(0)) + " " + std::to_string(lam(1)) + " " + std::to_string(lam(2));
	wxBusyInfo wait550(s550.c_str());
	Sleep(1500);
	
	

	vectorsd.col(0) = vectorsd.col(0) / vectorsd.col(0).norm();
	vectorsd.col(01) = vectorsd.col(01) / vectorsd.col(01).norm();
	vectorsd.col(02) = vectorsd.col(02) / vectorsd.col(02).norm();

	vectorsd.col(2) = vectorsd.col(0).cross(vectorsd.col(01));

	
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
			indlam(1) == i;

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
			V_N.col(0) = V_N.col(1).cross(V_N.col(2));
		}
	}
	if (V_N(1, 0) < 0)
	{
		for (int i = 0; i < 3; i++)
		{
			V_N(i, 0) = -V_N(i, 0);
			V_N.col(1) = V_N.col(2).cross(V_N.col(0));
		}
	}
	for (int i = 0; i < 3; i++)
	if (V_N(i, 1)*Perp_MidLn < 0)
	{
		V_N(i, 1) = -V_N(i, 1);
		V_N.col(2) = V_N.col(0).cross(V_N.col(1));
	}



	string s300= "VN ok ";
	wxBusyInfo wait300(s300.c_str());
	Sleep(1500);


	Eigen::Matrix3Xd aa;
	aa.resize(3, sz);
	Eigen::Vector3f bbb;
	for (int i = 0; i < sz; i++)
	for (int j = 0; j < 3; j++)
	{
		
		aa(j,i) = G_Orig(j);
	}

	
	Eigen::Matrix3Xd	Loc_XYZ_Vertx = V_N.transpose()*((vertex1).transpose() - aa);
	
	string s2 = "Loc_XYZ_Vertx size " + std::to_string(Loc_XYZ_Vertx.cols());
	wxBusyInfo wait2(s2.c_str());
	Sleep(500);
	for (int i = 0; i < sz; i++)
	{
		Loc_XYZ_Vertx(1,i) = 0;
	}


	
	Eigen::Vector3d S;

	Eigen::Matrix3Xd P = Loc_XYZ_Vertx.cwiseProduct(Loc_XYZ_Vertx);

	string s350 = "P ok ";
	wxBusyInfo wait350(s350.c_str());
	Sleep(1500);
	

	S = P.rowwise().sum();
	Eigen::Vector3d Dist_Loc_XYZ_Vertx = S.cwiseProduct(S);
	
	Eigen::Matrix3Xd Dir_Loc_XYZ_Vertx;

	for (int i = 0; i < 3; i++)
	{
		bbb(i) = Dist_Loc_XYZ_Vertx(i);
	}
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < sz; j++)
	{
		Dir_Loc_XYZ_Vertx.resize(3,j+1);
		Dir_Loc_XYZ_Vertx(i, j) = Loc_XYZ_Vertx(i, j) / Dist_Loc_XYZ_Vertx(i);
	}

	
	double Ang_Step_5 = 10 * 3.14159 / 180;
	Eigen::RowVectorXf Ang_Scan, X_cos, Z_sin;
	
	for (int i = 0; i < (2 * 3.14159) / Ang_Step_5; i++)
	{
		Ang_Scan.resize(i+1);
		X_cos.resize(i + 1);
		Z_sin.resize(i + 1);
		Ang_Scan(i) = i*Ang_Step_5;
		X_cos(i) = std::cos(Ang_Scan(i));
		Z_sin(i) = std::sin(Ang_Scan(i));
	}




	Eigen::Vector3f ff;
	ff(0) = X_cos(0); ff(1) = 0; ff(2) = Z_sin(0);
	string s450 = "ff ok ";
	wxBusyInfo wait450(s450.c_str());
	Sleep(1500);
	Eigen::MatrixX3f m;
	for (int k = 0; k < sz; k++)
	{
		m.resize(k + 1, 3);
		m(k,0) =ff(0);
		m(k, 1) = ff(1);
		m(k, 2) = ff(2);

	}

	Eigen::MatrixX3d Nm;
	Eigen::RowVector3d Prj_dir;
	for (int j = 0; j < 3; j++)
	for (int i = 0; i < sz; i++)
	{

		Nm.resize(i+1,3);
		Nm(i, j) = Dir_Loc_XYZ_Vertx(j, i)*m(j);
	}

	
	for (int i = 0; i < 3; i++)
	{
		double sum = 0;
		for (int k = 0; k < sz; k++)
			sum = sum + Nm(k,i);
		Prj_dir(i) = sum;
	}


	double Ang_cmp = Ang_Step_5 / 3;

	Eigen::Vector3d  Ind_ang;
	for (int i = 0; i < 3; i++)
	{
		Ind_ang(i) = std::acos(Prj_dir(i)) < 9 * Ang_cmp;
	}
	
	string s500 = "Loc_XYZ_Vertx_prj... ";
	wxBusyInfo wait500(s500.c_str());
	Sleep(1500);
	Eigen::MatrixX3d Loc_XYZ_Vertx_prj, Loc_XYZ_Vertx_prjPow2;

	Loc_XYZ_Vertx_prj.resize(sz, 3);
	Loc_XYZ_Vertx_prjPow2.resize(sz, 3);
	for (int i = 0; i < sz; i++)
	{
		
		Loc_XYZ_Vertx_prj(i,0) = Loc_XYZ_Vertx( Ind_ang(0),i);
		Loc_XYZ_Vertx_prj(i, 1) = Loc_XYZ_Vertx(Ind_ang(1), i);
		Loc_XYZ_Vertx_prj(i, 2) = Loc_XYZ_Vertx(Ind_ang(2), i);

		Loc_XYZ_Vertx_prjPow2(i,0) = Loc_XYZ_Vertx( Ind_ang(0),i)*Loc_XYZ_Vertx( Ind_ang(0),i);
		Loc_XYZ_Vertx_prjPow2(i, 1) = Loc_XYZ_Vertx(Ind_ang(1), i)*Loc_XYZ_Vertx(Ind_ang(1), i);
		Loc_XYZ_Vertx_prjPow2(i, 2) = Loc_XYZ_Vertx(Ind_ang(2), i)*Loc_XYZ_Vertx(Ind_ang(2), i);
	}
	string s500d = "Loc_XYZ_Vertx_prj ok "+std::to_string(sz);
	wxBusyInfo wait500d(s500d.c_str());
	Sleep(1500);
	//Eigen::Vector3d::Index   maxIndex;
	Eigen::Vector3d Loc_XYZ_Vertx_prjPow2Sum = Loc_XYZ_Vertx_prjPow2.colwise().sum();
	//Loc_XYZ_Vertx_prjPow2.rowwise().sum().maxCoeff(&maxIndex);
	double Max_dist = std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(0));
	int ind_Max_Dist = 0;

	
	for (int i = 1; i <Loc_XYZ_Vertx_prjPow2Sum.cols(); i++)
	{
		if (std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(i))>Max_dist)
		{

			Max_dist = std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(i));
			ind_Max_Dist = i;

		}
	}

	Eigen::Vector3d Curr_point_0;
	Curr_point_0(0) = Loc_XYZ_Vertx_prjPow2(ind_Max_Dist, 0);
	Curr_point_0(1) = Loc_XYZ_Vertx_prjPow2(ind_Max_Dist, 1);
	Curr_point_0(2) = Loc_XYZ_Vertx_prjPow2(ind_Max_Dist, 2);


	string s5ee50 = "Ceeurr_point_0... ";
	wxBusyInfo waitee550(s5ee50.c_str());
	Sleep(1500);

	Eigen::MatrixX3d Test_Pnt;
	Test_Pnt.resize(1, 3);
	Test_Pnt(0, 0) = Curr_point_0(0);
	Test_Pnt(0, 1) = Curr_point_0(01);
	Test_Pnt(0, 2) = Curr_point_0(02);


	double S_trg = 0;// Cross_Sec_Area
	//Circumf= 0; //current Circumference
	// global Perp_MidLn Cross_Sec_PGD_LCS ICR_MHA_Yes S_trg CrSec_Ratio Circumf



	
	for (int i = 1; i < Ang_Scan.cols(); i++)

	{
	
		
		ff(0) = X_cos(i); ff(1) = 0; ff(2) = Z_sin(i);
		Eigen::MatrixX3f m;
		for (int k = 0; k < sz; k++)
		{
			m.resize(k + 1, 3);
			m(k, 0) = ff(0);
			m(k, 1) = ff(1);
			m(k, 2) = ff(2);

		}


		

		//Eigen::Matrix3Xd Nm;
		//Eigen::RowVector3d Prj_dir;
		for (int j = 0; j < 3; j++)
		for (int i = 0; i < sz; i++)
		{
			Nm(i, j) = Dir_Loc_XYZ_Vertx(j, i)*m(j);
		}

	

		

		for (int i = 0; i < 3; i++)
		{
			double sum = 0;
			for (int k = 0; k < sz; k++)
				sum = sum + Nm(k,i);
			Prj_dir(i) = sum;
		}


		
		
		double Ang_cmp = Ang_Step_5 / 3;

		Eigen::Vector3d   Ind_ang;
		for (int i = 0; i < 3; i++)
			Ind_ang(i) = std::acos(Prj_dir(i)) < 9 * Ang_cmp;


		//Eigen::RowVectorXd Loc_XYZ_Vertx_prj, Loc_XYZ_Vertx_prjPow2;
		for (int i = 0; i < sz; i++)
		{
			Loc_XYZ_Vertx_prj(i, 0) = Loc_XYZ_Vertx(Ind_ang(0), i);
			Loc_XYZ_Vertx_prj(i, 1) = Loc_XYZ_Vertx(Ind_ang(1), i);
			Loc_XYZ_Vertx_prj(i, 2) = Loc_XYZ_Vertx(Ind_ang(2), i);

			Loc_XYZ_Vertx_prjPow2(i, 0) = Loc_XYZ_Vertx(Ind_ang(0), i)*Loc_XYZ_Vertx(Ind_ang(0), i);
			Loc_XYZ_Vertx_prjPow2(i, 1) = Loc_XYZ_Vertx(Ind_ang(1), i)*Loc_XYZ_Vertx(Ind_ang(1), i);
			Loc_XYZ_Vertx_prjPow2(i, 2) = Loc_XYZ_Vertx(Ind_ang(2), i)*Loc_XYZ_Vertx(Ind_ang(2), i);
		}
		string s750 = "Loc_XYZ_Vertx_prjPow2... ";
		wxBusyInfo wait750(s750.c_str());
		Sleep(1500);
		//Eigen::RowVectorXd
		Loc_XYZ_Vertx_prjPow2Sum =  Loc_XYZ_Vertx_prjPow2.colwise().sum();
		double Max_dist = std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(0));
		int ind_Max_Dist = 0;
		for (int i = 1; i <Loc_XYZ_Vertx_prjPow2Sum.cols(); i++)
		{
			if (std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(i))>Max_dist)
			{

				Max_dist = std::sqrt(Loc_XYZ_Vertx_prjPow2Sum(i));
				ind_Max_Dist = i;

			}
		}

		//string s751 = "Loc_XYZ_Vertx_prjPow2 ok ";
		//wxBusyInfo wait751(s751.c_str());
		//Sleep(1500);
		Eigen::Vector3d Curr_point_1;
		Curr_point_1(0) = Loc_XYZ_Vertx_prj( ind_Max_Dist,0);
		Curr_point_1(01) = Loc_XYZ_Vertx_prj( ind_Max_Dist,1);
		Curr_point_1(02) = Loc_XYZ_Vertx_prj(ind_Max_Dist,2);
		double a = Curr_point_0.norm();
		double b = Curr_point_1.norm();
		double c = (Curr_point_0 - Curr_point_1).norm();
		//string s752 = "a b c ok ";
		//wxBusyInfo wait752(s752.c_str());
		//Sleep(1500);
		double p = 0.5*(a + b + c);
		S_trg = S_trg + sqrt(p*(p - a)*(p - b)*(p - c));
		(Circumf) = (Circumf) + c; // current Circumference
		Test_Pnt.resize(Test_Pnt.rows()+1,3);
		Test_Pnt(Test_Pnt.rows()-1,0) = Curr_point_1(0);
		Test_Pnt(Test_Pnt.rows() - 1, 1) = Curr_point_1(1);
		Test_Pnt(Test_Pnt.rows() - 1, 2) = Curr_point_1(2);
		Curr_point_0 = Curr_point_1;

		//string s800 = "}} ";
		//wxBusyInfo wait800(s800.c_str());
		//Sleep(1500);
		
	}



	//string s650 = "szTstPts... ";
	//wxBusyInfo wait650(s650.c_str());
	//Sleep(1500);

	double szTstPts;
	//double CrSec_Ratio;
	szTstPts = Test_Pnt.rows();
	if (szTstPts > 5)
	{
	
		

		ellipse_struct* ellipse_t = fitellipse(Test_Pnt);


		
		if (ellipse_t->a > 0)
		{

			double a = std::max(ellipse_t->a, ellipse_t->b);
			double b = std::min(ellipse_t->a, ellipse_t->b);
			double X0 = ellipse_t->X0;
			double Y0 = ellipse_t->Y0;
			// the ellipse
			Eigen::VectorXd theta_r, ellipse_x_r, ellipse_y_r, zer;
			for (int i = 0; i < 40; i++)
			{
				theta_r.resize(i + 1); 
				ellipse_x_r.resize(i + 1);
				ellipse_y_r.resize(i + 1);
				zer.resize(i + 1);
				theta_r(i) = 2 * 3.14159 / (40 - i);
				ellipse_x_r(i) = X0 + a * cos(theta_r(i));
				ellipse_y_r(i) = Y0 + b * std::sin(theta_r(i));
				zer(i) = 0;
			}

			

			Test_Pnt << ellipse_x_r; zer; ellipse_y_r;
			
			S_trg = 3.14159*a*b;

			Circumf = 3.14159*(3 * (a + b) - sqrt((3 * a + b)*(a + 3 * b))); //current Circumference
			CrSec_Ratio = b / a;



		}
	}

	
	
	S_trg = 1;
	if (S_trg > 0) // NEW LCS 

	{

		//string s455 = std::to_string(Test_Pnt.rows()) + " " + std::to_string(Test_Pnt.cols());
		//wxBusyInfo wait455(s455.c_str());
		//Sleep(2500);
		Eigen::MatrixX3d vertex1 = Test_Pnt;//Test_Pnt.transpose();

		
		sz = vertex1.rows();
		mean = vertex1.colwise().mean();
		

		for (int i = 0; i < sz; i++)
		{
			centeredVertex.resize(i + 1, 3);
			centeredVertex(i, 0) = vertex1(i, 0) - mean(0);
			centeredVertex(i, 1) = vertex1(i, 1) - mean(1);
			centeredVertex(i, 2) = vertex1(i, 2) - mean(2);
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

		Eigen::Vector3d valuesd = values.cwiseAbs2();
		Eigen::Matrix3d vectorsd = vectors.cwiseAbs2();

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
			vectorsd(0,2) = -vectorsd(0,2);
			vectorsd(1, 2) = -vectorsd(1, 2);
			vectorsd(2, 2) = -vectorsd(2, 2);
		}

		


		V_N.col(0) = vectorsd.col(0) / vectorsd.col(0).norm();
		V_N.col(1) = vectorsd.col(2) / vectorsd.col(2).norm();
		V_N.col(2) = vectorsd.col(1) / vectorsd.col(1).norm();
		//lam_Out << [lam(1) lam(3) lam(2)]';
	
		
		
		if ((Test_Pnt.rows() < 6) || (ellipse_t.a == 0))
		{
			CrSec_Ratio = lam(2) / lam(1);
		}



		V_N.col(2) = V_N.col(0).cross(V_N.col(1));

		
	}
	
	
	bool ICR_MHA_Yes = false;// true;
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
		

		double de = fi * 180 / 3.14159;
		Eigen::RowVectorXd k;
		int nbrrow = Cross_Sec_PGD_LCS.size();
		//std::string eeevee = "size Cross_Sec_PGD_LCS "+std::to_string(nbrrow) ;
		//wxBusyInfo wait10eeveee45(eeevee.c_str());
		//Sleep(2500);
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
	
	//std::string eeevee = "eeeve" + std::to_string(V_N(0, 0)) + " " + std::to_string(V_N(0, 1)) + " " + std::to_string(V_N(0, 2));
	//wxBusyInfo wait10eeveee45(eeevee.c_str());
	//Sleep(2500);

	//string s4050 = "end principal axes";
	//wxBusyInfo wait4050(s4050.c_str());
	//Sleep(2500);
		
	return V_N;
}
double mafVMECenterLine::Max_Eigen_v(int N,int NDIM, Eigen::Matrix3d A, double EPS)
{
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
	return E_lam;

}
Eigen::Vector4d mafVMECenterLine::extract(Eigen::Matrix3d r)
{
	//Extracts the  unit  vector  u of the screw axis and the rotation angle fi
	//  from a rotation matrix stored in the 3 * 3 left - upper submatrix of a matrix R.
	//EXTRACT performs the inverse operation of ROTAT.

	//[u, angle] = extract(R)

	
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
	double deg = fi * 180 / 3.14159;
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

	


	return result;



}


Eigen::Matrix3d mafVMECenterLine::theta2r(Eigen::Vector3d th)
{
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


	return R;

}
ellipse_struct* mafVMECenterLine::fitellipse(Eigen::MatrixX3d pts)
{
	Eigen::VectorXd xx, yy, x2, y2;
	
	Eigen::MatrixXd XX;
	double* center = new double[3];
	double orientation_tolerance = 1e-3;
	double mean_x = 0, mean_y = 0;
	for (int i = 0; i < pts.rows(); i++)
	{
		mean_x = mean_x + pts(i,0);
		mean_y = mean_y + pts(i,2);
	}
	mean_x = mean_x / pts.rows();
	mean_y = mean_y / pts.rows();

	//std::vector<double> x, y;

	for (int i = 0; i < pts.rows(); i++)
	{

		xx.resize(i+1);
		yy.resize(i+1);
		x2.resize(i + 1);
		y2.resize(i + 1);
		xx[i] = pts(i,0) - mean_x;
		yy[i] = pts(i,2) - mean_y;
		x2(i) = xx(i) * xx(i);
		y2(i) = yy(i) * yy(i);
	}
	
	
	for (int i = 0; i < xx.rows(); i++)
	{
		XX.resize(i + 1,5);
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
	//	a(i) = a(i) / vv(i);
	//}



	


	double X0=0,X0_in=0, Y0=0,Y0_in=0, F=0, aa=0, bb=0, cc=0, dd=0, ee=0, cos_phi=0, sin_phi=0, orientation_rad=0, long_axis=0, short_axis=0;
	if (min(abs(a(1) / a(0)), abs(a(1) / a(2))) > orientation_tolerance)
	{
	
		
		
		orientation_rad = 1 / 2 * atan(a(1) / (a(2) - a(0)));
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
 
   m_Gui->Button(ID_Surface_LINK, &m_SurfaceName, _("Surface"), _("Select Surace"));


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


	  string ee;
	  if (e->GetVme() == NULL)
		 ee = "VME NOT found";
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
						  mafString title = _("Choose surface vme link");
						  e->SetId(VME_CHOOSE);
						  e->SetArg((long)&mafVMEPlane::VMEAccept);
						  e->SetString(&title);
						  ForwardUpEvent(e);
						  mafNode *n = e->GetVme();
						  if (n != NULL)
						  {

							  SetSurfaceLink("SurfaceVME", n);
							  m_SurfaceName = n->GetName();
							  surface = (mafVMESurface*)n;

							  m_Gui->Update();

						  }
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
	
	Eigen::Matrix3Xd result;
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
			wxBusyInfo wait1020("nbr >0");
			Sleep(1500);

			double mmean[3];
			mmean[0] = mmean[1] = mmean[2] = 0;
			for (int i = 0; i < nbr; i++)

			{
				coord = surface->GetSurfaceOutput()->GetVTKData()->GetPoint(i);
				m_TmpTransform1->TransformPoint(coord, coord);

				

				(vert)(0, i) = coord[0];
				(vert)(1, i) = coord[1];
				(vert)(2, i) = coord[2];

				(vertT)(i,0) = coord[0];
				(vertT)(i,1) = coord[1];
				(vertT)(i,2) = coord[2];

				
				(coordd)[i][0] = coord[0];
				(coordd)[i][1] = coord[1];
				(coordd)[i][2] = coord[2];
				mmean[0] = mmean[0] + coord[0]/nbr;
				mmean[1] = mmean[1] + coord[1] / nbr;
				mmean[2] = mmean[2] + coord[2] / nbr;
				

			}

			//principal axes computation
			double CrSec_Ratio=0;
			double Circumf=0;
			Eigen::Matrix3d PAxes;
			Eigen::Vector3d G_Orig_D;
		//	Eigen::MatrixXd Cross_Sec_PGD_LCS;
			std::vector<std::vector<double>> Cross_Sec_PGD_LCS(0, vector<double>(7));
			if (nbr > 3)
			{
				PAxes = principalAxes(nbr, coordd, CrSec_Ratio, Circumf, Cross_Sec_PGD_LCS);

				G_Orig_D = getOrigin(nbr, coordd);
			}
			else
			{
				wxBusyInfo wai5tsds1ueefqqcu20("error2 size==0");
				Sleep(1500);
			}
			
			//std::string dde = "mmean" + std::to_string(G_Orig_D(0)) + " " + std::to_string(G_Orig_D(1)) + " " + std::to_string(G_Orig_D(2));
			//wxBusyInfo wait10dee45(dde.c_str());
			//Sleep(2500);

		
			//std::string eeevee = "Paxis1" + std::to_string(PAxes(0,0)) + " " + std::to_string(PAxes(0,1)) + " " + std::to_string(PAxes(0,2));
			//wxBusyInfo wait10eeveee45(eeevee.c_str());
			//Sleep(2500);

			int cols = vert.cols();
			Eigen::Matrix3Xd  af;
			af.resize(3, cols);
			for (int i = 0; i < cols; i++)
			{
				
				af(0, i) = G_Orig_D(0);
				af(1, i) = G_Orig_D(1);
				af(2, i) = G_Orig_D(2);
			}
			//wxBusyInfo wait140("af ok");
			//Sleep(1500);
			Eigen::Matrix3Xd D = PAxes.transpose()*(vert - af);

			//wxBusyInfo wait1400("D ok");
			//Sleep(1500);
			double Delt_1 = 5;///5
			double Mult_01 = 0.05;
			double	D_1mm = 1.5;
			double	D_50mm = 25;
			double	Fi_stp_5 = 3;
			int ind_Z = 2;
			Eigen::VectorXd X_ind, X_ind_not;
			Eigen::Matrix3Xd D_Lft;
			Eigen::Matrix3Xd D_rgt;
			int nbrD=0;
			int nbrG = 0;
			for (int i = 0; i < D.cols(); i++)
			{
			//	X_ind.resize(i + 1);

				if (D(0, i) > 0 - Delt_1)
				{
					nbrD++;
			//		X_ind(i) = 1;
			//		D_Lft.resize(3, D_Lft.cols() + 1);
			//		D_Lft(0, D_Lft.cols() - 1) = D(0, i);
			//		D_Lft(1, D_Lft.cols() - 1) = D(1, i);
			//		D_Lft(2, D_Lft.cols() - 1) = D(2, i);
				}
				else
				{
					nbrG++;
					//X_ind(i) = 0;
				}
			}


		D_rgt.resize(3, nbrD);
		D_Lft.resize(3, nbrG);
		int k = 0;
		int kk = 0;
			for (int i = 0; i < D.cols(); i++)
			{
			//	std::string eee = "D" + std::to_string(((vert)-af)(0, i)) + " " + std::to_string(((vert)-af)(1, i)) + " " + std::to_string(((vert)-af)(02, i));
			//	wxBusyInfo wait10ee45(eee.c_str());
			//	Sleep(500);
				//X_ind_not.resize(i + 1);
				if (D(0, i) > 0 - Delt_1)
				{	
			//		X_ind_not(i) = 1;
			//	D_rgt.resize(3, D_rgt.cols() + 1);
			//	D_rgt(0, D_rgt.cols() - 1) = D(0, i);
			//	D_rgt(1, D_rgt.cols() - 1) = D(1, i);
			//	D_rgt(2, D_rgt.cols() - 1) = D(2, i);
					D_rgt(0, k) = D(0, i);
					D_rgt(1, k) = D(1, i);
					D_rgt(2, k) = D(2, i);
					k++;

				//std::string eee = "D_rgt cols" + std::to_string(D_rgt.cols());
				//wxBusyInfo wait10ee45(eee.c_str());
				//Sleep(500);

				}
				else
				{
				//	X_ind(i) = 1;
					//D_Lft.resize(3, D_Lft.cols() + 1);
					//D_Lft(0, D_Lft.cols() - 1) = D(0, i);
					//D_Lft(1, D_Lft.cols() - 1) = D(1, i);
					//D_Lft(2, D_Lft.cols() - 1) = D(2, i);

					D_Lft(0, kk) = D(0, i);
					D_Lft(1, kk) = D(1, i);
					D_Lft(2, kk) = D(2, i);

					kk++;

					//std::string eee = "D_Lft cols" + std::to_string(D_Lft.cols());
					//wxBusyInfo wait10ee45(eee.c_str());
					//Sleep(1500);
				}
			}

		//	std::string eee = "D_Lft cols " + std::to_string(nbrG) + "D_rgt cols " + std::to_string(nbrD);;
		//	wxBusyInfo wait10ee45(eee.c_str());
		//	Sleep(1500);

		//	std::string ee = "D cols"+ std::to_string(D.cols()) ;
		//	wxBusyInfo wait10e45(ee.c_str());
		//	Sleep(1500);
			//for (int i = 0; i <D.cols(); i++)
			//{
			//	if (X_ind(i)>0)
			//	{
			//		D_Lft.resize(3,D_Lft.cols() + 1);
			//		D_Lft(0,D_Lft.cols() - 1) = D(0,i);
			//		D_Lft(1,D_Lft.cols() - 1) = D(1,i);
			//		D_Lft(2,D_Lft.cols() - 1) = D(2,i);
			//	}
			//}
		//	wxBusyInfo wait1440("D_Left ok");
		//	Sleep(1500);
			int N_D_Lft = D_Lft.cols();

//
		//	std::string fe = "N_D_Lft "+ std::to_string(N_D_Lft);
		//	wxBusyInfo waitf1045(fe.c_str());
		//	Sleep(1500);
			
			//for (int i = 0; i < D.cols(); i++)
			//{
			//	if (X_ind_not(i)>0)
			//	{
			//		D_rgt.resize(3,D_rgt.cols() + 1);
			//		D_rgt(0,D_rgt.cols() - 1) = D(0,i);
			//		D_rgt(1,D_rgt.cols() - 1) = D(1,i);
			//		D_rgt(2,D_rgt.cols() - 1) = D(2,i);
			//	}
			//}
			int N_D_rgt = D_rgt.cols();

			wxBusyInfo wait14d("D_rgt ok");
			Sleep(1500);
			//Eigen::RowVectorXd::Index I_Lft, I_rgt;
			Eigen::VectorXd vwlft = D_Lft.row(2);
			Eigen::VectorXd vwrgt = D_rgt.row(2);
		//	std::string e = std::to_string(D_rgt.rows()) + " " + std::to_string(D_rgt.cols()) + " " + std::to_string(D_Lft.rows()) + " " + std::to_string(D_Lft.cols());
		//	wxBusyInfo wait1045(e.c_str());
		//	Sleep(1500);
			double D_min_lcs = vwlft(0);// = vwlft.minCoeff();
			double D_max_Lft = vwlft(0);// = vwlft.maxCoeff();
			double D_max_rgt = vwrgt(0);// = vwrgt.maxCoeff();
		
			wxBusyInfo wait145("vw ok");
			Sleep(1500);
			int I_Lft = 0;
			int I_rgt = 0;
			for (int i = 1; i < vwlft.rows(); i++)
			{
				//std::string err = std::to_string(vwlft(i)) + " " ;
				//wxBusyInfo wait1ucu2rrzvv0(err.c_str());
				//Sleep(1500);
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

			wxBusyInfo wait14(" ok");
			Sleep(1500); 
			double H_Z = -(D_max_Lft - D_min_lcs);
			Eigen::Vector3d lft, rgt;
			lft(0) = D_Lft(0,I_Lft);
			lft(1) = 0;
			lft(2) = D_Lft(ind_Z,I_Lft) - Mult_01*H_Z;
			rgt(0) = D_rgt(0,I_rgt);
			rgt(1) = 0;
			rgt(2) = D_rgt(ind_Z,I_rgt) - Mult_01*H_Z;

			wxBusyInfo wait120("left and right construction");
			Sleep(1500);

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
				D_1mm = 8;
				D_50mm = 35;
			}
			wxBusyInfo wait1uu20("centrer et normer");
			Sleep(1500);


			Eigen::Vector3d V_lft = lft - Ori_XYZ;
			V_lft = V_lft / V_lft.norm();
			Eigen::Vector3d V_rgt = rgt - Ori_XYZ;
			V_rgt = V_rgt / V_rgt.norm();
			/////
			Eigen::Vector3d u;
			u << 0, 0, -1;
			double Ang_lft = std::acos(V_lft.transpose()*u);
			double Ang_rgt = std::acos(V_rgt.transpose()*u);

			double del_Fi = Fi_stp_5*3.14159 / 180;
			double c_fi = std::cos(del_Fi);
			double s_fi = std::sin(del_Fi);
			wxBusyInfo wait1ucu20("RotY...");
			Sleep(1500);
			Eigen::Matrix3d Rot_Y;
			Rot_Y << c_fi, 0, -s_fi, 0, 1, 0, s_fi, 0, c_fi;
			int I_lft_tot = round((Ang_lft) / del_Fi) + 1;
			int I_lft_rgt = round((Ang_lft + Ang_rgt) / del_Fi) + 1;
			Eigen::Vector3d V_lft_curr = V_lft;
			int Ind_D_Lft, Ind_D_rgt;
			std::string er = "lft  "+std::to_string(lft(0)) + " " + std::to_string(lft(1)) + " " + std::to_string(lft(2));
			wxBusyInfo wait1ucu2vv0(er.c_str());
			Sleep(1500);
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
			Eigen::VectorXd All_Circumf ;
			double S_trg=0;
			All_CrSec_Ratio.resize(I_lft_rgt);
			All_Circumf.resize(I_lft_rgt);
			string uooi = "init ok    " ;
			wxBusyInfo wait1init(uooi.c_str());
			Sleep(1500);
			for (int I_lft = 0; I_lft < I_lft_rgt; I_lft++)
			{
				Eigen::Vector3d v, m;
				v << 0, -1, 0;
				string iio = "I_lft   " + std::to_string(I_lft) + "/ "+std::to_string(I_lft_rgt);
				wxBusyInfo wait12ucrtyvv0(iio.c_str());
				Sleep(1500);
				Eigen::Vector3d Perp_V_lft_curr = V_lft_curr.cross(v);
				wxBusyInfo wait1ucrtyvv0("cross ok");
				Sleep(1500);
				if (I_lft <= I_lft_tot)
				{
					Eigen::Matrix3Xd ooDD;
					ooDD.resize(3, N_D_Lft);
					for (int i = 0; i < N_D_Lft; i++)
					{

						ooDD(0,i) = Ori_XYZ(0);
						ooDD(0,i) = Ori_XYZ(1);
						ooDD(0,i) = Ori_XYZ(2);
					}

					wxBusyInfo wait1ucu20("for1");
					Sleep(1500);
					Eigen::VectorXd mplo = Perp_V_lft_curr.transpose()*(D_Lft - ooDD);
					
					int nnbr=0;
					for (int kk = 0; kk<mplo.rows(); kk++)
					{
						if (std::fabs(mplo(kk)) < D_1mm)
						{
							nnbr++;
							Pnt_mn(0) = D_Lft(0, kk);
							Pnt_mn(1) = D_Lft(1, kk); 
							Pnt_mn(1)=  D_Lft(2, kk);

						}
					}
					Pnt_mn(0) = Pnt_mn(0) / nnbr;
					Pnt_mn(1) = Pnt_mn(1) / nnbr;
					Pnt_mn(2) = Pnt_mn(2) / nnbr;

					wxBusyInfo wait1uqqcu20("for2");
					Sleep(1500);
					if (I_lft == 1)
						Pnt_mn_prp = Pnt_mn;

					
					if (I_lft > 1)
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

						
						wxBusyInfo waitsds1ufqqcu290("resize1");
						Sleep(1500);
						for (int k = 0; k < lmp.rows(); k++)
						{
							
							Ind_Perp_MidLn(k) = std::fabs(lmp(k)) < D_1mm;

						}

						Eigen::Matrix3Xd lmp2 = (D_Lft - mm).array()*(D_Lft - mm).array();
						Eigen::VectorXd sum;
						sum.resize(lmp2.cols());

						wxBusyInfo waitmmpufqqcu290("resize2");
						Sleep(1500);
						for (int kk = 0; kk < lmp2.cols(); kk++)
						{

							sum(kk) = sqrt((lmp2(0, kk) + lmp2(1, kk) + lmp2(2, kk)) / 3)<D_50mm;
						}
						for (int kk = 0; kk < lmp2.cols(); kk++)
						{
							Ind_Perp_MidLn(kk) = Ind_Perp_MidLn(kk) * sum(kk);
						}

						Eigen::MatrixX3d newDLFt;

						string bng = "i5ffg   " + std::to_string(Ind_Perp_MidLn.rows()) + " "+std::to_string(Ind_Perp_MidLn.size());
						wxBusyInfo waitsds1ufqqcu20(bng.c_str());
						Sleep(1500);
						std::vector<std::vector<double>>newDLFt1;

						int sznewDLFt = 0;
						for (int kk = 0; kk < Ind_Perp_MidLn.rows(); kk++)
						{
						
							if (Ind_Perp_MidLn(kk)>0)

							{
								sznewDLFt++;
							}
						}
						string bn = "i5ff   " + std::to_string(sznewDLFt);
						wxBusyInfo wai5tsds1ufqqcu20(bn.c_str());
						Sleep(2500);
						newDLFt.resize( sznewDLFt,3 );
						int uf = 0;
						for (int kk = 0; kk <Ind_Perp_MidLn.rows(); kk++)
						{
							if (Ind_Perp_MidLn(kk)>0)

							{
								
								newDLFt(uf,0) = D_Lft(0,kk);
								newDLFt(uf,1) = D_Lft(1, kk);
								newDLFt(uf,2) = D_Lft(2, kk);
								uf++;
								newDLFt1.push_back({ D_Lft(0, kk), D_Lft(1, kk), D_Lft(2, kk) });

							}

						}
					
						if (sznewDLFt > 3)
						{
							Eigen::Matrix3d R_V_N = principalAxes(sznewDLFt, newDLFt1, CrSec_Ratio, Circumf, Cross_Sec_PGD_LCS);

							wxBusyInfo wai5tsds1ueefqqcu20("i5ffprincipalaxis");
							Sleep(1500);
							Eigen::Vector3d R_G_Orig = getOrigin(nbr, newDLFt1);
							wxBusyInfo waorigini5tsds1ufqqcu20("i5fforigin");
							Sleep(1500);
							Eigen::Vector3d  Pnt_mnt_prp = R_G_Orig;
						}
						else
						{

							wxBusyInfo wai5tsds1ueefqqcu20("error0 size==0");
							Sleep(1500);
						}
					}


					wxBusyInfo waitsds1uqqcu20("if");
					Sleep(1500);

				}
				else
				{
					string ff = "N_D_rgt " + std::to_string(N_D_rgt);
					wxBusyInfo waitsds1uqqcu20(ff.c_str());
					Sleep(1500);
					Eigen::Matrix3Xd ooDD;
					ooDD.resize(3, N_D_rgt);
					for (int i = 0; i < N_D_rgt; i++)
					{

						ooDD(0, i) = Ori_XYZ(0);
						ooDD(0, i) = Ori_XYZ(1);
						ooDD(0, i) = Ori_XYZ(2);
					}
					Eigen::VectorXd mplo = Perp_V_lft_curr.transpose()*(D_rgt - ooDD);
					ff = "mplo " + std::to_string(mplo.rows());
					wxBusyInfo waitsds1uzzzaqqcu20(ff.c_str());
					Sleep(1500);
					int nnbr = 0;
					for (int kk = 0; kk < mplo.rows(); kk++)
					{
						if (std::fabs(mplo(kk)) < D_1mm)
						{
							nnbr++;
							Pnt_mn(0) = Pnt_mn(0)+ D_rgt(0, kk);
							Pnt_mn(1) = Pnt_mn(1)+ D_rgt(1, kk);
							Pnt_mn(2) = Pnt_mn(2)+ D_rgt(2, kk);

						}
					}

					ff = "mplo end "+std::to_string(nnbr) ;
					wxBusyInfo waitiisds1uzzzaqqcu20(ff.c_str());
					Sleep(1500);
					if (nnbr>0)
					{
						Pnt_mn(0) = Pnt_mn(0) / nnbr;
						Pnt_mn(1) = Pnt_mn(1) / nnbr;
						Pnt_mn(2) = Pnt_mn(2) / nnbr;

						string h = "Mid_Ln size" + std::to_string(Mid_Ln.rows()) + " " + std::to_string(Mid_Ln.cols());
						wxBusyInfo wait1uzzzaqqcu20(h.c_str());
						Sleep(1500);

						//if (I_lft == 1)
						//	Pnt_mn_prp = Pnt_mn;

					}

					string h = "loop " + std::to_string(I_rgt) + " " + std::to_string(N_D_rgt);
					wxBusyInfo wait1uzzzaqqcu20(h.c_str());
					Sleep(1500);
					if (I_lft > 1)
					{
						Eigen::Matrix3Xd mm;
						Eigen::Vector3d m;

						string hh0 = "loop0 " + std::to_string(Mid_Ln.cols()) + " " + std::to_string(I_rgt);
						wxBusyInfo waita020(hh0.c_str());
						Sleep(1500);
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

						string hh = "loop1 ";
						wxBusyInfo waitaqqcu20(hh.c_str());
						Sleep(1500);

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
							sum(kk) = sqrt((lmp2(0, kk) + lmp2(1, kk) + lmp2(2, kk)) / 3)<D_50mm;
						}

						string hhh = "loop2 size Ind_Perp_MidLn size sum " + std::to_string(Ind_Perp_MidLn.rows()) + " " + std::to_string(sum.rows());
						wxBusyInfo waitaqmmqcu20(hhh.c_str());
						Sleep(1500);
						for (int u = 0; u < Ind_Perp_MidLn.rows(); u++)
						{
							Ind_Perp_MidLn(u) = Ind_Perp_MidLn(u) * sum(u);
						}


						Eigen::MatrixX3d newDLFt;
						std::vector<std::vector<double>>newDLFt1;
						int uf = 0;

						string hhhh = "loop3 ";
						wxBusyInfo waitaqqcuh20(hhhh.c_str());
						Sleep(1500);
						for (int kk = 0; kk <Ind_Perp_MidLn.rows(); kk++)
						{
							if (Ind_Perp_MidLn(kk)>0)

							{
								(newDLFt).resize( (newDLFt).cols() + 1,3);
								(newDLFt)( (newDLFt).cols() - 1,0) = D_rgt(0, kk);

								(newDLFt)( (newDLFt).cols() - 1,1) = D_rgt(1, kk);
								(newDLFt)( (newDLFt).cols() - 1,2) = D_rgt(2, kk);

								newDLFt1.push_back({ D_rgt(0, kk), D_rgt(1, kk), D_rgt(2, kk) });
								uf++;
							}

						}


						wxBusyInfo waitsds1uqqcueee20("PAxes20");
						Sleep(1500);
						if (uf>3)
						{
							R_V_N = principalAxes(uf, newDLFt1, CrSec_Ratio, Circumf, Cross_Sec_PGD_LCS);
							Eigen::Vector3d R_G_Orig = getOrigin(uf, newDLFt1);

							Eigen::Vector3d  Pnt_mnt_prp = R_G_Orig;
						}
						else
						{
							wxBusyInfo wai5tsds1ueefqqcu20("error1 size==0");
							Sleep(1500);

						}

					}

					wxBusyInfo wait1urrrr20("if end");
					Sleep(1500);


				}
			
			
			///////////


	//		wxBusyInfo wait1h0urrrr20("init2");
	//		Sleep(1500);
			Mid_Ln.resize(3,Mid_Ln.cols() + 1);
			Mid_Ln(0,Mid_Ln.cols()-1)=Pnt_mn(0);
			Mid_Ln(1,Mid_Ln.cols()-1) = Pnt_mn(1);
			Mid_Ln(2,Mid_Ln.cols()-1) = Pnt_mn(2);

			Mid_Ln_Perp.resize(3,Mid_Ln_Perp.cols() + 1);
			Mid_Ln_Perp(0,Mid_Ln_Perp.cols()-1) = Pnt_mn_prp(0);
			Mid_Ln_Perp(1,Mid_Ln_Perp.cols()-1) = Pnt_mn_prp(1);
			Mid_Ln_Perp(2,Mid_Ln_Perp.cols()-1) = Pnt_mn_prp(2);
	//		wxBusyInfo wait1h0urrrr200("if..2");
	//		Sleep(1500);
			if (I_lft > 1)
			{
				Twist_X_ax.resize(3,Twist_X_ax.cols()+1);
				Twist_X_ax(0,Twist_X_ax.cols() -1) =  R_V_N(0, 1);
				Twist_X_ax(1,Twist_X_ax.cols() - 1) = R_V_N(1, 1);
				Twist_X_ax(2,Twist_X_ax.cols() - 1) = R_V_N(2, 1);


	//			wxBusyInfo wait1h0urrarr20("aa");
	//			Sleep(1500);
				//S_trg_Area.resize(S_trg_Area.rows() + 1);
				//S_trg_Area(S_trg_Area.rows()-1) = S_trg;
				S_trg_Area(I_lft - 1) = S_trg;
				Eigen::Vector3d fg;
				fg(0) = Pnt_mn_prp(0) - Mid_Ln_Perp(0, I_lft - 1);
				fg(1) = Pnt_mn_prp(1) - Mid_Ln_Perp(1, I_lft - 1);
				fg(2) = Pnt_mn_prp(2) - Mid_Ln_Perp(2, I_lft - 1);
	//			wxBusyInfo wait1h0uraararr20("aaa");
	//			Sleep(1500);
				Rib_Long_Pos(I_lft) = Rib_Long_Pos(I_lft - 1) + fg.norm();
				//wxBusyInfo wait1h0urvvdrarr20("bb");
				//Sleep(1500);
				//All_CrSec_Ratio.resize(All_CrSec_Ratio.rows() + 1);
				//All_CrSec_Ratio(All_CrSec_Ratio.rows() - 1) = CrSec_Ratio;
				All_CrSec_Ratio(I_lft - 1) = CrSec_Ratio;
	//			wxBusyInfo wait1h0urccrarr20("cc");
	//			Sleep(1500);
				//All_Circumf.resize(All_Circumf.rows() + 1);
				//All_Circumf(All_Circumf.rows() - 1) = Circumf;
				All_Circumf(I_lft - 1) = Circumf;
				
			}
			V_lft_curr = Rot_Y*V_lft_curr;
			}
//			wxBusyInfo wait1ucvv0("for");
//			Sleep(1500);
			Eigen::Vector3d D_min;
			Eigen::Vector3d D_max;
			
				for (int j = 0; j <vert.cols(); j++)
				for (int i = 0; i < 3; i++)
				{
					if (D_min(i) <(vert)(i, j))
						D_min(i) = (vert)(i, j);

					if (D_max(i) > (vert)(i, j))
						D_max(i) = (vert)(i, j);
				} 
	//			wxBusyInfo wait1ucvoov0("fooor");
	//			Sleep(1500);
			Eigen::Vector3d Bone_D_size_XYZ = D_max - D_min; 



			/////////////
			/////////if ICR_MHA_Yes
			int Ind_Call = 2;
			double epsiAxe = 1.;
			int Mult_Vert_epsiAxe = 1;
			double Angle_filtr_T = epsiAxe*Mult_Vert_epsiAxe;

		//	wxBusyInfo wait1ucviniv0("init....3");
		//	Sleep(1500);
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
			wxBusyInfo waitRm_in_PGD_T("Rm_in_PGD_T...");
			Sleep(1500);

			std::vector<std::vector<double>> Rm_in_PGD_T;
			//Eigen::MatrixXd Rm_in_PGD_T;
			//Rm_in_PGD_T.resize(Cross_Sec_PGD_LCS.size(), 6);
			for (int u = 0; u < Cross_Sec_PGD_LCS.size(); u++)
			{
				Rm_in_PGD_T.push_back({ fgg(u, 0)*((Cross_Sec_PGD_LCS)[u][1]), fgg(u, 1)*((Cross_Sec_PGD_LCS)[u][2]), fgg(u, 2)*((Cross_Sec_PGD_LCS)[u][3]), (Cross_Sec_PGD_LCS)[u][4], (Cross_Sec_PGD_LCS)[u][5], (Cross_Sec_PGD_LCS)[u][6] });
				//Rm_in_PGD_T(u, 0) = fgg(u, 0)*((Cross_Sec_PGD_LCS)[u][1]);
				//Rm_in_PGD_T(u, 1) = fgg(u, 1)*((Cross_Sec_PGD_LCS)[u][2]);
				//Rm_in_PGD_T(u, 2) = fgg(u, 2)*((Cross_Sec_PGD_LCS)[u][3]);;
				//Rm_in_PGD_T(u, 3) = (Cross_Sec_PGD_LCS)[u][4];
				//Rm_in_PGD_T(u, 4) = (Cross_Sec_PGD_LCS)[u][5];
				//Rm_in_PGD_T(u, 5) = (Cross_Sec_PGD_LCS)[u][6];
			}
			//wxBusyInfo waitRm_in_PGD_1T("Rm_in_PGD_T");
			//Sleep(1500);
			std::vector<std::vector<double>> cr_pos_T(3, vector<double>(3)), disp_sq_T(3, vector<double>(3)), E_MHA_T(3, vector<double>(3)), disp_ang_T(3, vector<double>(3));
			double yu=ShV_ICR_MHA(Ind_Call, Cross_Sec_PGD_LCS.size(),Rm_in_PGD_T, Angle_filtr_T, cr_pos_T, disp_sq_T, E_MHA_T, disp_ang_T);
			
			string s = std::to_string(yu);
		
			wxBusyInfo waitShV_ICR_MHA(s.c_str());
			//Sleep(1500);
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
				smTw(0,u) = std::sqrt(Twist_X_ax(0, u)*Twist_X_ax(0, u) + Twist_X_ax(1, u)*Twist_X_ax(1, u) + Twist_X_ax(2, u)*Twist_X_ax(2, u));
				smTw(1, u) = std::sqrt(Twist_X_ax(0, u)*Twist_X_ax(0, u) + Twist_X_ax(1, u)*Twist_X_ax(1, u) + Twist_X_ax(2, u)*Twist_X_ax(2, u));
				smTw(2, u) = std::sqrt(Twist_X_ax(0, u)*Twist_X_ax(0, u) + Twist_X_ax(1, u)*Twist_X_ax(1, u) + Twist_X_ax(2, u)*Twist_X_ax(2, u));
			}

			for (int u = 0; u < Twist_X_ax.cols(); u++)
			{
				Twist_X_ax(0,u) = Twist_X_ax(0,u) / smTw(0, u);
				Twist_X_ax(1, u) = Twist_X_ax(1, u) / smTw(1, u);
				Twist_X_ax(2, u) = Twist_X_ax(2, u) / smTw(2, u);
			}

			wxBusyInfo waitTwist_X_ax("Twist_X_axssssss...");
			Sleep(1500);
	
			Eigen::Matrix3Xd ff;
			ff.resize(3, I_lft_rgt-2);
			for (int u = 2; u < I_lft_rgt; u++)
			{
				ff(0,u-2)=Mid_Ln_Perp(0, u) - Mid_Ln_Perp(0, u - 1);
				ff(1, u - 2) = Mid_Ln_Perp(1, u) - Mid_Ln_Perp(1, u - 1);
				ff(2, u - 2) = Mid_Ln_Perp(2, u) - Mid_Ln_Perp(2, u - 1);
			}

			wxBusyInfo waisstTwist_X_ax("Twist_X_ff...");
			Sleep(1500);
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

			wxBusyInfo waisstTwistoo_X_ax("smMid_Ln...");
			Sleep(1500);
			Eigen::Matrix3Xd N_Mid_Ln_Perp;// , gl3;

			N_Mid_Ln_Perp.resize(3, ff.cols());
			for (int u = 0; u < ff.cols(); u++)
			{
				N_Mid_Ln_Perp(0, u) = ff(0, u) / (smMid_Ln(0, u)+0.001);
				N_Mid_Ln_Perp(1, u) = ff(1, u) / (smMid_Ln(1, u)+0.001);
				N_Mid_Ln_Perp(2, u) = ff(2, u) / (smMid_Ln(2, u)+0.001);
			}

			
			
			Eigen::Matrix3Xd gl3,gl4;
			Eigen::Vector3d gl1, gl2, gl;
			
			gl4.resize(3, Twist_X_ax.cols() - 1);
			
			//gl1.resize(3, Twist_X_ax.cols() - 2);
			//gl2.resize(3, Twist_X_ax.cols() - 2);
			string jkl = "size verification " + std::to_string(Twist_X_ax.cols()) + " " + std::to_string(I_lft_rgt - 1);
			wxBusyInfo waddisstTwistoo_X_ax(jkl.c_str());
			Sleep(1500);
			for (int u = 0; u < Twist_X_ax.cols()-1; u++)
			{
		
				gl1(0) = Twist_X_ax(0, u);
				gl1(1) = Twist_X_ax(1, u);
				gl1(2) = Twist_X_ax(2, u);

				gl2(0) = Twist_X_ax(0, u+1);
				gl2(1) = Twist_X_ax(1, u+1);
				gl2(2) = Twist_X_ax(2, u+1);

				gl=gl1.cross(gl2);

				gl4(0, u) = gl(0);
				gl4(1, u) = gl(01);
				gl4(2, u) = gl(02);
			}
			
			string jkl3 = "gl done " ;
			wxBusyInfo waddiseestTwistoo_X_ax(jkl3.c_str());
			Sleep(1500);
			Eigen::VectorXd sds, AngTwist_X_ax;
			
			sds.resize(N_Mid_Ln_Perp.cols()-1);
			AngTwist_X_ax.resize(N_Mid_Ln_Perp.cols()-1);
			gl3.resize(3, N_Mid_Ln_Perp.cols()-1);
			string jkl2 = "size verification " + std::to_string(N_Mid_Ln_Perp.cols()) + " " + std::to_string(gl4.cols()) + " " + std::to_string(gl3.cols());
			wxBusyInfo wadTwistoo_X_ax(jkl2.c_str());
			Sleep(1500);
			for (int u = 0; u <N_Mid_Ln_Perp.cols()-1; u++)
			{
				
				
				gl3(0, u) = N_Mid_Ln_Perp(0, u)*gl4(0, u);
				gl3(1, u) = N_Mid_Ln_Perp(1, u)*gl4(1, u);
				gl3(2, u) = N_Mid_Ln_Perp(2, u)*gl4(2, u);

				sds(u) = gl3(0, u) + gl3(1, u) + gl3(2, u);
				AngTwist_X_ax(u) = 180 / 3.14159*std::asin(sds(u));
			}
			
			string jkl20 = "gl 2 done ";
			wxBusyInfo waddisstTwisto00o_X_ax(jkl20.c_str());
			Sleep(1500);

			int I_Strt = 2;
			Eigen::VectorXd Sum_AngTwist_X_ax, Dst_AngTwist_X_ax;
			Dst_AngTwist_X_ax.resize(AngTwist_X_ax.cols());
			Sum_AngTwist_X_ax.resize(AngTwist_X_ax.cols());

			Sum_AngTwist_X_ax(0) = 0;
			Dst_AngTwist_X_ax(0)=0;
			std::vector<double> Dst_AngTwist_X_ax1, Sum_AngTwist_X_ax1, p_PGD1;
			
			for (int i = I_Strt; i<AngTwist_X_ax.cols() - 1; I_Strt++)
			{
				Sum_AngTwist_X_ax(i - I_Strt + 2) = Sum_AngTwist_X_ax(i - I_Strt + 1)+AngTwist_X_ax(i-1);
				Dst_AngTwist_X_ax(i - I_Strt + 2) = Dst_AngTwist_X_ax(i - I_Strt + 1) +(Mid_Ln_Perp.col(i) - Mid_Ln_Perp.col(i-1)).norm();
				Sum_AngTwist_X_ax1.push_back(Sum_AngTwist_X_ax(i - I_Strt + 1) + AngTwist_X_ax(i - 1));
				Dst_AngTwist_X_ax1.push_back(Dst_AngTwist_X_ax(i - I_Strt + 1) + (Mid_Ln_Perp.col(i) - Mid_Ln_Perp.col(i - 1)).norm());
			}
			
			string jkl200 = "gl 20 done ";
			wxBusyInfo waddX_ax(jkl200.c_str());
			Sleep(1500);
			int Poly_Pow = 3;


			Eigen::VectorXd p_PGD = polyfit(Dst_AngTwist_X_ax1, Sum_AngTwist_X_ax1, Poly_Pow);
			for (int k = 0; k < p_PGD.size(); k++)
			{
				p_PGD1.push_back(p_PGD[k]);
			}
			Eigen::VectorXd PGD_Gl_11 = polyeval(p_PGD1, Dst_AngTwist_X_ax1);
			/////////
			wxBusyInfo wait102("update ok");
			Sleep(2500);
		}
	}
	else
		{
			string s = "vertices extraction problem";
			wxBusyInfo wait12(s.c_str());
			Sleep(2500);
		}


	

		
			
			//
		vtkPoints* pts = vtkPoints::New();
		vtkPolyData *polydata = vtkPolyData::New(); 
		int num = result.cols();
		int pointId[2];
		double* f;
		f = new double[3];


		string s = std::to_string(num);
		wxBusyInfo wait12(s.c_str());
		Sleep(1500);


		pts->SetNumberOfPoints(num);
		vtkMAFSmartPointer<vtkCellArray> cellArray;
		for (int ij = 0; ij< num; ij++)
			{

				f[0] = result(0,ij);
				f[1] = result(1,ij);
				f[2] = result(2,ij);

				string ii = "pt inserted " + std::to_string(ij) + " " + std::to_string(f[0]) + " " + std::to_string(f[1]) + " " + std::to_string(f[2]);
				wxBusyInfo wait122i(ii.c_str());
				Sleep(1500);
				pts->SetPoint(ij, f);

				if (ij > 0)
				{
					pointId[0] = ij - 1;
					pointId[1] = ij;
					cellArray->InsertNextCell(2, pointId);
				}					


			}

		string sss = "polydata...";
		wxBusyInfo wait120(sss.c_str());
		Sleep(1500);



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


		//wxBusyInfo wait("ShV_ICR_MHAinit");
		//Sleep(1500);

		for (int j = 0; j < 3; j++)
			x.push_back( 3.14159*Rm_in_PGD[0][j] / 180);


		M_1 = ov_mr_ShV(x);

		Transl_k1(0) = Rm_in_PGD[0][3];
		Transl_k1(1) = Rm_in_PGD[0][4];
		Transl_k1(2) = Rm_in_PGD[0][5];
	
		double Eps_PC = 1.e-12, Gr_Rad = atan(1.) / 45.;
		Eigen::Vector3d Qv_t = Eigen::Vector3d::Zero();
		Eigen::Matrix3d Qt_t = Eigen::Matrix3d::Zero();
		Eigen::Matrix3d R_MHA = Eigen::Matrix3d::Zero();
		int k_fr_rez = 0;
		string ii = "k frames " + std::to_string(k_frames) ;
		wxBusyInfo wait4(ii.c_str());
		Sleep(1500);
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
			{
				std::vector<double> x1;
			
				for (int j = 0; j < 3; j++)
					x1.push_back( 3.14159*Rm_in_PGD[k][j] / 180);
					
					
					
					
				M_2 = ov_mr_ShV(x1);
								
				
				Transl_k2(0) = Rm_in_PGD[k][3];
				Transl_k2(1) = Rm_in_PGD[k][4];
				Transl_k2(2) = Rm_in_PGD[k][5];
			}

			Eigen::Matrix3d R = M_2*M_1.transpose();

			double	cq = 0.5*(R(1, 1) + R(2, 2) + R(3, 3) - 1.); 
			double	qc = 0.;
			
			string ii = "pt inserted " + std::to_string(M_2(0)) + " " + std::to_string(M_2(1)) + " " + std::to_string(M_2(2)) ;
			wxBusyInfo wait4(ii.c_str());
			Sleep(1500);
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
				u(0) = uu(0); u(1) = uu(1); u(2) = uu(2);
				double teta_out = uu(3);
				double t_t = .5 / (1. - cos(teta_out));
				//////////

				Qv_hist.col(k_fr_rez) = Qv*t_t;
				u_hist.col( k_fr_rez) = u;  
				R_MHA = R_MHA + eye - u*u.transpose();
				Qv_t = Qv_t + Qv;
				Qt_t = Qt_t + Qt;
				////////
				Eigen::Matrix3d R_inv;// cr_pos, disp_sq, disp_ang;
				//wxBusyInfo wait4("ShV_ICR_MH4");
				//Sleep(1500);
				if (k_fr_rez > 1)
				{
					R_inv = Qt_t.inverse();
					cr_pos[0][k_fr_rez - 1] = (R_inv*Qv_t).transpose()(0);
					cr_pos[1][k_fr_rez - 1] = (R_inv*Qv_t).transpose()(1);
					cr_pos[2][k_fr_rez - 1] = (R_inv*Qv_t).transpose()(2);
					double disp_tmp = 0;
					Eigen::RowVector3d C_22,pp;
					double C_2,C_1;
					
					for (int k_3 = 0; k_3 <k_fr_rez; k_3++)
					{
						pp(0) = cr_pos[k_fr_rez - 1][0];
						pp(1) = cr_pos[k_fr_rez - 1][1];
						pp(2) = cr_pos[k_fr_rez - 1][2];
						C_1 = (pp - Qv_hist.col(k_3).transpose())*u_hist.col(k_3);
						C_22 = pp - Qv_hist.col(k_3).transpose();
						C_2 = C_22*C_22.transpose();
						disp_tmp = disp_tmp + (C_2-C_1*C_1);


					}
					for (int k = 0; disp_sq[0].size();k++)
						disp_sq[k_fr_rez - 1][k] = std::sqrt(disp_tmp/k_fr_rez);

					R_inv = R_MHA.inverse();
					double E_ang=Max_Eigen_v(3,3,R_inv,Eps_PC * 1.e5);

					double E_ang_k_fr_rez = std::sqrt(E_ang / k_fr_rez);
					if (std::abs(E_ang_k_fr_rez) > 1)
						E_ang_k_fr_rez = 1;

					for (int k = 0; disp_ang[0].size(); k++)
						disp_ang[k_fr_rez - 1][k] = std::asin(E_ang_k_fr_rez / Gr_Rad);

				//	wxBusyInfo wait5("ShV_ICR_MHA5");
				//	Sleep(1500);
				}
				
				M_1 = M_2;
				Transl_k1 = Transl_k2;

			}
			k_fr_rez = k_fr_rez -1;
		}
/////
/////
/////
		return k_fr_rez;

}
Eigen::RowVectorXd mafVMECenterLine::deg2rad(Eigen::RowVectorXd a)
{
	Eigen::RowVectorXd r;

	for (int i = 0; i < a.rows(); i++)
	{
		r.resize(i + 1);
		r(i) = a(i) - 3.14159 / 180;
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
			parent->StoreMatrix("Transform", &m_Transform->GetMatrix()) == MAF_OK

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
    if (node->RestoreMatrix("Transform",&matrix)==MAF_OK 
			
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
	return mafVMESurface::SafeDownCast(GetLink("SurfaceVME"));
}
void mafVMECenterLine::UpdateLinks()
//-------------------------------------------------------------------------
{
	mafID sub_id = -1;
	mafNode* nd = NULL;



	surface = GetSurfaceVME();
	if (surface && surface->IsMAFType(mafVMESurface))
	{
		nd = GetLink("SurfaceVME");
		m_SurfaceName = (nd != NULL) ? ((mafVMESurface *)surface)->GetName() : _("none");
	}
	else
		m_SurfaceName = surface ? surface->GetName() : _("none");
		

}

void mafVMECenterLine::SetSurfaceLink(const char *link_name, mafNode *n)
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
	Eigen::MatrixXd A(xvals.size(), order + 1);


	wxBusyInfo wait1020("polyfit ... ok11");
	Sleep(2500);
	for (int i = 0; i < xvals.size(); i++) {
		A(i, 0) = 1.0;
	}

	for (int j = 0; j < xvals.size(); j++) {
		for (int i = 0; i < order; i++) {
			A(j, i + 1) = A(j, i) * xvals[j];
		}
	}
	

	wxBusyInfo wait102("polyfit ... ok");
	Sleep(2500);
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
	//Eigen::Matrix3d V_N;
	Eigen::RowVector3d mean;
	

	string s = "sz " + std::to_string(sz) + " " ;
	//wxBusyInfo wait30(s.c_str());
	//Sleep(1500);
	mean(0) = mean(1) = mean(2) = 0;
	for (int i = 0; i < sz; i++)
	{
		mean(0) = mean(0)+vertex[i][0] / sz;
		mean(1) = mean(1) + vertex[i][1] / sz;
		mean(2) = mean(2) + vertex[i][2] / sz;

	//	string s3 = "input vertex " + std::to_string(vertex[i][0]) + " " + std::to_string(vertex[i][1]) + " " + std::to_string(vertex[i][2]);
	//	wxBusyInfo wait3(s3.c_str());
	//Sleep(1500);
	}
	//mean = vertex.colwise().mean();
	/*double Perp_MidLn = 1;
	Eigen::MatrixX3d centeredVertex;

	for (int i = 0; i < sz; i++)
	{
		centeredVertex.resize(i + 1, 3);
		centeredVertex(i, 0) = vertex(i, 0) - mean(0);
		centeredVertex(i, 1) = vertex(i, 1) - mean(1);
		centeredVertex(i, 2) = vertex(i, 2) - mean(2);
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

	}*/


	const Eigen::Vector3d G_Orig = mean.transpose();
	//string s3 = "input vertex Origin" + std::to_string(mean(0)) + " " + std::to_string(mean(1)) + " " + std::to_string(mean(2));
	//wxBusyInfo wait3(s3.c_str());
	//Sleep(1500);
	return G_Orig;


}