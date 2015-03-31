/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpVMEMeshRSScanImporter.cpp,v $
Language:  C++
Date:      $Date: 2008-02-14 16:21:12 $
Version:   $Revision: 1.10 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

// debug facility
// #define DEBUG_MODE

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpVMEMeshRSScanImporter.h"

#include "vtkMAFSmartPointer.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkFieldData.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"

#include "mafVMEMesh.h"
#include "mafVMESurface.h"
#include "mafVMELandmarkCloud.h"
#include "mafTagArray.h"


// vcl includes
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vectors.h>

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------

const int CHAR_BUF_SIZE = 1000;

lhpVMEMeshRSScanImporter::lhpVMEMeshRSScanImporter()
{
  m_PlateParamsFileName = "";
  m_DataFileName = "";
  m_NumCols      = 0;
  m_NumRows      = 0;
  m_SizeX        = 1.0;
  m_SizeY        = 1.0;
  m_Scale        = 1.0;
  m_TimeShift    = 0.0;
  m_Output = NULL;
}
//----------------------------------------------------------------------------
lhpVMEMeshRSScanImporter::~lhpVMEMeshRSScanImporter()
{
  mafDEL(m_Output);
}

//----------------------------------------------------------------------------
int lhpVMEMeshRSScanImporter::Read()
{
  vnl_matrix<double> DataFileMatrix;
  vcl_ifstream paramsfstr(m_PlateParamsFileName, std::ios::in);
  vcl_ifstream fstr(m_DataFileName, std::ios::in);
  vcl_string v_string;
  int    frame;
  double ts;

  if(paramsfstr.is_open() == 0)
    return MAF_ERROR;

  while(!paramsfstr.eof())
  {
    const char *frs = NULL;
    vcl_getline(paramsfstr, v_string);
    if(v_string[0] == '#')
      continue;
    if((frs = strstr(v_string.c_str(), "NumRows")) != NULL)
      sscanf(frs, "NumRows %i", &m_NumRows);
    else if((frs = strstr(v_string.c_str(), "NumCols")) != NULL)
      sscanf(frs, "NumCols %i", &m_NumCols);
    else if((frs = strstr(v_string.c_str(), "SizeX")) != NULL)
      sscanf(frs, "SizeX %lf", &m_SizeX);
    else if((frs = strstr(v_string.c_str(), "SizeY")) != NULL)
      sscanf(frs, "SizeY %lf", &m_SizeY);
  }


  mafString RSValuesArrayName("RSValues");
  mafVMEMesh    *mesh = NULL;
  mafVMESurface *surf = NULL;

  if(fstr.is_open() == 0)
    return MAF_ERROR;
  for(;;)
  {
    const char *frs = NULL;
    do
    {
      vcl_getline(fstr, v_string);
      frs = strstr(v_string.c_str(), "Frame ");
    }
    while(!fstr.eof() && frs == NULL);
    if(fstr.eof())
    {
      if(frs != NULL)
        return MAF_ERROR;
      return MAF_OK;
    }
    sscanf(frs, "Frame %i (%lf ms)", &frame, &ts);
    ts += m_TimeShift;
    vtkMAFSmartPointer<vtkUnstructuredGrid> grid;
    grid->Initialize();
    GenerateGrid(grid);
    vnl_matrix<double> TempMatrix;
    DataFileMatrix.set_size(m_NumRows, m_NumCols);
    TempMatrix.set_size(m_NumRows, m_NumCols - 1);
    TempMatrix.read_ascii(fstr);
    for(int i = 0; i < m_NumRows; i++)
    {
      for(int j = 0; j < m_NumCols - 1; j++)
      {
        DataFileMatrix[i][j] = m_Scale * TempMatrix[i][j];
      }
      DataFileMatrix[i][m_NumCols - 1] = 0.0;
    }
    AddIntArrayToUnstructuredGridCellData(grid, DataFileMatrix, 0, RSValuesArrayName, true);

    if (m_Output == NULL)
    {
      mafNEW(m_Output);
      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("NATURAL");
      m_Output->GetTagArray()->SetTag(tag_Nature);
      m_Output->SetName("RSScan");
      mafNEW(mesh);
      mafTagItem tagNature;
      tagNature.SetName("VME_NATURE");
      tagNature.SetValue("NATURAL");
      mesh->GetTagArray()->SetTag(tag_Nature);
      mesh->SetName("RSScan");
      mesh->ReparentTo(m_Output);
      vtkPoints *src, *trg;
      src = vtkPoints::New();
      trg = vtkPoints::New();
      double s[3];
      s[0] = 0.5 * m_SizeX;
      s[1] = 1.5 * m_SizeY;
      s[2] = 0.0;
      src->InsertNextPoint(s);
      s[0] = 0.5 * m_SizeX;
      s[1] = (m_NumRows - 0.5) * m_SizeY;
      src->InsertNextPoint(s);
      s[0] = (m_NumCols - 0.5) * m_SizeX;
      s[1] = (m_NumRows - 0.5) * m_SizeY;
      src->InsertNextPoint(s);
      m_Sensors->GetLandmark("SNS3", s);
      trg->InsertNextPoint(s);
      m_Sensors->GetLandmark("SNS1", s);
      trg->InsertNextPoint(s);
      m_Sensors->GetLandmark("SNS2", s);
      trg->InsertNextPoint(s);


      vtkPoints    *newPts;
      vtkCellArray *newPolys;
      vtkPolyData  *output;
      vtkNEW(output);
      newPts = vtkPoints::New();
      newPts->Allocate(4,10000);
      newPolys = vtkCellArray::New();
      newPolys->Allocate(2, 20000);


      V3d<double> coord;
      coord = V3d<double>(0.0, 0.0, 0.0);
      newPts->InsertNextPoint(coord.components);
      coord = V3d<double>(m_SizeX * m_NumCols, 0.0, 0.0);
      newPts->InsertNextPoint(coord.components);
      coord = V3d<double>(m_SizeX * m_NumCols, m_SizeY * m_NumRows, 0.0);
      newPts->InsertNextPoint(coord.components);
      coord = V3d<double>(0.0, m_SizeY * m_NumRows, 0.0);
      newPts->InsertNextPoint(coord.components);


      vtkIdType indx[3];
      indx[0] = 0;
      indx[1] = 1;
      indx[2] = 2;
      newPolys->InsertNextCell(3, indx);
      indx[0] = 0;
      indx[1] = 2;
      indx[2] = 3;
      newPolys->InsertNextCell(3, indx);

      //
      // Update ourselves
      //
      output->SetPoints(newPts);
      output->SetPolys(newPolys);
      output->Squeeze();
      newPts->Delete();
      newPolys->Delete();

      mafNEW(surf);

      mafTagItem tagN;
      tagN.SetName("VME_NATURE");
      tagN.SetValue("NATURAL");
      surf->GetTagArray()->SetTag(tagN);
      surf->SetName("RSScan");
      surf->SetData(output, ts * 0.001);
      surf->ReparentTo(m_Output);
      vtkDEL(output);

      mafMatrix m;
      vtkWeightedLandmarkTransform *RegisterTransform;
      vtkNEW(RegisterTransform);

      //setup transform calculator
      RegisterTransform->SetSourceLandmarks(src);
      RegisterTransform->SetTargetLandmarks(trg);
      RegisterTransform->SetModeToRigidBody();
      RegisterTransform->Update();
      RegisterTransform->GetMatrix(m.GetVTKMatrix());
      vtkDEL(RegisterTransform);
      vtkDEL(src);
      vtkDEL(trg);
      m.SetTimeStamp(ts * 0.001);
      mesh->SetMatrix(m);
      surf->SetMatrix(m);

    }
    mesh->SetDataByDetaching(grid, ts * 0.001);
  }
  mafDEL(mesh);
  mafDEL(surf);
  return MAF_OK;
}

