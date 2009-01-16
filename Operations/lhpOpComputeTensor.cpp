/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpComputeTensor.cpp,v $
Language:  C++
Date:      $Date: 2009-01-16 12:15:11 $
Version:   $Revision: 1.1.2.2 $
Authors:   Gregor Klajnsek
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpOpComputeTensor.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafDecl.h"
#include "mafVME.h"
#include "mafVMEGeneric.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEItemVTK.h"
#include "mafDataVector.h"
#include "mafDataPipe.h"
#include "mafGUI.h"
#include "mafDevice.h"
#include "mafGUIButton.h"
#include "mafGUIValidator.h"
//#include "mafGU"

#include "mafTagArray.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkRenderWindow.h"
#include "vtkContourFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeTextureMapper2D.h"
#include "vtkOutlineFilter.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkXRayVolumeMapper.h"
#include "vtkMAFVolumeTextureMapper2D.h"
#include "vtkHedgehog.h"
#include "vtkConeSource.h"
#include "vtkGlyph3d.h"
#include "vtkTensorGlyph.h"
#include "vtkSphereSource.h"
#include "vtkPolyData.h"


//#define UI_TEST


//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpComputeTensor);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpComputeTensor::lhpOpComputeTensor(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  
  // VMEs
  m_VmeData = NULL;
  m_VmeDisplacements = NULL;
  m_VmeTensors = NULL;
  
  // vtkComponents
  m_OutlineBox = NULL;
  m_DataVolume = NULL;
  m_DataIsosurfaceActor = NULL;
  m_DataVolumeActor = NULL;
  m_TensorVolume = NULL;
  m_TensorVolumeActor = NULL;
  m_TensorGlyphActor = NULL;
  m_VectorGlyphActor = NULL;
  m_VectorHedgehogActor = NULL;
  m_ContourFilter = NULL;
  m_IsoMapper = NULL;

  // variables
  m_InterpolationType = INTERPOLATION_POINT_GAUSS;
  m_bCreateOutput = false;
  m_bAddScalarsToOutput = false; 
  m_bAddTensorsToOutput = false;

  m_bShowInputDataset = true;
  m_InpuDataVisualizationType = RENDER_AS_ISOSURFACE;
  m_VectorDataVisualizationType = 0;
  
  m_bShowVectors = false;
  m_VectorDataVisualizationType = RENDER_AS_HEDGEHOG;

  m_bShowTensors = true;
  m_TensorDataVisualizationType = RENDER_AS_SCALAR_VOLUME;
  m_SelectedTensorComponent = 0;

  m_IsosurfaceValue = 128.0;


  // GUI components
  m_Dialog = NULL;
  m_Rwi = NULL;
}
//----------------------------------------------------------------------------
lhpOpComputeTensor::~lhpOpComputeTensor()
//----------------------------------------------------------------------------
{  
  mafDEL(m_VmeTensors);
}
//----------------------------------------------------------------------------
mafOp* lhpOpComputeTensor::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpComputeTensor *cp = new lhpOpComputeTensor(m_Label);
  return cp;
}

//----------------------------------------------------------------------------
bool lhpOpComputeTensor::Accept(mafNode* node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMEVolumeGray));
}


//----------------------------------------------------------------------------
void lhpOpComputeTensor::OpRun()   
//----------------------------------------------------------------------------
{
  // Get the type of the input dataset
  mafVMEVolumeGray* vmeData = mafVMEVolumeGray::SafeDownCast(m_Input) ;
  
  // Look for a dataset containing displacement vectors
  mafVMEVolumeRGB* vmeDisplacements= NULL;
  for (int i=0; i< vmeData->GetNumberOfChildren() && vmeDisplacements == NULL; )
    if (vmeData->GetChild(i)->IsMAFType(mafVMEVolumeRGB))
       vmeDisplacements = mafVMEVolumeRGB::SafeDownCast(vmeData->GetChild(i));
  
  // check the validity of attributes and report error message
  // TODO: use wxWidgets instead of windows message boxes
  if (vmeData == NULL)  
    MessageBox(NULL, "The original dataset is not of the correct type", "OpComputeTensor", MB_OK);
  if (vmeDisplacements == NULL)
    MessageBox(NULL, "Could not find displacement vectors", "OpComputeTensor", MB_OK);

#ifdef UI_TEST
  if (!vmeData)
#else
  if (!vmeData || !vmeDisplacements)
#endif
    {
    mafEventMacro(mafEvent(this,OP_RUN_CANCEL)); // TODO: terminated? or just cancel?
    return;     // TODO: this breaks the selection in the lhpBuilder
    }
    //OpStop() 

  // fill in the data and show the GUI
  m_VmeData = vmeData;
  m_VmeDisplacements = vmeDisplacements;
#ifndef UI_TEST
  ComputeTensors();
#endif
  CreateOpDialog();
  
  int result;
  int ret_dlg = m_Dialog->ShowModal();
  if( ret_dlg == wxID_OK )
  {
    result = OP_RUN_OK;
  }
  else 
  {
    result = OP_RUN_CANCEL;
  }

  DeleteOpDialog();
  mafDEL(m_VmeTensors);

  mafEventMacro(mafEvent(this,result));
}


