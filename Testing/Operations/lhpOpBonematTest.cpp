/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpBonematTest.cpp,v $
Language:  C++
Date:      $Date: 2008-05-20 13:21:59 $
Version:   $Revision: 1.1 $
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
#include "lhpOpBonematTest.h"

#include "mafVMEMesh.h"
#include "mafVMEVolumeGray.h"

#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkDirectory.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"
#include "vtkRectilinearGrid.h"
#include "vtkRectilinearGridReader.h"

void lhpOpBonematTest::setUp()
{
  
}

void lhpOpBonematTest::tearDown()
{
  
}

void lhpOpBonematTest::TestConstructorDestructor()
{
  lhpOpBonemat *op = new lhpOpBonemat("op bonemat");
  cppDEL(op);
}

void lhpOpBonematTest::TestPrintSelf()
{
  lhpOpBonemat *op = new lhpOpBonemat("op bonemat");
  op->PrintSelf(std::cout);
  cppDEL(op);

}

void lhpOpBonematTest::TestSaveLoadConfigurationFile()
{
 
  lhpOpBonemat *op = new lhpOpBonemat("op bonemat");

  /*double HU0_d0_el0 = rand();
  double HU0_d0_el1 = rand();
  double HU1_d1_el0 = rand();
  double HU1_d1_el1 = rand();*/

  double ROIntercept = rand();
  double ROSlope = rand();

  double Ea0_Eb0_Ec0_V3_el0 = rand();
  double Ea0_Eb0_Ec0_V3_el1 = rand();
  double Ea0_Eb0_Ec0_V3_el2 = rand();

  double Ea1_Eb1_Ec1_V3_el0 = rand();
  double Ea1_Eb1_Ec1_V3_el1 = rand();
  double Ea1_Eb1_Ec1_V3_el2 = rand();

  double Ea2_Eb2_Ec2_V3_el0 = rand();
  double Ea2_Eb2_Ec2_V3_el1 = rand();
  double Ea2_Eb2_Ec2_V3_el2 = rand();

  double Ea_Eb_Ec_V2_el0 = rand();
  double Ea_Eb_Ec_V2_el1 = rand();
  double Ea_Eb_Ec_V2_el2 = rand();

  double Egap = rand();
  double StepsNumber = rand();
  double RO1_RO2_el0 = rand();
  double RO1_RO2_el1 = rand();

  double Ea_Eb_Ec_V3_OneDensityInterval_el0 = rand();
  double Ea_Eb_Ec_V3_OneDensityInterval_el1 = rand();
  double Ea_Eb_Ec_V3_OneDensityInterval_el2 = rand();

  //Ro Calibration Flag
  double ROCalibrationCorrectionActivation = rand();
  double ROCalibrationCorrectionType       = rand();
  double ROCorrectionDensityInterval0 = rand();
  double ROCorrectionDensityInterval1 = rand();

  //single interval ro calibration
  double RoCorrection1IntervalCoefficient0 = rand();
  double RoCorrection1IntervalCoefficient1 = rand();

  //three intervals ro calibration
  double RoCorrection3IntervalsFirstCoefficient0 = rand();
  double RoCorrectio3IntervalsFirstCoefficient1 = rand();

  double RoCorrection3IntervalsSecondCoefficient0 = rand();
  double RoCorrection3IntervalsSecondCoefficient1 = rand();

  double RoCorrection3IntervalsThirdCoefficient0 = rand();
  double RoCorrection3IntervalsThirdCoefficient1 = rand();


  //op->m_HU0_d0_el0 = HU0_d0_el0;
  //op->m_HU0_d0_el1 = HU0_d0_el1;
  //op->m_HU1_d1_el0 = HU1_d1_el0;
  //op->m_HU1_d1_el1 = HU1_d1_el1;

  op->m_ROIntercept = ROIntercept;
  op->m_ROSlope = ROSlope;

  op->m_Ea0_Eb0_Ec0_V3_el0 = Ea0_Eb0_Ec0_V3_el0;
  op->m_Ea0_Eb0_Ec0_V3_el1 = Ea0_Eb0_Ec0_V3_el1;
  op->m_Ea0_Eb0_Ec0_V3_el2 = Ea0_Eb0_Ec0_V3_el2;

  op->m_Ea1_Eb1_Ec1_V3_el0 = Ea1_Eb1_Ec1_V3_el0;
  op->m_Ea1_Eb1_Ec1_V3_el1 = Ea1_Eb1_Ec1_V3_el1;
  op->m_Ea1_Eb1_Ec1_V3_el2 = Ea1_Eb1_Ec1_V3_el2;

  op->m_Ea2_Eb2_Ec2_V3_el0 = Ea2_Eb2_Ec2_V3_el0;
  op->m_Ea2_Eb2_Ec2_V3_el1 = Ea2_Eb2_Ec2_V3_el1;
  op->m_Ea2_Eb2_Ec2_V3_el2 = Ea2_Eb2_Ec2_V3_el2;

  op->m_Ea_Eb_Ec_V2_el0 = Ea_Eb_Ec_V2_el0;
  op->m_Ea_Eb_Ec_V2_el1 = Ea_Eb_Ec_V2_el1;
  op->m_Ea_Eb_Ec_V2_el2 = Ea_Eb_Ec_V2_el2;

  op->m_Egap = Egap;
  op->m_StepsNumber = StepsNumber;
  op->m_RO1_RO2_el0 = RO1_RO2_el0;
  op->m_RO1_RO2_el1 = RO1_RO2_el1;

  op->m_Ea_Eb_Ec_V3_OneDensityInterval_el0 = Ea_Eb_Ec_V3_OneDensityInterval_el0;
  op->m_Ea_Eb_Ec_V3_OneDensityInterval_el1 = Ea_Eb_Ec_V3_OneDensityInterval_el1;
  op->m_Ea_Eb_Ec_V3_OneDensityInterval_el2 = Ea_Eb_Ec_V3_OneDensityInterval_el2;

  //Ro Calibration Flag
  op->m_ROCalibrationCorrectionIsActive = ROCalibrationCorrectionActivation;
  op->m_ROCalibrationCorrectionType       = ROCalibrationCorrectionType;
  op->m_RO1 = ROCorrectionDensityInterval0;
  op->m_RO2 = ROCorrectionDensityInterval1;

  //single interval ro calibration
  op->m_RoCorrection1IntervalCoefficient0 = RoCorrection1IntervalCoefficient0;
  op->m_RoCorrection1IntervalCoefficient1 = RoCorrection1IntervalCoefficient1;

  //three intervals ro calibration
  op->m_RoCorrection3IntervalsFirstCoefficient0 = RoCorrection3IntervalsFirstCoefficient0;
  op->m_RoCorrectio3IntervalsFirstCoefficient1 = RoCorrectio3IntervalsFirstCoefficient1;

  op->m_RoCorrection3IntervalsSecondCoefficient0 = RoCorrection3IntervalsSecondCoefficient0;
  op->m_RoCorrection3IntervalsSecondCoefficient1 = RoCorrection3IntervalsSecondCoefficient1;

  op->m_RoCorrection3IntervalsThirdCoefficient0 = RoCorrection3IntervalsThirdCoefficient0;
  op->m_RoCorrection3IntervalsThirdCoefficient1 = RoCorrection3IntervalsThirdCoefficient1;

  mafString dirPrefix = LHP_DATA_ROOT;


  mafString configurationFileName = dirPrefix.Append("/confTest.txt");


  int res = op->SaveConfigurationFile(configurationFileName.GetCStr());
  CPPUNIT_ASSERT(res == MAF_OK);

  cppDEL(op);

  op = new lhpOpBonemat("second instance");
  op->PrintSelf(std::cout);
  res = op->LoadConfigurationFile(configurationFileName.GetCStr());

  CPPUNIT_ASSERT(res == MAF_OK);
  
  //CPPUNIT_ASSERT(op->m_HU0_d0_el0 == HU0_d0_el0);
  //CPPUNIT_ASSERT(op->m_HU0_d0_el1 == HU0_d0_el1);
  //CPPUNIT_ASSERT(op->m_HU1_d1_el0 == HU1_d1_el0);
  //CPPUNIT_ASSERT(op->m_HU1_d1_el1 == HU1_d1_el1);

  CPPUNIT_ASSERT(op->m_ROIntercept == ROIntercept);
  CPPUNIT_ASSERT(op->m_ROSlope == ROSlope);

  CPPUNIT_ASSERT(op->m_Ea0_Eb0_Ec0_V3_el0 == Ea0_Eb0_Ec0_V3_el0);
  CPPUNIT_ASSERT(op->m_Ea0_Eb0_Ec0_V3_el1 == Ea0_Eb0_Ec0_V3_el1);
  CPPUNIT_ASSERT(op->m_Ea0_Eb0_Ec0_V3_el2 == Ea0_Eb0_Ec0_V3_el2);

  CPPUNIT_ASSERT(op->m_Ea1_Eb1_Ec1_V3_el0 == Ea1_Eb1_Ec1_V3_el0);
  CPPUNIT_ASSERT(op->m_Ea1_Eb1_Ec1_V3_el1 == Ea1_Eb1_Ec1_V3_el1);
  CPPUNIT_ASSERT(op->m_Ea1_Eb1_Ec1_V3_el2 == Ea1_Eb1_Ec1_V3_el2);

  CPPUNIT_ASSERT(op->m_Ea2_Eb2_Ec2_V3_el0 == Ea2_Eb2_Ec2_V3_el0);
  CPPUNIT_ASSERT(op->m_Ea2_Eb2_Ec2_V3_el1 == Ea2_Eb2_Ec2_V3_el1);
  CPPUNIT_ASSERT(op->m_Ea2_Eb2_Ec2_V3_el2 == Ea2_Eb2_Ec2_V3_el2);

  CPPUNIT_ASSERT(op->m_Ea_Eb_Ec_V2_el0 == Ea_Eb_Ec_V2_el0);
  CPPUNIT_ASSERT(op->m_Ea_Eb_Ec_V2_el1 == Ea_Eb_Ec_V2_el1);
  CPPUNIT_ASSERT(op->m_Ea_Eb_Ec_V2_el2 == Ea_Eb_Ec_V2_el2);

  CPPUNIT_ASSERT(op->m_Egap == Egap);
  CPPUNIT_ASSERT(op->m_StepsNumber == StepsNumber);
  CPPUNIT_ASSERT(op->m_RO1_RO2_el0 == RO1_RO2_el0);
  CPPUNIT_ASSERT(op->m_RO1_RO2_el1 == RO1_RO2_el1);

  CPPUNIT_ASSERT(op->m_Ea_Eb_Ec_V3_OneDensityInterval_el0 == Ea_Eb_Ec_V3_OneDensityInterval_el0);
  CPPUNIT_ASSERT(op->m_Ea_Eb_Ec_V3_OneDensityInterval_el1 == Ea_Eb_Ec_V3_OneDensityInterval_el1);
  CPPUNIT_ASSERT(op->m_Ea_Eb_Ec_V3_OneDensityInterval_el2 == Ea_Eb_Ec_V3_OneDensityInterval_el2);

  //Ro Calibration Flag
  CPPUNIT_ASSERT(op->m_ROCalibrationCorrectionIsActive == ROCalibrationCorrectionActivation);
  CPPUNIT_ASSERT(op->m_ROCalibrationCorrectionType       == ROCalibrationCorrectionType);
  CPPUNIT_ASSERT(op->m_RO1 == ROCorrectionDensityInterval0);
  CPPUNIT_ASSERT(op->m_RO2 == ROCorrectionDensityInterval1);

  //single interval ro calibration
  CPPUNIT_ASSERT(op->m_RoCorrection1IntervalCoefficient0 == RoCorrection1IntervalCoefficient0);
  CPPUNIT_ASSERT(op->m_RoCorrection1IntervalCoefficient1 == RoCorrection1IntervalCoefficient1);

  //three intervals ro calibration
  CPPUNIT_ASSERT(op->m_RoCorrection3IntervalsFirstCoefficient0 == RoCorrection3IntervalsFirstCoefficient0);
  CPPUNIT_ASSERT(op->m_RoCorrectio3IntervalsFirstCoefficient1 == RoCorrectio3IntervalsFirstCoefficient1);

  CPPUNIT_ASSERT(op->m_RoCorrection3IntervalsSecondCoefficient0 == RoCorrection3IntervalsSecondCoefficient0);
  CPPUNIT_ASSERT(op->m_RoCorrection3IntervalsSecondCoefficient1 == RoCorrection3IntervalsSecondCoefficient1);

  CPPUNIT_ASSERT(op->m_RoCorrection3IntervalsThirdCoefficient0 == RoCorrection3IntervalsThirdCoefficient0);
  CPPUNIT_ASSERT(op->m_RoCorrection3IntervalsThirdCoefficient1 == RoCorrection3IntervalsThirdCoefficient1);
  
  op->PrintSelf(std::cout);

  CPPUNIT_ASSERT(true);

  vtkDEL(op);
}

