/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpFactoryTagHandler.cpp,v $
  Language:  C++
  Date:      $Date: 2007-12-17 09:17:32 $
  Version:   $Revision: 1.3 $
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
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Type, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Type_Field, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Type_Dimension, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Type_VolumeType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Type_Timevarying, "Time varying VME or static VME");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_DatasetSize, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_EntityCount, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Size_TimeFramesCount, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_DatasetURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileSize, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_FileFormat, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Encryption, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_TimeSpace_TimeStampVector, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootName, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes_OwnerID, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType, "");
  lhpPlugTagHandlerMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir, "");
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
