/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEArrow.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:15:29 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef __mafVMEArrow_h
#define __mafVMEArrow_h

#include "mafDefines.h"

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
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;
class vtkPolyData;
class mmaMaterial;

//-----------------------------------------------------------------------
// class mafVMEArrow
//-----------------------------------------------------------------------
class mafVMEArrow : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMEArrow,mafVMEGeneric);

  enum 
  {
    ID_NAME_REF_SYS = Superclass::ID_LAST,
    ID_SCALE_FACTOR,
    ID_PRINT,
    ID_LAST
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  void     OnEvent(mafEventBase *maf_event);
  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString(_R("mafPipeSurface"));};
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Used to change the axes size */
  void SetScaleFactor(double scale);

  /** Return the axes size */
  double GetScaleFactor();

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  //virtual void SetMatrix(const mafMatrix &mat);

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){kframes.clear();}

protected:
  mafVMEArrow();
  virtual ~mafVMEArrow();

  virtual int InternalStore(mafStorageElementBuilder& parent);
  virtual int InternalRestore(const mafStorageElement& node);

  mafGUI *CreateGui();
  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();
  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** update the output data structure */
  //virtual void InternalUpdate();

  /** update the output data structure */
  void UpdateCS();

  /** Used to change the axes size */
  void UpdateScaleFactor();

  vtkArrowSource             *m_ZArrow;

  vtkTransformPolyDataFilter *m_ZAxis;
  vtkTransform               *m_ZAxisTransform;

  vtkTransformPolyDataFilter *m_ScaleAxis;
  vtkTransform               *m_ScaleAxisTransform;

  vtkAppendPolyData          *m_Axes;

  double                     m_ScaleFactor;
  double                     m_AngleFactor;

  double                     m_StepToNext;

  mafTransform               *m_Transform; ///< pose matrix for the slicer plane
private:
  mafVMEArrow (const mafVMEArrow &); // Not implemented
  void operator=(const mafVMEArrow &); // Not implemented
};
#endif
