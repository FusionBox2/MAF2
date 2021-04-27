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


#include "mafDefines.h" 
#include "mafDecl.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafLUTLibrary.h"

#include "mafGUILutPreset.h"

#include "vtkDirectory.h"
#include "vtkLookupTable.h"
#include "vtkMAFSmartPointer.h"

//#include <iostream>
//#include <fstream>

//using namespace std;

const bool DEBUG_MODE = false;

//----------------------------------------------------------------------------
void mafLUTLibrary::Clear(bool removeLibraryFromDisk)
{
  // destroy lib
  for(auto it = m_LutMap.begin(); it != m_LutMap.end(); ++it)
  {
    vtkLookupTable *lut = (*it).second;
    if (DEBUG_MODE)
      {
	    mafString lutName = (*it).first;
        std::ostringstream stringStream;
        stringStream << "destroying lut:" << lutName.GetCStr() << " at " << lut << std::endl;
        mafLogMessage(_M(stringStream.str().c_str()));
      }
    lut->Delete();
  }

  m_LutMap.clear();

  if (removeLibraryFromDisk)
  {
    assert(mafDirExists(m_LibraryDir));
    mafDirRemove(m_LibraryDir);
  }
}

mafLUTLibrary::mafLUTLibrary()
{
}

mafLUTLibrary::~mafLUTLibrary()
{
  Clear();
}

void mafLUTLibrary::PrintLut( std::ostringstream &stringStream, vtkLookupTable *lut )
{
  stringStream << "lut: " << std::endl;

  lut->PrintSelf(stringStream, 0);

  int n = lut->GetNumberOfTableValues();
  if(n>256) n=256;

  for(int i=0; i<n; i++)
  {
    double *rgba;
    rgba = lut->GetTableValue(i);
    stringStream << i  << '\t' << rgba[0] << '\t' << rgba[1] << '\t' << rgba[2] 
    << '\t' << rgba[3] << std::endl;
  }

}

void mafLUTLibrary::SaveLUT( vtkLookupTable *inLut, const mafString& outFileName)
{
  assert(inLut);
  vtkLookupTable *lut = inLut;

  mafString fileName = outFileName;

  std::ofstream output;

  output.open(fileName.GetCStr());

  double range[2];
  lut->GetTableRange(range);
  output << "TableRange" << '\t' << range[0] << '\t' << range[1] <<  std::endl;
  output << std::endl;

  int numberOfTableValues = lut->GetNumberOfTableValues();
  if(numberOfTableValues > 256) numberOfTableValues = 256;

  output << "NumberOfTableValues" << '\t' << numberOfTableValues << std::endl;
  output << std::endl;

  output << "id"  << '\t' << "red" << '\t' << "green" << '\t' << "blue" << '\t'
    << "alpha" << std::endl;

  for(int i=0; i<numberOfTableValues; i++)
  {
    double *rgba;
    rgba = lut->GetTableValue(i);
    output << i  << '\t' << rgba[0] << '\t' << rgba[1] << '\t' << rgba[2] 
    << '\t' << rgba[3] << std::endl;
  }

  output.close();

  assert(mafFileExists(fileName));

}

void mafLUTLibrary::LoadLUT(const mafString& lutFileName, vtkLookupTable *targetLut )
{
  assert(targetLut);
  vtkLookupTable *lut = targetLut;

  mafString fileName = lutFileName;

  std::ifstream input;

  input.open(fileName.GetCStr());

  std::string tmp;
  double range[2];
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }

  input  >> range[0] >> range[1];
  lut->SetTableRange(range);

  int numberOfTableValues;
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }
  input >> numberOfTableValues;

  lut->SetNumberOfTableValues(numberOfTableValues);


  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }
  input >> tmp;
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Skipping: " << tmp  << std::endl;
    mafLogMessage(_M(stringStream.str().c_str()));
  }

  for(int i=0; i<numberOfTableValues; i++)
  {
    int toSkip;
    double rgba[4];
    input >> toSkip;   
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Skipping: " << toSkip  << std::endl;
      mafLogMessage(_M(stringStream.str().c_str()));
    }
    input  >> rgba[0] >> rgba[1] >> rgba[2] >> rgba[3];
    lut->SetTableValue(i, rgba);
  }

  input.close();

  assert(mafFileExists(fileName));

}

