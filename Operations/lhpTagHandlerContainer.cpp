/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpTagHandlerContainer.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-09 09:33:22 $
  Version:   $Revision: 1.13 $
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
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafAbsMatrixPipe.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkUnstructuredGrid.h"
#include "mafVMEOutputSurface.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMEOutputVolume.h"
#include "mafVMEOutputPointSet.h"


#include <string>
#include <fstream>
#include <iostream>

using namespace std; 


mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type::lhpTagHandler_L0000_resource_data_Type()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type_Field);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type_Field::lhpTagHandler_L0000_resource_data_Type_Field()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type_Field::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type_Dimension);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type_Dimension::lhpTagHandler_L0000_resource_data_Type_Dimension()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type_Dimension::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Type_VolumeType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Type_VolumeType::lhpTagHandler_L0000_resource_data_Type_VolumeType()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Type_VolumeType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Size_DatasetSize::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Dataset_FileSize);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Dataset_FileSize::lhpTagHandler_L0000_resource_data_Dataset_FileSize()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Dataset_FileSize::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
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


  ////////////////////////////

  wxString temp;
  temp.Append(inputMSF.GetCStr());
  temp = temp.BeforeLast('/');
  temp.Append("/");
  temp.Append(result);

  mafString fileToComputeLength = temp;
  fstream fp;
  fp.open(fileToComputeLength);

  bool fileOpened = false; 
  if(fp.fail() == false)
  {
    fp.seekg(0, ios::end);
    length = fp.tellg();
    fp.close();
    fileOpened = true;
  }

  //Process with length
  if(fileOpened) cargo->SetTagHandlerGeneratedString(wxString::Format("%d",length));
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

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose::lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TimeSpace_VMEabsoluteMatrixPose::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
	mafVME *vme = cargo->GetInputVme();
	mafString value;
	mafAbsMatrixPipe *absMatrixPipe = vme->GetAbsMatrixPipe();

	std::vector<mafTimeStamp> timeStamps;
	vme->GetAbsTimeStamps(timeStamps);
	
	long timeCount;
	for(timeCount = 0; timeCount < timeStamps.size(); timeCount++)
	{
		absMatrixPipe->SetTimeStamp(timeStamps[timeCount]);
   
		value << absMatrixPipe->GetMatrix();
	}

	// tag handling code
	cargo->SetTagHandlerGeneratedString(value);
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
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeRootURI::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
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
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeChildURI1::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate::lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_TreeInfo_VmeTreeCreationDate::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes::lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_OwnerAttributes::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
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
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1)
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1::lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_QualityAttributes_QualityScore1::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType::lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
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

mafCxxTypeMacro(lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir);
//------------------------------------------------------------------------------------
lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir::lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void lhpTagHandler_L0000_resource_data_Attributes_SourceAttributes_SourceType_SourceDir::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
	mafVME *vme = cargo->GetInputVme();
	mafString value;
	if(vme->GetTagArray()->IsTagPresent("SOURCE_TYPE_SOURCE_DIR"))
	{
		vme->GetTagArray()->GetTag("SOURCE_TYPE_SOURCE_DIR")->GetValueAsSingleString(value);
	}
	else
	{
		value = "NOT PRESENT";
	}

	// tag handling code
	cargo->SetTagHandlerGeneratedString(value);
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



mafCxxTypeMacro(L0000_resource_Pricing_Quotation1_GroupID)
//------------------------------------------------------------------------------------
L0000_resource_Pricing_Quotation1_GroupID::L0000_resource_Pricing_Quotation1_GroupID()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Pricing_Quotation1_GroupID::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}



mafCxxTypeMacro(L0000_resource_Pricing_Quotation1_Price)
//------------------------------------------------------------------------------------
L0000_resource_Pricing_Quotation1_Price::L0000_resource_Pricing_Quotation1_Price()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Pricing_Quotation1_Price::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}


mafCxxTypeMacro(L0000_resource_Pricing_Quotation1_Policy)
//------------------------------------------------------------------------------------
L0000_resource_Pricing_Quotation1_Policy::L0000_resource_Pricing_Quotation1_Policy()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Pricing_Quotation1_Policy::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}


mafCxxTypeMacro(L0000_resource_Status)
//------------------------------------------------------------------------------------
L0000_resource_Status::L0000_resource_Status()
//------------------------------------------------------------------------------------
{
  ExtractTagName();
}
//------------------------------------------------------------------------------------
void L0000_resource_Status::HandleAutoTag(lhpTagHandlerInputOutputParametersCargo *cargo)
//------------------------------------------------------------------------------------
{
  // tag handling code
  cargo->SetTagHandlerGeneratedString("Automated Tag Not Handled (instance exists)");
}