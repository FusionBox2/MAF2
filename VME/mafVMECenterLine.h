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

#ifndef __mafVMECenterLine_h
#define __mafVMECenterLine_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafEvent.h"
#include "vtkSmartPointer.h"
#include "vtkPointSet.h"
#include "../Eigen/Eigen/Dense"
#include "mafVMEPlane.h"
#include "mafVMEMeter.h"
#include "mafVMELineSeg.h"
#include "vtkAppendPolyData.h"
#include "unsupported/Eigen/LevenbergMarquardt"
#include <vector>
#include <iostream>
#include <fstream>
#include "mafVMEPlane.h"
#include "mafVMESurface.h"

#include <vector>

using namespace Eigen;
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafNode;
class mmaMaterial;

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


struct ellipse_struct
{
	double a;
	double b;
	double phi;// orientation_rad, ...
	double X0;
	double Y0;
	double  X0_in;
	double Y0_in;
	double  long_axis;
	double  short_axis;
	double status;


};


class MAF_EXPORT mafVMECenterLine : public mafVME
{
public:

	mafTypeMacro(mafVMECenterLine, mafVME);
	mafString FBaseName = _R("");
  ellipse_struct* fitellipse(Eigen::MatrixX3d,int ,int);

  double Circumf = 0;
  double CrSec_Ratio = 0;
  Eigen::Matrix3Xd Twist_X_ax;
  Eigen::VectorXd All_Circumf;
  Eigen::VectorXd All_CrSec_Ratio;

  Eigen::Matrix3d principalAxesLCSRib(int, std::vector<std::vector<double>>& vertex, std::vector<std::vector<double>>& Cross_Sec_PGD_LCS, Eigen::Vector3d);
  Eigen::Matrix3d principalAxesLCS(int nbr, std::vector<std::vector<double>>& vertex, std::vector<std::vector<double>>& Cross_Sec_PGD_LCS);
  virtual void Update();
  Eigen::Matrix3d theta2r(Eigen::Vector3d);

  Eigen::RowVectorXd deg2rad(Eigen::RowVectorXd);
  Eigen::Vector4d extract(Eigen::Matrix3d R);

  double Max_Eigen_v(int N, int NDIM, Eigen::Matrix3d A, double EPS, std::vector<std::vector<double>>&);
  int sign(double);
 

  /** Copy the contents of another mafVMESurfaceParametric into this one. */
  virtual int DeepCopy(mafNode *a);

  
  virtual bool Equals(mafVME *vme);

 
  /** return the right type of output */  
 // mafVMEOutputSurface *GetSurfaceOutput();
  mafVMEOutputPolyline *GetPolylineOutput();
 
  void SetSurfaceLink(const mafString&, mafNode *);
  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return the suggested pipe-typename for the visualization of this vme */
  //virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};
  virtual mafString GetVisualPipe() { return mafString(_R("mafPipePolyline")); };
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
 // static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);
  ellipse_struct ellipse_t;

  void SetTimeStamp(mafTimeStamp t);
  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified 
  
  . When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/


  virtual void SetMatrix(const mafMatrix &mat);
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  mafVMECenterLine();
  virtual ~mafVMECenterLine();
protected:
	mafVMEMeter* chord, * chordP;
	int m_Rib = 0;
	int m_FS = 1;
	mafString m_File = _L("ptsResults.txt");
  enum PARAMETRIC_SURFACE_WIDGET_ID
  {


//	CHANGE_VALUE_POINTS1,
	ID_LAST,
	CHANGE_VALUE_CenterLine,
	CHANGE_VALUE_RIB,
	CHANGE_VALUE_FileSaving,
	ID_FILENAME,
	ID_Surface_LINK
  };

  void SetInputPointsCloud(vtkPoints *);
  mafVMELandmarkCloud	*m_CloudPath1;
  double Delt_1 = 5;///5
  double Mult_01 = 0.05;
  double	D_1mm = 1.5;
  double	D_50mm = 25;
  double	Fi_stp_5 =  3;

	virtual int InternalStore(mafStorageElementBuilder& parent);
	virtual int InternalRestore(const mafStorageElement& node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
 void InternalUpdate();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();



  mafString m_SurfaceName;


 // mafGUI *m_GuiSelectPoints;
  double* centerTemp;
//  vtkAppendPolyData *m_Goniometer;


  mafTransform *m_Transform; 
  vtkPolyData  *m_PolyData;

  vtkNew<vtkPoints> points;

  
 
  void UpdateLinks();

  Eigen::VectorXd polyeval(std::vector<double>& coeffs, std::vector<double>& x);
  Eigen::Matrix3d rotationMat;
 
  Eigen::VectorXd polyfit(std::vector<double>& xvals, std::vector<double>& yvals, int order);
  Eigen::Matrix3d ov_mr_ShV(std::vector<double>& a);
  Eigen::Vector3d getOrigin(int, std::vector<std::vector<double>>& a);
  double ShV_ICR_MHA(int Ind_Call,int nbr, std::vector<std::vector<double>>& Rm_in_PGD, double Angle_filtr, std::vector<std::vector<double>>&, std::vector<std::vector<double>>&, std::vector<std::vector<double>>&, std::vector<std::vector<double>>&);
  
  mafVMESurface* surface;
  

  protected:

	  mafVMESurface* GetSurfaceVME();
	  
	
	  

private:
	mafVMECenterLine(const mafVMECenterLine&); // Not implemented
	void operator=(const mafVMECenterLine&); // Not implemented
};
#endif
