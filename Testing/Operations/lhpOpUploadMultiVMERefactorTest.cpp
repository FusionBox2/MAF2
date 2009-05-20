/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpUploadMultiVMERefactorTest.cpp,v $
Language:  C++
Date:      $Date: 2009-05-20 08:10:19 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
#include "lhpDefines.h"
#include <wx/file.h>

//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "lhpOpUploadMultiVMERefactorTest.h"

#include "medVMEFactory.h"
#include "lhpUser.h"

#include "mafVMERoot.h"
#include "mafVMEMesh.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEStorage.h"

#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDirectory.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridReader.h"

void lhpOpUploadMultiVMERefactorTest::setUp()
{
  
}

void lhpOpUploadMultiVMERefactorTest::tearDown()
{
  
}

void lhpOpUploadMultiVMERefactorTest::TestConstructorDestructor()
{
  lhpOpUploadMultiVMERefactor *op = new lhpOpUploadMultiVMERefactor("lhpOpUploadMultiVMERefactor");
  cppDEL(op);
}

void lhpOpUploadMultiVMERefactorTest::TestPrintSelf()
{
  lhpOpUploadMultiVMERefactor *op = new lhpOpUploadMultiVMERefactor("lhpOpUploadMultiVMERefactor");
  cppDEL(op);
}

void lhpOpUploadMultiVMERefactorTest::TestLoadInputVMEsIdsFile()
{
  int result = medVMEFactory::Initialize();
  CPPUNIT_ASSERT(result == MAF_OK);

  int vmeIds[15] = {1,
    4,
    5,
    11,
    12,
    16,
    23,
    24,
    25,
    27,
    28,
    29,
    30,
    31,
    32
};
  wxString msfFileName = LHP_DATA_ROOT;
  msfFileName.append("/lhpOpUploadMultiVMERefactorTest/testMSFWithManyVMETypesAndSmallData/msf_test_import_export_VME.msf");

  mafVMEStorage loadStorage;
  loadStorage.SetURL(msfFileName);

  mafVMERoot *vmeRoot = loadStorage.GetRoot();
  loadStorage.Restore();

  wxString vmeIdsFileName = LHP_DATA_ROOT;
  vmeIdsFileName.append("/lhpOpUploadMultiVMERefactorTest/testMSFWithManyVMETypesAndSmallData/vmeIDs.txt");

  CPPUNIT_ASSERT(wxFileExists(vmeIdsFileName));
  
  lhpOpUploadMultiVMERefactor *opUploadMultiVME = new lhpOpUploadMultiVMERefactor("lhpOpUploadMultiVMERefactor");  
  opUploadMultiVME->TestModeOn();
  opUploadMultiVME->SetInput(vmeRoot);
  opUploadMultiVME->LoadInputVMEsIdsFile(vmeIdsFileName.c_str());

  int vmeNum = opUploadMultiVME->m_VMEsToUploadIdsVector.size();   
  CPPUNIT_ASSERT_EQUAL(vmeNum, 15);

  for (int i = 0; i < vmeNum; i++) 
  {
    CPPUNIT_ASSERT_EQUAL(opUploadMultiVME->m_VMEsToUploadIdsVector[i], vmeIds[i]);
  }
  
  CPPUNIT_ASSERT(wxFileExists(msfFileName));

  for (int i = 0; i < vmeNum; i++) 
  {
    mafNode *node = vmeRoot->FindInTreeById(vmeIds[i]);
    cout << vmeIds[i] << " " << node << std::endl;
    CPPUNIT_ASSERT(node);
  }

    
  assert(true);

  cppDEL(opUploadMultiVME);
}


