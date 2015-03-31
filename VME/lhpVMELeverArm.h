/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVMELeverArm.h,v $
  Language:  C++
  Date:      $Date: 2008-09-18 08:59:51 $
  Version:   $Revision: 1.23 $
  Authors:   Marco Petrone, Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpVMELeverArm_h
#define __lhpVMELeverArm_h

#include "lhpDefines.h"

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEHelAxis.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEVolumeGray.h"
#include "mafEvent.h"
#include "vectors.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmaMeter;
class mmaMaterial;
class vtkLineSource;
class vtkLineSource;
class vtkAppendPolyData;

class vtkXYPlotActor;
class mafRWI;
class mafGUIDialogPreview;


/** lhpVMELeverArm - 
*/
class MAF_EXPORT lhpVMELeverArm : public mafVME
{
public:
  MAF_ID_DEC(LENGTH_THRESHOLD_EVENT);

  enum METER_COLOR_TYPE_ID
  {
    ONE_COLOR=0,
    RANGE_COLOR
  };
  enum METER_REPRESENTATION_ID
  {
    LINE_REPRESENTATION=0,
    TUBE_REPRESENTATION
  };
  enum METER_MEASURE_ID
  {
    ABSOLUTE_MEASURE=0,
    RELATIVE_MEASURE
  };
  mafTypeMacro(lhpVMELeverArm,mafVME);

  enum METER_WIDGET_ID
  {
    ID_AXIS_LINK = Superclass::ID_LAST,
    ID_LINE_LINK,
    ID_LAST
  };

  static bool AxisAccept(mafNode *node) {return(node != NULL && node->IsMAFType(mafVMEHelAxis));};
  static bool LineAccept(mafNode *node) {return(mafVME::SafeDownCast(node) != NULL && mafVMEOutputPolyline::SafeDownCast(mafVME::SafeDownCast(node)->GetOutput()) != NULL);};

  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Meter into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Meter. */
  virtual bool Equals(mafVME *vme);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("lhpPipeLeverArm");};

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

  /** Store the min and max distance to associate with colors.*/
  void SetDistanceRange(double min, double max);
  void SetDistanceRange(double range[2]) {SetDistanceRange(range[0],range[1]);};

  /** Retrieve the distance range associated with colors.*/
  double *GetDistanceRange();
  void GetDistanceRange(double range[2]);

  /** Color the meter with a range colors extracted by a LookupTable or in flat mode selected by material library. */
  void SetMeterColorMode(int mode);
  void SetMeterColorModeToOneColor() {this->SetMeterColorMode(lhpVMELeverArm::ONE_COLOR);}
  void SetMeterColorModeToRangeColor() {this->SetMeterColorMode(lhpVMELeverArm::RANGE_COLOR);}

  /** Get the color mode of the meter. */
  int GetMeterColorMode();

  /** Set the measure type to absolute or relative to the initial measure. */
  void SetMeterMeasureType(int type);
  void SetMeterMeasureTypeToAbsolute() {this->SetMeterMeasureType(lhpVMELeverArm::ABSOLUTE_MEASURE);}
  void SetMeterMeasureTypeToRelative() {this->SetMeterMeasureType(lhpVMELeverArm::RELATIVE_MEASURE);}

  /** Get the measure type. */
  int GetMeterMeasureType();

  /** Represent the meter with a tube or as a line. */
  void SetMeterRepresentation(int representation);
  void SetMeterRepresentationToLine() {this->SetMeterRepresentation(lhpVMELeverArm::LINE_REPRESENTATION);}
  void SetMeterRepresentationToTube() {this->SetMeterRepresentation(lhpVMELeverArm::TUBE_REPRESENTATION);}

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

  /** Get the percentage elongation. */
  int GetDeltaPercent();

  /** return distance extracting it from the connected pipe. */
  double GetDistance();

  /** return the meter's attributes */
  mmaMeter *GetMeterAttributes();

  mafVME *GetHAxisVME();
  mafVME *GetLineVME();
  void GetStartGlobal(double *pos){pos[0] = m_StartPointGlobal[0];pos[1] = m_StartPointGlobal[1];pos[2] = m_StartPointGlobal[2];}

  /** Set links for the meter*/
  void SetMeterLink(const char *link_name, mafNode *n);

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

protected:
  lhpVMELeverArm();
  virtual ~lhpVMELeverArm();

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


  double m_Distance;
  V3d<double> m_StartPoint;
  V3d<double> m_EndPoint;

  V3d<double> m_StartPointGlobal;

  mafTransform *m_Transform;

  vtkLineSource     *m_LineSource;
  vtkAppendPolyData *m_Goniometer;
  vtkPolyData       *m_PolyData;
  mafTransform      *m_TmpTransform;

  mafString m_HAxisVmeName;
  mafString m_LineVmeName;

private:
  lhpVMELeverArm(const lhpVMELeverArm&); // Not implemented
  void operator=(const lhpVMELeverArm&); // Not implemented
};
#endif
