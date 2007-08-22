/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMtrLMCReader.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-22 14:01:40 $
  Version:   $Revision: 1.1 $
  Authors:   Fedor Moiseev / Vladik Aranov
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

#include "mafMTRLMCReader.h"

#include "vtkSystemIncludes.h"

//----------------------------------------------------------------------------
mafMTRLMCReader* mafMTRLMCReader::New() 
//----------------------------------------------------------------------------
{ 
  return new mafMTRLMCReader; 
} 

// Construct object with merging set to true.
//----------------------------------------------------------------------------
mafMTRLMCReader::mafMTRLMCReader()
//----------------------------------------------------------------------------
{
  m_FileName   = NULL;
  m_Set        = 0;
  m_PointsRead = 0;
  m_PointShift = 20.5;
  m_Radius     = 3.0;
}

//----------------------------------------------------------------------------
mafMTRLMCReader::~mafMTRLMCReader()
//----------------------------------------------------------------------------
{
  if(this->m_FileName)
  {
    delete [] this->m_FileName;
    this->m_FileName = NULL;
  }
  for(int i = 0; i < m_PointSet.size(); i++)
    mafDEL(m_PointSet[i]);
}


//void mafMTRLMCReader::PrintSelf(ostream& os, vtkIndent indent)
//{
//
//  os << indent << "File Name: " 
//     << (this->m_FileName ? this->m_FileName : "(none)") << "\n";
//  
//}

//----------------------------------------------------------------------------
void mafMTRLMCReader::Execute()
//----------------------------------------------------------------------------
{
  FILE *fp;

  if(!this->m_FileName)
    return;

  // Initialize
  fp = fopen(m_FileName, "rt");
  if(fp == NULL)
    return;

  //Depending upon file type, read differently
  ReadASCIIMTR(fp);
  fclose(fp);
  return;
}

#define MAX_LINE 1000
//----------------------------------------------------------------------------
int mafMTRLMCReader::ReadASCIIMTR(FILE *fp)
//----------------------------------------------------------------------------
{
  double    x[3];
  wxInt32   nI, nJ;
  char      sLine[MAX_LINE];
  wxInt32   nPrevNumber = -1;
  wxInt32   nCurNumber;
  char      *pRet;
  int       nSet = 0; 
  double    rTemp;

  //just read them line by line for first uncommented string
  nI = 0;
  while(TRUE)
  {
    pRet = fgets(sLine, MAX_LINE, fp);
    if(pRet == NULL)
      return FALSE;
    if(sLine[0] != '#')
      break;
    nI++;
  }

  //parse first uncommented line 
  //sscanf(sLine, "%d %d\n", &nPointsNumber, &nBonesNumber);
  
  pRet = fgets(sLine, MAX_LINE, fp);
  if(pRet == NULL)
    return FALSE;
  if(sscanf(sLine, "%lf", &rTemp) != 1)
    return FALSE;

  if(m_Set != mafMTRLMCReader::SetNotDefined)
  {
    //look for start of our set
    for(nJ = 0; ; nJ++)
    {
      nCurNumber = (int)(rTemp);
      if(nCurNumber <= nPrevNumber)
        nSet++;
      nPrevNumber = nCurNumber;
      if(nSet == m_Set)
        break;

      pRet = fgets(sLine, MAX_LINE, fp);
      if(pRet == NULL)
        return FALSE;
      if(sscanf(sLine, "%lf", &rTemp) != 1)
        return FALSE;
    }
  }

  m_PointSet.resize(1);
  mafNEW(m_PointSet[0]);
  m_PointSet[0]->Open();
  m_PointSet[0]->SetName("Tempora memorie");
  m_PointSet[0]->SetRadius(m_Radius);

  
  //look for start of our set
  for(; ; nJ++)
  {
    int cloudIndex = m_PointSet.size() - 1;
    //account prereaded line
    sscanf(sLine, "%lf %lf %lf %lf", &rTemp, x, x+1, x+2);

    wxString number;
    number.Printf("%1.0lf", rTemp);
    int ret = m_PointSet[cloudIndex]->SetNumberOfLandmarks(m_PointSet[cloudIndex]->GetNumberOfLandmarks() + 1);
    if(ret==MAF_OK)
    {
      m_PointSet[cloudIndex]->SetLandmarkName(m_PointSet[cloudIndex]->GetNumberOfLandmarks()-1,number);
      m_PointSet[cloudIndex]->SetLandmark(m_PointSet[cloudIndex]->GetNumberOfLandmarks()-1,*x, *(x+1), *(x+2));
      m_PointSet[cloudIndex]->SetRadius(m_Radius);
    }
    else
    {
       //vtkErrorMacro("Cannot add new landmark");
    }
    m_PointsRead++;

    //next line
    pRet = fgets(sLine, MAX_LINE, fp);
    if(pRet == NULL)
      return TRUE;

    //analyze
    if(sscanf(sLine, "%lf", &rTemp) != 1)
      return TRUE;

    //and not take more
    nCurNumber = (int)(rTemp);
    if(nCurNumber < nPrevNumber)
    {
      nSet++;
      if(m_Set != mafMTRLMCReader::SetNotDefined)
        break;
      m_PointSet.resize(m_PointSet.size() + 1);
      mafNEW(m_PointSet[m_PointSet.size() - 1]);
      m_PointSet[m_PointSet.size() - 1]->Open();
      m_PointSet[m_PointSet.size() - 1]->SetName("Tempora memorie");
      m_PointSet[m_PointSet.size() - 1]->SetRadius(3);

    }
    nPrevNumber = nCurNumber;
  }
  return TRUE;
}
