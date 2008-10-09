/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.h,v $
  Language:  C++
  Date:      $Date: 2008-10-09 08:53:24 $
  Version:   $Revision: 1.10 $
  Authors:   Stefano Perticoni - Daniele Giunchi - Roberto Mucci
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpTagHandlerContainer_h
#define __lhpTagHandlerContainer_h

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------

#include "lhpTagHandler.h"

class lhpTagHandler_L0000_resource_data_DataType_Field: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Field,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_Field();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};


class lhpTagHandler_L0000_resource_data_DataType_Dimension: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Dimension,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_Dimension();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_DataType_VolumeType: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_VolumeType,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_VolumeType();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};
 
class lhpTagHandler_L0000_resource_data_DataType_Timevarying: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Timevarying,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_Timevarying();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Size_FileSize: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Size_FileSize,mafObject);

    lhpTagHandler_L0000_resource_data_Size_FileSize();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Size_EntityCount: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Size_EntityCount,mafObject);

    lhpTagHandler_L0000_resource_data_Size_EntityCount();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Size_TimeFramesCount: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount,mafObject);

    lhpTagHandler_L0000_resource_data_Size_TimeFramesCount();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Dataset_DatasetURI: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_DatasetURI,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_DatasetURI();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Dataset_UploadDate: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_UploadDate();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum,mafObject);

  lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};


class lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_MAF_VmeType: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_MAF_VmeType,mafObject);

  lhpTagHandler_L0000_resource_MAF_VmeType();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose,mafObject);

    lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector,mafObject);

    lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_MAF_Procedural_VmeLinkURI1: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_MAF_Procedural_VmeLinkURI1,mafObject);

  lhpTagHandler_L0000_resource_MAF_Procedural_VmeLinkURI1();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application,mafObject);

  lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural,mafObject);

  lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Tracebility_Ownership: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership,mafObject);

    lhpTagHandler_L0000_resource_data_Tracebility_Ownership();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID,mafObject);

    lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Source_DicomSource: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_MASource: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MASource,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MASource();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class L0000_resource_Documentation: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Documentation,mafObject);

  L0000_resource_Documentation();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};


class L0000_resource_Access_Policy1_GroupID: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Policy1_GroupID,mafObject);

  L0000_resource_Access_Policy1_GroupID();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};


class L0000_resource_Access_Policy1_Price: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Policy1_Price,mafObject);

  L0000_resource_Access_Policy1_Price();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};


class L0000_resource_Access_Policy1_Usage: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Policy1_Usage,mafObject);

  L0000_resource_Access_Policy1_Usage();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class L0000_resource_Access_Publishing_PublishingStatus: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Publishing_PublishingStatus,mafObject);

  L0000_resource_Access_Publishing_PublishingStatus();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyDate: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyDate,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyDate();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Modality: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Modality,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Modality();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Manufacturer: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Manufacturer,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Manufacturer();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_InstitutionName: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_InstitutionName,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_InstitutionName();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StationName: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StationName,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StationName();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ManufacturerModelName: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ManufacturerModelName,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ManufacturerModelName();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientID,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientID();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientSex: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientSex,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientSex();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ScanOptions: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ScanOptions,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ScanOptions();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_KVP: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_KVP,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_KVP();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DataCollectionDiameter: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DataCollectionDiameter,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DataCollectionDiameter();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ReconstructionDiameter: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ReconstructionDiameter,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ReconstructionDiameter();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToDetector: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToDetector,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToDetector();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToPatient: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToPatient,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToPatient();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_GantryDetectorTilt: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_GantryDetectorTilt,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_GantryDetectorTilt();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_TableHeight: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_TableHeight,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_TableHeight();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RotationDirection: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RotationDirection,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RotationDirection();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ExposureTime: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ExposureTime,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ExposureTime();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_XRayTubeCurrent: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_XRayTubeCurrent,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_XRayTubeCurrent();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Exposure: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Exposure,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Exposure();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FilterType: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FilterType,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FilterType();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FocalSpot: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FocalSpot,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FocalSpot();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ConvolutionKernel: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ConvolutionKernel,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ConvolutionKernel();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientPosition: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientPosition,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientPosition();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyID,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyID();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ImagePositionPatient: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ImagePositionPatient,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ImagePositionPatient();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelSpacing: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelSpacing,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelSpacing();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelPaddingValue: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelPaddingValue,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelPaddingValue();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowCenter: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowCenter,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowCenter();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowWidth: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowWidth,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowWidth();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleIntercept: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleIntercept,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleIntercept();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleSlope: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleSlope,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleSlope();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

#endif