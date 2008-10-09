/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.cpp,v $
  Language:  C++
  Date:      $Date: 2008-10-09 08:53:18 $
  Version:   $Revision: 1.25 $
  Authors:   Stefano Perticoni - Daniele Giunchi - Roberto Mucci
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
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafAbsMatrixPipe.h"
#include "mafDataVector.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"
#include "mafVMEOutputSurface.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEOutputVolume.h"
#include "mafVMEOutputPointSet.h"
#include "mafVMEGenericAbstract.h"


#include <string>
#include <fstream>
#include <iostream>

using namespace std; 


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Field);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_DataType_Field::lhpTagHandler_L0000_resource_data_DataType_Field()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_DataType_Field::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Dimension);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_DataType_Dimension::lhpTagHandler_L0000_resource_data_DataType_Dimension()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_DataType_Dimension::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
	mafVME *vme = cargo->GetInputVme();
	mafString value;
  if(mafVMEOutputSurface::SafeDownCast(vme->GetOutput()))
	{
		value = "SURFACE";
	}
  else if(mafVMEOutputPolyline::SafeDownCast(vme->GetOutput())) 
  {
    value = "CURVE";
  }
  else if(mafVMEOutputVolume::SafeDownCast(vme->GetOutput()))
  {
    value = "VOLUME";
  }
  else if(mafVMEOutputPointSet::SafeDownCast(vme->GetOutput()))
  {
    value = "POINT";
  }
	else
	{
		value = "NOT PRESENT";
	}

	// tag handling code
	cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_DataType_VolumeType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_DataType_VolumeType::lhpTagHandler_L0000_resource_data_DataType_VolumeType()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_DataType_VolumeType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
	mafVME *vme = cargo->GetInputVme();
	mafString value;
  if(vtkImageData::SafeDownCast(vme->GetOutput()->GetVTKData()))
	{
		value = "STRUCTURED";
	}
  else if (vtkRectilinearGrid::SafeDownCast(vme->GetOutput()->GetVTKData()))
  {
    value = "CARTESIAN";
  }
  else if (vtkUnstructuredGrid::SafeDownCast(vme->GetOutput()->GetVTKData()))
  {
    value = "UNSTRUCTURED";
  }
