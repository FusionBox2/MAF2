/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpComputeTensor.h,v $
Language:  C++
Date:      $Date: 2009-01-15 17:17:35 $
Version:   $Revision: 1.1.2.1 $
Authors:   Gregor Klajnsek
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#ifndef __lhpOpComputeTensor_H__
#define __lhpOpComputeTensor_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVME.h"
#include "mafVMEVolume.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEGeneric.h"
#include "mafGUIDialog.h"
#include "mafRWI.h"

#include "vtkActor.h"
#include "vtkVolume.h"
#include "vtkImageData.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"

#include <wx/statline.h>

#include "lhpComputeTensor.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// lhpOpComputeTensors :
//----------------------------------------------------------------------------
/** TODO: Add your own description here

*/



class lhpOpComputeTensor: public mafOp 
{
public:
 lhpOpComputeTensor(const wxString &label = "lhpOpComputeTensor");
  ~lhpOpComputeTensor(); 

  mafTypeMacro(lhpOpComputeTensor, mafOp);

  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* node);

  /** Builds operation's interface. */
  void OpRun();

  /** Create new VME which will hold tensor values, return MAF_OK on success. */
  virtual int ComputeTensors();

  /** Create GUI */
  void CreateGui();

  /** Create operation dialog and visual pipe*/
  void CreateOpDialogNew();
  void CreateOpDialog();

  /** Delete operation dialog */
  void DeleteOpDialog();

  /** Add new VME into the scene */
  void CreateVisualPipes();

  /** Update camera */
  void UpdateCamera();
  

  /** Creates the dataset that the operation will produce on finish */
  void CreateOutputDataset();

  /** TODO: Add coment */
  void UpdateScalarsInTensorDataset();

  /** TODO: Add coment */
  void UpdateIsosurface();


  /** Create outline pipeline */
  void CreateOutlinePipeline();

  /** Create isosurface pipeline */
  void CreateDataIsosurfacePipeline();

  /** Create volume pipeline */
  void CreateTensorVolumePipeline();

  /** Create vector hedgehog pipeline */
  void CreateVectorHedgehogPipeline();

  /** /TODO: comment */
  void CreateVectorGlyphPipeline();
  
  /** /TODO: comment */
  void CreateTensorGlyphPipeline();


  



  /** Handling events sent from the GUI */
  virtual void OnEvent(mafEventBase *maf_event); 
  void SetDataView();
  void SetDisplacementVectorsView();
  void SetTensorView();
  void TensorComponentChange();

protected:
  mafVMEVolumeGray *m_VmeData;           // VME that contains the input scalar dataset
  mafVMEVolumeRGB *m_VmeDisplacements;   // VME that contains the displacement vectors for the dataset 
  mafVMEVolumeGray *m_VmeTensors;        // VME that contains calculated tensor field
  

  
  // vtkDatasets - required for rendering
  vtkActor*         m_OutlineBox;          // vtkActor used for showing an outline of the data 
  vtkImageData*     m_DataVolume;          // scalar dataset created from the input scalar volume - required?
  vtkActor*         m_DataIsosurfaceActor; // vtkActor used if we are rendering the input volume as isosurface
  vtkVolume*        m_DataVolumeActor;     // vtkVolume used if we are rendering the input volume as volume 
  vtkImageData*     m_TensorVolume;        // tensor volume that is used in visualization
  vtkVolume*        m_TensorVolumeActor;   // vtkVolume used for rendering one component of a tensor volume
  vtkActor*         m_TensorGlyphActor;    // vtkActor used if we are rendering tensors as a tensor glyph
  vtkActor*         m_VectorGlyphActor;    // vtkActor used if we are rendering displacement vectors
  vtkActor*         m_VectorHedgehogActor;    // vtkActor used if we are rendering displacement vectors
  vtkContourFilter* m_ContourFilter;       // for creating the isosurface TODO: update comment
  vtkPolyDataMapper *m_IsoMapper;          // for creating the isosurface TODO: update comment  

  // control variables 
  int m_InterpolationType;         // which sampling will be used when we calculate the tensor field
  int m_bCreateOutput;             // will we create an output dataset that will store the tensor field - needed?
  int m_bAddScalarsToOutput;       // will we also fill the values of the selected component into the output dataset - needed?
  int m_bAddTensorsToOutput;   
  int m_IsosurfaceValue;

  int m_bShowOutline;              // should we show the outline of the volume
  int m_bShowInputDataset;         // is the input dataset visible
  int m_InpuDataVisualizationType;  // which approach to visualization will be used for rendering input data
  int m_bShowVectors;              // are the displacement vectors  visible
  int m_VectorDataVisualizationType;  // which approach to visualization will be used for rendering vector data
  int m_bShowTensors;              // is the tensor dataset visible
  int m_TensorDataVisualizationType;  // which approach to visualization will be used for rendering tensor data
  int m_SelectedTensorComponent;      // which component of the tensor field is currently selected (0 - 8)


  // GUI
  mafGUIDialog		*m_Dialog;        // dialog and interactor
  mafRWI            *m_Rwi;           // render window and interactor

  wxStaticText* m_staticTextData;
  wxCheckBox* m_checkBoxShowData;
  wxRadioBox* m_radioBoxDataRender;
  wxStaticText* m_staticTextIsosurfaceValue;
  wxTextCtrl* m_textCtrlIsosurfaceValue;
  wxSlider* m_sliderIsosurfaceValue;
  wxStaticLine* m_staticline1;
  wxStaticText* m_staticTextVectors;
  wxCheckBox* m_checkBoxShowVectors;
  wxRadioBox* m_radioBoxVectorRender;
  wxStaticLine* m_staticline2;
  wxStaticText* m_staticTextTensors;
  wxCheckBox* m_checkBoxShowTensors;
  wxRadioBox* m_radioBoxTensorRender;
  wxRadioBox* m_radioBoxTensorComponent;
  wxStaticLine* m_staticline3;
  wxStaticText* m_staticTextOutput;
  wxCheckBox* m_checkBoxCreateOutput;
  wxCheckBox* m_checkBoxFillTensorArray;
  wxCheckBox* m_checkBoxFillScalarArray;
  wxButton* m_buttonOK;
  wxButton* m_buttonCancel;

  // enum declarations for better code readability
  enum INTERPOLATION_POINT {
    INTERPOLATION_POINT_CENTROID = 0,
    INTERPOLATION_POINT_GAUSS
  };

  enum DATA_VISUALIZATION_TYPE {
    RENDER_AS_VOLUME = 0,
    RENDER_AS_ISOSURFACE,
   };

  enum VECTOR_VISUALIZATION_TYPE {
    RENDER_AS_HEDGEHOG = 0,
    RENDER_AS_GLYPH
  };

  enum TENSOR_VISUALIZATION_TYPE {
    RENDER_AS_SCALAR_VOLUME = 0,
    RENDER_AS_ELLIPSOIDS
  };


};
#endif