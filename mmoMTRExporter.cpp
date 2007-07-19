/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoMTRExporter.cpp,v $
  Language:  C++
  Date:      $Date: 2007-07-19 12:37:35 $
  Version:   $Revision: 1.3 $
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
  m_Input   = NULL;
  m_State   = NULL;

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
  if(!node->IsMAFType(mafVMERoot) && node->IsMAFType(mafVMELandmarkCloud))
    return true;

  for(int i=0; i<node->GetNumberOfChildren(); i++)
  {
    if(node->GetChild(i)->IsMAFType(mafVMELandmarkCloud)) return true;
  }

  return false;
}
//----------------------------------------------------------------------------
void mmoMTRExporter::OpRun()
//----------------------------------------------------------------------------
{
  assert(m_Input);
  wxString proposed = (mafGetApplicationDirectory() + "/Data/External/").c_str();
  proposed += m_Input->GetName();
  proposed += ".mtr";
  wxString wildc = "FARO MTR file (*.mtr)|*.mtr";
  wxString f = mafGetSaveFile(proposed,wildc).c_str(); 

  int result = OP_RUN_CANCEL;
  if(f != "") 
  {
    m_File = f;
    ExportLandmark();
    result = OP_RUN_OK;
  }
  mafEventMacro(mafEvent(this,result));
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

  f_Out.open(fileName);

  if(m_Input->IsMAFType(mafVMELandmarkCloud))
  {

    std::vector<mafTimeStamp> timeStamps;
    mafVME *vmeTemp = mafVME::SafeDownCast(m_Input);
    vmeTemp->GetTimeStamps(timeStamps);

    mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(vmeTemp);

    int numberLandmark = vmeCloud->GetNumberOfLandmarks();

    // if cloud is closed , open it
    bool initState = vmeCloud->IsOpen();
    if(!initState)
    {
      vmeCloud->Open();
    }

    // pick up the values and write them into the file
    f_Out<<"Index     Xmm        Ymm        Zmm     A(deg)     B(deg)     C(deg)\n";
    for (int index = 0; index < timeStamps.size(); index++)
    {
      for(int j=0; j < numberLandmark; j++)
      {
        char strng[256];
        wxString name = vmeCloud->GetLandmarkName(j);
        const char* nameLandmark = (name);                                        

        mafVMELandmark *landmark = vmeCloud->GetLandmark(nameLandmark);

        double xLandmark, yLandmark, zLandmark;
        landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
        sprintf(strng, "%d      %.6f      %.6f      %.6f      %.6f      %.6f      %.6f\n", j + 1, xLandmark, yLandmark, zLandmark, 0.0, 0.0, 0.0);
        f_Out << strng;
      }
    }

    // and now, close the cloud
    if(!initState)
    {
      vmeCloud->Close();
    }
  }
  else
  {
    int numberChildren = m_Input->GetNumberOfChildren();

    if(numberChildren > 0)
    {  
      std::vector<mafTimeStamp> timeStamps;
      mafVME *vmeTemp = mafVME::SafeDownCast(m_Input);
      vmeTemp->GetTimeStamps(timeStamps);

      m_State = new bool[numberChildren];
      for (int i= 0; i< numberChildren; i++)
      {
        mafNode *child = vmeTemp->GetChild(i);
        if (child->IsMAFType(mafVMELandmarkCloud))
        {
          mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(child);
          m_State[i] = vmeCloud->IsOpen();
          
           // if cloud is closed , open it
          if (!m_State[i])
          {
            vmeCloud->Open();
          }
        } 
      }
      f_Out<<"Index     Xmm        Ymm        Zmm     A(deg)     B(deg)     C(deg)\n";
      // pick up the values and write them into the file
      for (int index = 0; index < timeStamps.size(); index++)
      {
        for (int i=0; i< numberChildren; i++)
        {
          mafNode *child = m_Input->GetChild(i);
          if (child->IsMAFType(mafVMELandmarkCloud))
          {
            mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(child);
            int numberLandmark = vmeCloud->GetNumberOfLandmarks();

            for(int j = 0; j < numberLandmark; j++)
            {
              char strng[256];
              wxString name = vmeCloud->GetLandmarkName(j);
              const char* nameLandmark = (name);                                        

              mafVMELandmark *landmark = vmeCloud->GetLandmark(nameLandmark);

              double xLandmark, yLandmark, zLandmark;
              landmark->GetPoint(xLandmark,yLandmark,zLandmark,timeStamps[index]);
              sprintf(strng, "%d      %.6f      %.6f      %.6f      %.6f      %.6f      %.6f\n", j + 1, xLandmark, yLandmark, zLandmark, 0.0, 0.0, 0.0);
              f_Out << strng;
            } 
          }
        }
      }
       // and now, close the cloud
      for (i=0; i< numberChildren; i++)
      {
        mafNode *child = m_Input->GetChild(i);

        if (child->IsMAFType(mafVMELandmarkCloud))
        {
          mafVMELandmarkCloud *vmeCloud = mafVMELandmarkCloud::SafeDownCast(child);  
          if (!m_State[i])
          {
            vmeCloud->Close();
          }
        }
      }
    }
  }

  f_Out.close();
  if(m_State)
  {
    delete[] m_State;
    m_State = NULL;
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