/*  else if (? ::SafeDownCast(vme->GetOutput()->GetVTKData())) BREP
  {
    value = "BREP"; //not yet supported
  }*/
	else
	{
		value = "NOT PRESENT";
	}

	// tag handling code
	cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_DataType_Timevarying);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_DataType_Timevarying::lhpTagHandler_L0000_resource_data_DataType_Timevarying()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_DataType_Timevarying::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;
  value = vme->IsAnimated()?"1":"0";

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Size_FileSize);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Size_FileSize::lhpTagHandler_L0000_resource_data_Size_FileSize()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_FileSize::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  //here there is also the controller for zip archive
	long length = 0;
	mafString inputMSF = cargo->GetInputMSF();
	mafString id ;
	id << cargo->GetInputVme()->GetId();

  if(cargo->GetInputVme()->GetId() == -1) return;

	//here put code for filename
	wxString oldDir = wxGetCwd();
	mafLogMessage( _T("Current working directory is: '%s' "), wxGetCwd().c_str() );
	wxSetWorkingDirectory(m_PythonUploadFullPath.GetCStr());
	mafLogMessage( _T("Now current working directory is: '%s' "), wxGetCwd().c_str() );

	// get manual tags
	wxString command2execute;
	command2execute.Clear();
	command2execute = m_PythonwExe;

	command2execute.Append(" lhpCheckBinaryName.py ");
  command2execute.Append("\"");
	command2execute.Append(inputMSF.GetCStr());
  command2execute.Append("\"");
	command2execute.Append(" ");
	command2execute.Append(id.GetCStr());

	//mafLogMessage( _T("Executing command: '%s'"), command2execute.c_str() );

	long pid = wxExecute(command2execute, wxEXEC_SYNC);

	wxArrayString output;
	wxArrayString errors;

	pid = wxExecute(command2execute, output, errors);

	wxString result = output[output.size() - 1];
  
	wxSetWorkingDirectory(oldDir);

  //if result == "", no binary data has been found
  if (result == "")
  {
    cargo->SetTagHandlerGeneratedString(wxString::Format("%d",0));
    return;
  }

	////////////////////////////

	wxString temp;
	temp.Append(inputMSF.GetCStr());
	temp = temp.BeforeLast('/');
	temp.Append("/");
	temp.Append(result);

  wxString extension;
  extension.Append(result);
  extension = extension.AfterLast('.');

  bool fileOpened = false;
  mafString fileToComputeLength = temp;
  if(extension == "zvtk")
  {
    wxFileInputStream in(fileToComputeLength.GetCStr());
    wxZipInputStream zip(in);
    if (!in || !zip)
    {
      ;
    }
    else
    {
      wxZipEntry *entry = NULL;

      // call GetNextEntry() until the required internal name is found
      // to be re-factored for efficiency reasons.

      do 
      {
        entry = zip.GetNextEntry();
        if(entry)
          length += entry->GetSize();
      } while(entry != NULL);
    }
  }
  else
  {
    
    fstream fp;
    fp.open(fileToComputeLength);

    if(fp.fail() == false)
    {
      fp.seekg(0, ios::end);
      length = fp.tellg();
      fp.close();
      fileOpened = true;
    }
  }

  //Process with length
	if(fileOpened) cargo->SetTagHandlerGeneratedString(wxString::Format("%d",length));
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Size_EntityCount);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Size_EntityCount::lhpTagHandler_L0000_resource_data_Size_EntityCount()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_EntityCount::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
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
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_DatasetURI::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("This tag will be filled during the upload process");
}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_UploadDate);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_UploadDate::lhpTagHandler_L0000_resource_data_Dataset_UploadDate()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_UploadDate::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("This tag will be filled during the upload process");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum::lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_LocalFileCheckSum::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("This tag will be filled during the upload process");
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
  cargo->SetTagHandlerGeneratedString(value.GetCStr());
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity::lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType_Endianity::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
	
	mafString value;
	value = mafIsLittleEndian()? "Little Endian" : "Big Endian";

	// tag handling code
	cargo->SetTagHandlerGeneratedString(value);
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
mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_VmeType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_VmeType::lhpTagHandler_L0000_resource_MAF_VmeType()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_VmeType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  value = vme->GetTypeName();
  cargo->SetTagHandlerGeneratedString(value.GetCStr());
}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose::lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_TimeSpace_VMEabsoluteMatrixPose::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
	mafVME *vme = cargo->GetInputVme();
	mafString value;
  std::vector<mafTimeStamp> timeStamps;
  mafAbsMatrixPipe *absMatrixPipe;
  long finalTimeStamps = 0;

  if (vme->IsAnimated())
  {
    mafVMEGenericAbstract *vmeGeneric = mafVMEGenericAbstract::SafeDownCast(vme);
    if (vmeGeneric != NULL)
    {
      vmeGeneric->GetMatrixTimeStamps(timeStamps);
      finalTimeStamps = timeStamps.size();
    }
  }
  else
  {
    vme->GetAbsTimeStamps(timeStamps);
    finalTimeStamps = 1;
  }

  
  absMatrixPipe = vme->GetAbsMatrixPipe();

	long timeCount;
	for(timeCount = 0; timeCount < finalTimeStamps; timeCount++)
	{
		absMatrixPipe->SetTimeStamp(timeStamps[timeCount]);
		value << absMatrixPipe->GetMatrix();
	}

	// tag handling code
	cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector::lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_TimeSpace_TimeStampVector::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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

mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI::lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootURI::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName::lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeRootName::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;
  value = vme->GetRoot()->GetName();

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1::lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeChildURI1::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("This tag will be filled during the upload process");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_Procedural_VmeLinkURI1);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_Procedural_VmeLinkURI1::lhpTagHandler_L0000_resource_MAF_Procedural_VmeLinkURI1()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_Procedural_VmeLinkURI1::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("This tag will be filled during the upload process");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate::lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_MAF_TreeInfo_VmeTreeCreationDate::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application::lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_Application::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetRoot()->GetTagArray()->IsTagPresent("APP_STAMP"))
  {
    value = vme->GetRoot()->GetTagArray()->GetTag("APP_STAMP")->GetValue();
  }
  else
  {
    value = "Not found";
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural::lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Tracebility_CreateEvent_IsNatural::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("VME_NATURE"))
  {
    value = vme->GetTagArray()->GetTag("VME_NATURE")->GetValue();
  }
  else
  {
    value = "Not found";
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());
}



mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Tracebility_Ownership::lhpTagHandler_L0000_resource_data_Tracebility_Ownership()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Tracebility_Ownership::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID::lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Tracebility_Ownership_OwnerID::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafString value = cargo->GetInputUser()->GetName();

  // tag handling code
  cargo->SetTagHandlerGeneratedString(value);
}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource::lhpTagHandler_L0000_resource_data_Source_DicomSource()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_MASource)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_MASource::lhpTagHandler_L0000_resource_data_Source_MASource()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_MASource::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}


mafCxxTypeMacro(L0000_resource_Documentation)
//------------------------------------------------------------------------------------
L0000_resource_Documentation::L0000_resource_Documentation()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Documentation::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}



mafCxxTypeMacro(L0000_resource_Access_Policy1_GroupID)
//------------------------------------------------------------------------------------
L0000_resource_Access_Policy1_GroupID::L0000_resource_Access_Policy1_GroupID()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Access_Policy1_GroupID::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}



mafCxxTypeMacro(L0000_resource_Access_Policy1_Price)
//------------------------------------------------------------------------------------
L0000_resource_Access_Policy1_Price::L0000_resource_Access_Policy1_Price()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Access_Policy1_Price::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}


mafCxxTypeMacro(L0000_resource_Access_Policy1_Usage)
//------------------------------------------------------------------------------------
L0000_resource_Access_Policy1_Usage::L0000_resource_Access_Policy1_Usage()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Access_Policy1_Usage::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}


mafCxxTypeMacro(L0000_resource_Access_Publishing_PublishingStatus)
//------------------------------------------------------------------------------------
L0000_resource_Access_Publishing_PublishingStatus::L0000_resource_Access_Publishing_PublishingStatus()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Access_Publishing_PublishingStatus::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