void lhpOpBonematTest::TestBonematFromConfigFile()
{

  mafString dirPrefix = LHP_DATA_ROOT;
  mafString inputDataDir = dirPrefix;
  CPPUNIT_ASSERT(wxDirExists(inputDataDir));

  inputDataDir.Append("/lhpOpBonematTest/TestData/InputData/");

  CPPUNIT_ASSERT(wxDirExists(inputDataDir));

  mafString inputVTKMesh = inputDataDir;
  inputVTKMesh.Append("InputMesh.vtk");

  bool exist = wxFile::Exists(inputVTKMesh.GetCStr());

  CPPUNIT_ASSERT(exist == TRUE);

  mafString inputVTKVolume = inputDataDir;
  inputVTKVolume.Append("InputVolume.vtk");

  exist = wxFile::Exists(inputVTKVolume.GetCStr());

  CPPUNIT_ASSERT(exist == TRUE);

  mafString outputDataCorrectDir = dirPrefix;
  outputDataCorrectDir.Append("/lhpOpBonematTest/TestData/OutputData/");

  //mafString outputVTKMeshCorrect = outputDataCorrectDir;
  //outputVTKMeshCorrect.Append("vtk_postmatlab_Builderbonemat.vtk");

  //exist = wxFile::Exists(outputVTKMeshCorrect.GetCStr());

  
  // load the VTK data for the mesh
  vtkMAFSmartPointer<vtkUnstructuredGridReader> reader;
  reader->SetFileName(inputVTKMesh);
  reader->Update();

  // create the VME mesh
  mafVMEMesh *vmeMesh;
  mafNEW(vmeMesh);
  vmeMesh->SetData(reader->GetOutput(), -1);
  vmeMesh->GetOutput()->GetVTKData()->Update();

  vtkUnstructuredGrid *inGrid = vmeMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  CPPUNIT_ASSERT(inGrid->GetNumberOfCells()  == 15);
 
  vtkMAFSmartPointer<vtkRectilinearGridReader> vreader;
  vreader->SetFileName(inputVTKVolume);
  vreader->Update();

  mafVMEVolumeGray *vmeVolumeGray;

  mafNEW(vmeVolumeGray);

  vmeVolumeGray->SetData(vreader->GetOutput(), -1);
  vmeVolumeGray->GetOutput()->GetVTKData()->Update();

  vtkRectilinearGrid *inVolume = vmeVolumeGray->GetVolumeOutput()->GetRectilinearData();
  
  int dim[3] = {0,0,0};
  inVolume->GetDimensions(dim);

  CPPUNIT_ASSERT( dim[0] == 23);
  CPPUNIT_ASSERT( dim[1] == 23);
  CPPUNIT_ASSERT( dim[2] == 10);

  mafString outputFrequencyFile = outputDataCorrectDir;
  outputFrequencyFile.Append("outputFrequencyFileToCheck.freq");

  mafString inputConfFile = inputDataDir;

  // inputConfFile.Append("config_builderBoneMat.conf"); 
  // skazza sempre con o senza calib corr e ha val di calib corr diversi
  // dal default quindi posso provaarlo solo con calibr corr disattivata 
  // e deve dare il solito risultato  
  
  // inputConfFile.Append("config_builderBoneMat_calibcorr_default.conf");
  // creato con calib corr attiva e val di default di calib corr e funge  
  
  // inputConfFile.Append("config_builderBoneMat_nocalibcorr.conf"); 
  // creato senza calib corr ed e` da usare senza calib corr e ha i 
  // valori di calib corr di default

  inputConfFile.Append("Default.conf");
  
  exist = wxFile::Exists(inputConfFile.GetCStr());
  CPPUNIT_ASSERT(exist == TRUE);

  // create the bonemat operation
  lhpOpBonemat *op = new lhpOpBonemat("op bonemat");
  op->TestModeOn();
  op->SetInput(vmeMesh);
  op->SetVolume(vmeVolumeGray);
  op->LoadConfigurationFile(inputConfFile.GetCStr());
  op->SetFrequencyFileName(outputFrequencyFile.GetCStr());
  
  // execute bonemat
  int result = op->YoungModuleIntegration();
  
  CPPUNIT_ASSERT(result == MAF_OK);

  mafString outputConfigurationFileToCheck = inputDataDir;
  outputConfigurationFileToCheck.Append("outputConfigurationFileToCheck.conf");
  op->SaveConfigurationFile(outputConfigurationFileToCheck.GetCStr());

  // compare results against the correct one
  op->PrintSelf(std::cout);

  mafString outputMeshFileName = outputDataCorrectDir;
  outputMeshFileName.Append("outputMeshToCheck.vtk");

  vtkUnstructuredGrid *outputMesh = vmeMesh->GetUnstructuredGridOutput()->GetUnstructuredGridData();
  
  vtkMAFSmartPointer<vtkUnstructuredGridWriter> writer;
  writer->SetInput(outputMesh);
  writer->SetFileTypeToASCII();
  writer->SetFileName(outputMeshFileName.GetCStr());
  writer->Write();

  // clean up
  cppDEL(op);
  mafDEL(vmeMesh);
  mafDEL(vmeVolumeGray);

}
