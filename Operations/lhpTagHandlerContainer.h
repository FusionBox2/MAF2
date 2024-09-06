/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.h,v $
  Language:  C++
  Date:      $Date: 2009-01-07 10:46:20 $
  Version:   $Revision: 1.14.2.10 $
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


class lhpTagHandler_L0000_resource_DictionaryURI: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_DictionaryURI,mafObject);

  lhpTagHandler_L0000_resource_DictionaryURI();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_DictionaryVersion: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_DictionaryVersion,mafObject);

  lhpTagHandler_L0000_resource_DictionaryVersion();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_DataType_Field: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Field,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_Field();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};


class lhpTagHandler_L0000_resource_data_DataType_Dimension: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Dimension,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_Dimension();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_DataType_VolumeType: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_VolumeType,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_VolumeType();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};
 
class lhpTagHandler_L0000_resource_data_DataType_Timevarying: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Timevarying,mafObject);

    lhpTagHandler_L0000_resource_data_DataType_Timevarying();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Size_FileSize: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Size_FileSize,mafObject);

    lhpTagHandler_L0000_resource_data_Size_FileSize();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Size_EntityCount: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Size_EntityCount,mafObject);

    lhpTagHandler_L0000_resource_data_Size_EntityCount();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Size_TimeFramesCount: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount,mafObject);

    lhpTagHandler_L0000_resource_data_Size_TimeFramesCount();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Dataset_DatasetURI: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_DatasetURI,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_DatasetURI();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Dataset_UploadDate: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_UploadDate();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum,mafObject);

  lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};


class lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_MAF_VmeType: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_MAF_VmeType,mafObject);

  lhpTagHandler_L0000_resource_MAF_VmeType();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose,mafObject);

    lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector,mafObject);

    lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_MAF_Procedural: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_MAF_Procedural,mafObject);

  lhpTagHandler_L0000_resource_MAF_Procedural();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_MAF_Procedural_VMElinkURI1: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_MAF_Procedural_VMElinkURI1,mafObject);

  lhpTagHandler_L0000_resource_MAF_Procedural_VMElinkURI1();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};


class lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Operation: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Operation,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Operation();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_CreationDate: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_CreationDate,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_CreationDate();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Application: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Application,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Application();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_IsNatural: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_IsNatural,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_IsNatural();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_OperatorID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_OperatorID,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_OperatorID();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Parameters: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Parameters,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_CreateEvent_Parameters();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Operation: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Operation,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Operation();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_ModifyDate: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_ModifyDate,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_ModifyDate();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Application: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Application,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Application();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_OperatorID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_OperatorID,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_OperatorID();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Parameters: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Parameters,mafObject);

  lhpTagHandler_L0000_resource_data_Traceability_ModifyEvent1_Parameters();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Traceability_Ownership: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_Ownership,mafObject);

    lhpTagHandler_L0000_resource_data_Traceability_Ownership();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};

class lhpTagHandler_L0000_resource_data_Traceability_Ownership_OwnerID: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Traceability_Ownership_OwnerID,mafObject);

    lhpTagHandler_L0000_resource_data_Traceability_Ownership_OwnerID();
    void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;
  
};