void mafLUTLibrary::Add( vtkLookupTable *inputLUT, const mafString& lutName )
{

  vtkLookupTable *lut = m_LutMap[lutName];

  if (lut != NULL)
  {
    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Overwriting: " << lutName.GetCStr() << std::endl;
      mafLogMessage(_M(stringStream.str().c_str()));

      lut->Delete();
    }
  }

  vtkLookupTable *newLut = vtkLookupTable::New();
  newLut->DeepCopy(inputLUT);
  m_LutMap[lutName] = newLut;

  mafString lutFileName = m_LibraryDir + _R("/") + lutName + _R(".lut");
  SaveLUT(newLut, lutFileName);

  assert(mafFileExists(lutFileName));
}

void mafLUTLibrary::Save()
{
  mafDirMake(m_LibraryDir);
  assert(mafDirExists(m_LibraryDir));

  for(auto it = m_LutMap.begin(); it != m_LutMap.end(); ++it)
  {
    mafString s = (*it).first;
    vtkLookupTable *lut = (*it).second;
    mafString absFileName = m_LibraryDir  + _R("/") + s + _R(".lut");
    SaveLUT(lut, absFileName);
  }  
}


void mafLUTLibrary::SetDir(const mafString& dir )
{
  m_LibraryDir = dir;
}

const mafString& mafLUTLibrary::GetDir()
{
  return m_LibraryDir;
}

void mafLUTLibrary::Load()
{
  Clear();
  m_LutMap.clear();

  vtkMAFSmartPointer<vtkDirectory> vtkDir;
  int result = vtkDir->Open(m_LibraryDir.GetCStr());
  assert(result == 1);

  int numFiles = vtkDir->GetNumberOfFiles();
  for (int fileId = 0; fileId < numFiles; fileId++) 
  { 
    std::string lutLocalFileName = vtkDir->GetFile(fileId);
    size_t foundPosition;

    foundPosition=lutLocalFileName.find(".lut");

    int nameLength = lutLocalFileName.size();

    bool foundLUTExtensionInName = (foundPosition!=std::string::npos);
    bool isExtensionAtTheEndOfName = (nameLength - foundPosition)  == 4 ? true : false ;

    bool isLutFile = foundLUTExtensionInName && isExtensionAtTheEndOfName;

    if (isLutFile)
    {
      cout << "found .lut at: " << int(foundPosition) << " in " << lutLocalFileName << std::endl;
      mafString lutAbsFileName = m_LibraryDir + _R(lutLocalFileName.c_str());
      assert(mafFileExists(lutAbsFileName));

      std::string lutName = lutLocalFileName.erase(foundPosition);
      vtkLookupTable *vtkLut = vtkLookupTable::New();
      LoadLUT(lutAbsFileName, vtkLut);
      m_LutMap[_R(lutName.c_str())] = vtkLut;
    }
  }  

  for(auto it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    mafLogMessage(_M(it->first));
  }

  assert(true);
}


int mafLUTLibrary::Remove(const mafString& lutName )
{
  auto it = m_LutMap.find(lutName);

  if (it == m_LutMap.end())
  {
    cout << "The LUT lib doesn't have an element "
      << "with key: " << lutName.GetCStr() << std::endl;
    return MAF_ERROR;
  }
  else
  {
    cout << "removing element: " << lutName.GetCStr() << std::endl;
    vtkLookupTable *lut = m_LutMap[lutName];
    lut->Delete();
    m_LutMap.erase(lutName);
    RemoveLUTFromDisk(lutName);
    return MAF_OK;
  } 
}

void mafLUTLibrary::RemoveLUTFromDisk(const mafString& lutName)
{
  mafString lutFileName = m_LibraryDir + _R("/") + lutName + _R(".lut");
  assert(mafFileExists(lutFileName));
  mafFileRemove(lutFileName);
}

int mafLUTLibrary::GetNumberOfLuts()
{
  return m_LutMap.size();
}

std::vector<mafString> mafLUTLibrary::GetLutNames()
{
  std::vector<mafString> names;
  for(auto it = m_LutMap.begin(); it != m_LutMap.end(); it++)
  {
    names.push_back(it->first);
  }
  return names;
}

vtkLookupTable *mafLUTLibrary::GetLutByName(const mafString& name)
{
  if (HasLut(name))
  {
    return m_LutMap[name];
  } 
  else
  {
    return NULL;
  }
}

bool mafLUTLibrary::HasLut(const mafString& name)
{
  auto it = m_LutMap.find(name);

  if (it == m_LutMap.end())
  {
    cout << "The LUT lib doesn't have an element "
      << "with key: " << name.GetCStr() << std::endl;
    return false;
  }
  else
  {
    return true;
  }
}

