/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDictionary.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "mafDictionary.cpp"
#endif

#include "wx/wxprec.h"
#include "wx/textfile.h"

#include "mafDictionary.h"

namespace
{
  //----------------------------------------------------------------------------
  void  ParseString(wxString& pFirstLine, wxString& sOne, wxString& sTwo)
    //----------------------------------------------------------------------------
  {
    wxInt32 nJ, nK;
    wxInt32 length = pFirstLine.Length();

    sOne = "";
    sTwo = "";

    //skip to first word
    for (nJ = 0; nJ < length; nJ++)
    {
      if (pFirstLine[nJ] != ' ' && pFirstLine[nJ] != '\t')//if(pFirstLine[nJ] == '\"')
      {
        break;
      }
    }
    if (nJ == length)
      return;
    if (pFirstLine[nJ] != '\"')
    {
      //skip first word
      for (nK = nJ; nK < length; nK++)
      {
        if (pFirstLine[nK] == ' ' || pFirstLine[nK] == '\t')
        {
          break;
        }
      }
    }
    else
    {
      nJ++;
      //skip first word
      for (nK = nJ; nK < length; nK++)
      {
        if (pFirstLine[nK] == '\"')
          break;
      }
    }
    sOne = pFirstLine.Mid(nJ, nK - nJ);
    if (nK == length)
      return;
    if (pFirstLine[nK] == '\"')
      nK++;


    //skip to second word
    for (nJ = nK; nJ < length; nJ++)
    {
      if (pFirstLine[nJ] != ' ' && pFirstLine[nJ] != '\t')//if(pFirstLine[nJ] == '\"')
      {
        break;
      }
    }


    if (nJ == length)
      return;
    if (pFirstLine[nJ] != '\"')
    {
      //skip first word
      for (nK = nJ; nK < length; nK++)
      {
        if (pFirstLine[nK] == ' ' || pFirstLine[nK] == '\t')
        {
          break;
        }
      }
    }
    else
    {
      nJ++;
      //skip first word
      for (nK = nJ; nK < length; nK++)
      {
        if (pFirstLine[nK] == '\"')
          break;
      }
    }
    sTwo = pFirstLine.Mid(nJ, nK - nJ);
  }
}

//----------------------------------------------------------------------------
bool ReadDictionary(mafString *fileName, std::vector<std::pair<mafString, mafString> >&  dictionary)
//----------------------------------------------------------------------------
{
  wxTextFile   *pFile;
  wxInt32      nI; 
  wxString     sFirstName("");
  wxString     sSecondName("");

  pFile = new wxTextFile(fileName->toWx());

  if(pFile == NULL)
  {
    return false;
  }
  pFile->Open();
  if(!pFile->IsOpened())
  {
    cppDEL(pFile);
    return false;
  }

  //clean up old data if any
  dictionary.clear();

  for(nI = 0; nI < pFile->GetLineCount(); )
  {
    wxString &pFirstLine = pFile->GetLine(nI);
    //match it as beginning of block
    if(pFirstLine == "" || pFirstLine[0] == '#')
    {
      nI++;
      continue;
    }    

    ParseString(pFirstLine, sFirstName, sSecondName);

    if(sFirstName == "" || sSecondName == "")
    {
      //consider string in invalid
      mafWarningMessage(_M(_R("Syntax error in file ") + *fileName + mafString::Format(_R(", line %d. Ignoring."), nI + 1)));
      nI++;
      continue;
    }
    //just add to dictionary
    dictionary.push_back(std::make_pair(mafWxToString(sFirstName), mafWxToString(sSecondName)));// Add(pEntry);
    // to next
    nI++;
  }
  pFile->Close();
  cppDEL(pFile); 
  return true;
}

//----------------------------------------------------------------------------
mafString const *LookupStdName(const mafString& name, std::vector<std::pair<mafString, mafString> >&  dictionary)
//----------------------------------------------------------------------------
{
  wxInt32      nI; 

  for(nI = 0; nI < dictionary.size(); nI++)
  {
    if(dictionary[nI].second == name)
    {
      return &dictionary[nI].first;
    }
    //already a ref one
    if(dictionary[nI].first == name)
    {
      return &dictionary[nI].first;
    }
  }
  //failed lookup
  return NULL;
}

//----------------------------------------------------------------------------
mafString const* LookupUserName(const mafString& name, std::vector<std::pair<mafString, mafString> >& dictionary)
//----------------------------------------------------------------------------
{
  wxInt32      nI; 

  for(nI = 0; nI < dictionary.size(); nI++)
  {
    if(dictionary[nI].first == name)
    {
      return &dictionary[nI].second;
    }
    //already a ref one
    if(dictionary[nI].second == name)
    {
      return &dictionary[nI].second;
    }
  }
  //failed lookup
  return NULL;
}
