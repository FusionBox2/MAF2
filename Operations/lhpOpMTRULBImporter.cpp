/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpMTRULBImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-05 14:19:02 $
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

#include "lhpOpMTRULBImporter.h"

#include "mafDecl.h"

#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMESurface.h"
#include "mafVMEPolyline.h"
#include "mafVMEGroup.h"
#include "mafMatrixVector.h"
#include "mafINPReader.h"
#include "mafMtrLMCReader.h"
#include "mafMtrReader.h"

#include "vtkMatrix4x4.h"
#include "vtkPolyData.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpMTRULBImporter);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
 lhpOpMTRULBImporter:: lhpOpMTRULBImporter(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  //m_File    = "";
  m_Canundo  = true;
  //m_Group    = NULL;

  m_FileDir = mafGetApplicationDirectory();
}
//----------------------------------------------------------------------------
 lhpOpMTRULBImporter::~ lhpOpMTRULBImporter()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Groups.size(); i++)
    mafDEL(m_Groups[i]);
}  
//----------------------------------------------------------------------------
mafOp * lhpOpMTRULBImporter::Copy()
//----------------------------------------------------------------------------
{
  lhpOpMTRULBImporter *cp = new  lhpOpMTRULBImporter(GetLabel());
  cp->m_Files           = m_Files;
  cp->m_FileDir         = m_FileDir;
  return cp; 
}
//----------------------------------------------------------------------------
void  lhpOpMTRULBImporter::OpRun()   
//----------------------------------------------------------------------------
{
  mafString vrml_wildc  = _R("MTR FARO data (*.mtr)|*.mtr|MTR File list (*.dat)|*.dat");

  m_Files.clear();
  //if (m_File.IsEmpty())
  {
    mafGetOpenMultiFiles(m_FileDir,vrml_wildc, m_Files);
  }

  int result = OP_RUN_CANCEL;

  if(m_Files.size() != 0) 
  {
    result = OP_RUN_OK;
    ImportData();
  }

  mafEventMacro(mafEvent(this,result));
}

void lhpOpMTRULBImporter::ReadMatrix(const char *fstring, vtkMatrix4x4 *mat)
{

  if(mat == NULL)
    return;
  mat->Identity();
  float x[12];
  sscanf(fstring, "%E %E %E %E %E %E %E %E %E %E %E %E", 
                  &x[0], &x[1], &x[2], &x[3], &x[4], &x[5], 
                  &x[6], &x[7], &x[8], &x[9], &x[10], &x[11]);

  mat->SetElement(0, 3, x[3]);
  mat->SetElement(1, 3, x[7]);
  mat->SetElement(2, 3, x[11]);

  mat->SetElement(0, 0, x[0]);
  mat->SetElement(1, 0, x[4]);
  mat->SetElement(2, 0, x[8]);

  mat->SetElement(0, 1, x[1]);
  mat->SetElement(1, 1, x[5]);
  mat->SetElement(2, 1, x[9]);

  mat->SetElement(0, 2, x[2]);
  mat->SetElement(1, 2, x[6]);
  mat->SetElement(2, 2, x[10]);
  return;
}



