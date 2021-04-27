/*=========================================================================

 Program: MAF2
 Module: mafLUTLibrary
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafLUTLibrary_H__
#define __mafLUTLibrary_H__

#include "mafString.h"
#include "vtkMAFSmartPointer.h"
#include "vtkLookupTable.h"

#include <map>
#include <vector>

/** A component to handle lookup table libraries */
class MAF_EXPORT mafLUTLibrary
{
public:
  mafLUTLibrary();
  ~mafLUTLibrary();

  /** set get the library directory */
  void SetDir(const mafString& dir);
  const mafString& GetDir();

  /** load lut library */
  void Load();

  /** save lut library */
  void Save();

  /** beware! if a lut with the same name exists it will be replaced */
  void Add(vtkLookupTable *inLUT, const mafString& lutName);

  /** remove a lut from the library: return MAF_ERROR  if lutName does not exist, 
  MAF_OK otherwise */
  int Remove(const mafString& lutName);

  /** return the number of luts */
  int GetNumberOfLuts();

  /** return the lut names vector */
  std::vector<mafString> GetLutNames();

  /** get a lut by name */
  vtkLookupTable *GetLutByName(const mafString& name);
  
  /** return if the named lut is present or not  */
  bool HasLut(const mafString& name);

  /** Clear the library; do not remove it from disk unless removeLibraryFromDisk is true */
  void Clear(bool removeLibraryFromDisk = false);

  void PrintLut( std::ostringstream &stringStream, vtkLookupTable *lut );

private:
  std::map<mafString, vtkLookupTable *> m_LutMap;
  mafString m_LibraryDir;

  void RemoveLUTFromDisk(const mafString& lutName);
  void LoadLUT(const mafString& inLutFileName, vtkLookupTable *targetLut);
  void SaveLUT( vtkLookupTable *inLut, const mafString& outFileName);
};
#endif
