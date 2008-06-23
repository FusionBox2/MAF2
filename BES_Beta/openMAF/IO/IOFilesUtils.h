/**=======================================================================
  
  File:    	 IOFilesUtils.h
  Language:  C++
  Date:      11:2:2008   12:36
  Version:   $Revision: 1.1 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
This class just enhance the manipulation with files using IO 
by introducing std::exceptions as reactions to failures
=========================================================================*/

#ifndef _WIN32
#pragma error "IOFileUtils is not supported for platforms other than Win32"

//Sorry for that. Things to do in order to support Linux platforms: 
//1) #include <sys/io.h>
//2) there is no _lseeki64 -> fseeko64 must be used but it works with FILE*
#endif

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <share.h>

class IOFileUtils
{
public:
	//opens an existing file for R/W or RO (bRO == true)
	//and returns the IO handle
	//throws std::exceptions if an error occurs
	inline static int OpenFile(const char* fname, bool bRO = true) throw(...);

	//creates a new file and returns the IO handle
	//throws std::exceptions if an error occurs
	inline static int CreateFile(const char* fname) throw(...);

	//reads count bytes from the buffer into the file
	//throws std::exceptions if an error occurs
	inline static void ReadFile(int fhandle, void* buffer, int count) throw(...);

	//writes count bytes from the buffer into the file
	//throws std::exceptions if an error occurs
	inline static void WriteFile(int fhandle, void* buffer, int count) throw(...);

	//closes the file (with no exception!) and invalidates it
	inline static void CloseFile(int& fhandle) {
		if (fhandle >= 0) {
			_close(fhandle); fhandle = -1;
		}
	}

	//set the current position in file to pos
	inline static void Seek(int fhandle, unsigned long long pos) throw(...);

	//returns file size, throwing exception if an error occurs (e.g., file not found)
	inline static unsigned long long GetFileSize(const char* fname) throw(...);
};

//creates a new file and returns the IO handle
//throws std::exceptions if an error occurs
inline /*static*/ int IOFileUtils::CreateFile(const char* fname) throw(...)
{
	//create the large file, we will have to use IO.h operations, 
	//as it is only "OS independent" way to deal with files larger than 2GB
	int file = _open(fname, _O_RDWR | _O_CREAT | _O_BINARY | _O_TRUNC, _S_IREAD | _S_IWRITE);
	if (file < 0)	//error			
		throw std::ios::failure(
		(const char*)wxString::Format(_("Cannot create '%s'."), fname));

	return file;
}

//opens an existing file for R/W or RO (bRO == true)
//and returns the IO handle
//throws std::exceptions if an error occurs
inline /*static*/ int IOFileUtils::OpenFile(const char* fname, bool bRO) throw(...)
{
	int file = _open(fname, (bRO ? _O_RDONLY : _O_RDWR) | _O_BINARY, _S_IREAD | _S_IWRITE);

	if (file < 0)	//error			
		throw std::ios::failure(
		(const char*)wxString::Format(_("Cannot open '%s'."), fname));

	return file;
}

//writes count bytes from the buffer into the file
//throws std::exceptions if an error occurs
inline /*static*/ void IOFileUtils::WriteFile(int fhandle, void* buffer, int count) throw(...)
{
	if (_write(fhandle, buffer, count) < 0)
	{
		throw std::ios::failure(
			(errno == ENOSPC ? _("There is not enough space on disk.") :
		_("Unspecified I/O error while storing data."))
			);	
	}
}

//reads count bytes from the buffer into the file
//throws std::exceptions if an error occurs
inline /*static*/ void IOFileUtils::ReadFile(int fhandle, void* buffer, int count) throw(...)
{
	int r = _read(fhandle, buffer, count);
	if (r != count)
	{
		throw std::ios::failure((r >= 0 ?
			_("Reached the end of the file (EOF). The file is corrupted and unreadable.") :
		_("Unspecified I/O error while reading data."))
			);	
	}
}

//set the current position in file to pos
inline /*static*/ void IOFileUtils::Seek(int fhandle, unsigned long long pos) throw(...)
{
	if (_lseeki64(fhandle, (__int64)pos, SEEK_SET) < 0)
		throw std::ios::failure(_("Unable to seek in the specified file."));
}

//returns file size, throwing exception if an error occurs (e.g., file not found)
inline /*static*/ unsigned long long IOFileUtils::GetFileSize(const char* fname) throw(...)
{
	int f = _open(fname, _O_RDONLY, _SH_DENYNO);
	if (f < 0)
		throw std::ios::failure(_("Unable to open the specified file."));

	_lseeki64(f, 0, SEEK_END);
	__int64 ret = _telli64(f);
	_close(f);

	return (unsigned long long)ret;
}