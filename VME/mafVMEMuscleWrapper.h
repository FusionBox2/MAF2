/*=========================================================================

 Program: MAF2
 Module: mafVMEMeter
 Authors: Marco Petrone, Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEMuscleWrapperAQ_h
#define __mafVMEMuscleWrapperAQ_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEVolumeGray.h"
#include "mafEvent.h"
#include "mafLandmark.hpp"
#include "mafVMEEllipsoid.h"
#include "mafQuadraticSurface.hpp"
//dictionary
#include "mafGUIHolder.h"
#include "mafGUISplittedPanel.h"
#include "mafGUINamedPanel.h"
#include "mafGUIDictionaryWidget.h"
#include "mafVMEMeter.h"
#include "unsupported/Eigen/LevenbergMarquardt"
//#include "mafOpExplodeCollapse.h"
//#include "mafSmartPointer.h"
#include <vector>
//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmaMuscleWrapperAQ;
class mmaMaterial;
class vtkLineSource;
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
	int values() const { return m; }

	// The number of parameters, i.e. inputs.
	int n;

	// Returns 'n', the number of inputs.
	int inputs() const { return n; }

};

/** mafVMEMeter - 
*/
class MAF_EXPORT mafVMEMuscleWrapperAQ : public mafVME
{
public:
  MAF_ID_DEC(LENGTH_THRESHOLD_EVENT);

  enum MuscleWrapperAQ_MEASURE_TYPE_ID
  {
    POINT_DISTANCE=0,
    LINE_DISTANCE,
    LINE_ANGLE,
  };
  enum MuscleWrapperAQ_COLOR_TYPE_ID
  {
    ONE_COLOR=0,
    RANGE_COLOR
  };
  enum MuscleWrapperAQ_REPRESENTATION_ID
  {
    LINE_REPRESENTATION=0,
    TUBE_REPRESENTATION
  };
  enum MuscleWrapperAQ_MEASURE_ID
  {
    ABSOLUTE_MEASURE=0,
    RELATIVE_MEASURE
  };
  mafTypeMacro(mafVMEMuscleWrapperAQ, mafVME);

  enum MuscleWrapperAQ_WIDGET_ID
  {
    ID_START_METER_LINK = Superclass::ID_LAST,
    ID_START2_METER_LINK,
    ID_END1_METER_LINK,
    ID_END2_METER_LINK,
    ID_INFINITE_LINE,
    ID_LINE_ANGLE2,
    ID_PLOTTED_VME_LINK,
	  ID_METER_MODE,
    ID_PLOT_PROFILE,
    ID_LAST
  };

