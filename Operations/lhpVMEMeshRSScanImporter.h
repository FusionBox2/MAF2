/*========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpVMEMeshRSScanImporter.h,v $
Language:  C++
Date:      $Date: 2008-02-05 10:12:22 $
Version:   $Revision: 1.6 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __lhpVMEMeshRSScanImporter_h
#define __lhpVMEMeshRSScanImporter_h

#include "vtkUnstructuredGrid.h"

#include "mafVMEMesh.h"
#include "mafVMEGroup.h"
#include "mafVMELandmarkCloud.h"

#include "vtkPoints.h"
#include "vtkWeightedLandmarkTransform.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"


#include "vnl/vnl_matrix.h"

#include <map>
#include <fstream>

/**
 .NAME lhpVMEMeshRSScanImporter

  Build a mafVMEMesh from information contained
  in some text files. This text files are actually
  ANSYS list files without any header, only numbers.
  Supported elements type are:
  linear tetra, parabolic tetra, linear hexa, parabolic hexa
 
  BEWARE Hybrid meshes are not upported

  See lhpVMEMeshRSScanImporterTest for use cases

 .SECTION Description

 .SECTION ToDo
*/
class lhpVMEMeshRSScanImporter
{
public:

  /**
  // Set/Get nodes file name*/
  void SetPlateParamsFileName(const char *name)   {m_PlateParamsFileName = name;}
  const char *GetPlateParamsFileName() {return m_PlateParamsFileName;}

  // Set/Get nodes file name*/
  void SetScale(double scale){m_Scale = scale;}
  double GetScale() {return m_Scale;}

  void SetTimeShift(double timeShift){m_TimeShift = timeShift;}
  double GetTimeShift() {return m_TimeShift;}


  void SetSensors(mafVMELandmarkCloud *sensors){m_Sensors = sensors;}

  /**
  // Set/Get elements file name*/
  void SetDataFileName(const char *name)   {m_DataFileName = name;}
  const char *GetDataFileName() {return m_DataFileName;}

  /**
  // Read files information; returns MAF_OK if successful otherwise MAF_ERROR*/
  int Read();

  /**
  Get the vme created by the reader*/
  mafVMEGroup *GetOutput() {return m_Output;};
  
  // supported elements 
  enum
  {  
    TETRA4 = 0,
    HEXA8,  
    TETRA10,
    HEXA20,
    NUM_SUPPORTED_ELEMENTS,
    UNSUPPORTED_ELEMENT,
  };

  enum
  {
    LINEAR = 0,
    PARABOLIC,
    UNKNOWN,
    INVALID_MESH_FILE,
  };

  lhpVMEMeshRSScanImporter();
  ~lhpVMEMeshRSScanImporter();

protected:
  /**
  // Read nodes*/
  int GenerateGrid(vtkUnstructuredGrid *grid);

  /**
  // Nodes file name*/
  const char *m_PlateParamsFileName;

  /**
  // Elements file name*/
  const char *m_DataFileName;

  // The output vme fem
  mafVMEGroup *m_Output;

  mafVMELandmarkCloud *m_Sensors;

  int        m_NumRows;
  int        m_NumCols;
  double     m_SizeX;
  double     m_SizeY;
  double     m_Scale;
  double     m_TimeShift;

  /** 
  utility functions */
  int ReadMatrix(vnl_matrix<double> &M, const char *fname);
  void AddIntArrayToUnstructuredGridCellData(vtkUnstructuredGrid *grid, vnl_matrix<double> &dataFileMatrix, int column, mafString outputArrayName, bool activeScalar = false);
};

#endif


