/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMtrReader.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-05 14:15:36 $
  Version:   $Revision: 1.0 $
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

#include "mafMTRReader.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkMergePoints.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "assert.h"
#include <vector>
#include "mafVectors.h"
#include <forarray.h>
#include <splines.h>
#include "createSplineSurf.h"
//#include "mafEvent.h"

#define wxInt32    int    signed
#define DIASSERT(c) if (!c) {vtkErrorMacro(<<"A FileName must be specified.");}

vtkCxxRevisionMacro(mafMTRReader, "$Revision: 1.1 $");
vtkStandardNewMacro(mafMTRReader);


// Construct object with merging set to true.
mafMTRReader::mafMTRReader()
{
  m_FileName    = NULL;
  m_Set         = 0; 
  m_PointsRead  = 0;
  m_PointShift  = 20.5; 
  m_linesetMode = true;
  m_tendonMode  = false;
}

mafMTRReader::~mafMTRReader()
{
  if (this->m_FileName)
  {
    delete [] this->m_FileName;
    this->m_FileName = NULL;
  }
}


void mafMTRReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "File Name: " 
     << (this->m_FileName ? this->m_FileName : "(none)") << "\n";
  
}

void mafMTRReader::Execute()
{
  FILE         *fp;
  vtkPoints    *newPts;
  vtkCellArray *newCells;
  vtkPolyData  *output = this->GetOutput();
  
  // All of the data in the first piece.
  if(output->GetUpdatePiece() > 0)
  {
    return;
  }
  
  if(!this->m_FileName)
  {
    vtkErrorMacro(<<"A FileName must be specified.");
    return;
  }

  // Initialize
  //
  fp = fopen(m_FileName, "rt");
  if(fp == NULL)
  {
    vtkErrorMacro(<< "File " << this->m_FileName << " not found");
    return;
  }

  newPts = vtkPoints::New();
  newPts->Allocate(5000,10000);
  newCells = vtkCellArray::New();
  newCells->Allocate(10000,20000);

  // Depending upon file type, read differently
  //
  if(!m_linesetMode && m_Set != mafMTRReader::SetNotDefined)
    return;
  if(ReadASCIIMTR(fp,newPts,newCells) == FALSE)
  {
    return;
  }
  vtkDebugMacro(<< "Read: " 
  << newPts->GetNumberOfPoints() << " points, "
  << newCells->GetNumberOfCells() << " elements");

  fclose(fp);
  
//
// Update ourselves
//
  output->SetPoints(newPts);
  output->Update();
  if(m_linesetMode)
    output->SetLines(newCells);
  else
  {
    output->SetPolys(newCells);
    output->Squeeze();
  }
  output->Modified();
  output->Update();
  newPts->Delete();
  newCells->Delete();
}

static void _addSegments(const std::vector<V3d<double> >& coords, unsigned from, unsigned to, vtkPoints *pnts, vtkCellArray  *cells)
{
  vtkIdType     pts[2];
  if(to <= from || to > coords.size())
    return;
  for(unsigned i = from; i < to; i++)
    pnts->InsertNextPoint(coords[i].components);
  for(unsigned i = from + 1; i < to; i++)
  {
    pts[0] = pnts->GetNumberOfPoints() - (to - i) - 1;
    pts[1] = pts[0] + 1;
    cells->InsertNextCell(2, pts);
  }
}


