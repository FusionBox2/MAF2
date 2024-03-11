/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEHelAxis.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafVMEHelAxis_h
#define __mafVMEHelAxis_h

//-----------------------------------------------------------------------
// Includes:
//-----------------------------------------------------------------------
#include "mafVME.h"
#include "mafVmeOutput.h"
#include "mafVMEGeneric.h"
#include "mafVectors.h"

//-----------------------------------------------------------------------
// class forwarding
//-----------------------------------------------------------------------
class vtkArrowSource;
class vtkSphereSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;
class vtkPolyData;
class mmaMaterial;

//-----------------------------------------------------------------------
// class mafVMEHelAxis
//-----------------------------------------------------------------------
class mafVMEHelAxis : public mafVME
{
public:
  mafTypeMacro(mafVMEHelAxis,mafVME);

  enum 
  {
    ID_NAME_REF_SYS = Superclass::ID_LAST,
    ID_LENGTH_FACTOR,
    ID_RADIUS_FACTOR,
    ID_MIN_ANGLE,
    ID_MIN_TIME,
    ID_MAX_TIME,
    ID_REF_TIME,
    ID_MODE,
    ID_ALIGNING,
    ID_PROXIMAL,
    ID_RESETPROXIMAL,
    ID_DISTAL,
    ID_RESETDISTAL,
    ID_LAST
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;
  bool     CanReparentTo(mafNode *parent) {return GetParent() == NULL || parent == NULL;}

  void     OnEvent(mafEventBase *maf_event);
  /** Copy the contents of another VME-RefSys into this one. */
  virtual int DeepCopy(mafNode *a);
  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString(_R("mafPipeSurface"));};
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Used to change the axes size */
  void SetLengthFactor(double scale);
  /** Used to change the axes size */
  void SetRadiusFactor(double scale);

  /** Return the axes size */
  double GetLengthFactor();
  /** Return the axes size */
  double GetRadiusFactor();

  const V3d<double>& GetDirection(){return m_Direction;}
  const V3d<double>& GetStartPoint(){return m_StartPoint;}
  double GetAngle(){return m_Angle;}
  double GetTranslation(){return m_Translation;}

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  void CalculateMatrix(mafMatrix &mtr, mafTimeStamp tsTime);
  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){kframes.clear();}

protected:
  mafVMEHelAxis();
  virtual ~mafVMEHelAxis();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  mafGUI *CreateGui();
  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();
  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** update the output data structure */
  virtual void InternalUpdate();

  /** update the output data structure */
  void UpdateCS();

  /** update the output data structure */
  bool AlignAxis(const V3d<double>& direction, const mafMatrix& prox, int mode);

  /** Used to change the axes size */
  void UpdateLengthFactor();

  vtkArrowSource             *m_ZArrow;
  vtkSphereSource            *m_CenterSphere;

  vtkTransformPolyDataFilter *m_Center;
  vtkTransform               *m_CenterTransform;

  vtkTransformPolyDataFilter *m_ZAxis;
  vtkTransform               *m_ZAxisTransform;

  vtkTransformPolyDataFilter *m_ScaleAxis;
  vtkTransform               *m_ScaleAxisTransform;

  vtkAppendPolyData          *m_Axes;

  double                     m_LengthFactor;
  double                     m_RadiusFactor;
  double                     m_AngleFactor;

  double                     m_MinAngle;
  double                     m_MinTime;
  double                     m_MaxTime;
  double                     m_RefTime;
  int                        m_Mode;
  int                        m_AligningMode;

  V3d<double>                m_StartPoint;
  V3d<double>                m_Direction;
  double                     m_Angle;
  double                     m_Translation;

  mafString                  m_StrDir[3];
  mafString                  m_StrPnt[3];
  mafString                  m_StrAng;
  mafString                  m_StrTrl;

  int                        m_MeanChanges;

  mafVME                     *GetProximal();
  mafVME                     *GetDistal();

  void                       SetProximal(mafVME *proximal);
  void                       SetDistal(mafVME *distal);
  mafString                  m_ProximalName;
  mafString                  m_DistalName;

  mafTransform               *m_Transform; ///< pose matrix for the slicer plane
private:
  mafVMEHelAxis (const mafVMEHelAxis &); // Not implemented
  void operator=(const mafVMEHelAxis &); // Not implemented
};
#endif
