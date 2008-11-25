/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpFactoryTagHandler.cpp,v $
  Language:  C++
  Date:      $Date: 2008-11-25 08:17:01 $
  Version:   $Revision: 1.14.2.3 $
  Authors:   Stefano Perticoni
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


#include "lhpFactoryTagHandler.h"
#include "mafVersion.h"
#include "mafIndent.h"

#include <string>
#include <ostream>

// #include "lhpTagHandler.h"  to be defined in a separate file...
// #include "lhpDefaultTagHandler.h..." to be defined in a separate file...

lhpFactoryTagHandler *lhpFactoryTagHandler::m_Instance=NULL;

mafCxxTypeMacro(lhpFactoryTagHandler);

//------------------------------------------------------------------------
lhpFactoryTagHandler::lhpFactoryTagHandler()
//------------------------------------------------------------------------------
{
  //lhpPlugTagHandlerMacro(lhpTagHandler,"General tag handler");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_Field, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_Dimension, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_VolumeType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_Timevarying, "Time varying VME or static VME");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_FileSize, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_EntityCount, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_DatasetURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_VmeType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_Procedural_VMElinkURI1, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Operation, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_CreationDate, "")
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_OperatorID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Parameters, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MicroCTSource, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Ownership_OwnerID, "");


  //DICOM
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_StudyDate, ""); 
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Modality, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Manufacturer, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_InstitutionName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_StationName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_ManufacturerModelName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_PatientID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_PatientSex, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_ScanOptions, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_KVP, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DataCollectionDiameter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_ReconstructionDiameter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DistanceSourceToDetector, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DistanceSourceToPatient, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_GantryDetectorTilt, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_TableHeight, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_RotationDirection, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_ExposureTime, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_XRayTubeCurrent, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Exposure, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_FilterType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_FocalSpot, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_ConvolutionKernel, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_PatientPosition, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_StudyID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_ImagePositionPatient, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_PixelSpacing, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_PixelPaddingValue, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_WindowCenter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_WindowWidth, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_RescaleIntercept, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_RescaleSlope, "");
  // END DICOM

  //MA
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource_General_SamplingFrequency, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource_DataType_IsLandmark_NumberOfLandmarks, "");
  

  //END MA

  lhpPlugTagHandlerMacro(L0000_resource_Documentation, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Policy1_GroupID, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Policy1_Price, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Policy1_Usage, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Publishing_PublishingStatus, "");
}

std::vector<std::string> lhpFactoryTagHandler::m_TagHandlerNames;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int lhpFactoryTagHandler::Initialize()
//----------------------------------------------------------------------------
{
  if (m_Instance==NULL)
  {
    m_Instance=lhpFactoryTagHandler::New();

    if (m_Instance)
    {
      m_Instance->RegisterFactory(m_Instance);
      return MAF_OK;  
    }
    else
    {
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}


//------------------------------------------------------------------------------
const char* lhpFactoryTagHandler::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  // to be defined LHP_SOURCE_VERSION...
  // return LHP_SOURCE_VERSION;
  // return "LHP_SOURCE_VERSION_UNDEFINED!!! Fix LHP_SOURCE_VERSION";
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* lhpFactoryTagHandler::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for LHDL tags handlers";
}

//------------------------------------------------------------------------------
lhpTagHandler *lhpFactoryTagHandler::CreateTagHandlerInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return lhpTagHandler::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void lhpFactoryTagHandler::RegisterNewTagHandler(const char* tagHandlerName, const char* description, mafCreateObjectFunction createFunction)
//------------------------------------------------------------------------------
{
  m_TagHandlerNames.push_back(tagHandlerName);
  RegisterNewObject(tagHandlerName,description,createFunction);
}