//----------------------------------------------------------------------------
void  lhpOpMTRULBImporter::ProcessSingleFile(const mafString &fileName)
//----------------------------------------------------------------------------
{

  mafString vmeName;
  //wxBusyInfo wait("Loading file: ...");  
  //assert(!m_vme);
  mafString path;
  wxInt32  nI = 0;
  mafString grpName, extension;
  mafVMEGroup *grp;

  mafSplitPath(fileName,&path,&grpName,&extension);

  mafNEW(grp);
  grp->SetName(grpName);
  m_Groups.push_back(grp);

  while(TRUE)
  {
    mafMTRLMCReader *LMCReader = mafMTRLMCReader::New();
    mafMTRReader    *readermc  = mafMTRReader::New();
    mafMTRReader    *readertn  = mafMTRReader::New();
    mafMTRReader    *readerSf  = mafMTRReader::New();
    LMCReader->SetFileName(fileName.GetCStr());
    readermc->SetFileName(fileName.GetCStr());
    readertn->SetFileName(fileName.GetCStr());
    readerSf->SetFileName(fileName.GetCStr());
    mafString name, ext;

    mafSplitPath(fileName,&path,&name,&ext);

    LMCReader->SetSet(mafMTRLMCReader::SetNotDefined);
    readermc->SetSet(mafMTRReader::SetNotDefined);
    readertn->SetSet(mafMTRReader::SetNotDefined);
    readerSf->SetSet(mafMTRReader::SetNotDefined);
    readermc->SetMode(true);
    readertn->SetMode(true);
    readerSf->SetMode(false);
    readermc->SetTendonMode(false);
    readertn->SetTendonMode(true);
    LMCReader->Execute();
    readermc->Update();
    readertn->Update();
    readerSf->Update();

    if(LMCReader->GetPointsRead() == 0)
    {
      vtkDEL(readertn);
      vtkDEL(readermc);
      vtkDEL(readerSf);
      cppDEL(LMCReader);
      break;
    }

    const std::vector<std::pair<mafVMELandmarkCloud*, int> >& clouds = LMCReader->GetClouds();
    int ori = 0;
    int ins = 0;
    int fib = 0;
    for(int i = 0; i < clouds.size(); i++)
    {
      int curnumber;
      mafVMELandmarkCloud *cloud;
      cloud = clouds[i].first;
      if(clouds[i].second == 0)
      {
        vmeName   = _R("Fbr_");
        curnumber = ++fib;
      }
      else if(clouds[i].second == 1)
      {
        vmeName   = _R("Ori_");
        curnumber = ++ori;
      }
      else if(clouds[i].second == 2)
      {
        vmeName   = _R("Ins_");
        curnumber = ++ins;
      }

      mafString  curNumberStr = mafToString(curnumber);
      vmeName = vmeName + curNumberStr;
      vmeName += _R("_"); 
      vmeName += name;


      mafTagItem tag_Nature;
      tag_Nature.SetName(_R("VME_NATURE"));
      tag_Nature.SetValue(_R("NATURAL"));

      cloud->SetName(vmeName);
      cloud->GetTagArray()->SetTag(tag_Nature);
      cloud->Close();
      cloud->ReparentTo(grp);
    }
    cppDEL(LMCReader);
    {
      mafVMEPolyline *lines;
      mafTimeStamp t;
      vtkPolyData *data = readertn->GetOutput();
      //data->Update();
      if(data->GetNumberOfPoints() != 0 && data->GetNumberOfCells() != 0)
      {
        t = ((mafVME *)m_Input)->GetTimeStamp();
        mafNEW(lines);
        lines->SetName(_R("TndFbr_") + name);
        lines->SetData(data,t);


        mafTagItem tag_Nature;
        tag_Nature.SetName(_R("VME_NATURE"));
        tag_Nature.SetValue(_R("NATURAL"));

        lines->GetTagArray()->SetTag(tag_Nature);

        lines->ReparentTo(grp);

        mafDEL(lines);
      }
      vtkDEL(readertn);
    }
    {
      mafVMEPolyline *lines;
      mafTimeStamp t;
      vtkPolyData *data = readermc->GetOutput();
      //data->Update();
      if(data->GetNumberOfPoints() != 0 && data->GetNumberOfCells() != 0)
      {
        t = ((mafVME *)m_Input)->GetTimeStamp();
        mafNEW(lines);
        lines->SetName(_R("MscFbr_") + name);
        lines->SetData(data,t);


        mafTagItem tag_Nature;
        tag_Nature.SetName(_R("VME_NATURE"));
        tag_Nature.SetValue(_R("NATURAL"));

        lines->GetTagArray()->SetTag(tag_Nature);

        lines->ReparentTo(grp);

        mafDEL(lines);
      }
      vtkDEL(readermc);
    }
    {
      mafVMESurface *surf;
      mafTimeStamp t;
      vtkPolyData *data = readerSf->GetOutput();
      //data->Update();
      if(data->GetNumberOfPoints() != 0 && data->GetNumberOfCells() != 0)
      {
        t = ((mafVME *)m_Input)->GetTimeStamp();
        mafNEW(surf);
        surf->SetName(_R("Surf_") + name);
        surf->SetData(data,t);


        mafTagItem tag_Nature;
        tag_Nature.SetName(_R("VME_NATURE"));
        tag_Nature.SetValue(_R("NATURAL"));

        surf->GetTagArray()->SetTag(tag_Nature);

        surf->ReparentTo(grp);

        mafDEL(surf);
      }
      vtkDEL(readerSf);
    }
    break;
  }

  //read inputs
  FILE         *fp;
  fp = fopen(fileName.GetCStr(), "rt");
  if(fp == NULL)
  {
    mafEventMacro(mafEvent(this, VME_ADD, grp));
    return;
  }


  wxInt32 const maxStrLen = 1000;
  char      sLine[maxStrLen];
  char      *pRet;
  wxInt32   nPointsNumber = 0;
  wxInt32   nBonesNumber = 0;

  while(TRUE)
  {
    pRet = fgets(sLine, maxStrLen, fp);
    if(pRet == NULL)
    {
      fclose(fp);
      mafEventMacro(mafEvent(this, VME_ADD, grp));
      return;
    }
    if(sLine[0] != '#')
      break;
  }

  if(2 != sscanf(sLine, "%d %d\n", &nPointsNumber, &nBonesNumber))
  {
    fclose(fp);
    mafEventMacro(mafEvent(this, VME_ADD, grp));
    return;
  }
  //skip points
  for(nI = 0; nI < nPointsNumber; nI++)
  {
    pRet = fgets(sLine, maxStrLen, fp);
  }
  //read bones
  for(nI = 0; nI < nBonesNumber; nI++)
  {
    pRet = fgets(sLine, maxStrLen, fp);
    mafINPReader *reader = mafINPReader::New();
    //mafProgressMacro(reader,"reading file");
    wxString inpName(sLine);

    inpName.Truncate(inpName.length() - 1);
    inpName = path.toWx() + "\\" + inpName;
    reader->SetFileName(inpName);
    mafString name, ext, inpPath;
    mafSplitPath(mafWxToString(inpName), &inpPath,&name,&ext);

    reader->Update();

    mafVMESurface *surface;
    mafTimeStamp t;
    t = ((mafVME *)m_Input)->GetTimeStamp();
    mafNEW(surface);
    surface->SetName(name);
    vtkPolyData *data = reader->GetOutput();
    surface->SetData(data,t);


    mafTagItem tag_Nature;
    tag_Nature.SetName(_R("VME_NATURE"));
    tag_Nature.SetValue(_R("NATURAL"));

    surface->GetTagArray()->SetTag(tag_Nature);

    surface->ReparentTo(grp);

    vtkDEL(reader);

    std::string sLinedMatrix;
    for(int nJ = 0; nJ < 3; nJ++)
    {
      pRet = fgets(sLine, maxStrLen, fp);
      sLinedMatrix += sLine;
    }

    vtkMatrix4x4 *idvtkMat = NULL;
    vtkNEW(idvtkMat);
    ReadMatrix(sLinedMatrix.c_str(), idvtkMat);
    surface->SetAbsMatrix(idvtkMat, t);
    vtkDEL(idvtkMat);
    mafDEL(surface);
  }
  fclose(fp);
  mafEventMacro(mafEvent(this, VME_ADD, grp));
}