void lhpOpUploadMultiVMERefactorTest::TestUploadManyVMETypesWithSmallData()
{
  int result = medVMEFactory::Initialize();
  CPPUNIT_ASSERT(result == MAF_OK);

  int vmeIds[15] = {1,
    4,
    5,
    11,
    12,
    16,
    23,
    24,
    25,
    27,
    28,
    29,
    30,
    31,
    32
  };
  wxString msfFileName = LHP_DATA_ROOT;
  msfFileName.append("/lhpOpUploadMultiVMERefactorTest/testMSFWithManyVMETypesAndSmallData/msf_test_import_export_VME.msf");

  mafVMEStorage loadStorage;
  loadStorage.SetURL(msfFileName);

  mafVMERoot *vmeRoot = loadStorage.GetRoot();
  loadStorage.Restore();

  wxString vmeIdsFileName = LHP_DATA_ROOT;
  vmeIdsFileName.append("/lhpOpUploadMultiVMERefactorTest/testMSFWithManyVMETypesAndSmallData/vmeIDs.txt");

  CPPUNIT_ASSERT(wxFileExists(vmeIdsFileName));

  wxInitialize();

  lhpOpUploadMultiVMERefactor *opUploadMultiVME = new lhpOpUploadMultiVMERefactor("lhpOpUploadMultiVMERefactor");  
  opUploadMultiVME->TestModeOn();
  opUploadMultiVME->m_PythonExe = "python.exe ";
  opUploadMultiVME->SetInput(vmeRoot);
  opUploadMultiVME->OpRun();

  wxString msfABSFolder = LHP_DATA_ROOT;
  msfABSFolder.append("/lhpOpUploadMultiVMERefactorTest/testMSFWithManyVMETypesAndSmallData/");

  wxString VMEUploaderDownloaderABSFolderName = LHP_DATA_ROOT;
  VMEUploaderDownloaderABSFolderName.append("/../../VMEUploaderDownloaderRefactor");
  CPPUNIT_ASSERT(wxDirExists(VMEUploaderDownloaderABSFolderName));

  opUploadMultiVME->m_OpUploadVME->TestModeOn();
  opUploadMultiVME->m_OpUploadVME->m_VMEUploaderDownloaderABSFolderName = VMEUploaderDownloaderABSFolderName;
  opUploadMultiVME->m_OpUploadVME->m_PythonExe = "python.exe ";
  opUploadMultiVME->m_OpUploadVME->m_PythonwExe = "pythonw.exe ";

  lhpUser *user = new lhpUser(NULL);
  int proxyFlag = 0;
  mafString proxyHost = "";
  mafString proxyPort = "";
  int rememberMe = 0;
  user->SetCredentials(mafString("testuser"),mafString("6w8DHF"),proxyFlag,proxyHost,proxyPort,rememberMe);
  opUploadMultiVME->m_OpUploadVME->m_User = user;
  opUploadMultiVME->m_User = user;
  //ws.Username = 'testuser'
  //ws.Password = 'GRDPt8'

  opUploadMultiVME->m_OpUploadVME->m_MSFFileABSFolder = msfABSFolder ;
  opUploadMultiVME->LoadInputVMEsIdsFile(vmeIdsFileName.c_str());

  int vmeNum = opUploadMultiVME->m_VMEsToUploadIdsVector.size();   
  CPPUNIT_ASSERT_EQUAL(vmeNum, 15);

  for (int i = 0; i < vmeNum; i++) 
  {
    CPPUNIT_ASSERT_EQUAL(opUploadMultiVME->m_VMEsToUploadIdsVector[i], vmeIds[i]);
  }


  CPPUNIT_ASSERT(wxFileExists(msfFileName));

  for (int i = 0; i < vmeNum; i++) 
  {
    mafNode *node = vmeRoot->FindInTreeById(vmeIds[i]);
    cout << vmeIds[i] << " " << node << std::endl;
    CPPUNIT_ASSERT(node);
  }

  opUploadMultiVME->Upload();
  cppDEL(opUploadMultiVME);

  assert(true);

  cppDEL(user);
  cppDEL(opUploadMultiVME);
}