class lhpTagHandler_L0000_resource_data_Ownership_OwnerID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Ownership_OwnerID,mafObject);

  lhpTagHandler_L0000_resource_data_Ownership_OwnerID();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Quality_QualityScore1_URL: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1_URL,mafObject);

  lhpTagHandler_L0000_resource_data_Quality_QualityScore1_URL();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Quality_QualityScore1_Score: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1_Score,mafObject);

  lhpTagHandler_L0000_resource_data_Quality_QualityScore1_Score();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Quality_QualityScore1_ScoreDate: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1_ScoreDate,mafObject);

  lhpTagHandler_L0000_resource_data_Quality_QualityScore1_ScoreDate();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_Access_Policy1_GroupID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_Access_Policy1_GroupID,mafObject);

  lhpTagHandler_L0000_resource_Access_Policy1_GroupID();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_Access_Policy1_Price: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_Access_Policy1_Price,mafObject);

  lhpTagHandler_L0000_resource_Access_Policy1_Price();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_Access_Policy1_Usage: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_Access_Policy1_Usage,mafObject);

  lhpTagHandler_L0000_resource_Access_Policy1_Usage();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_Access_Publishing_PublishingStatus: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_Access_Publishing_PublishingStatus,mafObject);

  lhpTagHandler_L0000_resource_Access_Publishing_PublishingStatus();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy,mafObject);

  lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryURI: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryURI,mafObject);

  lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryURI();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryVersion: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryVersion,mafObject);

  lhpTagHandler_L0000_resource_data_Representation_RepresentationType_Description_FunctionalAnatomy_FunctionalAnatomy_DictionaryVersion();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryURI: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryURI,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryURI();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryVersion: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryVersion,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DictionaryVersion();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_MASource: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MASource,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MASource();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryURI: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryURI,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryURI();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryVersion: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryVersion,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DictionaryVersion();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_MASource_MASource_General_SamplingFrequency: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_General_SamplingFrequency,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MASource_MASource_General_SamplingFrequency();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DataType_IsLandmark_NumberOfLandmarks: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DataType_IsLandmark_NumberOfLandmarks,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MASource_MASource_DataType_IsLandmark_NumberOfLandmarks();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};



class lhpTagHandler_L0000_resource_data_Source_MicroCTSource: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MicroCTSource,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MicroCTSource();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryURI: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryURI,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryURI();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryVersion: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryVersion,mafObject);

  lhpTagHandler_L0000_resource_data_Source_MicroCTSource_MicroCTSource_DictionaryVersion();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class L0000_resource_Documentation: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Documentation,mafObject);

  L0000_resource_Documentation();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};


class L0000_resource_Access_Policy1_GroupID: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Policy1_GroupID,mafObject);

  L0000_resource_Access_Policy1_GroupID();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};


class L0000_resource_Access_Policy1_Price: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Policy1_Price,mafObject);

  L0000_resource_Access_Policy1_Price();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};


class L0000_resource_Access_Policy1_Usage: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Policy1_Usage,mafObject);

  L0000_resource_Access_Policy1_Usage();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class L0000_resource_Access_Publishing_PublishingStatus: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Access_Publishing_PublishingStatus,mafObject);

  L0000_resource_Access_Publishing_PublishingStatus();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyDate: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyDate,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyDate();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Modality: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Modality,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Modality();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Manufacturer: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Manufacturer,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Manufacturer();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_InstitutionName: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_InstitutionName,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_InstitutionName();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StationName: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StationName,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StationName();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ManufacturerModelName: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ManufacturerModelName,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ManufacturerModelName();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientID,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientID();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientSex: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientSex,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientSex();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ScanOptions: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ScanOptions,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ScanOptions();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_KVP: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_KVP,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_KVP();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DataCollectionDiameter: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DataCollectionDiameter,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DataCollectionDiameter();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ReconstructionDiameter: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ReconstructionDiameter,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ReconstructionDiameter();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToDetector: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToDetector,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToDetector();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToPatient: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToPatient,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_DistanceSourceToPatient();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_GantryDetectorTilt: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_GantryDetectorTilt,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_GantryDetectorTilt();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_TableHeight: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_TableHeight,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_TableHeight();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RotationDirection: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RotationDirection,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RotationDirection();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ExposureTime: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ExposureTime,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ExposureTime();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_XRayTubeCurrent: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_XRayTubeCurrent,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_XRayTubeCurrent();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Exposure: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Exposure,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_Exposure();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FilterType: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FilterType,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FilterType();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FocalSpot: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FocalSpot,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_FocalSpot();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ConvolutionKernel: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ConvolutionKernel,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ConvolutionKernel();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientPosition: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientPosition,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PatientPosition();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyID: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyID,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_StudyID();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ImagePositionPatient: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ImagePositionPatient,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_ImagePositionPatient();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelSpacing: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelSpacing,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelSpacing();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelPaddingValue: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelPaddingValue,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_PixelPaddingValue();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowCenter: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowCenter,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowCenter();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowWidth: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowWidth,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_WindowWidth();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleIntercept: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleIntercept,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleIntercept();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

class lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleSlope: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleSlope,mafObject);

  lhpTagHandler_L0000_resource_data_Source_DicomSource_DicomSource_RescaleSlope();
  void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo) override;

};

#endif