#define MAX_LINE 1000
int mafMTRReader::ReadASCIIMTR(FILE *fp, vtkPoints *newPts, vtkCellArray *newPolys)
{
  V3d<double>              x;
  vtkIdType                pts[3];
  wxInt32                  nI, nJ;
  char                     sLine[MAX_LINE];
  std::vector<std::vector<V3d<double> >*> allValues;
  std::vector<V3d<double> > coords;
  std::vector<V3d<double> > smoothed;
  wxInt32                  nPrevNumber = -1;
  wxInt32                  nCurNumber;
  char                     *pRet;
  int                      nSet = 0; 
  double                   rTemp;
  bool                     newFormat = false;

  vtkDebugMacro(<< " Reading ASCII MTR file");

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
  if(strstr(sLine, "*** OIF Ph3D ***") != NULL)
    newFormat = true;

  pRet = fgets(sLine, MAX_LINE, fp);
  if(pRet == NULL)
    return FALSE;
  if(sscanf(sLine, "%lf", &rTemp) != 1)
    return FALSE;
  
  if(m_Set != mafMTRReader::SetNotDefined)
  {
    if(!m_linesetMode)
      return false;
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
    for(; ; nJ++)
    {
      //account prereaded line
      sscanf(sLine, "%lf %lf %lf %lf", &rTemp, x.components, x.components+1, x.components+2);

      wxString number;
      number.Printf("%1.0lf", rTemp);
      newPts->InsertNextPoint(x.components);
      m_PointsRead++;

      //next line
      int scanRes = 0;
      pRet = fgets(sLine, MAX_LINE, fp);
      if(pRet != NULL)
        scanRes = sscanf(sLine, "%lf", &rTemp);
      if(pRet != NULL && scanRes == 1)
        nCurNumber = (int)(rTemp);

      //and not take more
      if(pRet == NULL || scanRes != 1 || nCurNumber < nPrevNumber)
      {
        break;
      }
      nPrevNumber = nCurNumber;
    }
    for(unsigned i = 1; i < newPts->GetNumberOfPoints(); i++)
    {
      pts[0] = i - 1;
      pts[1] = pts[0] + 1;
      newPolys->InsertNextCell(2, pts);
    }
    return true;
  }

  coords.clear();
  //look for start of our set
  bool tendon = true;
  bool ori = false;
  bool ins = false;
  unsigned from   = 0;
  for(; ; )
  {
    bool                OriIns;
    //account prereaded line
    sscanf(sLine, "%lf %lf %lf %lf", &rTemp, x.components, x.components+1, x.components+2);
    if(newFormat)
    {
      bool o = (strstr(sLine, "*** Ori") != NULL);
      bool i = (strstr(sLine, "*** Ins") != NULL);
      bool f = (strstr(sLine, "*** Fbr") != NULL);
      if(o || i || f)
      {
        ori = o;
        ins = i;
      }
    }
    else
    {
      ori = (nSet == 0);
      ins = (nSet == 1);
    }
    OriIns = ori || ins;

    wxString number;
    number.Printf("%1.0lf", rTemp);

    if(coords.size() == 0 || ((coords[coords.size() - 1] | x) > 4.0) || OriIns)
      coords.push_back(x);
    else if(coords.size() > 0)
    {
      if(m_linesetMode && m_tendonMode == tendon)
        _addSegments(coords, from, coords.size(), newPts, newPolys);
      from = coords.size() - 1;
      tendon = !tendon;
    }
    m_PointsRead++;

    //next line
    int scanRes = 0;
    pRet = fgets(sLine, MAX_LINE, fp);
    if(pRet != NULL)
      scanRes = sscanf(sLine, "%lf", &rTemp);
    if(pRet != NULL && scanRes == 1)
      nCurNumber = (int)(rTemp);

    //and not take more
    if(pRet == NULL || scanRes != 1 || nCurNumber < nPrevNumber)
    {
      if(!OriIns)
      {
        std::vector<V3d<double> > *arr = new std::vector<V3d<double> >(coords);
        allValues.push_back(arr);
      }
      if(m_linesetMode && m_tendonMode == tendon)
        _addSegments(coords, from, coords.size(), newPts, newPolys);
      coords.clear();
      tendon = true;
      from   = 0;
      nSet++;
      if(pRet == NULL || scanRes != 1 || m_Set != mafMTRReader::SetNotDefined)
        break;
    }
    nPrevNumber = nCurNumber;
  }
  bool validPoints = true;
  if(allValues.size() <= 1)
    validPoints = false;
  for(unsigned i = 0; i < allValues.size(); i++)
  {
    if(allValues[i] == NULL || allValues[i]->size() <= 1)
    {
      validPoints = false;
      break;
    }
  }
  if(!m_linesetMode && validPoints)
  {
    produceRegularGrid(allValues, 10, 0.0, 2, true, smoothed);
    createSurface(smoothed, allValues.size(), 10, 2, 2, 10, 10, 0.0, 0.0, newPts, newPolys);
  }
  for(unsigned i = 0; i < allValues.size(); i++)
    delete allValues[i];

  return TRUE;
}