//////////
//DICOM///
//////////
mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyDate)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyDate::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyDate()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyDate::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("StudyDate"))
  {
    value = vme->GetTagArray()->GetTag("StudyDate")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Modality)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Modality::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Modality()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Modality::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("Modality"))
  {
    value = vme->GetTagArray()->GetTag("Modality")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Manufacturer)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Manufacturer::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Manufacturer()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Manufacturer::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("Manufacturer"))
  {
    value = vme->GetTagArray()->GetTag("Manufacturer")->GetValue();
  }


  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_InstitutionName)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_InstitutionName::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_InstitutionName()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_InstitutionName::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("InstitutionName"))
  {
    value = vme->GetTagArray()->GetTag("InstitutionName")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StationName)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StationName::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StationName()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StationName::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("StationName"))
  {
    value = vme->GetTagArray()->GetTag("StationName")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ManufacturerModelName)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ManufacturerModelName::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ManufacturerModelName()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ManufacturerModelName::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("ManufacturerModelName"))
  {
    value = vme->GetTagArray()->GetTag("ManufacturerModelName")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientID)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientID::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientID()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientID::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("PatientID"))
  {
    value = vme->GetTagArray()->GetTag("PatientID")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientSex)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientSex::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientSex()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientSex::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("PatientSex"))
  {
    value = vme->GetTagArray()->GetTag("PatientSex")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ScanOptions)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ScanOptions::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ScanOptions()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ScanOptions::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("ScanOptions"))
  {
    value = vme->GetTagArray()->GetTag("ScanOptions")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_KVP)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_KVP::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_KVP()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_KVP::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("KVP"))
  {
    value = vme->GetTagArray()->GetTag("KVP")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DataCollectionDiameter)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DataCollectionDiameter::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DataCollectionDiameter()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DataCollectionDiameter::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("DataCollectionDiameter"))
  {
    value = vme->GetTagArray()->GetTag("DataCollectionDiameter")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ReconstructionDiameter)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ReconstructionDiameter::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ReconstructionDiameter()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ReconstructionDiameter::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("ReconstructionDiameter"))
  {
    value = vme->GetTagArray()->GetTag("ReconstructionDiameter")->GetValue();
  }
  else

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToDetector)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToDetector::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToDetector()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToDetector::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("DistanceSourceToDetector"))
  {
    value = vme->GetTagArray()->GetTag("DistanceSourceToDetector")->GetValue();
  }
 
  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToPatient)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToPatient::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToPatient()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_DistanceSourceToPatient::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("DistanceSourceToPatient"))
  {
    value = vme->GetTagArray()->GetTag("DistanceSourceToPatient")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_GantryDetectorTilt)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_GantryDetectorTilt::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_GantryDetectorTilt()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_GantryDetectorTilt::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("GantryDetectorTilt"))
  {
    value = vme->GetTagArray()->GetTag("GantryDetectorTilt")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_TableHeight)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_TableHeight::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_TableHeight()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_TableHeight::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("TableHeight"))
  {
    value = vme->GetTagArray()->GetTag("TableHeight")->GetValue();
  }
 
  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RotationDirection)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RotationDirection::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RotationDirection()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RotationDirection::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("RotationDirection"))
  {
    value = vme->GetTagArray()->GetTag("RotationDirection")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ExposureTime)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ExposureTime::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ExposureTime()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ExposureTime::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("ExposureTime"))
  {
    value = vme->GetTagArray()->GetTag("ExposureTime")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_XRayTubeCurrent)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_XRayTubeCurrent::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_XRayTubeCurrent()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_XRayTubeCurrent::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("XrayTubeCurrent"))
  {
    value = vme->GetTagArray()->GetTag("XrayTubeCurrent")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Exposure)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Exposure::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Exposure()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_Exposure::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("Exposure"))
  {
    value = vme->GetTagArray()->GetTag("Exposure")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FilterType)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FilterType::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FilterType()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FilterType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("FilterType"))
  {
    value = vme->GetTagArray()->GetTag("FilterType")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FocalSpot)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FocalSpot::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FocalSpot()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_FocalSpot::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("FocalSpot"))
  {
    value = vme->GetTagArray()->GetTag("FocalSpot")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ConvolutionKernel)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ConvolutionKernel::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ConvolutionKernel()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ConvolutionKernel::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("ConvolutionKernel"))
  {
    value = vme->GetTagArray()->GetTag("ConvolutionKernel")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientPosition)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientPosition::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientPosition()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PatientPosition::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("PatientPosition"))
  {
    value = vme->GetTagArray()->GetTag("PatientPosition")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyID)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyID::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyID()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_StudyID::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("StudyID"))
  {
    value = vme->GetTagArray()->GetTag("StudyID")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ImagePositionPatient)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ImagePositionPatient::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ImagePositionPatient()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_ImagePositionPatient::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("ImagePositionPatient"))
  {
    value = vme->GetTagArray()->GetTag("ImagePositionPatient")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelSpacing)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelSpacing::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelSpacing()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelSpacing::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("PixelSpacing"))
  {
    value = vme->GetTagArray()->GetTag("PixelSpacing")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelPaddingValue)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelPaddingValue::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelPaddingValue()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_PixelPaddingValue::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("PixelPaddingValue"))
  {
    value = vme->GetTagArray()->GetTag("PixelPaddingValue")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowCenter)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowCenter::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowCenter()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowCenter::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("WindowCenter"))
  {
    value = vme->GetTagArray()->GetTag("WindowCenter")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowWidth)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowWidth::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowWidth()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_WindowWidth::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("WindowWidth"))
  {
    value = vme->GetTagArray()->GetTag("WindowWidth")->GetValue();
  }
  else
  {
    value = "Not found";
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleIntercept)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleIntercept::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleIntercept()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleIntercept::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("RescaleIntercept"))
  {
    value = vme->GetTagArray()->GetTag("RescaleIntercept")->GetValue();
  }

  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleSlope)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleSlope::lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleSlope()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source_DicomSource_Type_RescaleSlope::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  mafVME *vme = cargo->GetInputVme();
  mafString value;

  if(vme->GetTagArray()->IsTagPresent("RescaleSlope"))
  {
    value = vme->GetTagArray()->GetTag("RescaleSlope")->GetValue();
  }


  cargo->SetTagHandlerGeneratedString(value.GetCStr());

}
