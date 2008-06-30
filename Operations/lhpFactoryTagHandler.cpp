/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpFactoryTagHandler.cpp,v $
  Language:  C++
  Date:      $Date: 2008-06-30 09:16:48 $
  Version:   $Revision: 1.7 $
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
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_Field, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_Dimension, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_VolumeType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_DataType_Timevarying, "Time varying VME or static VME");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_FileSize, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_EntityCount, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_DatasetURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Quality, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource, "");


  //DICOM
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type, ""); 
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StudyDate, ""); 
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_Modality, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_Manufacturer, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_InstitutionName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StationName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_ManufacturerModelName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_PatientID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_PatientSex, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_ScanOptions, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_KVP, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_DataCollectionDiameter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_ReconstructionDiameter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_DistanceSourceToDetector, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_DistanceSourceToPatient, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_GantryDetectorTilt, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_TableHeight, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_RotationDirection, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_ExposureTime, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_XRayTubeCurrent, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_Exposure, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_FilterType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_FocalSpot, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_ConvolutionKernel, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_PatientPosition, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_StudyID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_ImagePositionPatient, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_PixelSpacing, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_PixelPaddingValue, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_WindowCenter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_WindowWidth, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_RescaleIntercept, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir_Type_RescaleSlope, "");
  // END DICOM

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
