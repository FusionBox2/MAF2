#include "ftk/Operations/TagHandlerFactory.h"

#include "lhpTagHandlerContainer.h"
#include "lhpTagHandlerDICOMContainer.h"

int TagHandlerFactory::Initialize()
{
  //lhpPlugTagHandlerMacro(lhpTagHandler,"General tag handler");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_DictionaryURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_DictionaryVersion, "");
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
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_Procedural, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_Procedural_VMElinkURI1, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Operation, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_CreationDate, "")
    lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Application, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_IsNatural, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_OperatorID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Parameters, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_Ownership, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_Ownership_OwnerID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Operation, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_ModifyDate, "")
    lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Application, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_OperatorID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Parameters, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_Ownership, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Traceability_Ownership_OwnerID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryVersion, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MicroCTSource, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Ownership_OwnerID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1_URL, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1_Score, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1_ScoreDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_Access_Policy1_GroupID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_Access_Policy1_Price, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_Access_Policy1_Usage, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_Access_Publishing_PublishingStatus, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryVersion, "");


  //DICOM
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryVersion, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Modality, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Manufacturer, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_InstitutionName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StationName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ManufacturerModelName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientSex, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ScanOptions, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_KVP, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DataCollectionDiameter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ReconstructionDiameter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToDetector, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToPatient, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_GantryDetectorTilt, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_TableHeight, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RotationDirection, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ExposureTime, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_XRayTubeCurrent, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Exposure, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FilterType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FocalSpot, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ConvolutionKernel, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientPosition, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ImagePositionPatient, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelSpacing, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelPaddingValue, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowCenter, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowWidth, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleIntercept, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleSlope, "");
  // END DICOM

  //MA
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryVersion, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_General_SamplingFrequency, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DataType_IsLandmark_NumberOfLandmarks, "");


  //END MA

  lhpPlugTagHandlerMacro(L0000_resource_Documentation, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Policy1_GroupID, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Policy1_Price, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Policy1_Usage, "");
  lhpPlugTagHandlerMacro(L0000_resource_Access_Publishing_PublishingStatus, "");

  return MAF_OK;
}

TagHandlerCreateType TagHandlerFactory::CreateTagHandler(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_CreateObject();
  }
  return nullptr;
}

const char* TagHandlerFactory::GetDescription(const char* ClassName)
{
  auto pos = GetFactory().m_creatorsMap.find(ClassName);
  if (pos != GetFactory().m_creatorsMap.end())
  {
    return pos->second.m_Description.c_str();
  }
  return nullptr;
}

const std::vector<std::string>& TagHandlerFactory::GetTagHandlerNames()
{
  return GetFactory().m_TagHandlerNames;
}

void TagHandlerFactory::RegisterNewTagHandler(const char* ClassName, const char* description, CreateTagHandlerFunction createFunction)
{
  GetFactory().m_TagHandlerNames.push_back(ClassName);
  CreateInformation info;
  info.m_Description = description;
  info.m_CreateObject = createFunction;
  GetFactory().m_creatorsMap[ClassName] = info;
}

TagHandlerFactory& TagHandlerFactory::GetFactory()
{
  static TagHandlerFactory instance;
  return instance;
}
