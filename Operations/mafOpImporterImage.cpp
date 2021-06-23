/*=========================================================================

 Program: MAF2
 Module: mafOpImporterImage
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpImporterImage.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUI.h"

#include "mafTagArray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEImage.h"
#include "mafVMEItem.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkTIFFReader.h"

#include <algorithm>

//----------------------------------------------------------------------------
// Global Function (locale to this file) to sort the filenames
//----------------------------------------------------------------------------
bool CompareNumber(const mafString& first, const mafString& second)
{
	mafString first_path, first_name, first_ext;
	mafString second_path, second_name, second_ext;
	long first_num, second_num;

	mafSplitPath(first,&first_path,&first_name,&first_ext);
	mafSplitPath(second,&second_path,&second_name,&second_ext);

	first_name.toWx().ToLong(&first_num);
	second_name.toWx().ToLong(&second_num);

  return (first_num - second_num) < 0;   // compare number
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpImporterImage);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpImporterImage::mafOpImporterImage(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;
  m_Files.clear();
  m_NumFiles = 0;
  m_BuildVolumeFlag = 0;

  m_FilePrefix    = _R("");
  m_FilePattern   = _R("%s%04d");
  m_FileExtension = _R("");
  m_FileOffset    = 0;
  m_FileSpacing = 1;
  m_ImageZSpacing = 1.0;

  m_ImportedImage = NULL;
  m_ImportedImageAsVolume = NULL;

  m_FileDirectory = _R("");//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterImage::~mafOpImporterImage()
//----------------------------------------------------------------------------
{
  mafDEL(m_ImportedImage);
  mafDEL(m_ImportedImageAsVolume);
}
//----------------------------------------------------------------------------
// constant ID
//----------------------------------------------------------------------------
enum IMAGE_IMPORTER_ID
{
  ID_BUILD_VOLUME = MINID,
  ID_STRING_PREFIX,
  ID_STRING_PATTERN,
  ID_STRING_EXT,
  ID_OFFSET,
  ID_SPACING,
  ID_DATA_SPACING,
};
//----------------------------------------------------------------------------
void mafOpImporterImage::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = _R("Images (*.bmp;*.jpg;*.png;*.tif)| *.bmp;*.jpg;*.png;*.tif");
	
  if (!m_TestMode)
  {
    m_Files.clear();
	  m_Gui = new mafGUI(this);
  
    mafGetOpenMultiFiles(m_FileDirectory,wildc,m_Files);
  }

  m_NumFiles = m_Files.size();

  if(m_NumFiles == 0)
  {
    OpStop(OP_RUN_CANCEL);
  }
  else
  {
    mafSplitPath(m_Files[0],&m_FileDirectory,&m_FilePrefix,&m_FileExtension);
    
    if (!m_TestMode)
    {
      m_Gui->Bool(ID_BUILD_VOLUME,_R("Volume"),&m_BuildVolumeFlag,0,_R("Check to build volume, otherwise a sequence of image is generated!"));
	    m_Gui->String(ID_STRING_PREFIX,_R("file pref."), &m_FilePrefix);
	    m_Gui->String(ID_STRING_PATTERN,_R("file patt."), &m_FilePattern);
	    m_Gui->String(ID_STRING_EXT,_R("file ext."), &m_FileExtension);
	    m_Gui->Integer(ID_OFFSET,_R("file offset:"),&m_FileOffset,0, MAXINT,_R("set the first slice number in the files name"));
	    m_Gui->Integer(ID_SPACING,_R("file spc.:"),&m_FileSpacing,1, MAXINT, _R("set the spacing between the slices in the files name"));
	    m_Gui->Double(ID_DATA_SPACING,_R("data spc.:"),&m_ImageZSpacing,1);
      m_Gui->OkCancel();

      m_Gui->Enable(ID_STRING_PREFIX,false);
      m_Gui->Enable(ID_STRING_PATTERN,false);
      m_Gui->Enable(ID_STRING_EXT,false);
      m_Gui->Enable(ID_OFFSET,false);
      m_Gui->Enable(ID_SPACING,false);
      m_Gui->Enable(ID_DATA_SPACING,false);
      m_Gui->Update();

			m_Gui->Divider();

      ShowGui();
    }

    if(m_NumFiles == 1)
    {
      ImportImage();
      if (!m_TestMode)
      {
        OpStop(OP_RUN_OK);
      }
    }
  }
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterImage::Copy()   
//----------------------------------------------------------------------------
{
  return (new mafOpImporterImage(GetLabel()));
}
//----------------------------------------------------------------------------
void mafOpImporterImage::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mafOpImporterImage::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_BUILD_VOLUME:
        if (!m_TestMode)
        {
          m_Gui->Enable(ID_STRING_PREFIX,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_STRING_PATTERN,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_STRING_EXT,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_OFFSET,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_SPACING,m_BuildVolumeFlag != 0);
          m_Gui->Enable(ID_DATA_SPACING,m_BuildVolumeFlag != 0);
          m_Gui->Update();
        }
      break;
      case wxOK:
        ImportImage();
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafOpImporterImage::ImportImage()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyCursor wait;
  }
  if(m_BuildVolumeFlag)
    BuildVolume();        //Build volume
  else
    BuildImageSequence(); // Build image sequence

  mafTagItem tag_Nature;
  tag_Nature.SetName(_R("VME_NATURE"));
  tag_Nature.SetValue(_R("NATURAL"));

  m_Output->GetTagArray()->SetTag(tag_Nature);
}
//----------------------------------------------------------------------------
void mafOpImporterImage::BuildImageSequence()
//----------------------------------------------------------------------------
{
	long time;
  mafString path, name, ext;

	mafNEW(m_ImportedImage);
  
  mafSplitPath(m_Files[0],&path,&name,&ext);
  if(wxString(name.GetCStr()).IsNumber())
    std::sort(m_Files.begin(),m_Files.end(),CompareNumber);
  else
    std::sort(m_Files.begin(),m_Files.end());

	mafTimeStamp start_time = ((mafVME *)m_Input->GetRoot())->GetTimeStamp();
  long progress_value = 0;
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  for(int i=0; i<m_NumFiles; i++)
	{
    if (mafFloatEquals(fmod(i,10.0f),0.0f))
    {
      progress_value = (i*100)/m_NumFiles;
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(intptr_t)progress_value));
    }

    mafSplitPath(m_Files[i],&path,&name,&ext);
		ext.MakeUpper();
		if(name.toWx().IsNumber())
			name.toWx().ToLong(&time);
		else
			time = i;

		if(ext == _R("BMP"))
		{
			vtkMAFSmartPointer<vtkBMPReader> r;
			r->SetFileName(m_Files[i].GetCStr());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		} 
		else if (ext == _R("JPG") || ext == _R("JPEG") )
		{
			vtkMAFSmartPointer<vtkJPEGReader> r;
			r->SetFileName(m_Files[i].GetCStr());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else if (ext == _R("PNG"))
		{
			vtkMAFSmartPointer<vtkPNGReader> r;
			r->SetFileName(m_Files[i].GetCStr());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else if (ext == _R("TIF") || ext == _R("TIFF") )
		{
			vtkMAFSmartPointer<vtkTIFFReader> r;
			r->SetFileName(m_Files[i].GetCStr());
			r->Update();
      m_ImportedImage->SetData(r->GetOutput(),time);
		}
		else
			wxMessageBox((_R("unable to import ") + m_Files[i] + _R(", unrecognized type")).toWx());
	}

  if(m_NumFiles > 1)
    m_ImportedImage->SetName(_R("Imported Images"));
  else
    m_ImportedImage->SetName(name);

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  m_ImportedImage->SetTimeStamp(start_time);
  m_ImportedImage->ReparentTo(m_Input);
  m_Output = m_ImportedImage;
}
//----------------------------------------------------------------------------
void mafOpImporterImage::BuildVolume()
//----------------------------------------------------------------------------
{
  wxString prefix  = (m_FileDirectory + _R("\\") + m_FilePrefix).toWx();
  prefix.Replace("/", "\\");
  wxString pattern = (m_FilePattern  + _R(".")  + m_FileExtension).toWx();
  int extent[6];

  mafNEW(m_ImportedImageAsVolume);
  m_ImportedImageAsVolume->SetName(_R("Imported Volume"));

  if(m_FileExtension.Upper() == _R("BMP"))
	{
    vtkBMPReader *r = vtkBMPReader::New();
    r->SetFileName(m_Files[0].GetCStr());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
    r = vtkBMPReader::New();
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataVOI(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
    
    r->Delete();
	} 
	else if (m_FileExtension.Upper() == _R("JPG") || m_FileExtension.Upper() == _R("JPEG"))
	{
		vtkJPEGReader *r = vtkJPEGReader::New();
    r->SetFileName(m_Files[0].GetCStr());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
		r = vtkJPEGReader::New();
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
    
    r->Delete();
	}
	else if (m_FileExtension.Upper() == _R("PNG"))
	{
		vtkPNGReader *r = vtkPNGReader::New();
    r->SetFileName(m_Files[0].GetCStr());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
		r = vtkPNGReader::New();
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();

    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());
    
    r->Delete();
	}
	else if (m_FileExtension.Upper() == _R("TIF") || m_FileExtension.Upper() == _R("TIFF") )
	{
		vtkTIFFReader *r = vtkTIFFReader::New();
    r->SetFileName(m_Files[0].GetCStr());
    r->UpdateInformation();
    r->GetDataExtent(extent);
    r->Delete();
		r = vtkTIFFReader::New();
    mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,r));
    r->SetFileDimensionality(2);
    r->SetFilePrefix(prefix);
    r->SetFilePattern(pattern);
    r->SetFileNameSliceSpacing(m_FileSpacing);
    r->SetFileNameSliceOffset(m_FileOffset);
    r->SetDataExtent(extent[0], extent[1], extent[2], extent[3], extent[4], m_NumFiles - 1);
    r->SetDataOrigin(0, 0, m_FileOffset);
    r->SetDataSpacing(1.0,1.0,m_ImageZSpacing);
    r->Update();
    
    m_ImportedImageAsVolume->SetData(r->GetOutput(),((mafVME *)m_Input)->GetTimeStamp());

    r->Delete();
	}
	else
  {
		mafLogMessage(_M(_R("unable to import ") + m_Files[0] + _R(", unrecognized type")));
    mafDEL(m_ImportedImageAsVolume);
  }
  
  m_Output = m_ImportedImageAsVolume;
}
//----------------------------------------------------------------------------
void mafOpImporterImage::SetFileName(const char *file_name)
//----------------------------------------------------------------------------
{
 m_Files.push_back(_R(file_name));
 m_NumFiles = m_Files.size();
}
