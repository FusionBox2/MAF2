/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.h,v $
  Language:  C++
  Date:      $Date: 2008-06-30 09:17:11 $
  Version:   $Revision: 1.7 $
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

class lhpTagHandler_L0000_resource_data_DataType: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_DataType,mafObject);

    lhpTagHandler_L0000_resource_data_DataType();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};


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

class lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};


class lhpTagHandler_L0000_resource_data_Dataset_UploadDate: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_UploadDate();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Dataset_FileType: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileType();
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

class lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate,mafObject);

    lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate();
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

class lhpTagHandler_L0000_resource_data_Quality: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Quality,mafObject);

    lhpTagHandler_L0000_resource_data_Quality();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Quality_QualityScore1: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1,mafObject);

    lhpTagHandler_L0000_resource_data_Quality_QualityScore1();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};


class lhpTagHandler_L0000_resource_data_Source: public lhpTagHandler
{
public:
  mafTypeMacro(lhpTagHandler_L0000_resource_data_Source,mafObject);

  lhpTagHandler_L0000_resource_data_Source();
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

#endif