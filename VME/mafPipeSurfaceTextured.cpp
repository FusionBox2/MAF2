/*=========================================================================

 Program: MAF2
 Module: mafPipeSurfaceTextured
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "mafPipeSurfaceTextured.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafGUIMaterialButton.h"
#include "mafAxes.h"
#include "mmaMaterial.h"

#include "mafDataVector.h"
#include "mafVMESurface.h"
#include "mafVMEGenericAbstract.h"
#include "mafEventSender.h"
#include "mafGUIValidator.h"
#include "mafNodeIterator.h"
#include "wx/busyinfo.h"

#include <ctime>
#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkTextureMapToCylinder.h"
#include "vtkTextureMapToPlane.h"
#include "vtkTextureMapToSphere.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "mafLODActor.h"
#include "vtkCleanPolyData.h"
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkActor.h"
#include "vtkVRMLExporter.h"
#include "vtkRenderWindow.h"
#include "vtkJPEGWriter.h"
#include "vtkOBJExporter.h"
#include "vtkSTLWriter.h"
#include "vtkSTLReader.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTriangleFilter.h"

#include <iostream>
#include <fstream>
#include <vector>


using namespace std;
//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeSurfaceTextured);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeSurfaceTextured::mafPipeSurfaceTextured()
//----------------------------------------------------------------------------
{
  m_Texture         = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
  m_MaterialButton  = NULL;
  m_SurfaceMaterial = NULL;
  m_Gui             = NULL;

  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;

  m_UseVTKProperty  = 1;
  m_UseTexture      = 1;
  m_UseLookupTable  = 0;

  m_EnableActorLOD  = 0;
  m_VmeImageName = _R("");
	m_ShowAxis = 1;
  m_SelectionVisibility = 1;
  m_File = _R("");
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::Create(mafNode *node, mafView *view/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(node, view);
  int iterator = 0;
  m_Selected = false;
  m_Texture         = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;
  m_File = _R("");

  m_Vme->Update();
  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface));
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();
  vtkPolyData *data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  assert(data);
  data->Update();

  m_Vme->AddObserver(this);

  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};
  if(scalars != NULL)
  {
    m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  m_SurfaceMaterial = surface_output->GetMaterial();
  assert(m_SurfaceMaterial);  // all vme that use PipeSurface must have the material correctly set

  vtkNEW(m_Mapper);

  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    m_UseTexture = 1;
    m_UseVTKProperty = 0;
    m_UseLookupTable = 0;
    if (data->GetPointData()->GetTCoords() == NULL)
    {
      GenerateTextureMapCoordinate();
    }
    else
    {
      m_Mapper->SetInput(data);
    }
  }
  else
  {
    m_Mapper->SetInput(data);
  }
  
  m_RenderingDisplayListFlag = m_Vme->IsAnimated() ? 1 : 0;
  m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);

  vtkNEW(m_Texture);
  m_Texture->SetQualityTo32Bit();
  m_Texture->InterpolateOn();
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    if (m_SurfaceMaterial->GetMaterialTexture() != NULL)
    {
      vtkImageData *image1 = m_SurfaceMaterial->GetMaterialTexture();
	  m_Texture->SetInput(image1);
	  image1->GetScalarRange(sr);
    }
	else if (!m_SurfaceMaterial->GetMaterialTextureName().IsEmpty())
	{
		mafVME *texture_vme = mafVME::SafeDownCast(m_Vme->GetRoot()->FindInTreeByName(m_SurfaceMaterial->GetMaterialTextureName()));
		texture_vme->GetOutput()->GetVTKData()->Update();
		vtkImageData *image1 = (vtkImageData *)texture_vme->GetOutput()->GetVTKData();
		m_Texture->SetInput((vtkImageData*)image1);
		image1->GetScalarRange(sr);
	}
    else if (m_SurfaceMaterial->GetMaterialTextureID() != -1)
    {
		int id = m_SurfaceMaterial->GetMaterialTextureID();

      mafVME *texture_vme = mafVME::SafeDownCast(m_Vme->GetRoot()->FindInTreeById(m_SurfaceMaterial->GetMaterialTextureID()));
      texture_vme->GetOutput()->GetVTKData()->Update();
	  vtkImageData *image1 = (vtkImageData *)texture_vme->GetOutput()->GetVTKData();
	  m_Texture->SetInput((vtkImageData*)image1);
	  image1->GetScalarRange(sr);
    }
    else
    {
      mafLogMessage(_M(mafString((_L("texture info not correctly stored inside material!!")))));
    }
  }

  m_Mapper->SetScalarRange(sr);

  vtkNEW(m_Actor);
  m_Actor->SetMapper(m_Mapper);
  m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
  
  if (m_EnableActorLOD == false)
  {
	  // bug 2454 fix
	  // http://bugzilla.b3c.it/show_bug.cgi?id=2454
	  // small LOD rectangle is activated even if LOD is disabled
	  // Setting PixelThreshold to 1 will deactivate small square visualization 
	  // ie entering
	  /*
	  
	  if( sz < m_PixelThreshold)
	  {
		  // Assign the actor's transformation matrix to the m_FlagActor 
		  // to put this one at the same position of the actor
		  m_FlagActor->PokeMatrix(GetMatrix());

		  glPointSize( m_FlagDimension );
		  m_FlagActor->Render(ren,m_FlagMapper); 
		  glPointSize( 1 );
	  }

	  */
	  //
	  // code section in mafLODActor.cpp
	  //
	  m_Actor->SetPixelThreshold(1);
  }

  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
  {
    m_UseTexture = 0;
    m_UseVTKProperty = 0;
    m_UseLookupTable = 1;
  }
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
    m_UseTexture = 0;
    m_UseVTKProperty = 1;
    m_UseLookupTable = 0;
    m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
  }
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    m_UseLookupTable = 1;
    m_Texture->SetLookupTable(m_SurfaceMaterial->m_ColorLut);
    m_Actor->SetTexture(m_Texture);
  }

  if (m_Gui)
  {
    m_Gui->Update();
  }

  if(m_AssemblyBack)
    m_AssemblyBack->AddPart(m_Actor);
  else
    m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
  vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(data);  

  vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInput(corner->GetOutput());

  vtkMAFSmartPointer<vtkProperty> corner_props;
	corner_props->SetColor(1,1,1);
	corner_props->SetAmbient(1);
	corner_props->SetRepresentationToWireframe();
	corner_props->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(corner_mapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(corner_props);

  if(m_AssemblyBack)
    m_AssemblyBack->AddPart(m_OutlineActor);
  else
    m_AssemblyFront->AddPart(m_OutlineActor);

	if(m_RenFront)
	{
      m_Axes = new mafAxes(m_RenFront, m_Vme);
      m_Axes->SetVisibility(0);
	}

  m_GhostActor = NULL;
  if(m_AssemblyBack != NULL)
  {
    vtkNEW(m_GhostActor);
    m_GhostActor->SetMapper(m_Mapper);
    m_GhostActor->PickableOff();
    m_GhostActor->GetProperty()->SetOpacity(0);
    m_GhostActor->GetProperty()->SetRepresentationToPoints();

	// Set to gouraud shading instead of flat to solve pixellation 
	// look and feel in vertical application (medViewCrossCT in DP app) 
    m_GhostActor->GetProperty()->SetInterpolationToGouraud();
    m_AssemblyFront->AddPart(m_GhostActor);
  }
}
//----------------------------------------------------------------------------
mafPipeSurfaceTextured::~mafPipeSurfaceTextured()
//----------------------------------------------------------------------------
{
  m_Vme->RemoveObserver(this);

  if(m_AssemblyBack)
  {
    m_AssemblyBack->RemovePart(m_Actor);
    m_AssemblyBack->RemovePart(m_OutlineActor);
  }
  else
  {
    m_AssemblyFront->RemovePart(m_Actor);
    m_AssemblyFront->RemovePart(m_OutlineActor);
  }
  

  vtkDEL(m_Texture);
	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineActor);
  cppDEL(m_Axes);
  cppDEL(m_MaterialButton);

  if(m_GhostActor) 
  {
    m_AssemblyFront->RemovePart(m_GhostActor);
  }
  vtkDEL(m_GhostActor);
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel && m_SelectionVisibility);
    m_Axes->SetVisibility(sel&&m_ShowAxis);
	}
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  if (m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE)
  {
    double sr[2];
    sr[0] = m_SurfaceMaterial->m_TableRange[0];
    sr[1] = m_SurfaceMaterial->m_TableRange[1];
    m_Mapper->SetScalarRange(sr);
  }
}
//----------------------------------------------------------------------------
mafGUI *mafPipeSurfaceTextured::CreateGui()
//----------------------------------------------------------------------------
{
  mafString mapping_mode[3] = {_R("Plane"), _R("Cylinder"),_R("Sphere")};

  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);
  m_Gui->Bool(ID_RENDERING_DISPLAY_LIST,_R("displaylist"),&m_RenderingDisplayListFlag,0,_R("turn on/off \nrendering displaylist calculation"));
  m_Gui->Bool(ID_SCALAR_VISIBILITY,_R("scalar vis."), &m_ScalarVisibility,0,_R("turn on/off the scalar visibility"));
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_VTK_PROPERTY,_R("property"),&m_UseVTKProperty);
  m_MaterialButton = new mafGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_TEXTURE,_R("texture"),&m_UseTexture);
  m_Gui->Button(ID_CHOOSE_TEXTURE, &m_VmeImageName,_L("Select the texture image"),_L("texture"));
  mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  //m_Gui->Combo(ID_TEXTURE_MAPPING_MODE,"mapping",&surface_output->GetMaterial()->m_TextureMappingMode,3,mapping_mode);
  //m_Gui->Enable(ID_CHOOSE_TEXTURE,m_UseTexture != 0);
  m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_UseTexture != 0);
  m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE && m_UseTexture != 0);
  
  mafString wildc1 = _R("vrml (*.wrl)|*.wrl");
  m_Gui->FileSave(ID_CHOOSE_FILENAME1, _R("wrl file"), &m_File, wildc1, _R("Save As..."));

  mafString wildc2 = _R("obj (*.obj)|*.obj");
  m_Gui->FileSave(ID_CHOOSE_FILENAME2, _R("obj file"), &m_File, wildc2, _R("Save As..."));
  m_Gui->Divider();
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_LOOKUP_TABLE,_R("lut"),&m_UseLookupTable);
  m_Gui->Lut(ID_LUT,_R("lut"),m_SurfaceMaterial->m_ColorLut);
  m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
  m_Gui->Divider(2);
  m_Gui->Bool(ID_ENABLE_LOD,_R("LOD"),&m_EnableActorLOD);
  m_Gui->Label(_R(""));

  if (m_SurfaceMaterial == NULL)
  {
    mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    m_SurfaceMaterial = surface_output->GetMaterial();
  }
  bool texture_falg = m_SurfaceMaterial->GetMaterialTexture() != NULL || m_SurfaceMaterial->GetMaterialTextureID() != -1;
  m_Gui->Enable(ID_USE_TEXTURE, texture_falg);
  m_Gui->Enable(ID_USE_LOOKUP_TABLE, m_SurfaceMaterial->m_ColorLut != NULL);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_SCALAR_VISIBILITY:
      {
        m_Mapper->SetScalarVisibility(m_ScalarVisibility);
        if (m_ScalarVisibility)
        {
          vtkPolyData *data = (vtkPolyData *)m_Vme->GetOutput()->GetVTKData();
          assert(data);
          double range[2];
          data->GetScalarRange(range);
          m_Mapper->SetScalarRange(range);
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
    	break;
      case ID_LUT:
        m_SurfaceMaterial->UpdateFromLut();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_ENABLE_LOD:
        m_Actor->SetEnableHighThreshold(m_EnableActorLOD);
        m_OutlineActor->SetEnableHighThreshold(m_EnableActorLOD);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_USE_VTK_PROPERTY:
        if (m_UseVTKProperty != 0)
        {
          m_Actor->SetProperty(m_SurfaceMaterial->m_Prop);
        }
        else
        {
          m_Actor->SetProperty(NULL);
        }
		if (m_MaterialButton != NULL)
		{
		  m_MaterialButton->Enable(m_UseVTKProperty != 0);
		}
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_USE_LOOKUP_TABLE:
        m_Gui->Enable(ID_LUT,m_UseLookupTable != 0);
      break;
      case ID_USE_TEXTURE:
        if (m_UseTexture)
        {
          m_Actor->SetTexture(m_Texture);
        }
        else
        {
          m_Actor->SetTexture(NULL);
        }
        m_Gui->Enable(ID_CHOOSE_TEXTURE,m_UseTexture != 0);
        m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_UseTexture != 0);
        m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,m_SurfaceMaterial->m_MaterialType == mmaMaterial::USE_TEXTURE && m_UseTexture != 0);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_CHOOSE_TEXTURE:
      {
        mafString title = _R("Choose texture");
        e->SetId(VME_CHOOSE);
        e->SetArg((long)&mafPipeSurfaceTextured::ImageAccept);
        e->SetString(&title);
        mafEventMacro(*e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
			//SetImageLink("image", n);
			m_VmeImageName = n->GetName();
			m_Gui->Update();
			//UpdateLinks();
          vtkImageData* image1 = vtkImageData::SafeDownCast(((mafVME *)n)->GetOutput()->GetVTKData());
		  
          m_Gui->Enable(ID_USE_TEXTURE,image1 != NULL);
          if (image1)
          {
            image1->Update();
           // m_SurfaceMaterial->SetMaterialTexture(n->GetId());
			mafString na = n->GetName();
			
			m_SurfaceMaterial->SetMaterialTexture(image1,na);
			m_SurfaceMaterial->m_MaterialType =  mmaMaterial::USE_TEXTURE;
            m_Texture->SetInput(image1);
            m_Actor->SetTexture(m_Texture);
            m_Gui->Enable(ID_TEXTURE_MAPPING_MODE,true);
			m_UseTexture = 1;
			m_Gui->Update();
			mafEventMacro(mafEvent(this, CAMERA_UPDATE));
          }
        }
      }
      break;
      case ID_TEXTURE_MAPPING_MODE:
        GenerateTextureMapCoordinate();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      case ID_RENDERING_DISPLAY_LIST:
        m_Mapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
	 
	  case ID_CHOOSE_FILENAME1:
	  {
								 mafString imageNameTemp = m_File;
								 imageNameTemp.Erase(imageNameTemp.Length() - 6, imageNameTemp.Length() - 1);
								 imageNameTemp.Append(_R("temp.wrl"));
								 
								 
								 vtkMAFSmartPointer<vtkVRMLExporter> writer;
								 writer->SetFileName(imageNameTemp.GetCStr());
								 vtkRenderer* renderer = vtkRenderer::New();
								 renderer->AddActor(m_Actor);
								 vtkRenderWindow* renderWindow = vtkRenderWindow::New();
								 renderWindow->AddRenderer(renderer);

								 wxBusyInfo wait("Writing temp file: ...");
								 
								 //mafEventMacro(mafEvent(m_Gui, BIND_TO_PROGRESSBAR, writer));
								 writer->SetRenderWindow(renderWindow);
								 writer->Write();
								 
								
								 // mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
								 //long dummyProgressValue = 50;
								 //mafEventMacro(mafEvent(this, PROGRESSBAR_SET_VALUE, dummyProgressValue));
								 //writer->SetFileName(m_File.GetCStr());
								 //writer->Write();
								 //mafEventMacro(mafEvent(this, PROGRESSBAR_HIDE));
									
									 vtkMAFSmartPointer<vtkJPEGWriter> exporter;
									 //mafEventMacro(mafEvent(m_Actor->GetTexture()->GetInput(), BIND_TO_PROGRESSBAR, exporter));
									 
									 if (m_Actor->GetTexture()!= NULL)
									 {
										 exporter->SetInput(m_Actor->GetTexture()->GetInput());

										 mafString imageName = m_File;
										 imageName.Erase(imageName.Length() - 5, imageName.Length() - 1);



										 exporter->SetFileName(imageName.Append(_R("jpg")).GetCStr());
										 wxBusyInfo wait2("Writing texture file: ...");
										 exporter->Write();
										 string search1 = "            texture PixelTexture {";
										 string search2 = "          geometry IndexedFaceSet {";
										 string readLine;
										 ifstream input;
										 input.open(imageNameTemp.GetCStr());
										 ofstream output;
										 output.open(m_File.GetCStr());


										 //converting vrml2 using temp file

										 wxBusyInfo wait3("Writing vrml file: ...");

										 //clock_t begin1 = clock();
										 while (std::getline(input, readLine))
										 {


											 if (readLine == search1)
											 {
												 output << (_R("texture ImageTexture{ url \"") + imageName + _R("\" repeatS TRUE repeatT TRUE }}")).toStd() << std::endl;

												 while (readLine != search2)
												 {
													 ;
													 std::getline(input, readLine);
												 }
											 }
											 output << readLine << std::endl;

										 }
										 // clock_t end1 = clock();
										 // clock_t begin2 = clock();
										 /*while (input >> readLine)
										 {
										 if (readLine == search1){
										 readLine = "texture ImageTexture{ url \"" + imageName + "\" repeatS TRUE repeatT TRUE }}";
										 //found = true;

										 readLine += "\n";
										 output << readLine;
										 //if(found) break;
										 while (readLine != search2)
										 {

										 input >> readLine;
										 }
										 }

										 output << readLine << std::endl;
										 }*/
										 // clock_t end2 = clock();

										 input.clear();
										 input.close();
										 output.close();
										 Sleep(1);

										 remove(imageNameTemp.GetCStr());
										 //clock_t time1 = end1 - begin1;
										 //clock_t time2 = end2 - begin2;
										 //ofstream timeFile;
										 //timeFile.open("timeFile.txt");
										 //timeFile << time1 << " " << time2 << std::endl;
										 //timeFile.close();
									 }
	  }

      break;
	  case ID_CHOOSE_FILENAME2:
	  {
								  
								  mafVMEOutputSurface *data = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
								  data->Update();

								  vtkPolyData* datachild = vtkPolyData::SafeDownCast(data->GetVTKData());
								  //assert(data);
								  datachild->Update();
								 
							
								//  vtkMAFSmartPointer<vtkTriangleFilter>triangles;
								//  vtkMAFSmartPointer<vtkTransformPolyDataFilter> v_tpdf;
								//  triangles->SetInput(data->GetSurfaceData());
								//  triangles->Update();

							//	  v_tpdf->SetInput(triangles->GetOutput());
								
							//	  v_tpdf->Update();
								  
								//  vtkPolyData *data2 = v_tpdf->GetOutput();// stlreader->GetOutput();
								//  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
								  						  
								//  mapper->SetInput((vtkPolyData*)data->GetVTKData());
								  

								 // vtkActor *actor= vtkActor::New();
								 // actor->SetMapper(mapper);
								  m_Actor->SetUserTransform(data->GetAbsTransform()->GetVTKTransform());
								 // actor->SetTexture(m_Texture);

								  vtkCleanPolyData* vtkCleaner = vtkCleanPolyData::New();
								  vtkCleaner->SetInput(datachild);
								  vtkCleaner->SetTolerance(0.0);
								  vtkCleaner->ConvertStripsToPolysOn();
								  vtkCleaner->ConvertPolysToLinesOn();
								  vtkCleaner->PointMergingOn();
								  vtkCleaner->Update();
								  ;
								  vtkPolyDataMapper* mapperchild = vtkPolyDataMapper::New();
								  vtkActor* actorchild = vtkActor::New();
								  mapperchild->SetInput(vtkCleaner->GetOutput());
								  actorchild->SetMapper(mapperchild);

								 

								  vtkRenderer* renderer = vtkRenderer::New();
								  renderer->AddActor(actorchild);
								  
								  vtkRenderWindow* renderWindow = vtkRenderWindow::New();
								  renderWindow->AddRenderer(renderer);

								  mafString mtlName = m_File;
								  mtlName.Erase(mtlName.Length() - 5, mtlName.Length() - 1);
								  mtlName.Append(_R("obj.mtl"));

								  wxBusyInfo wait523(m_File.toWx());
								  
								  Sleep(1500);
								  

								  vtkMAFSmartPointer<vtkOBJExporter> writer;
								  writer->SetRenderWindow(renderWindow);
								  mafString name = m_File;
								  writer->SetFilePrefix(name.GetCStr());


								  
								  wxBusyInfo wait("writing obj file ...");
								  writer->Write();
								  Sleep(100);
								  //open MTLfile and add the texture
								  
								  if (m_Actor->GetTexture() != NULL)
								  {
									  wxBusyInfo wait("texture file: ...");
									  Sleep(100);
									  pathName = m_File;
									  pathName.ExtractPathName();

									  vtkMAFSmartPointer<vtkJPEGWriter> exporter;
									  exporter->SetInput(m_Actor->GetTexture()->GetInput());
									  mafString imageName = m_File;
									  imageName.Erase(imageName.Length() - 5, imageName.Length() - 1);

									  exporter->SetFileName(imageName.Append(_R("jpg")).GetCStr());
									  exporter->Write();
									  std::ofstream mtlfile;

									  mtlfile.open(mtlName.GetCStr(), std::ios_base::app);
									  mtlfile << (_R("map_Kd ") + imageName).toStd();
								  }
								  else
								  {

									  wxBusyInfo wait("texture not found ");
									  Sleep(100);
								  }


								  //if comboBox Child included
								  /* int nbrChd = this->m_Node->GetNumberOfChildren();
								   const mafNode::mafChildrenVector *children=this->m_Node->GetChildren();
								   mafID  idx;
								   for (int i = 0; i++; i < nbrChd)
								   {

								   //const mafNode::mafChildrenVector *children=this->m_Node->GetChildren();
								   //mafNode*  m_VMEChild = this->m_Node->GetFirstChild();
								   mafVMEOutputSurface *surface_outputchild=mafVMEOutputSurface::SafeDownCast(this->m_Node->GetFirstChild());
								   //vtkPolyData *data = vtkPolyData::SafeDownCast(surface_outputchild->GetVTKData());
								   //data->Update();

								   surface_outputchild->
								   }*/

							/*	  mafNodeIterator *iter = NULL;
								  m_SubTreeExportOBJ = 1;
								  if (m_SubTreeExportOBJ == TRUE)
								  {
									  iter = this->m_Node->NewIterator();
								  }
								  else
								  {
									  iter = this->m_Node->GetRoot()->NewIterator();
								  }

								  for (mafNode *node = iter->GetFirstNode(); node; node = iter->GetNextNode())
								  {
									  //m_CurrentVme = node;
				  
									  if (node->IsA("mafVMESurface"))
									  {
										  m_OBJs.push_back(node);						  
								 
									  }

								  }
								  iter->Delete();
								  if (m_OBJs.size() != 0)
								  {
									  exportOBJs();
								  }
								 
								  m_OBJs.clear();*/
	  }

		  break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
  else if (maf_event->GetSender() == m_Vme)
  {
    if(maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
    {
      UpdateProperty();
    }
  }
}

