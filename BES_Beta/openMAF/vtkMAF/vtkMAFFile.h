/**=======================================================================
  
  File:    	 vtkMAFFile.h
  Language:  C++
  Date:      11:2:2008   12:36
  Version:   $Revision: 1.2 $
  Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)
  
  Copyright (c) 2008
  University of Bedfordshire
=========================================================================
This class provides a platform independent support for reading/writing
files larger than 2GB that are not supported by C++ iostream classes 
under _WIN32 (although they are supported for _WIN64)
=========================================================================*/

#include <stdio.h>
#if defined(_MSC_VER) && _MSC_VER < 1400
//MS Visual Studio 2003 does not define _fseeki64 and _ftelli64 in
//stdio.h but in private src/internal.h file
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
int __cdecl _fseeki64(FILE *, __int64, int);
__int64 __cdecl _ftelli64(FILE *);
#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif

#include <errno.h>
#include "vtkObject.h"

class VTK_COMMON_EXPORT vtkMAFFile : vtkObject
{
protected:
  FILE* m_pFile;    //associated FILE

public:
  vtkTypeRevisionMacro(vtkMAFFile, vtkObject);
  static vtkMAFFile* New();

protected:
  vtkMAFFile() {
    m_pFile = NULL;
  }

  ~vtkMAFFile() {
    Close();
  }

private:
  vtkMAFFile(const vtkMAFFile&);  // Not implemented.
  void operator = (const vtkMAFFile&);  // Not implemented.	


public:
  //creates a new file
  //throws std::exceptions if an error occurs
  void Create(const char* fname) throw(...);

  //opens an existing file for R/W or RO (bRO == true)  
  //throws std::exceptions if an error occurs
  void Open(const char* fname, bool bRO = true) throw(...);

	//reads count bytes from the file into buffer
	//throws std::exceptions if an error occurs
	inline void Read(void* buffer, int count) throw(...);

	//writes count bytes from the buffer into the file
	//throws std::exceptions if an error occurs
	inline void Write(void* buffer, int count) throw(...);

	//closes the file (with no exception!) and invalidates it
	inline void Close();

	//set the current position in file to a new location according to
  //pos and origin. If origin is SEEK_CUR - pos is given as the distance 
  //from the current position, SEEK_END - pos is the distance from the 
  //end of file. SEEK_SET (default) - from the beginning of file
	inline void Seek(long long pos, int origin = SEEK_SET) throw(...);

  //returns the current position in file, throws an exception if an error occurs
  inline unsigned long long GetCurrentPos() throw(...);

  //returns the current file length, throws an exception if an error occurs
  unsigned long long GetFileSize() throw(...);

	//returns file size, throwing exception if an error occurs (e.g., file not found)
	static unsigned long long GetFileSize(const char* fname) throw(...);
};


//writes count bytes from the buffer into the file
//throws std::exceptions if an error occurs
inline void vtkMAFFile::Write(void* buffer, int count) throw(...)
{
	if (fwrite(buffer, 1, count, m_pFile) != count)
	{
		throw std::ios::failure(
			(errno == ENOSPC ? ("There is not enough space on disk.") :
		  ("Unspecified I/O error while storing data."))
			);	
	}
}

//reads count bytes from the file into the buffer
//throws std::exceptions if an error occurs
inline void vtkMAFFile::Read(void* buffer, int count) throw(...)
{
	int r = fread(buffer, 1, count, m_pFile);
	if (r != count)
	{
		throw std::ios::failure((feof(m_pFile) ?
			("Reached the end of the file (EOF). The file is corrupted and unreadable.") :
		  ("Unspecified I/O error while reading data."))
			);	
	}
}

//closes the file (with no exception!) and invalidates it
inline void vtkMAFFile::Close()
{
  if (m_pFile != NULL)
  {
    fclose(m_pFile);
    m_pFile = NULL;
  }
}

//set the current position in file to pos
inline /*static*/ void vtkMAFFile::Seek(long long pos, int origin) throw(...)
{
#ifdef _WIN32
  int res = _fseeki64( m_pFile, (__int64)pos, origin );
#else
  int res = fseeko64( m_pFile, (off64_t)pos, origin );
#endif

	if (res < 0)
		throw std::ios::failure(("Unable to seek in the specified file."));
}

//returns the current position in file, throws an exception if an error occurs
inline unsigned long long vtkMAFFile::GetCurrentPos() throw(...)
{
#ifdef _WIN32
  __int64 res = _ftelli64( m_pFile);
  if (res < 0)
#else
  fpos64_t res;  
  if (fgetpos64(m_pFile, &res) < 0)  
#endif  
    throw std::ios::failure(("Unable to seek in the specified file."));

  return (unsigned long long)res;
}