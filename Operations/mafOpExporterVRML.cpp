/*=========================================================================

 Program: MAF2
 Module: mafOpExporterVRML
 Authors: Taha Jerbi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExporterVRML.h"

#include "mafDecl.h"
#include "mafGUI.h"

#include "vtkMAFSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMERoot.h"
#include "mafTransformBase.h"

#include "mafVME.h"
#include "mafVMEOutputSurface.h"
#include "mafPipeSurfaceTextured.h"
#include "mafTransformBase.h"
#include "mmaMaterial.h"

#include "vtkMAFSmartPointer.h"
#include "vtkVRMLExporter.h"
#include "vtkPolyDataMapper.h" 
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTriangleFilter.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkSTLWriter.h"
#include "vtkBMPWriter.h"
#include "vtkImageData.h"
#include "mafVMEGroup.h"
#include "vtkPolyData.h"
#include "vtkTexture.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"

#include <iostream>
#include <fstream>

using namespace std;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExporterVRML);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExporterVRML::mafOpExporterVRML(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_EXPORTER;
  m_Canundo = true;
  m_File    = "";



	m_FileDir = "";//mafGetApplicationDirectory().c_str();
}
//----------------------------------------------------------------------------
mafOpExporterVRML::~mafOpExporterVRML()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
enum STL_EXPORTER_ID
{
  ID_STL_BINARY_FILE = MINID,
	ID_ABS_MATRIX_TO_STL,
  ID_CHOOSE_FILENAME,
};
//----------------------------------------------------------------------------
void mafOpExporterVRML::OpRun()   
//----------------------------------------------------------------------------
{
  mafString wildc = "Stereo Litography (*.wrl)|*.wrl";

  m_Gui = new mafGUI(this);
	m_Gui->FileSave(ID_CHOOSE_FILENAME,"wrl file", &m_File, wildc,"Save As...");
  //m_Gui->Label("file type",true);
	//m_Gui->Bool(ID_STL_BINARY_FILE,"binary",&m_Binary,0);
	//m_Gui->Label("absolute matrix",true);
	//m_Gui->Bool(ID_ABS_MATRIX_TO_STL,"apply",&m_ABSMatrixFlag,0);
	m_Gui->OkCancel();
  m_Gui->Enable(wxOK,m_File != "");
	
	m_Gui->Divider();

	ShowGui();
}
//----------------------------------------------------------------------------
void mafOpExporterVRML::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
	 // ExportVRML();
	 // OpStop(OP_RUN_OK);
	  
    switch(e->GetId())
    {
      case wxOK:
		/*		{
					//mafString FileDir = mafGetApplicationDirectory().c_str();
					//FileDir<<"\\";
          mafString name = m_Input->GetName();
          if (name.FindChr('\\') != -1 || name.FindChr('/') != -1 || name.FindChr(':') != -1 || 
            name.FindChr('?')  != -1 || name.FindChr('"') != -1 || name.FindChr('<') != -1 || 
            name.FindChr('>')  != -1 || name.FindChr('|') != -1 )
          {
            mafMessage("Node name contains invalid chars.\nA node name can not contain chars like \\ / : * ? \" < > |");
            m_File = "";
          }
          else
          {
            m_FileDir << this->m_Input->GetName();
            m_FileDir << ".stl";
            mafString wildc = "STL (*.stl)|*.stl";
            m_File = mafGetSaveFile(m_FileDir.GetCStr(), wildc.GetCStr());
          }

					if(m_File.IsEmpty())
					{
            OpStop(OP_RUN_CANCEL);
					}
					else
          {
            ExportVRML();
            OpStop(OP_RUN_OK);
          }
				}*/

		
		/*mafNode *n = e->GetVme();
		vtkImageData *image = vtkImageData::SafeDownCast(((mafVME *)n)->GetOutput()->GetVTKData());
		vtkMAFSmartPointer<vtkBMPWriter> exporter;
		exporter->SetInput(image);
		exporter->SetFileName("C:\texture.bmp");
		exporter->Write();
		exporter->Delete();*/
        ExportVRML();
		
        OpStop(OP_RUN_OK);
      break;
      case ID_CHOOSE_FILENAME:
        m_Gui->Enable(wxOK,m_File != "");
      break;
      case wxCANCEL:
        OpStop(OP_RUN_CANCEL);
      break;
      default:
        e->Log();
      break;
    }
	}
	
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void mafOpExporterVRML::ExportVRML()
//----------------------------------------------------------------------------
{
	
	mafVMEOutputSurface *out_surface = mafVMEOutputSurface::SafeDownCast(((mafVME *)m_Input)->GetOutput());
  out_surface->Update();
   
  mafNode::mafAttributesMap* attributes =m_Input->GetAttributes();
 
  
  ofstream file1;
  file1.open("texture.txt");
  
  file1 << "size " << attributes->size() << " Name " << m_Input->GetName()<<" Link "<<m_Input->GetLinks();
  file1.close();
  mafNode::mafLinksMap *linkedNodes = m_Input->GetLinks();
  
  

  vtkPolyData *data = vtkPolyData::SafeDownCast(out_surface->GetVTKData());
  assert(data);
  data->Update();
  vtkPointData *ptData=data->GetPointData();
  vtkDataArray *scalars=ptData->GetScalars() ;
  double sr[2] = { 0, 1 };
  int m_ScalarVisibility=0;
  if (scalars != NULL)
  {

	  m_ScalarVisibility = 1;
	  scalars->GetRange(sr);
  }
  else
  {
	  mafLogMessage(_("Scalars NULL!!"));
  }
  
  m_SurfaceMaterial = out_surface->GetMaterial();
  assert(m_SurfaceMaterial);

  vtkMAFSmartPointer<vtkTriangleFilter>triangles;
  vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
  triangles->SetInput(out_surface->GetSurfaceData());
  triangles->Update();

  v_tpdf->SetInput(triangles->GetOutput());
  v_tpdf->SetTransform(out_surface->GetAbsTransform()->GetVTKTransform());
  
  v_tpdf->Update();
	/**/
	/**/
	vtkMAFSmartPointer<vtkVRMLExporter> writer;
	//mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,writer));
	writer->SetFileName(m_File.GetCStr());
	/*if(this->m_ABSMatrixFlag)
		writer->SetInput(v_tpdf->GetOutput());
	else
		writer->SetInput(triangles->GetOutput());*/

	vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
	mapper->SetInput(data);
	mapper->SetScalarVisibility(m_ScalarVisibility);
	mapper->SetScalarRange(sr);


	vtkActor* actor=vtkActor::New();
	actor->SetMapper(mapper);

	//actor->SetTexture();

	vtkRenderer* renderer=vtkRenderer::New();
	renderer->AddActor(actor);
	vtkRenderWindow* renderWindow =vtkRenderWindow::New();
	renderWindow->AddRenderer(renderer);
	writer->SetRenderWindow(renderWindow);
	writer->Write();
	writer->Delete();
	renderWindow->Delete();
	renderer->Delete();
	actor->Delete();
	mapper->Delete();
	/**/

	
	vtkMAFSmartPointer<vtkBMPWriter> exporter;
	vtkImageData *image;
	
	
	vtkTexture *m_Texture=vtkTexture::New();
	m_Texture->SetQualityTo32Bit();
	m_Texture->InterpolateOn();
	if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
	{
		if (m_SurfaceMaterial->GetMaterialTexture() != NULL)
		{
			vtkImageData *image = m_SurfaceMaterial->GetMaterialTexture();
			m_Texture->SetInput(image);
			image->GetScalarRange(sr);
		}
		else if (m_SurfaceMaterial->GetMaterialTextureID() != -1)
		{
			mafVME *texture_vme = mafVME::SafeDownCast(m_Input->GetRoot()->FindInTreeById(m_SurfaceMaterial->GetMaterialTextureID()));
			texture_vme->GetOutput()->GetVTKData()->Update();
			vtkImageData *image = (vtkImageData *)texture_vme->GetOutput()->GetVTKData();
			m_Texture->SetInput(image);
			image->GetScalarRange(sr);
		}
		else
		{
			mafLogMessage(_("texture info not correctly stored inside material!!"));
		}
	}
/*	exporter->SetInput(image);
	exporter->SetFileName("texture.bmp");
	exporter->Write();
	exporter->Delete();*/
	
}
//----------------------------------------------------------------------------
mafOp* mafOpExporterVRML::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExporterVRML *cp = new mafOpExporterVRML(GetLabel());
  cp->m_File = m_File;
  return cp;
}
void mafOpExporterVRML::OpStop(int result)
//----------------------------------------------------------------------------
{
	HideGui();
	mafEventMacro(mafEvent(this, result));
}