void mafPipeSurfaceTextured::exportOBJs()
{
	
	vtkPolyData *datachild;
	mafVMEOutputSurface *surface_outputchild;
	vtkPolyDataMapper* mapperchild;
	vtkActor* actorchild;
	vtkRenderer* rendererchild;
	vtkRenderWindow* renderWindowchild;
	mafString mtlName;
	vtkMAFSmartPointer<vtkOBJExporter> writerchild;
	mafString objName;
	int i = 0;
	//for (int i = 0; i < m_OBJs.size(); i++)
	{
		surface_outputchild = mafVMEOutputSurface::SafeDownCast(m_OBJs[i]);
		//assert(surface_outputchild);
		surface_outputchild->Update();
		datachild = vtkPolyData::SafeDownCast(surface_outputchild->GetVTKData());
		//assert(data);
		datachild->Update();
		mapperchild->SetInput(datachild);
		actorchild->SetMapper(mapperchild);

		rendererchild = vtkRenderer::New();
		rendererchild->AddActor(actorchild);
		renderWindowchild = vtkRenderWindow::New();
		renderWindowchild->AddRenderer(rendererchild);

		/*mtlName = pathName;
		mtlName.Append("\\");
		mtlName.Append(m_OBJs[i]->GetName());
		mtlName.Append(".obj.mtl");*/


		//objName = pathName;
		//objName.Append("\\");
		objName.Append(m_OBJs[i]->GetName());
		wxBusyInfo wait((_R("writing child file...") + objName).toWx());
		writerchild->SetRenderWindow(renderWindowchild);
		writerchild->SetFilePrefix(objName.GetCStr());
		writerchild->Write();
		//open MTLfile and add the texture
		/*if (actorchild->GetTexture() != NULL)
		{

			vtkMAFSmartPointer<vtkJPEGWriter> exporter;
			exporter->SetInput(m_Actor->GetTexture()->GetInput());
			mafString imageName = m_OBJs[i]->GetName();
			

			exporter->SetFileName(imageName.Append(".jpg"));
			exporter->Write();
			std::ofstream mtlfile;

			mtlfile.open(mtlName, std::ios_base::app);
			mtlfile << "map_Kd " + imageName;
		}*/
	}
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::GenerateTextureMapCoordinate()
//----------------------------------------------------------------------------
{
  vtkPolyData *data = vtkPolyData::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  data->Update();

  if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::PLANE_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToPlane> plane_texture_mapper;
    plane_texture_mapper->SetInput(data);
    plane_texture_mapper->AutomaticPlaneGenerationOn();
    vtkPolyData *tdata = (vtkPolyData *)plane_texture_mapper->GetOutput();
    m_Mapper->SetInput(data);
  }
  else if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::CYLINDER_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToCylinder> cylinder_texture_mapper;
    cylinder_texture_mapper->SetInput(data);
    cylinder_texture_mapper->PreventSeamOff();
    m_Mapper->SetInput((vtkPolyData *)cylinder_texture_mapper->GetOutput());
  }
  else if (m_SurfaceMaterial->m_TextureMappingMode == mmaMaterial::SPHERE_MAPPING)
  {
    vtkMAFSmartPointer<vtkTextureMapToSphere> sphere_texture_mapper;
    sphere_texture_mapper->SetInput(data);
    sphere_texture_mapper->PreventSeamOff();
    m_Mapper->SetInput((vtkPolyData *)sphere_texture_mapper->GetOutput());
  }
  else
  {
    m_Mapper->SetInput(data);
  }
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::SetEnableActorLOD(bool value)
//----------------------------------------------------------------------------
{
  m_EnableActorLOD = (int) value;
  if(m_Gui)
    m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafPipeSurfaceTextured::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
  m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