int lhpVMEMeshRSScanImporter::GenerateGrid(vtkUnstructuredGrid *grid)
{
  vtkPoints* points = vtkPoints::New();
  points->SetNumberOfPoints((m_NumRows + 1) * (m_NumCols + 1));

  for(int i = 0; i <= m_NumRows; i++)
  {
    for(int j = 0; j <= m_NumCols; j++)
    {
      points->SetPoint(i* (m_NumCols + 1) + j, m_SizeX * j, m_SizeY * i, 0.0);
    }
  }

  // fill the POINT section of the Mesh
  grid->SetPoints(points);

  vtkDEL(points);


  int elementType     = TETRA4; 
  int vtkCellType     = 10;
  int nodesPerElement = 4; 
  int meshType        = LINEAR;
  int ret             = elementType;

  // id list for connectivity
  vtkIdList *id_list = vtkIdList::New();

  id_list->SetNumberOfIds(nodesPerElement);

  grid->Allocate(m_NumRows * m_NumCols, 1);

  // create the connectivity list for each cell from each row
  for (int i = 0; i < m_NumRows; i++)
  {
    for(int j = 0; j < m_NumCols; j++)
    {
      int id_index = 0;
      for (int k = 0; k < 4; k++ )
      {
        id_list->SetId(id_index, (i + id_index / 2) * (m_NumCols + 1) + (j + (id_index / 2 + id_index % 2) % 2));
        id_index++;
      }
      grid->InsertNextCell(vtkCellType, id_list);
    }
  }

  vtkDEL(id_list);


  return 0;
}

void lhpVMEMeshRSScanImporter::AddIntArrayToUnstructuredGridCellData( vtkUnstructuredGrid *grid, vnl_matrix<double> &dataFileMatrix, int column, mafString outputArrayName , bool activeScalar)
{
  // store info about cell_id <-> material_id association
  vtkIntArray *array = vtkIntArray::New();
  array->SetName(outputArrayName.GetCStr());
  array->SetNumberOfTuples(dataFileMatrix.rows() * dataFileMatrix.cols());

  assert(m_NumRows == dataFileMatrix.rows());
  assert(m_NumCols == dataFileMatrix.cols());
  if(m_NumRows != dataFileMatrix.rows() || m_NumCols != dataFileMatrix.cols())
    return;

  for (int i = 0; i < m_NumRows; i++)
  {
    for(int j = 0; j < m_NumCols; j++)
    {
      array->SetValue(i * m_NumCols + j, dataFileMatrix(i, j));
    }
  }

  if (activeScalar == true)
  {
    grid->GetCellData()->SetScalars(array);
  } 
  else
  {
    grid->GetCellData()->AddArray(array);
  }
  
  vtkDEL(array);
}


int lhpVMEMeshRSScanImporter::ReadMatrix(vnl_matrix<double> &M, const char *fname)
{
  vcl_ifstream v_raw_matrix(fname, std::ios::in);
  if(v_raw_matrix.is_open() != 0)
  {
    M.read_ascii(v_raw_matrix);
    return 0;
  }
  return 1;
}
