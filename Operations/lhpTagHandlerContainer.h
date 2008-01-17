/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.h,v $
  Language:  C++
  Date:      $Date: 2008-01-17 16:01:13 $
  Version:   $Revision: 1.5 $
  Authors:   Stefano Perticoni - Daniele Giunchi
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

class lhpTagHandler_L0000_resource_data_Type: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Type,mafObject);

    lhpTagHandler_L0000_resource_data_Type();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};


class lhpTagHandler_L0000_resource_data_Type_Field: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Type_Field,mafObject);

    lhpTagHandler_L0000_resource_data_Type_Field();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};


class lhpTagHandler_L0000_resource_data_Type_Dimension: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Type_Dimension,mafObject);

    lhpTagHandler_L0000_resource_data_Type_Dimension();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Type_VolumeType: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Type_VolumeType,mafObject);

    lhpTagHandler_L0000_resource_data_Type_VolumeType();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};
 
class lhpTagHandler_L0000_resource_data_Type_Timevarying: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Type_Timevarying,mafObject);

    lhpTagHandler_L0000_resource_data_Type_Timevarying();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Size_DatasetSize: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Size_DatasetSize,mafObject);

    lhpTagHandler_L0000_resource_data_Size_DatasetSize();
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

class lhpTagHandler_L0000_resource_data_Dataset_FileSize: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileSize,mafObject);

    lhpTagHandler_L0000_resource_data_Dataset_FileSize();
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

class lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose,mafObject);

    lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector,mafObject);

    lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI,mafObject);

    lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName,mafObject);

    lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1,mafObject);

    lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate,mafObject);

    lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes,mafObject);

    lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID,mafObject);

    lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes,mafObject);

    lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1,mafObject);

    lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType,mafObject);

    lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};

class lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir: public lhpTagHandler
{
  public:
    mafTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir,mafObject);

    lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir();
    virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);
  
};



class L0000_resource_Documentation: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Documentation,mafObject);

  L0000_resource_Documentation();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};



class L0000_resource_Pricing_Quotation1_GroupID: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Pricing_Quotation1_GroupID,mafObject);

  L0000_resource_Pricing_Quotation1_GroupID();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};



class L0000_resource_Pricing_Quotation1_Price: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Pricing_Quotation1_Price,mafObject);

  L0000_resource_Pricing_Quotation1_Price();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};



class L0000_resource_Pricing_Quotation1_Policy: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Pricing_Quotation1_Policy,mafObject);

  L0000_resource_Pricing_Quotation1_Policy();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};


class L0000_resource_Status: public lhpTagHandler
{
public:
  mafTypeMacro(L0000_resource_Status,mafObject);

  L0000_resource_Status();
  virtual void HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo);

};

#endif