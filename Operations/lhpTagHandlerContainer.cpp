/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.cpp,v $
  Language:  C++
  Date:      $Date: 2008-06-30 09:17:11 $
  Version:   $Revision: 1.18 $
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


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_DataType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_DataType::lhpTagHandler_L0000_resource_data_DataType()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_DataType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

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
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
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
    cargo->SetTagHandlerGeneratedString("No binary data associated");
    return;
  }
  else
  {
    mafTimeStamp currentTime = cargo->GetInputVme()->GetTimeStamp();
    mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(cargo->GetInputVme());
    if (vme->GetDataVector()->GetItem(currentTime)->GetTagArray()->IsTagPresent("MD5Checksum"))
    {
      mafTagItem *MD5Tag = vme->GetDataVector()->GetItem(currentTime)->GetTagArray()->GetTag("MD5Checksum");
      mafString MD5Str = MD5Tag->GetValue();
      cargo->SetTagHandlerGeneratedString(MD5Str);
    }
    else
    {
      cargo->SetTagHandlerGeneratedString("MD5 Checksum not Found");
    }
  }
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
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileType::lhpTagHandler_L0000_resource_data_Dataset_FileType()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
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
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
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

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Quality);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Quality::lhpTagHandler_L0000_resource_data_Quality()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Quality::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Quality_QualityScore1)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Quality_QualityScore1::lhpTagHandler_L0000_resource_data_Quality_QualityScore1()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Quality_QualityScore1::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Source);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Source::lhpTagHandler_L0000_resource_data_Source()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Source::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
	mafVME *vme = cargo->GetInputVme();
	mafString value;
  if(vme->GetTagArray()->IsTagPresent("SOURCE_TYPE"))
	{
     vme->GetTagArray()->GetTag("SOURCE_TYPE")->GetValueAsSingleString(value);
	}
	else
	{
		value = "NOT PRESENT";
	}

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