//----------------------------------------------------------------------------
int lhpOpComputeTensor::ComputeTensors()
//----------------------------------------------------------------------------
{  
  wxBusyInfo wait(_("Caclulating tensors"));

  mafNEW(m_VmeTensors);  
  m_VmeTensors->Update();
  m_VmeTensors->DeepCopy(m_VmeData); // this is the quickest solution, but is it the best?

  // Get the VTK datasets from the VMEs
  vtkImageData *volume = vtkImageData::SafeDownCast(m_VmeData->GetOutput()->GetVTKData());
  vtkRectilinearGrid* vectors = vtkRectilinearGrid::SafeDownCast(m_VmeDisplacements->GetOutput()->GetVTKData());
  if (!volume || !vectors)
    return MAF_ERROR;
  

  // update the data before running the computation
  volume->Update();
  vectors->Update();

  // calculate tensor
  mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(m_VmeTensors->GetDataVector()->GetItem(0));
  m_TensorVolume = vtkImageData::SafeDownCast(item->GetData());
  m_TensorVolume->Update();

  bool bSamplingInGaussPoints = false;
  if (m_InterpolationType == INTERPOLATION_POINT_GAUSS)
    bSamplingInGaussPoints = true;
  ComputeTensor(volume, vectors, m_TensorVolume, bSamplingInGaussPoints);
  m_VmeTensors->Update();
  return MAF_OK;
}


//----------------------------------------------------------------------------
void lhpOpComputeTensor::CreateOutputDataset()
//----------------------------------------------------------------------------
{
  // TODO: update the code
  if (m_bCreateOutput)
  {
    m_Output = m_VmeTensors;
    m_Output->ReparentTo(m_Input);
    const char* originalName = m_Input->GetName();
    const char suffix[11] = " - tensors";
    char* name = new char[strlen(originalName) + strlen(suffix) + 1];
    strcpy(name, originalName);
    strcat(name, suffix);
    m_Output->SetName(name);
    delete[] name;
  }
}


//----------------------------------------------------------------------------
void lhpOpComputeTensor::UpdateScalarsInTensorDataset()
//----------------------------------------------------------------------------
{
  // TODO: We currently take the range from the input dataset and scale the tensor values accordingly. 
  // Is this solution good enough? This also can 'hide' the range of the tensor components
  vtkImageData *volume = vtkImageData::SafeDownCast(m_VmeData->GetOutput()->GetVTKData());
  int min, max;
  GetArrayLimits(volume, min, max);
  TransformComponentToScalars(m_TensorVolume, m_SelectedTensorComponent, 0, max - min);
  m_TensorVolume->Update();
}




//----------------------------------------------------------------------------
//   GUI STUFF
//----------------------------------------------------------------------------

// ID enumerations required for GUI
enum COMPUTE_TENSOR_WIDGET_ID
{
  ID_FIRST = MINID,	
  ID_INTERPOLATION,
  ID_COMPONENT,
  ID_OK,
  ID_CANCEL, 
  ID_RADIOBOX_DATA_RENDERING,
  ID_RADIOBOX_VECTOR_RENDERING,
  ID_RADIOBOX_TENSOR_RENDERING,
  ID_RADIOBOX_TENSOR_COMPONENT_SELECTION,
  ID_SLIDER_ISOSURFACE_VALUE,
  ID_CHECK_SHOW_DATA,
  ID_CHECK_SHOW_DISPLACEMENT_VECTORS,
  ID_CHECK_SHOW_TENSORS, 
  ID_CHECK_CREATE_OUTPUT_DATASET,
  ID_CHECK_FILL_TENSOR_ARRAY,
  ID_CHECK_FILL_SCALAR_ARRAY,
  ID_TEXTBOX_ISOSURFACE_VALUE
};


//----------------------------------------------------------------------------
// tabed gui - now obsolete
void lhpOpComputeTensor::CreateGui() 
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  wxString sampling_choices[2] = {"Centroid","Gauss"};
  m_Gui->Radio(ID_INTERPOLATION,"Interpolation points:",&m_InterpolationType,2,sampling_choices);

  m_Gui->Divider();

  wxString component_choices[9] = {"1","2","3","4","5","6","7","8","9"};
  m_Gui->Radio(ID_COMPONENT,"Selected component for visualization:",&m_SelectedTensorComponent,9,component_choices);


  m_Gui->OkCancel();
  ShowGui();
}

