#include "mafOpImporterVTK.h"
#include <wx/busyinfo.h>
#include "mafEvent.h"

#include "mafVME.h"
#include "mafVMEGeneric.h"
#include "mafVMEImage.h"
//#include "mafVMEPointSet.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEPolyline.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEVolumeRGB.h"
#include "mafVMEMesh.h"

#include "mafTagArray.h"
#include "vtkSmartPointer.h"

#include "vtkDataSet.h"
#include "vtkDataSetReader.h"
#include "vtkPolyDataReader.h"
#include "vtkStructuredPointsReader.h"
#include "vtkStructuredGridReader.h"
#include "vtkRectilinearGridReader.h"
#include "vtkUnstructuredGridReader.h"

//----------------------------------------------------------------------------
mafOpImporterVTK::mafOpImporterVTK(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
	m_Canundo = true;
	m_File    = _R("");
  m_FileDir = _R("");//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpImporterVTK::~mafOpImporterVTK()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpImporterVTK::Copy()   
//----------------------------------------------------------------------------
{
  mafOpImporterVTK *cp = new mafOpImporterVTK(GetLabel());
  cp->m_File			= m_File;
  return cp;
}
//----------------------------------------------------------------------------
void mafOpImporterVTK::OpRun()   
//----------------------------------------------------------------------------
{
	mafString wildc = _R("vtk Data (*.vtk)|*.vtk");
  mafString f;
  if (m_File.empty())
  {
    f = mafGetOpenFile(m_FileDir, wildc, _L("Choose VTK file"));
    m_File = f;
  }

  int result = OP_RUN_CANCEL;
  if(!m_File.empty())
	{
    if (ImportVTK() == MAF_OK)
    {
      result = OP_RUN_OK;
    }
    else
    {
      if(!this->m_TestMode)
          mafErrorMessage(_M(mafString(_L("Unsupported file format"))));
    }
	}
	{mafEvent evUnq(this,result); InvokeEvent(evUnq);}
}
//----------------------------------------------------------------------------
int mafOpImporterVTK::ImportVTK()
//----------------------------------------------------------------------------
{
  bool success = false;
	if(!this->m_TestMode)
		wxBusyInfo wait(_("Loading file: ..."));
  
  vtkNew<vtkDataSetReader> reader;
  reader->SetFileName(m_File.GetCStr());

  vtkDataReader *preader = NULL;
  // workaround to avoid double reading
  switch (reader->ReadOutputType())
  {
    case VTK_POLY_DATA:
      preader = vtkPolyDataReader::New();
    break;
    case VTK_STRUCTURED_POINTS:
      preader = vtkStructuredPointsReader::New();
    break;
    case VTK_STRUCTURED_GRID:
      preader = vtkStructuredGridReader::New();
    break;
    case VTK_RECTILINEAR_GRID:
      preader = vtkRectilinearGridReader::New();
    break;
    case VTK_UNSTRUCTURED_GRID:
      preader = vtkUnstructuredGridReader::New();
    break;
    default:
      return MAF_ERROR;
  }
  {mafEvent evUnq(this,BIND_TO_PROGRESSBAR); evUnq.SetVtkObj(preader); InvokeEvent(evUnq);}
  preader->SetFileName(m_File.GetCStr());
  preader->Update();
  
  if (preader->GetNumberOfOutputPorts()>0)
  {
    mafString path, name, ext;
    mafSplitPath(m_File,&path,&name,&ext);

    vtkDataSet *data = vtkDataSet::SafeDownCast(preader->GetOutputDataObject(0));
    if (data)
    {
      auto vmeLandmarkCloud = mafVMELandmarkCloud::NewSPtr();
      //auto vmePointSet = mafVMEPointSet::NewSPtr();
      auto vmePolyLine = mafVMEPolyline::NewSPtr(); 
      auto vmeSurface =mafVMESurface::NewSPtr(); 
      auto vmeImage =mafVMEImage::NewSPtr(); 
      auto vmeGrayVol =mafVMEVolumeGray::NewSPtr(); 
      auto vmeRGBVol =mafVMEVolumeRGB::NewSPtr(); 
      auto vmeMesh =mafVMEMesh::NewSPtr(); 
      auto vmeGeneric =mafVMEGeneric::NewSPtr(); 
      //if (m_VmePointSet->SetDataByDetaching(data,0) == MAF_OK)
      //{
      //  SetOutput(m_VmePointSet);
      //}
	  if (vmeLandmarkCloud->SetDataByDetaching(data,0) == MAF_OK)
      {
        SetOutput(vmeLandmarkCloud);
      }
      else if (vmePolyLine->SetDataByDetaching(data,0) == MAF_OK)
      {
        SetOutput(vmePolyLine);
      }
      else if (vmeSurface->SetDataByDetaching(data,0) == MAF_OK)
      {
        SetOutput(vmeSurface);
      }
	  else if (vmeImage->SetDataByDetaching(data,0) == MAF_OK)
	  {
		  SetOutput(vmeImage);
	  }
      else if (vmeGrayVol->SetDataByDetaching(data,0) == MAF_OK)
      {
        SetOutput(vmeGrayVol);
      }
      else if (vmeRGBVol->SetDataByDetaching(data,0) == MAF_OK)
      {
        SetOutput(vmeRGBVol);
      }
      else if (vmeMesh->SetDataByDetaching(data,0) == MAF_OK)
      {
        SetOutput(vmeMesh);
      }
      else
      {
        vmeGeneric->SetDataByDetaching(data,0);
        SetOutput(vmeGeneric);
      }

      mafTagItem tag_Nature;
      tag_Nature.SetName(_R("VME_NATURE"));
      tag_Nature.SetValue(_R("NATURAL"));
      GetOutput()->GetTagArray()->SetTag(tag_Nature);
      mafNode::ReparentTo(GetOutput(), GetInput().get());
      GetOutput()->SetName(name);

      success = true;
    }
  }
  vtkDEL(preader);
  if(!success && !this->m_TestMode)
  {
    mafErrorMessage(_M(mafString(_L("Error reading VTK file."))));
    return MAF_ERROR;
  }
  return MAF_OK;
}
