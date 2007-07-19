/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEAFRefSys.h,v $
  Language:  C++
  Date:      $Date: 2007-07-19 12:37:34 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/
 
#ifndef __mfAFRefSys_h
#define __mfAFRefSys_h

//----------------------------------------------------------------------------
// Includes:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVmeOutput.h"
#include "mafVMEGeneric.h"
#include "mafVMERefSys.h"

//----------------------------------------------------------------------------
// class fowarding:
//----------------------------------------------------------------------------
class vtkArrowSource;
class vtkTransformPolyDataFilter;
class vtkTransform;
class vtkAppendPolyData;
class vtkPolyData;

//----------------------------------------------------------------------------
// class mafVMEAFRefSys
//----------------------------------------------------------------------------
class mafVMEAFRefSys : public mafVMEGeneric
{
public:
  mafTypeMacro(mafVMEAFRefSys,mafVMEGeneric);

  enum 
  {
    ID_NAME_REF_SYS = Superclass::ID_LAST,
    ID_SCALE_FACTOR,
    ID_PRINT,
    ID_LOAD_DICTIONARY,
    ID_X_OFFSET,
    ID_Y_OFFSET,
    ID_Z_OFFSET,
    ID_X_ROTATE,
    ID_Y_ROTATE,
    ID_Z_ROTATE,
    ID_LAST
  };

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;
  bool     CanReparentTo(mafNode *parent) {return GetParent() == NULL || parent == NULL;}

  void     OnEvent(mafEventBase *maf_event);

  void     SetTransf(double x, double y, double z, double xr, double yr, double zr);
  void     GetTransf(double &x, double &y, double &z, double &xr, double &yr, double &zr);

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};
  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

  /** Used to change the axes size */
  void SetScaleFactor(double scale);

  /** Return the axes size */
  double GetScaleFactor();

protected:
  mafVMEAFRefSys();
  virtual ~mafVMEAFRefSys();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  mmgGui *CreateGui();
  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();
  /** used to initialize and create the material attribute if not yet present */
  virtual int InternalInitialize();

  /** update the output data structure */
  virtual void InternalUpdate();

  //virtual void SetMatrix(const mafMatrix &mat){mafVME::SetMatrix(mat);};

  /** update the output data structure */
  void UpdateCS();

  vtkArrowSource             *m_XArrow;
  vtkArrowSource             *m_YArrow;
  vtkArrowSource             *m_ZArrow;

  vtkTransformPolyDataFilter *m_XAxis;
  vtkTransform               *m_XAxisTransform;

  vtkTransformPolyDataFilter *m_YAxis;
  vtkTransform               *m_YAxisTransform;

  vtkTransformPolyDataFilter *m_ZAxis;
  vtkTransform               *m_ZAxisTransform;

  vtkTransformPolyDataFilter *m_ScaleAxis;
  vtkTransform               *m_ScaleAxisTransform;

  vtkAppendPolyData          *m_Axes;

  double                     m_ScaleFactor;

  mafTransform               *m_Transform; ///< pose matrix for the slicer plane

  double                     m_XOffset, m_YOffset, m_ZOffset;
  double                     m_XRotate, m_YRotate, m_ZRotate;
  double                     m_XoOffset, m_YoOffset, m_ZoOffset;
  double                     m_XoRotate, m_YoRotate, m_ZoRotate;
private:
  mafVMEAFRefSys (const mafVMEAFRefSys &); // Not implemented
  void operator=(const mafVMEAFRefSys &); // Not implemented
};
#endif