//----------------------------------------------------------------------------
void lhpOpComputeTensor::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {

      case ID_OK:
        m_Dialog->EndModal(wxID_OK);
        break;

      case ID_CANCEL:
        m_Dialog->EndModal(wxID_CANCEL);
        break;

      case ID_RADIOBOX_DATA_RENDERING:
        // TODO: Add handler function
        break;

      case ID_RADIOBOX_VECTOR_RENDERING:
        SetDisplacementVectorsView();
        break;
    
      case ID_RADIOBOX_TENSOR_RENDERING:
        SetTensorView();
        break;

      case ID_RADIOBOX_TENSOR_COMPONENT_SELECTION:
        TensorComponentChange();  // TODO: change implementation
        break;

      case ID_CHECK_SHOW_DISPLACEMENT_VECTORS:
        SetDisplacementVectorsView();
        break;

      case ID_CHECK_SHOW_DATA:
        SetDataView();
        break;

      case ID_CHECK_SHOW_TENSORS:
        SetTensorView();
        break;   

      case ID_SLIDER_ISOSURFACE_VALUE:
        UpdateIsosurface();
        break;

      case ID_CHECK_CREATE_OUTPUT_DATASET:
        // TODO: Add handler function
        break;
  
      case ID_CHECK_FILL_TENSOR_ARRAY:
        // TODO: Add handler function
        break;

      case ID_CHECK_FILL_SCALAR_ARRAY:
        // TODO: Add handler function
        break;

      case wxOK:
        if (ComputeTensors() == MAF_OK)
          OpStop(OP_RUN_OK);
        else
          OpStop(OP_RUN_CANCEL);
        break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);    
        break;
      default:
        mafEventMacro(*maf_event);
    }
  }
}

void lhpOpComputeTensor::SetDisplacementVectorsView()
  {
  if (m_VectorDataVisualizationType == RENDER_AS_HEDGEHOG)
    {
      if (m_VectorHedgehogActor)
        m_VectorHedgehogActor->SetVisibility(m_bShowVectors);
      else
      {
        if (m_VectorGlyphActor)
        {
          m_Rwi->m_RenFront->RemoveActor(m_VectorGlyphActor);
          vtkDEL(m_VectorGlyphActor);
        }
      CreateVectorHedgehogPipeline();
      }

    }
  else if (m_VectorDataVisualizationType == RENDER_AS_GLYPH)
    {
    if (m_VectorGlyphActor)
        m_VectorGlyphActor->SetVisibility(m_bShowVectors);
    else
      {
      if (m_VectorHedgehogActor)
        {
        m_Rwi->m_RenFront->RemoveActor(m_VectorHedgehogActor);
        vtkDEL(m_VectorHedgehogActor);
        }
      CreateVectorGlyphPipeline();
      }
    }
  m_Rwi->CameraUpdate();
  }


void lhpOpComputeTensor::TensorComponentChange()
  {
  if (m_TensorDataVisualizationType != RENDER_AS_VOLUME)
    return;
  UpdateScalarsInTensorDataset();
  m_Rwi->CameraUpdate();
  }


void lhpOpComputeTensor::UpdateIsosurface()
  {
  if (m_InpuDataVisualizationType != RENDER_AS_ISOSURFACE)  
    return;         

  if (m_ContourFilter && m_IsoMapper && m_DataIsosurfaceActor)
    {
    m_ContourFilter->SetValue(0, m_IsosurfaceValue);
    m_ContourFilter->Update();
    m_IsoMapper->Update();
    m_Rwi->CameraUpdate();
    }
  }

void lhpOpComputeTensor::SetTensorView()
  {
  if (m_TensorDataVisualizationType == RENDER_AS_ELLIPSOIDS)
    {
    if (m_TensorGlyphActor)
      m_TensorGlyphActor->SetVisibility(m_bShowTensors);
    else
      {
      if (m_TensorVolumeActor)
        {
        m_Rwi->m_RenFront->RemoveActor(m_TensorVolumeActor);
        vtkDEL(m_TensorVolumeActor);
        }
      CreateTensorGlyphPipeline();     
      }
    }
  else if (m_TensorDataVisualizationType == RENDER_AS_VOLUME)
    {
    if (m_TensorVolumeActor) 
      m_TensorVolumeActor->SetVisibility(m_bShowTensors);
    else
      {
      if (m_TensorGlyphActor)
        {
        m_Rwi->m_RenFront->RemoveActor(m_TensorGlyphActor);
        vtkDEL(m_TensorGlyphActor);
        }
      CreateTensorVolumePipeline();
      }
    }
  m_Rwi->CameraUpdate();
  }


void lhpOpComputeTensor::SetDataView()
  {
  // TODO: Update code when we add the volume actor for the input data
  if (m_DataIsosurfaceActor)
    m_DataIsosurfaceActor->SetVisibility(m_bShowInputDataset);
  m_Rwi->CameraUpdate();
  }




