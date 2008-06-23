/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: vtkMAFFileDataProvider.cxx,v $ 
  Language: C++ 
  Date: $Date: 2008-06-23 16:43:55 $ 
  Version: $Revision: 1.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#pragma warning(disable: 4996) // deprecated

#include "vtkMAFFileDataProvider.h"
#include "vtkObjectFactory.h"
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

vtkCxxRevisionMacro(vtkMAFFileDataProvider, "$Revision: 1.1 $");
vtkStandardNewMacro(vtkMAFFileDataProvider);

#include "mafMemDbg.h"

//ctor / dtor
vtkMAFFileDataProvider::vtkMAFFileDataProvider() 
{
	this->FileName = NULL;
	this->File = -1;
}

vtkMAFFileDataProvider::~vtkMAFFileDataProvider()
{ 
	CloseFile();
	delete[] this->FileName;
}


//Attaches the given file. If bAutoClose is true, the attached file will be closed.
//If bDeleteOnClose is set to true, the file is considered to be temporary and
//will be removed during the close.
/*virtual*/ void vtkMAFFileDataProvider
	::AttachFile(int fhandle, const char* fname, bool bAutoClose, bool bDeleteOnClose)
{
	CloseFile();	//close the current underlaying file
	
	this->File = fhandle;
	if (fname != NULL) {
		this->FileName = new char[strlen(fname) + 1];
		strcpy(this->FileName, fname);
	}

	this->CloseAttachedFile = bAutoClose;
	this->DeleteOnClose = bDeleteOnClose;
}

//Detaches the underlaying file
/*virtual*/ int vtkMAFFileDataProvider::DetachFile() 
{
	int ret = File;
	File = NULL;

	delete[] FileName;
	FileName = NULL;
	return ret;
}

//Opens the specified file for reading/writing operations
//If the file does not exist and bOpenAlways is set to true, the
//file is created. Underlaying opened file is closed in prior to this operation.
//If bDeleteOnClose is set to true, the file is considered to be temporary and
//will be removed during the close.
//Returns 0 if an error occurs.
/*virtual*/ int vtkMAFFileDataProvider
	::OpenFile(const char* fname, bool bOpenForRO, bool bDeleteOnClose)
{
	if (fname == NULL) 
	{
		vtkErrorMacro(<< "Filename must be specified.");
		return 0;
	}

	// Close file from any previous call
	CloseFile();

	int f = _open(fname, 
		(bOpenForRO ? _O_RDONLY : _O_RDWR | _O_CREAT) | _O_BINARY, 
		(bOpenForRO ? _S_IREAD : _S_IREAD | _S_IWRITE)
		);
	if (f < 0)
		return 0;

	AttachFile(f, fname, true, bDeleteOnClose);
	this->Attached = false;
	return 1;
}

//Closes the underlaying file.
/*virtual*/ void vtkMAFFileDataProvider::CloseFile()
{
	if (this->File >= 0)
	{
		if (!this->Attached || this->CloseAttachedFile) {
			_close(this->File);		

			if (this->DeleteOnClose)
				_unlink(FileName);		//BES: 17.1.2008 - don't know if this is Unix compatible
		}

		this->File = -1;
	}

	delete[] FileName;
	FileName = NULL;
}


//Copies the binary data from the underlaying source into the given buffer. 
//Copying starts at startOffset position and at most count bytes are copied.
//The routine returns number of bytes successfully transfered.  
/*virtual*/ int vtkMAFFileDataProvider
	::ReadBinaryData(vtkIdType64 startOffset, void* buffer, int count)
{
	if (!Seek(startOffset))
		return 0;

	return _read(this->File, buffer, count);
}

//Copies the binary data from the given buffer into the underlaying data set at
//startOffset position. If the underlaying data set is not capable to hold the
//specified amount of bytes to be copied (count), it is automatically enlarge.	
//The routine returns number of bytes successfully transfered.	
/*virtual*/ int vtkMAFFileDataProvider
	::WriteBinaryData(vtkIdType64 startOffset, void* buffer, int count)
{
	if (!Seek(startOffset))
		return 0;

	return _write(this->File, buffer, count);
}

//Seeks the underlaying file
/*virtual*/ bool vtkMAFFileDataProvider::Seek(vtkIdType64 startOffset)
{
#if !defined(_WIN32) && !defined(_WIN64)
#pragma message("Warning: vtkMAFFileDataProvider is limited to files < 2GB. Win32 or Win64 platform required to handle larger files. ");
	if (_lseek(this->File, (long)startOffset, SEEK_SET) < 0)
#else
	if (_lseeki64(this->File, startOffset, SEEK_SET) < 0)
#endif	
	{
		vtkWarningMacro("File seek operation failed.");
		return false;
	}

	return true;
}