//----------------------------------------------------------------------------
void  lhpOpMTRULBImporter::ImportData()
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Groups.size(); i++)
    mafDEL(m_Groups[i]);
  m_Groups.clear();

  for(unsigned i = 0; i < m_Files.size(); i++)
  {
    if(m_Files[i].IsEmpty())
      continue;


    mafString name, ext;
    mafString path;
    mafSplitPath(m_Files[i],&path,&name,&ext);

    if(ext == _R("mtr"))
    {
      ProcessSingleFile(m_Files[i]);
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      continue;
    }
    FILE         *fp;
    fp = fopen(m_Files[i].GetCStr(), "rt");
    if(fp == NULL)
    {
      return;
    }
    wxInt32 const maxStrLen = 1000;
    char      sLine[maxStrLen];
    char      sfname[1024];
    char      *pRet;

    while((pRet = fgets(sLine, maxStrLen, fp)) != NULL)
    {
      sscanf(sLine, "%s\n", sfname);
      mafString fname(path);

      //m_vme = NULL;

      fname = path + _R("\\") + _R(sfname);
      ProcessSingleFile(fname);
    }
    fclose(fp);

    mafEventMacro(mafEvent(this,CAMERA_UPDATE));

  }
  return;
}

//----------------------------------------------------------------------------
void lhpOpMTRULBImporter::OpDo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Groups.size(); i++)
  {
    if (m_Groups[i])
    {
      m_Groups[i]->ReparentTo(m_Input);
      mafEventMacro(mafEvent(this, VME_ADD, m_Groups[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void lhpOpMTRULBImporter::OpUndo()   
//----------------------------------------------------------------------------
{
  for(unsigned i = 0; i < m_Groups.size(); i++)
  {
    if (m_Groups[i])
    {
      mafEventMacro(mafEvent(this, VME_REMOVE, m_Groups[i]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}