void lhpOpComputeTensor::CreateOpDialog()
{
  m_Dialog = new mafGUIDialog("Tensor viewer", mafCLOSEWINDOW | mafRESIZABLE);

  m_Rwi = new mafRWI(m_Dialog,ONE_LAYER,false);
  m_Rwi->SetListener(this);
  m_Rwi->CameraSet(CAMERA_PERSPECTIVE);

  m_Rwi->m_RenderWindow->SetDesiredUpdateRate(0.0001f);
  m_Rwi->SetSize(0,0,400,400);
  m_Rwi->Show(true);
  m_Rwi->m_RwiBase->SetMouse(m_Mouse) ;

#ifndef UI_TEST
  UpdateScalarsInTensorDataset();
  CreateVisualPipes();
#endif
  m_Dialog->SetSizeHints( wxDefaultSize, wxDefaultSize );


  //The following code was originally generated using wxFormBuilder
  //and modified here to work with MAF

#pragma region //wxFormBuilder Component Construction
  m_Dialog->SetSizeHints( wxDefaultSize, wxDefaultSize );

  wxBoxSizer* h_sizer_all;
  h_sizer_all = new wxBoxSizer( wxHORIZONTAL );

  wxBoxSizer* v_sizer_renderWindow;
  v_sizer_renderWindow = new wxBoxSizer( wxVERTICAL );

  h_sizer_all->Add( v_sizer_renderWindow, 3, wxEXPAND, 5 );

  wxBoxSizer* v_sizer_interface;
  v_sizer_interface = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* bSizerDataBox;
  bSizerDataBox = new wxBoxSizer( wxVERTICAL );

  m_staticTextData = new wxStaticText( m_Dialog, wxID_ANY, wxT("Data"), wxDefaultPosition, wxDefaultSize, 0 );
  m_staticTextData->Wrap( -1 );
  m_staticTextData->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );

  bSizerDataBox->Add( m_staticTextData, 0, wxALL, 5 );

  m_checkBoxShowData = new wxCheckBox( m_Dialog, ID_CHECK_SHOW_DATA, wxT("Show data"), wxDefaultPosition, wxDefaultSize, 0 );
  m_checkBoxShowData->SetValue(true);

  bSizerDataBox->Add( m_checkBoxShowData, 0, wxALL, 5 );

  wxString m_radioBoxDataRenderChoices[] = { wxT("as volume"), wxT("as surface") };
  int m_radioBoxDataRenderNChoices = sizeof( m_radioBoxDataRenderChoices ) / sizeof( wxString );
  m_radioBoxDataRender = new wxRadioBox( m_Dialog, ID_RADIOBOX_DATA_RENDERING, wxT("Render"), wxDefaultPosition, wxDefaultSize, m_radioBoxDataRenderNChoices, m_radioBoxDataRenderChoices, 1, wxRA_SPECIFY_ROWS );
  m_radioBoxDataRender->SetSelection( 1 );
  m_radioBoxDataRender->Enable( false );

  bSizerDataBox->Add( m_radioBoxDataRender, 0, wxALL, 5 );

  wxBoxSizer* bSizer12;
  bSizer12 = new wxBoxSizer( wxHORIZONTAL );

  m_staticTextIsosurfaceValue = new wxStaticText( m_Dialog, wxID_ANY, wxT("Isosurface value"), wxDefaultPosition, wxDefaultSize, 0 );
  m_staticTextIsosurfaceValue->Wrap( -1 );
  bSizer12->Add( m_staticTextIsosurfaceValue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

  m_textCtrlIsosurfaceValue = new wxTextCtrl( m_Dialog, ID_TEXTBOX_ISOSURFACE_VALUE, wxT("0"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxNO_BORDER );
  bSizer12->Add( m_textCtrlIsosurfaceValue, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

  bSizerDataBox->Add( bSizer12, 1, wxEXPAND, 5 );

  m_sliderIsosurfaceValue = new wxSlider( m_Dialog, ID_SLIDER_ISOSURFACE_VALUE, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_HORIZONTAL|wxSL_LABELS );
  bSizerDataBox->Add( m_sliderIsosurfaceValue, 0, wxALL|wxEXPAND, 5 );

  v_sizer_interface->Add( bSizerDataBox, 4, wxEXPAND, 5 );

  m_staticline1 = new wxStaticLine( m_Dialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  v_sizer_interface->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

  wxBoxSizer* bSizerVectorBox;
  bSizerVectorBox = new wxBoxSizer( wxVERTICAL );

  m_staticTextVectors = new wxStaticText( m_Dialog, wxID_ANY, wxT("Displacement vectors"), wxDefaultPosition, wxDefaultSize, 0 );
  m_staticTextVectors->Wrap( -1 );
  m_staticTextVectors->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );

  bSizerVectorBox->Add( m_staticTextVectors, 0, wxALL, 5 );

  m_checkBoxShowVectors = new wxCheckBox( m_Dialog, ID_CHECK_SHOW_DISPLACEMENT_VECTORS, wxT("Show displacement vectors"), wxDefaultPosition, wxDefaultSize, 0 );

  bSizerVectorBox->Add( m_checkBoxShowVectors, 0, wxALL, 5 );

  wxString m_radioBoxVectorRenderChoices[] = { wxT("as lines"), wxT("as cones") };
  int m_radioBoxVectorRenderNChoices = sizeof( m_radioBoxVectorRenderChoices ) / sizeof( wxString );
  m_radioBoxVectorRender = new wxRadioBox( m_Dialog, ID_RADIOBOX_VECTOR_RENDERING, wxT("Render"), wxDefaultPosition, wxDefaultSize, m_radioBoxVectorRenderNChoices, m_radioBoxVectorRenderChoices, 1, wxRA_SPECIFY_ROWS );
  m_radioBoxVectorRender->SetSelection( 0 );

  bSizerVectorBox->Add( m_radioBoxVectorRender, 0, wxALL, 5 );

  v_sizer_interface->Add( bSizerVectorBox, 2, wxEXPAND, 5 );

  m_staticline2 = new wxStaticLine( m_Dialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  v_sizer_interface->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

  wxBoxSizer* bSizerTensorBox;
  bSizerTensorBox = new wxBoxSizer( wxVERTICAL );

  m_staticTextTensors = new wxStaticText( m_Dialog, wxID_ANY, wxT("Tensors"), wxDefaultPosition, wxDefaultSize, 0 );
  m_staticTextTensors->Wrap( -1 );
  m_staticTextTensors->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );

  bSizerTensorBox->Add( m_staticTextTensors, 0, wxALL, 5 );

  m_checkBoxShowTensors = new wxCheckBox( m_Dialog, ID_CHECK_SHOW_TENSORS, wxT("Show tensor data"), wxDefaultPosition, wxDefaultSize, 0 );
  m_checkBoxShowTensors->SetValue(true);

  bSizerTensorBox->Add( m_checkBoxShowTensors, 0, wxALL, 5 );

  wxString m_radioBoxTensorRenderChoices[] = { wxT("as volume"), wxT("as ellipsoids") };
  int m_radioBoxTensorRenderNChoices = sizeof( m_radioBoxTensorRenderChoices ) / sizeof( wxString );
  m_radioBoxTensorRender = new wxRadioBox( m_Dialog, ID_RADIOBOX_TENSOR_RENDERING, wxT("Render"), wxDefaultPosition, wxDefaultSize, m_radioBoxTensorRenderNChoices, m_radioBoxTensorRenderChoices, 1, wxRA_SPECIFY_ROWS );
  m_radioBoxTensorRender->SetSelection( 0 );
  bSizerTensorBox->Add( m_radioBoxTensorRender, 0, wxALL, 5 );

  wxString m_radioBoxTensorComponentChoices[] = { wxT("Exx"), wxT("Exy"), wxT("Exz"), wxT("Eyx"), wxT("Eyy"), wxT("Eyz"), wxT("Ezx"), wxT("Ezy"), wxT("Ezz") };
  int m_radioBoxTensorComponentNChoices = sizeof( m_radioBoxTensorComponentChoices ) / sizeof( wxString );
  m_radioBoxTensorComponent = new wxRadioBox( m_Dialog, ID_RADIOBOX_TENSOR_COMPONENT_SELECTION, wxT("Selected component:"), wxDefaultPosition, wxDefaultSize, m_radioBoxTensorComponentNChoices, m_radioBoxTensorComponentChoices, 3, wxRA_SPECIFY_COLS );
  m_radioBoxTensorComponent->SetSelection( 0 );
  bSizerTensorBox->Add( m_radioBoxTensorComponent, 0, wxALL, 5 );

  v_sizer_interface->Add( bSizerTensorBox, 4, wxEXPAND, 5 );

  m_staticline3 = new wxStaticLine( m_Dialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
  v_sizer_interface->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );

  wxBoxSizer* bSizerOutputBox;
  bSizerOutputBox = new wxBoxSizer( wxVERTICAL );

  m_staticTextOutput = new wxStaticText( m_Dialog, wxID_ANY, wxT("Output"), wxDefaultPosition, wxDefaultSize, 0 );
  m_staticTextOutput->Wrap( -1 );
  m_staticTextOutput->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
  m_staticTextOutput->Enable(false);

  bSizerOutputBox->Add( m_staticTextOutput, 0, wxALL, 5 );

  m_checkBoxCreateOutput = new wxCheckBox( m_Dialog, ID_CHECK_CREATE_OUTPUT_DATASET, wxT("Create output volume"), wxDefaultPosition, wxDefaultSize, 0 );

  m_checkBoxCreateOutput->Enable( false );

  bSizerOutputBox->Add( m_checkBoxCreateOutput, 0, wxALL, 5 );

  m_checkBoxFillTensorArray = new wxCheckBox( m_Dialog, ID_CHECK_FILL_TENSOR_ARRAY, wxT("Tensor array"), wxDefaultPosition, wxDefaultSize, 0 );

  m_checkBoxFillTensorArray->Enable( false );

  bSizerOutputBox->Add( m_checkBoxFillTensorArray, 0, wxALL, 5 );

  m_checkBoxFillScalarArray = new wxCheckBox( m_Dialog, ID_CHECK_FILL_SCALAR_ARRAY, wxT("Scalar array"), wxDefaultPosition, wxDefaultSize, 0 );

  m_checkBoxFillScalarArray->Enable( false );

  bSizerOutputBox->Add( m_checkBoxFillScalarArray, 0, wxALL, 5 );

  v_sizer_interface->Add( bSizerOutputBox, 2, wxEXPAND, 5 );

  wxBoxSizer* bSizerOKCancel;
  bSizerOKCancel = new wxBoxSizer( wxHORIZONTAL );

  m_buttonOK = new wxButton( m_Dialog, ID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizerOKCancel->Add( m_buttonOK, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxALL, 5 );

  m_buttonCancel = new wxButton( m_Dialog, ID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizerOKCancel->Add( m_buttonCancel, 0, wxALIGN_BOTTOM|wxALL|wxRIGHT, 5 );

  v_sizer_interface->Add( bSizerOKCancel, 1, wxALIGN_RIGHT|wxEXPAND, 5 );

  h_sizer_all->Add( v_sizer_interface, 1, wxEXPAND, 5 );


#pragma endregion //wxFormBuilder

  // connect render window to the dialog
  v_sizer_renderWindow->Add( m_Rwi->m_RwiBase, 1, wxEXPAND | wxALL, 5 );

  // set the slider values
  vtkImageData *volume = vtkImageData::SafeDownCast(m_VmeData->GetOutput()->GetVTKData());
  double sr[2];
  volume->GetScalarRange(sr);
  m_sliderIsosurfaceValue->SetMin(sr[0]);
  m_sliderIsosurfaceValue->SetMax(sr[1]);
  m_sliderIsosurfaceValue->SetValue(m_IsosurfaceValue);

  // validate components
  m_buttonOK->SetValidator(mafGUIValidator(this,ID_OK,m_buttonOK));
  m_buttonCancel->SetValidator(mafGUIValidator(this,ID_CANCEL,m_buttonCancel));

  m_radioBoxDataRender->SetValidator(mafGUIValidator(this, ID_RADIOBOX_DATA_RENDERING, m_radioBoxDataRender, &m_InpuDataVisualizationType));
  m_radioBoxVectorRender->SetValidator(mafGUIValidator(this,ID_RADIOBOX_VECTOR_RENDERING, m_radioBoxVectorRender, &m_VectorDataVisualizationType));
  m_radioBoxTensorRender->SetValidator(mafGUIValidator(this, ID_RADIOBOX_TENSOR_RENDERING, m_radioBoxTensorRender, &m_TensorDataVisualizationType));
  m_radioBoxTensorComponent->SetValidator(mafGUIValidator(this, ID_RADIOBOX_TENSOR_COMPONENT_SELECTION, m_radioBoxTensorComponent, &m_SelectedTensorComponent));

  m_sliderIsosurfaceValue->SetValidator(mafGUIValidator(this, ID_SLIDER_ISOSURFACE_VALUE, m_sliderIsosurfaceValue, &m_IsosurfaceValue, m_textCtrlIsosurfaceValue));

  m_checkBoxShowData->SetValidator(mafGUIValidator(this, ID_CHECK_SHOW_DATA, m_checkBoxShowData, &m_bShowInputDataset));
  m_checkBoxShowVectors->SetValidator(mafGUIValidator(this, ID_CHECK_SHOW_DISPLACEMENT_VECTORS, m_checkBoxShowVectors, &m_bShowVectors));
  m_checkBoxShowTensors->SetValidator(mafGUIValidator(this, ID_CHECK_SHOW_TENSORS, m_checkBoxShowTensors, &m_bShowTensors));
  m_checkBoxCreateOutput->SetValidator(mafGUIValidator(this, ID_CHECK_CREATE_OUTPUT_DATASET, m_checkBoxCreateOutput, &m_bCreateOutput));  
  m_checkBoxFillScalarArray->SetValidator(mafGUIValidator(this, ID_CHECK_FILL_SCALAR_ARRAY, m_checkBoxFillScalarArray, &m_bAddScalarsToOutput));  
  m_checkBoxFillTensorArray->SetValidator(mafGUIValidator(this, ID_CHECK_FILL_TENSOR_ARRAY, m_checkBoxFillTensorArray, &m_bAddTensorsToOutput));


  // set position of dialog
  m_Dialog->Add(h_sizer_all, 1, wxEXPAND);
  m_Dialog->SetPosition(wxPoint(20,20)) ;

  m_Rwi->CameraUpdate();
}





//----------------------------------------------------------------------------
void lhpOpComputeTensor::DeleteOpDialog() 
//----------------------------------------------------------------------------
{
  cppDEL(m_Rwi); 
  cppDEL(m_Dialog);

  // delete vtk components

  //vtkDEL(m_TensorVolume);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_DataVolume);
  vtkDEL(m_DataIsosurfaceActor);
  vtkDEL(m_ContourFilter);
  vtkDEL(m_IsoMapper);

  vtkDEL(m_DataVolumeActor);
  vtkDEL(m_TensorVolumeActor);
  vtkDEL(m_TensorGlyphActor);
  vtkDEL(m_VectorGlyphActor);
  vtkDEL(m_VectorHedgehogActor);


}



//----------------------------------------------------------------------------
// Add new vme to scene - TODO: fix implementation or change the name of the function
void lhpOpComputeTensor::CreateVisualPipes()
//----------------------------------------------------------------------------
{
  CreateOutlinePipeline();
  CreateDataIsosurfacePipeline();
  CreateTensorVolumePipeline();
  //CreateVectorHedgehogPipeline();
  //CreateTensorGlyphPipeline();

}


//----------------------------------------------------------------------------
// Create outline of the dataset and set the camera 
void lhpOpComputeTensor::CreateOutlinePipeline()
//----------------------------------------------------------------------------
{
  vtkImageData *volume = vtkImageData::SafeDownCast(m_VmeData->GetOutput()->GetVTKData());
  // bounding box actor
  vtkOutlineCornerFilter* OutlineFilter = vtkOutlineCornerFilter::New();
  OutlineFilter->SetInput(volume);

  vtkPolyDataMapper* OutlineMapper = vtkPolyDataMapper::New();
  OutlineMapper->SetInput(OutlineFilter->GetOutput());

  m_OutlineBox = vtkActor::New();
  m_OutlineBox->SetMapper(OutlineMapper);
  m_OutlineBox->VisibilityOn();
  m_OutlineBox->PickableOff();
  m_OutlineBox->GetProperty()->SetColor(0,0,0.8);
  m_OutlineBox->GetProperty()->SetAmbient(1);
  m_OutlineBox->GetProperty()->SetRepresentationToWireframe();
  m_OutlineBox->GetProperty()->SetInterpolationToFlat();
  m_Rwi->m_RenFront->AddActor(m_OutlineBox);

  double m_BoundingBox[6];
  m_OutlineBox->GetBounds(m_BoundingBox);
  m_Rwi->SetGridPosition(m_BoundingBox[4]);
  m_Rwi->m_RenFront->ResetCamera(m_BoundingBox);
  m_Rwi->m_Camera->Dolly(1.2);
  m_Rwi->m_RenFront->ResetCameraClippingRange();

  vtkDEL(OutlineMapper);
  vtkDEL(OutlineFilter);
}


//----------------------------------------------------------------------------
// Create pipeline for rendering volumes
void lhpOpComputeTensor::CreateDataIsosurfacePipeline()
//----------------------------------------------------------------------------
{
  vtkImageData *volume = vtkImageData::SafeDownCast(m_VmeData->GetOutput()->GetVTKData());
  
  // temporarily use contour filter, because it requires less code
  m_ContourFilter = vtkContourFilter::New();
  m_ContourFilter->SetInput(volume);
  m_ContourFilter->ComputeNormalsOn();
  m_ContourFilter->ComputeGradientsOff();
  m_ContourFilter->ComputeScalarsOn();
  m_ContourFilter->SetValue(0, m_IsosurfaceValue);

  m_IsoMapper = vtkPolyDataMapper::New();
  m_IsoMapper->SetInput(m_ContourFilter->GetOutput());  
  m_IsoMapper->Update();

  m_DataIsosurfaceActor = vtkActor::New();
  m_DataIsosurfaceActor->SetMapper(m_IsoMapper);
  m_Rwi->m_RenFront->AddActor(m_DataIsosurfaceActor);

  }



// TODO: decide if we are going to use vtkXRayVolumeMapper (supports vtkIntArray) 
// or vtkVolumeTextureMapper2D (supports vtkUnsignedChar && vtkUnsignedShort) 
// - requires additional changed copy of data (vtkUnsignedShort shifted to have minimum value at 0?)
void lhpOpComputeTensor::CreateTensorVolumePipeline()
{
  vtkImageData *volume = m_TensorVolume; 
  double sr[2];
  volume->GetScalarRange(sr);
  
  // create transfer function for mapping scalar value to opacity
  vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
  opacityTransferFunction->AddPoint(sr[0],0.0);
  opacityTransferFunction->AddPoint(sr[1]/2.0, 1.0);

  // create transfer function for mapping scalar value to color
  vtkColorTransferFunction *colorTransferFunction = vtkColorTransferFunction::New();
  double range = sr[1] - sr[0];
  colorTransferFunction->AddRGBPoint( sr[0], 0.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint( sr[0] + range / 5.0 * 1, 0.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint( sr[0] + range / 5.0 * 2, 1.0, 0.0, 0.0);
  colorTransferFunction->AddRGBPoint( sr[0] + range /5.0 * 3, 0.0, 0.0, 1.0);
  colorTransferFunction->AddRGBPoint( sr[0] + range /5.0 * 4, 0.0, 1.0, 0.0);
  colorTransferFunction->AddRGBPoint( sr[1], 0.0, 0.2, 0.0);

  // the property describes how the data will look
  vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
  volumeProperty->SetColor(colorTransferFunction);
  volumeProperty->SetScalarOpacity(opacityTransferFunction);
  volumeProperty->SetInterpolationTypeToNearest();
 

  // the mapper and ray casting function
  /*vtkVolumeRayCastCompositeFunction *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
  vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
  volumeMapper->SetVolumeRayCastFunction(compositeFunction);
  volumeMapper->SetInput(volume);
  compositeFunction->Delete();*/
  vtkVolumeTextureMapper2D *volumeMapper = vtkVolumeTextureMapper2D::New();
  //vtkMAFVolumeTextureMapper2D *volumeMapper = vtkMAFVolumeTextureMapper2D::New();
  volumeMapper->SetInput(volume);

  // vtkXRayVolumeMapper works!
  /*vtkXRayVolumeMapper *volumeMapper = vtkXRayVolumeMapper::New();
  volumeMapper->SetInput(volume);*/

  // create the 'actor' and the property
  m_TensorVolumeActor = vtkVolume::New();
  m_TensorVolumeActor->SetMapper(volumeMapper);
  m_TensorVolumeActor->SetProperty(volumeProperty);
  m_TensorVolumeActor->SetVisibility(m_bShowTensors);

  // render
  m_Rwi->m_RenFront->AddVolume(m_TensorVolumeActor);

  // cleanup
  vtkDEL(colorTransferFunction);
  vtkDEL(opacityTransferFunction);
  vtkDEL(volumeProperty);
  vtkDEL(volumeMapper);
}


// TODO: description
void lhpOpComputeTensor::CreateVectorHedgehogPipeline()
{
  vtkRectilinearGrid *vectors = vtkRectilinearGrid::SafeDownCast(m_VmeDisplacements->GetOutput()->GetVTKData());
   
  // create vector as a hedgehog (set of lines)
  vtkHedgeHog *hedgehog = vtkHedgeHog::New();
  hedgehog->SetInput(vectors);
  hedgehog->SetVectorModeToUseVector();
  hedgehog->SetScaleFactor(1.0);
  hedgehog->Update();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(hedgehog->GetOutput());
  mapper->Update();
  
  m_VectorHedgehogActor = vtkActor::New();
  m_VectorHedgehogActor->SetMapper(mapper);
  m_VectorHedgehogActor->SetVisibility(m_bShowVectors);
  m_Rwi->m_RenFront->AddActor(m_VectorHedgehogActor); 

  // cleanup
  hedgehog->Delete(); 
  mapper->Delete();
}

// TODO: description
void lhpOpComputeTensor::CreateVectorGlyphPipeline()
{
  vtkRectilinearGrid *vectors = vtkRectilinearGrid::SafeDownCast(m_VmeDisplacements->GetOutput()->GetVTKData());

  vtkConeSource *cone = vtkConeSource::New();
  cone->SetResolution(3);
  cone->SetHeight(4);

  vtkGlyph3D  *glyph = vtkGlyph3D::New();
  glyph->SetInput(vectors);
  glyph->SetSource(cone->GetOutput());
  glyph->SetVectorModeToUseVector();
  glyph->SetScaleModeToScaleByVector();
  glyph->SetScaleFactor(1.0);
  glyph->Update();

  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
  mapper->SetInput(glyph->GetOutput());
  mapper->Update();

  m_VectorGlyphActor = vtkActor::New();
  m_VectorGlyphActor->SetMapper(mapper);
  m_VectorGlyphActor->SetVisibility(m_bShowVectors);

  m_Rwi->m_RenFront->AddActor(m_VectorGlyphActor); 

  // cleanup
  mapper->Delete();
  glyph->Delete();
  cone->Delete();
}





// TODO: description
void lhpOpComputeTensor::CreateTensorGlyphPipeline()
{
  vtkImageData* volume = m_TensorVolume;

  vtkSphereSource* sphere = vtkSphereSource::New();
  sphere->SetThetaResolution(3);
  sphere->SetPhiResolution(3);
  sphere->GetOutput()->Update();
  
  vtkTensorGlyph* ellipsoids = vtkTensorGlyph::New();
  ellipsoids->SetInput(volume);         // here we should input our tensors
  ellipsoids->SetSource(sphere->GetOutput());
  ellipsoids->SetScaleFactor(1000.0);
  ellipsoids->ClampScalingOff();

  // normal calculation is quite slooow ...
  /*vtkPolyDataNormals *ellipsoidNormals = vtkPolyDataNormals::New();
  ellipsoidNormals->SetInput(ellipsoids->GetOutput());*/

  // test count the number of polygons
  ellipsoids->GetOutput()->Update();


  vtkPolyDataMapper* ellipsoidMapper = vtkPolyDataMapper::New(); 
  //ellipsoidMapper->SetInput(ellipsoidNormals->GetOutput());
  ellipsoidMapper->SetInput(ellipsoids->GetOutput());

  m_TensorGlyphActor = vtkActor::New();
  m_TensorGlyphActor->SetMapper(ellipsoidMapper);
  m_TensorGlyphActor->SetVisibility(m_bShowTensors);  

  m_Rwi->m_RenFront->AddActor(m_TensorGlyphActor);


  // cleanup
  ellipsoidMapper->Delete();
  ellipsoids->Delete();
  sphere->Delete();
}
