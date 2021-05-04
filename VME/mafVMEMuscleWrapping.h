/*=========================================================================

 Program: MAF2
 Module: mafVMEMuscleWrapper2
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVMEMuscleWrapping_h
#define __mafVMEMuscleWrapping_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointSet.h"
#include "../Eigen/Eigen/Dense"
#include "mafVMEEllipsoid.h"
#include "mafVMECylinder.h"
#include "mafQuadraticSurface.hpp"
#include "mafVMEMeter.h"
#include "mafVMELineSeg.h"
#include "vtkAppendPolyData.h"
#include "../../Eigen/unsupported/Eigen/LevenbergMarquardt"
#include <vector>
#include <iostream>
#include <fstream>
//#include "../QuadricSurfaces/vtkContentActor.hpp"
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Matrix4d;
using Eigen::Matrix3d;
using Eigen::Matrix2d;
using Eigen::VectorXd;
using Eigen::Vector3d;
using Eigen::VectorXcd;
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class vtkPolyData;
class vtkPoints;
class mafVME;
class mafVMELandmark;
class mafVMELandmarkCloud;
class vtkLineSource;

class vtkAppendPolyData;

class vtkXYPlotActor;
class mafRWI;
class mafGUIDialogPreview;

struct LMFunctor : Eigen::DenseFunctor<double>
{
	LMFunctor(void) : DenseFunctor<double>(6, 1) {}
	// X -> F
	// Compute 'm' errors, one for each data point, for the given parameter values in 'x'
	int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &F) const
	{
		//F = VectorXd::Zero(6);

		// cout <<" x in () operator" << x << endl << endl;
		int nrSurfaces = surfaces->size();

		for (int i = 0; i<nrSurfaces; i++)
		{
			VectorXd tempF = VectorXd::Zero(6);

			Vector3d surfacePoint1(x[6 * i], x[6 * i + 1], x[6 * i + 2]);
			Vector3d surfacePoint2(x[6 * i + 3], x[6 * i + 4], x[6 * i + 5]);
			Vector3d connectionPoint1;
			Vector3d connectionPoint2;

			if (i == 0)
			{
				connectionPoint1 = p0;
			}
			else
			{
				connectionPoint1 = Vector3d(x[6 * (i - 1) + 3], x[6 * (i - 1) + 4], x[6 * (i - 1) + 5]);
			}
			if (i == nrSurfaces - 1)
			{
				connectionPoint2 = q0;
			}
			else
			{
				connectionPoint2 = Vector3d(x[6 * (i + 1) + 0], x[6 * (i + 1) + 1], x[6 * (i + 1) + 2]);
			}
			//  cout << "x" << endl << x << endl;
			//  cout << "connectionPoint1" << endl << connectionPoint1 << endl;
			//   cout << "surfacePoint1" << endl<<surfacePoint1 << endl;
			//   cout << "surfacePoint2" << endl<< surfacePoint2  << endl;
			//  cout << "connectionPoint2" << endl<< connectionPoint2 << endl;

			(*surfaces)[i]->wrapfunF(surfacePoint1, surfacePoint2, connectionPoint1, connectionPoint2, &tempF);
			// cout << surfacePoint1 << surfacePoint2 << connectionPoint1 << connectionPoint2 << endl;
			for (int k = 0; k<6; k++)
				F(6 * i + k) = tempF(k);

			// cout << F << endl;
		}
		// F(0) = Ftot.norm();
		// F = Ftot;
		// cout << "error value: " << F.norm() << endl << endl;
		//cout << "X after error computation " << x << endl;
		return 0;
	}
	// X -> J
	// Compute the jacobian of the functions
	int df(const Eigen::VectorXd &x, Eigen::MatrixXd &J) const
	{
		int nrSurfaces = surfaces->size();

		J = MatrixXd::Zero(6 * nrSurfaces, 6 * nrSurfaces);

		for (int i = 0; i<nrSurfaces; i++)
		{
			MatrixXd tempJ = MatrixXd::Zero(6, 6);

			Vector3d surfacePoint1(x[6 * i], x[6 * i + 1], x[6 * i + 2]);
			Vector3d surfacePoint2(x[6 * i + 3], x[6 * i + 4], x[6 * i + 5]);
			Vector3d connectionPoint1;
			Vector3d connectionPoint2;
			if (i == 0)
			{
				connectionPoint1 = p0;
			}
			else
			{
				connectionPoint1 = Vector3d(x[6 * (i - 1) + 3], x[6 * (i - 1) + 4], x[6 * (i - 1) + 5]);
			}
			if (i == nrSurfaces - 1)
			{
				connectionPoint2 = q0;
			}
			else
			{
				connectionPoint2 = Vector3d(x[6 * (i + 1) + 0], x[6 * (i + 1) + 1], x[6 * (i + 1) + 2]);
			}

			(*surfaces)[i]->wrapfunJ(surfacePoint1, surfacePoint2, connectionPoint1, connectionPoint2, &tempJ);

			/* for(int k=0;k<6;k++)
			for(int l=0;l<6;l++)
			J(6*i+k,6*i+l) = tempJ(k,l);
			*/
			J.block<6, 6>(6 * i, 6 * i) = tempJ;
			// cout << tempJ << endl;


		}
		//cout << "final gradient" << J << endl;
		//cout << "X after gradient computation " << x << endl;
		return 0;
	}

	vector<mafQuadraticSurface*>* surfaces = new vector<mafQuadraticSurface*>();
	Vector3d p0;
	Vector3d q0;


	// Number of data points, i.e. values.
	int m;

	// Returns 'm', the number of values.

	int values()  const { return m; }

	// The number of parameters, i.e. inputs.
	int n;

	// Returns 'n', the number of inputs.

	int inputs()  const { return n; }
	
	

};




