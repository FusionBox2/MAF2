/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMTRExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-12 10:23:14 $
  Version:   $Revision: 1.2 $
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

#include "mmoMTRExporter.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "mafVMERoot.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEGroup.h"
#include "mafNodeIterator.h"
#include "mafVMERawMotionData.h"

#include "vtkDataSetReader.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkPolyDataWriter.h"
#include "vtkRectilinearGridWriter.h"
#include "vtkStructuredPointsWriter.h"

//----------------------------------------------------------------------------
mmoMTRExporter::mmoMTRExporter(const wxString& label) : mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";
  m_FileDir = "";
  m_Input   = NULL;
}
//----------------------------------------------------------------------------
mmoMTRExporter::~mmoMTRExporter() 
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
bool mmoMTRExporter::Accept(mafNode *node)   
//----------------------------------------------------------------------------
{ 
  if(node == NULL)
    return false;

  return true;
}
//----------------------------------------------------------------------------
void mmoMTRExporter::OpRun()
//----------------------------------------------------------------------------
{
  int result = OP_RUN_CANCEL;

  assert(m_Input);
  wxString proposed = (mafGetApplicationDirectory() + "/Data/External/").c_str();

  if(m_Input->IsMAFType(mafVMELandmarkCloud))
  {
    proposed += m_Input->GetName();
    proposed += ".mtr";
    wxString wildc = "FARO MTR file (*.mtr)|*.mtr";

    wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

    if(f != "") 
    {
      m_File = f;
      ExportLandmark();
      result = OP_RUN_OK;
    }
  }
  else
  {
    wxMessageDialog dialog(mafGetFrame(), _("Do you want to create separate files?"),
      _("Options"), wxYES_NO|wxYES_DEFAULT);
    if(dialog.ShowModal() == wxID_NO)
    {

      proposed += m_Input->GetName();
      proposed += ".mtr";
      wxString wildc = "FARO MTR file (*.mtr)|*.mtr";
      wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

      if(f != "") 
      {
        m_File = f;
        ExportLandmark();
        result = OP_RUN_OK;
      }
    }
    else
    {
      wxString f = mafGetDirName(proposed).c_str();

      if(f != "") 
      {
        m_FileDir = f;
        ExportLandmark();
        result = OP_RUN_OK;
      }
    }
  }
  mafEventMacro(mafEvent(this,result));
}


void mmoMTRExporter::ExportOneCloud(std::ostream &out, mafVMELandmarkCloud* cloud)
{
  std::vector<mafTimeStamp> timeStamps;
  mafVME *vmeTemp = mafVME::SafeDownCast(m_Input);
  cloud->GetLocalTimeStamps(timeStamps);

  int numberLandmark = cloud->GetNumberOfLandmarks();

  // if cloud is closed , open it
  bool initState = cloud->IsOpen();
  if(!initState)
  {
    cloud->Open();
  }

  // pick up the values and write them into the file
  if(timeStamps.size() != 0)
  {
    for (int index = 0; index < timeStamps.size(); index++)
    {
      for(int j=0; j < numberLandmark; j++)
      {
        char strng[256];
        wxString name = cloud->GetLandmarkName(j);
        const char* nameLandmark = (name);                                        

        mafVMELandmark *landmark = cloud->GetLandmark(nameLandmark);

        double xLandmark, yLandmark, zLandmark;
        landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
        sprintf(strng, "%d      %.6f      %.6f      %.6f      %.6f      %.6f      %.6f\n", j + 1, xLandmark, yLandmark, zLandmark, 0.0, 0.0, 0.0);
        out << strng;
      }
    }
  }
  else
  {
    for(int j=0; j < numberLandmark; j++)
    {
      char strng[256];
      wxString name = cloud->GetLandmarkName(j);
      const char* nameLandmark = (name);                                        

      mafVMELandmark *landmark = cloud->GetLandmark(nameLandmark);

      double xLandmark, yLandmark, zLandmark;
      landmark->GetPoint(xLandmark,yLandmark,zLandmark);
      sprintf(strng, "%d      %.6f      %.6f      %.6f      %.6f      %.6f      %.6f\n", j + 1, xLandmark, yLandmark, zLandmark, 0.0, 0.0, 0.0);
      out << strng;
    }
  }

  // and now, close the cloud
  if(!initState)
  {
    cloud->Close();
  }
}

void mmoMTRExporter::ExportingTraverse(std::ostream &out, const char *dirName, mafNode* node)
{
  if(node->IsMAFType(mafVMELandmarkCloud))
  {
    if(dirName == NULL)
      ExportOneCloud(out, mafVMELandmarkCloud::SafeDownCast(node));
    else
    {
      wxString fn = dirName;
      fn += "\\";
      fn += node->GetName();
      fn += ".mtr";
      std::ofstream outF;
      outF.open(fn.c_str());
      outF<<"Index     Xmm        Ymm        Zmm     A(deg)     B(deg)     C(deg)\n";
      ExportOneCloud(outF, mafVMELandmarkCloud::SafeDownCast(node));
      outF.close();
    }
    return;
  }
  int numberChildren = node->GetNumberOfChildren();
  for (int i= 0; i< numberChildren; i++)
  {
    mafNode *child = node->GetChild(i);
    ExportingTraverse(out, dirName, child);
  }
}
//----------------------------------------------------------------------------
void mmoMTRExporter::ExportLandmark()
//----------------------------------------------------------------------------
{
  if (!m_TestMode)
  {
    wxBusyInfo wait("Saving landmark position: Please wait");
  }
  //file creation
  const char    *fileName = (m_File);
  std::ofstream f_Out;

  if(m_Input->IsMAFType(mafVMELandmarkCloud))
  {
    f_Out.open(fileName);
    f_Out<<"Index     Xmm        Ymm        Zmm     A(deg)     B(deg)     C(deg)\n";
    ExportOneCloud(f_Out, mafVMELandmarkCloud::SafeDownCast(m_Input));
    f_Out.close();
  }
  else
  {
    if(m_FileDir == "")
    {
      f_Out.open(fileName);
      f_Out<<"Index     Xmm        Ymm        Zmm     A(deg)     B(deg)     C(deg)\n";
      ExportingTraverse(f_Out, NULL, m_Input);
      f_Out.close();
    }
    else
    {
      ExportingTraverse(f_Out, m_FileDir.c_str(), m_Input);
    }
  }
}

//----------------------------------------------------------------------------
mafOp* mmoMTRExporter::Copy()   
//----------------------------------------------------------------------------
{
  mmoMTRExporter *cp = new mmoMTRExporter(m_Label);
  cp->m_Canundo      = m_Canundo;
  cp->m_OpType       = m_OpType;
  cp->m_Listener     = m_Listener;
  cp->m_Next         = NULL;
  cp->m_File         = m_File;
  cp->m_Input        = m_Input;
  return cp;
}
