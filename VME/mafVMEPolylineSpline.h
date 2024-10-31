/*=========================================================================

 Program: MAF2
 Module: mafVMEPolylineSpline
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafVMEPolylineSpline_h
#define __mafVMEPolylineSpline_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafNode;
class vtkPoints;
class mmaMaterial;

/** mafVMEPolylineSpline - a procedural VME computing the spline by a given polyline as link
  mafVMEPolylineSpline is a node implementing a spline starting from a polyline using vtkCardinalSpline
  algorithm.
  @sa mafVMEPolyline
  @todo
  - 
  */
class MAF_EXPORT mafVMEPolylineSpline : public mafVME
{
public:

  enum ID_VME_POLYLINE_SPLINE
  {
    AXIS_NONE = 0,
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
  };

  mafTypeMacro(mafVMEPolylineSpline,mafVME);

  /** print a dump of this object */
  void Print(std::ostream& os, const int tabs=0) override;

  /** Copy the contents of another VME-Spline into this one. */
  int DeepCopy(mafNode *a) override;

  /** Compare with another VME-Spline. */
  bool Equals(mafVME *vme) override;

  /** return the right type of output */  
  mafVMEOutputPolyline *GetPolylineOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  void SetMatrix(const mafMatrix &mat) override;

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override;

  /** return always false since (currently) the spline is not an animated VME (position 
      is the same for all timestamps). */
  bool IsAnimated() override;

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  bool IsDataAvailable() override;

  /** Return the suggested pipe-typename for the visualization of this vme */
  mafString GetVisualPipe() override {return mafString(_R("mafPipePolyline"));}

  /** return an xpm-icon that can be used to represent this node */
  static const char **GetIcon();

	/** Set links for the Polyline*/
  void SetPolylineLink(mafNode *n);

  /** Get links for the Polyline*/
	mafVME *GetPolylineLink();

  /** Order the points and cell in polydata*/
  void SplinePolyline(vtkPolyData *polyline);

  /** Order the points and cell in polydata*/
  void OrderPolyline(vtkPolyData *polyline);

  /** Return pointer to material attribute. */
  std::shared_ptr<mmaMaterial> GetMaterial();

  /** reorganize the points in order to follow an axis */
  void OrderPolylineByAxis(vtkPolyData* polyline, int axis);

	enum SPLINE_WIDGET_ID
  {
    ID_LINK_POLYLINE = Superclass::ID_LAST,
		ID_NUMBER_NODES,
    ID_LAST
  };

	static bool PolylineAccept(mafNode *node) {return(node != NULL && (node->IsA("mafVMEPolyline")));};

	/** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event) override;

  /** Get Coefficient used for calculated number of points*/ 
  int GetSplineCoefficient(){return m_SplineCoefficient;}
  void SetSplineCoefficient(int coeff){m_SplineCoefficient = coeff;}

  void SetOrderByAxisFlag(int axis){m_OrderByAxisMode = axis;};
 
  // obsolete function
  //void SetMinimumSpacingOptimization(double spacing){m_MinimumSpacing = spacing;}

protected:
  mafVMEPolylineSpline();
  ~mafVMEPolylineSpline() override;

  //void OptimizeMinimumSpacingSpline();

  void InternalStore(mafStorageElementBuilder& parent) override;
  void InternalRestore(const mafStorageElement& node) override;

  /** called to prepare the update of the output */
  void InternalPreUpdate() override;

  /** update the output data structure */
  void InternalUpdate() override;

	/** Internally used to create a new instance of the GUI.*/
  mafGUI *CreateGui() override;

  std::shared_ptr<mafTransform>     m_Transform; ///< pose matrix for the spline 

  vtkPolyData				*m_Polyline;
	vtkPoints					*m_PointsSplined;

	int								m_SplineCoefficient;
  //double            m_MinimumSpacing;

	mafString					m_PolylineLinkName;
  int               m_OrderByAxisMode;


private:
  mafVMEPolylineSpline(const mafVMEPolylineSpline&); // Not implemented
  void operator=(const mafVMEPolylineSpline&); // Not implemented
};

#endif