  static bool VMEAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVME));};
  static bool VolumeAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEVolumeGray));};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Meter into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Meter. */
  virtual bool Equals(mafVME *vme);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString(_R("mafPipeMeter"));};

  /** return the right type of output */  
  mafVMEOutputPolyline *GetPolylineOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /** Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();
  
  /** return an xpm-icon that can be used to represent this node */
  //static char ** GetIcon();

  /**Get/Set the kind of measure  performed: distance between points (POINT) or
  distance between a point and a line. */
  int GetMeterMode();
  void SetMeterMode(int mode);
  void SetMeterModeToPointDistance() { this->SetMeterMode(mafVMEMuscleWrapperAQ::POINT_DISTANCE); }
  void SetMeterModeToLineDistance() { this->SetMeterMode(mafVMEMuscleWrapperAQ::LINE_DISTANCE); }
  void SetMeterModeToLineAngle() { this->SetMeterMode(mafVMEMuscleWrapperAQ::LINE_ANGLE); }

  /** Store the min and max distance to associate with colors.*/
  void SetDistanceRange(double min, double max);
  void SetDistanceRange(double range[2]) {SetDistanceRange(range[0],range[1]);};

  /** Retrieve the distance range associated with colors.*/
  double *GetDistanceRange();
  void GetDistanceRange(double range[2]);

  /** Color the meter with a range colors extracted by a LookupTable or in flat mode selected by material library. */
  void SetMeterColorMode(int mode);
  void SetMeterColorModeToOneColor() { this->SetMeterColorMode(mafVMEMuscleWrapperAQ::ONE_COLOR); }
  void SetMeterColorModeToRangeColor() { this->SetMeterColorMode(mafVMEMuscleWrapperAQ::RANGE_COLOR); }

  /** Get the color mode of the meter. */
  int GetMeterColorMode();

  /** Set the measure type to absolute or relative to the initial measure. */
  void SetMeterMeasureType(int type);
  void SetMeterMeasureTypeToAbsolute() { this->SetMeterMeasureType(mafVMEMuscleWrapperAQ::ABSOLUTE_MEASURE); }
  void SetMeterMeasureTypeToRelative() { this->SetMeterMeasureType(mafVMEMuscleWrapperAQ::RELATIVE_MEASURE); }

  /** Get the measure type. */
  int GetMeterMeasureType();

  /** Represent the meter with a tube or as a line. */
  void SetMeterRepresentation(int representation);
  void SetMeterRepresentationToLine() { this->SetMeterRepresentation(mafVMEMuscleWrapperAQ::LINE_REPRESENTATION); }
  void SetMeterRepresentationToTube() { this->SetMeterRepresentation(mafVMEMuscleWrapperAQ::TUBE_REPRESENTATION); }

  /** Get the representation mode of the meter. */
  int GetMeterRepresentation();

  /** Represent the meter with a tube or as a line. */
  void SetMeterCapping(int capping);
  void SetMeterCappingOn() {this->SetMeterCapping(1);}
  void SetMeterCappingOff() {this->SetMeterCapping(0);}

  /** Get the capping of tube representation of the meter. */
  int GetMeterCapping();

  /** Enable-Disable event generation of length threshold. */
  void SetGenerateEvent(int generate);
  void SetGenerateEventOn() {SetGenerateEvent(1);}
  void SetGenerateEventOff() {SetGenerateEvent(0);}

  /** Get the length threshold event generation status. */
  int GetGenerateEvent();

  /** Set the initial measure. */
  void SetInitMeasure(double init_measure);

  /** Get the initial measure. */
  double GetInitMeasure();

  /** Set the meter radius when represented as tube. */
  void SetMeterRadius(double radius);

  /** Get the meter radius when represented as tube. */
  double GetMeterRadius();

  /** Set the percentage elongation. */
  void SetDeltaPercent(int delta_percent);

  void SetLineAngle2(int la2);
  int GetLineAngle2();


  /** Get the percentage elongation. */
  int GetDeltaPercent();

  /** return distance extracting it from the connected pipe. */
  double GetDistance();
  double GetValue();
  /** return angle extracting it from the connected pipe. */
  double GetAngle();

  /** return the meter's attributes */
  mmaMuscleWrapperAQ *GetMeterAttributes();

  mafVME *GetStartVME();
  mafVME *GetStart2VME();
  mafVME *GetEnd1VME();
  mafVME *GetEnd2VME();
  mafVME *GetPlottedVME();

  /** Set links for the meter*/
  virtual void SetMeterLink(const mafString& link_name, mafNode *n);

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** Turn On/Off the creation of the histogram.*/
  void GenerateHistogram(int generate);

  /** Turn On the generation of the histogram from the measured line. It force the measure to be on DISTANCE_BETWEEN_POINTS.*/
  void GenerateHistogramOn() {GenerateHistogram(1);};

  /** Turn Off the generation of the histogram.*/
  void GenerateHistogramOff() {GenerateHistogram(0);};

  /** Retrieve StartPoint coordinates*/
  double *GetStartPointCoordinate(){return m_StartPoint;};

  double *GetStart2PointCoordinate(){return m_StartPoint2;};

  /** Retrieve EndPoint1 coordinates*/
  double *GetEndPointCoordinate(){return m_EndPoint;};

  /** Retrieve EndPoint coordinates*/
  double *GetEndPoint2Coordinate(){return m_EndPoint2;};
  /*void ComputeWrap();

  mafdmLandmark* p0 = new mafdmLandmark(30, 30, 80, "p0");
  mafdmLandmark* q0 = new mafdmLandmark(30, 30, -80, "q0");

  mafdmEllipsoid* e = new mafdmEllipsoid(0.0, 0.0, 0.0, 20.0, 20.0, 30.0);

  LMFunctor functor;
  std::vector<mafQuadraticSurface*> surfaces;

  VectorXd X;


  mafdmLandmark* p;
  mafdmLandmark* q;

  vector<Vector3d> path;
  vtkSmartPointer<vtkPoints> points;*/
protected:

	mafVMEMuscleWrapperAQ();
	virtual ~mafVMEMuscleWrapperAQ();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** this creates the Material attribute at the right time... */
  virtual int InternalInitialize();

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** Update the names for the meter's links. */
  void UpdateLinks();

  /** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  /** 
  Create the histogram*/
  void CreateHistogram();

  double m_Distance;
  double m_Angle;
  double m_StartPoint[3];
  double m_StartPoint2[3];
  double m_EndPoint[3];
  double m_EndPoint2[3];
  int    m_InfiniteLine;
  int    m_LineAngle2;

  mafTransform *m_Transform;

  vtkLineSource     *m_LineSource1;
  vtkLineSource     *m_LineSource2;
  vtkLineSource     *m_LineSource3;
  vtkAppendPolyData *m_Goniometer;
  vtkPolyData       *m_PolyData;
  mafTransform      *m_TmpTransform;

  mafString m_StartVmeName;
  mafString m_StartVme2Name;
  mafString m_EndVme1Name;
  mafString m_EndVme2Name;
  mafString m_ProbeVmeName;

  vtkLineSource       *m_ProbingLine;

  mafGUIDialogPreview  *m_HistogramDialog;
  mafRWI    *m_HistogramRWI;
  vtkXYPlotActor *m_PlotActor;

  int m_GenerateHistogram;
  mafVMEVolumeGray *m_ProbedVME; ///< VME probed by the m_ProbingLine

  mafGUIDictionaryWidget *m_Dict;

private:
	mafVMEMuscleWrapperAQ(const mafVMEMuscleWrapperAQ&); // Not implemented
	void operator=(const mafVMEMuscleWrapperAQ&); // Not implemented

	

	




};
#endif