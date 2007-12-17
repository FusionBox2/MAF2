/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-17 13:39:58 $
  Version:   $Revision: 1.4 $
  Authors:   Stefano Perticoni - Daniele Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------------

#include "lhpTagHandlerContainer.h"

#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMERoot.h"


#include <string>
#include <ostream>


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type::lhpTagHandler_L0000_resource_data_Type()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type_Field);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type_Field::lhpTagHandler_L0000_resource_data_Type_Field()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type_Field::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type_Dimension);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type_Dimension::lhpTagHandler_L0000_resource_data_Type_Dimension()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type_Dimension::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type_VolumeType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type_VolumeType::lhpTagHandler_L0000_resource_data_Type_VolumeType()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type_VolumeType::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type_Timevarying);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type_Timevarying::lhpTagHandler_L0000_resource_data_Type_Timevarying()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type_Timevarying::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{
  mafString value;
  value = vme->IsAnimated()?"1":"0";

  // tag handling code
  vme->GetTagArray()->SetTag(m_TagName.GetCStr(), value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Size_DatasetSize);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Size_DatasetSize::lhpTagHandler_L0000_resource_data_Size_DatasetSize()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_DatasetSize::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Size_EntityCount);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Size_EntityCount::lhpTagHandler_L0000_resource_data_Size_EntityCount()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_EntityCount::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Size_TimeFramesCount::lhpTagHandler_L0000_resource_data_Size_TimeFramesCount()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_TimeFramesCount::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{
  mafString value;
  std::vector<mafTimeStamp> timeStamps;
  vme->GetTimeStamps(timeStamps);
  value << (long) timeStamps.size();

  // tag handling code
  vme->GetTagArray()->SetTag(m_TagName.GetCStr(), value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_DatasetURI);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_DatasetURI::lhpTagHandler_L0000_resource_data_Dataset_DatasetURI()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_DatasetURI::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileSize);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileSize::lhpTagHandler_L0000_resource_data_Dataset_FileSize()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileSize::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_UploadDate::lhpTagHandler_L0000_resource_data_Dataset_UploadDate()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_UploadDate::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType::lhpTagHandler_L0000_resource_data_Dataset_FileType()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat::lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity::lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption::lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{
  mafString value;
  value = vme->GetCrypting()?"1":"0";

  // tag handling code
  vme->GetTagArray()->SetTag(m_TagName.GetCStr(), value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose::lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector::lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{
  mafString value;
  std::vector<mafTimeStamp> timeStamps;
  vme->GetAbsTimeStamps(timeStamps);
  long timeCount;
  for(timeCount = 0; timeCount < timeStamps.size(); timeCount++)
  {
    value << timeStamps[timeCount];
    if(timeCount < timeStamps.size() - 1 )
    {
      value << " ";
    }
  }

  // tag handling code
  vme->GetTagArray()->SetTag(m_TagName.GetCStr(), value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI::lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName::lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{
  mafString value;
  value = vme->GetRoot()->GetName();

  // tag handling code
  vme->GetTagArray()->SetTag(m_TagName.GetCStr(), value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1::lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate::lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes::lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID::lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes::lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1::lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType::lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir::lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir::FillVMETag(mafVME *vme)
//------------------------------------------------------------------------------------
{

}