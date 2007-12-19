/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-19 16:19:22 $
  Version:   $Revision: 1.8 $
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
void lhpTagHandler_L0000_resource_data_Type::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Type_Field::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Type_Dimension::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Type_VolumeType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Type_Timevarying::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;
  value = vme->IsAnimated()?"1":"0";

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Size_DatasetSize);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Size_DatasetSize::lhpTagHandler_L0000_resource_data_Size_DatasetSize()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_DatasetSize::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Size_EntityCount::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Size_TimeFramesCount::lhpTagHandler_L0000_resource_data_Size_TimeFramesCount()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_TimeFramesCount::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();

  mafString value;
  std::vector<mafTimeStamp> timeStamps;
  vme->GetTimeStamps(timeStamps);
  value << (long) timeStamps.size();

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_DatasetURI);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_DatasetURI::lhpTagHandler_L0000_resource_data_Dataset_DatasetURI()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_DatasetURI::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Dataset_FileSize::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Dataset_UploadDate::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Dataset_FileType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat::lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafString value = "MAF2"; //for now the only file format supported
 
  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity::lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption::lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;
  value = vme->GetCrypting()?"1":"0";

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose::lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector::lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
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
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI::lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName::lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;
  value = vme->GetRoot()->GetName();

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1::lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID::lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafString value = cargo->GetInputUser()->GetName();

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes::lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes()
//------------------------------------------------------------------------------------
{

}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
void lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{

}