/** mafVMESurfaceParametric - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, cube, cone and plane.
*/
class MAF_EXPORT mafVMEMuscleWrapping : public mafVME
{
public:

	mafTypeMacro(mafVMEMuscleWrapping, mafVME);
    





  /** Copy the contents of another mafVMESurfaceParametric into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);

 
  /** return the right type of output */  

  mafVMEOutputPolyline *GetPolylineOutput();
 

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return the suggested pipe-typename for the visualization of this vme */

  virtual mafString GetVisualPipe() { return mafString("mafPipePolyline"); };
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
 // static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  void SetEllipsoidLink(const char *link_name, mafNode *n);
  double GetDistance();
  
  //void SetTimeStamp(mafTimeStamp t);
  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified 
  
  . When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);
  /*
  Fit an ellipsoid to a set of points
  */
  //void fitEllipsoid(vtkMAFSmartPointer<vtkPoints> points, double* xres, double* yres, double* zres, double*r1, double* r2, double* r3, Matrix3d* rotationMatrix);

  mafVMEMuscleWrapping();
	virtual ~mafVMEMuscleWrapping();
protected:


  enum PARAMETRIC_SURFACE_WIDGET_ID
  {
    //CHANGE_PARAMETER = Superclass::ID_LAST,

	ID_LAST,
	ID_ELLIPSOID_LINK,
	ID_ELLIPSOID2_LINK,
	ID_Q0_LINK,
	ID_P0_LINK,
	ID_insert1_LINK,
	ID_insert2_LINK,
	ID_DRate
  };

  enum ID_ORIENTATION_AXIS
  {
    ID_X_AXIS = 0,
    ID_Y_AXIS,
    ID_Z_AXIS,
  };
  void SetInputPointsCloud(vtkPoints *);

  /** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
  //void EnableQuadricSurfaceGui(int surfaceTypeID);

	virtual int InternalStore(mafStorageElement *parent);
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
 void InternalUpdate();
 void UpdateLinks();
  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();





  mafString m_EllipsoidVmeName;
  mafString m_EllipsoidVmeName2;
  int rate;
  double* centerTemp;
  vtkAppendPolyData *m_Goniometer;
  mafString m_P0LandmarkName;
  mafString m_Q0LandmarkName;
  mafString m_insert1LandmarkName;
  mafString m_insert2LandmarkName;
  mafTransform *m_Transform; 
  vtkPolyData  *m_PolyData;
  mafString gLength;
  mafString gError;
  double length;
  int m_ComputeStateCheckbox;
  vtkMAFSmartPointer<vtkPoints> points;
 

  void SetLandmarkLink(const char *, mafNode *);
 


//  double m_PointsR1;
//  double m_PointsR2;
//  double m_PointsR3;




  Eigen::Matrix3d rotationMat;
  //mafVMELandmarkCloud	*m_Cloud1;
  mafVMELandmarkCloud	*m_Cloud2;
  //mafVMELandmarkCloud	*m_CloudPath1;
  //mafVMELandmarkCloud	*m_CloudPath2;
  
  mafQuadraticSurface* ellip;
  mafQuadraticSurface* ellip2;
 // mafVMEEllipsoid* ellip;
 // mafVMEEllipsoid* ellip2;
  double ** computeLineIntersection(double l1[3], double l2[3], int*, mafQuadraticSurface* ellipsoid);


  mafString ss = " ";
  double globalLength;
  double globalError;
  protected:
	  mafVME* GetP0VME();
	  mafVME* GetQ0VME();

	  mafVME* Getinsert1VME();
	  mafVME* Getinsert2VME();
	  mafVME* GetEllipsoidVME();
	  mafVME* GetEllipsoidVME2();
	  
	  mafVME *P0;
	  mafVME *Q0;
	  mafVME* insert1;
	  mafVME* insert2;
	  mafVMELandmark* pp;
	  mafVMELandmark* qq;
	  mafVMELandmark* p;
	  mafVMELandmark* p2;
	  mafVMELandmark* q;
	  mafVMELandmark* q2;
	  double m_insert1Point[3];
	  double m_insert2Point[3];
	  double m_StartPoint[3];
	  double m_EndPoint[3];
	  double local_start[3];
	  double local_end[3];
	  double local_insert1[3];
	  double local_insert2[3];
	  double local_startP[3];
	  double local_endQ[3];
	  double localEllipCenter[3];
	  bool intersectionInit;
	//  mafVMELineSeg *line1;
	//  mafVMELineSeg *line2;
	  
	//  mafVMELineSeg *line3;
	//  mafVMELineSeg *line4;
	//  mafVMELineSeg *line5;

	  int n1;
	  int n2;


private:
	mafVMEMuscleWrapping(const mafVMEMuscleWrapping&); // Not implemented
	void operator=(const mafVMEMuscleWrapping&); // Not implemented
};
#endif
