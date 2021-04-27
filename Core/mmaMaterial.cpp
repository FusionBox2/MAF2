/*=========================================================================

 Program: MAF2
 Module: mmaMaterial
 Authors: Paolo Quadrani
 
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

#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/event.h>
#include <wx/settings.h>

#include "mmaMaterial.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUILutPreset.h"

#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafNode.h"

#include "vtkMAFSmartPointer.h"
#include "vtkProperty.h"
#include "vtkMAFTransferFunction2D.h"
#include "vtkTexturedSphereSource.h"
#include "vtkTexture.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkImageExport.h"
#include "vtkLookupTable.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmaMaterial)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmaMaterial::mmaMaterial()
//----------------------------------------------------------------------------
{  
	m_MaterialType= USE_VTK_PROPERTY;

  m_Name        = _R("MaterialAttributes");
  m_MaterialName= _R("new material");
  vtkNEW(m_ColorLut);
  vtkNEW(m_Prop);
  lutPreset(4,m_ColorLut);
  m_Icon        = NULL;
  m_TextureImage= NULL;

  m_TextureMappingMode = PLANE_MAPPING;

  m_Value            = 1.0;
  m_Ambient[0]       = 1.0;
  m_Ambient[1]       = 1.0;
  m_Ambient[2]       = 1.0;
  m_AmbientIntensity = 0.0;
  wxColour color = mafRandomColor();
  m_Diffuse[0]       = color.Red()   /255.0;
  m_Diffuse[1]       = color.Green() /255.0;
  m_Diffuse[2]       = color.Blue()  /255.0;
  m_DiffuseIntensity = 1.0;
  m_Specular[0]      = 1.0;
  m_Specular[1]      = 1.0;
  m_Specular[2]      = 1.0;
  m_SpecularIntensity= 0.0;
  m_SpecularPower    = 0.0;
  m_Opacity          = 1.0;
  m_Representation   = 2.0;

  m_HueRange[0]         = 0.0;
  m_HueRange[1]         = 0.6667;
  m_SaturationRange[0]  = 0;
  m_SaturationRange[1]  = 1;
  m_TableRange[0]       = 0.0;
  m_TableRange[1]       = 1.0;
  m_NumValues           = 128;

  m_TextureID           = -1;

  UpdateProp();
}
//----------------------------------------------------------------------------
mmaMaterial::~mmaMaterial()
//----------------------------------------------------------------------------
{
  vtkDEL(m_ColorLut);
	vtkDEL(m_Prop); 
	cppDEL(m_Icon);
}
//----------------------------------------------------------------------------
wxBitmap *mmaMaterial::MakeIcon()
//----------------------------------------------------------------------------
{
  //UpdateProp();

	vtkMAFSmartPointer<vtkCamera> camera;
  camera->ParallelProjectionOff();
  camera->SetViewAngle(30);
  camera->SetFocalPoint(0,0,0);
  camera->SetPosition(0,0,2);
  camera->SetViewUp(0,1,0);

	vtkMAFSmartPointer<vtkLight> light;
  light->SetPosition(-1,1,1);

  wxColour col = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
  float r = col.Red()  / 255.0;
  float g = col.Green()/ 255.0;
  float b = col.Blue() / 255.0;
  
	vtkMAFSmartPointer<vtkRenderer> ren;
  ren->SetBackground(r,g,b);
	ren->AddLight(light);
	ren->LightFollowCameraOff();
	ren->SetActiveCamera(camera);
	
  vtkMAFSmartPointer<vtkRenderWindow> renwin;
  renwin->AddRenderer(ren);
  renwin->OffScreenRenderingOn(); 
	renwin->SetSize(25, 25);

	vtkMAFSmartPointer<vtkTexturedSphereSource> ss;
	ss->SetPhiResolution(20);
	ss->SetThetaResolution(20);
	if (this->m_Prop->GetRepresentation() == 1)
	{
		ss->SetThetaResolution(10);
		ss->SetPhiResolution(3);
	}
	else 
	{
		ss->SetThetaResolution(20);
		ss->SetPhiResolution(20);
  } 
	
	vtkMAFSmartPointer<vtkTexture> texture;
  if (m_MaterialType == USE_TEXTURE)
  {
    texture->SetInput(m_TextureImage);
  }
  
  vtkMAFSmartPointer<vtkPolyDataMapper> pdm;
	pdm->SetInput(ss->GetOutput());
	pdm->SetImmediateModeRendering(0);

	vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(pdm);
  if (m_MaterialType == USE_VTK_PROPERTY)
  {
    actor->SetProperty(this->m_Prop);
  }
  else if (m_MaterialType == USE_TEXTURE)
  {
    actor->SetTexture(texture);
  }
  actor->SetPickable(1);
  actor->SetVisibility(1);
  actor->SetPosition(0,0,0); 
  actor->SetScale(1,1,1); 
	ren->AddActor(actor);
	
  renwin->Render();

  //capture image from renderer
	vtkMAFSmartPointer<vtkWindowToImageFilter> w2i;
	w2i->SetInput(renwin);
	w2i->Update();

  int dim[3];
	w2i->GetOutput()->GetDimensions(dim);
  assert( dim[0]==25 && dim[1]==25 );
	unsigned char buffer[25*25*3];

  //flip it - windows Bitmap are upside-down
  vtkMAFSmartPointer<vtkImageExport> ie;
	ie->SetInput(w2i->GetOutput());
  ie->ImageLowerLeftOff();
  ie->SetExportVoidPointer(buffer);
	ie->Export();

	//translate to a wxBitmap
	wxImage  *img = new wxImage(dim[0],dim[1],buffer,TRUE);
	wxBitmap *bmp = new wxBitmap(*img);
  delete img;

  cppDEL(m_Icon);

	m_Icon = bmp;
	return bmp;
}
//-------------------------------------------------------------------------
void mmaMaterial::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  // property
  m_MaterialName        = ((mmaMaterial *)a)->m_MaterialName;
  m_Value               = ((mmaMaterial *)a)->m_Value;
  m_Ambient[0]          = ((mmaMaterial *)a)->m_Ambient[0];
  m_Ambient[1]          = ((mmaMaterial *)a)->m_Ambient[1];
  m_Ambient[2]          = ((mmaMaterial *)a)->m_Ambient[2];
  m_AmbientIntensity    = ((mmaMaterial *)a)->m_AmbientIntensity;
  m_Diffuse[0]          = ((mmaMaterial *)a)->m_Diffuse[0];
  m_Diffuse[1]          = ((mmaMaterial *)a)->m_Diffuse[1];
  m_Diffuse[2]          = ((mmaMaterial *)a)->m_Diffuse[2];
  m_DiffuseIntensity    = ((mmaMaterial *)a)->m_DiffuseIntensity;
  m_Specular[0]         = ((mmaMaterial *)a)->m_Specular[0];
  m_Specular[1]         = ((mmaMaterial *)a)->m_Specular[1];
  m_Specular[2]         = ((mmaMaterial *)a)->m_Specular[2];
  m_SpecularIntensity   = ((mmaMaterial *)a)->m_SpecularIntensity;
  m_SpecularPower       = ((mmaMaterial *)a)->m_SpecularPower;
  m_Opacity             = ((mmaMaterial *)a)->m_Opacity;
  m_Representation      = ((mmaMaterial *)a)->m_Representation;
  // texture
  m_TextureID           = ((mmaMaterial *)a)->m_TextureID;
  m_TextureMappingMode  = ((mmaMaterial *)a)->m_TextureMappingMode;
  m_TextureImage        = ((mmaMaterial *)a)->m_TextureImage;
  // lut
  m_HueRange[0]         = ((mmaMaterial *)a)->m_HueRange[0];
  m_HueRange[1]         = ((mmaMaterial *)a)->m_HueRange[1];
  m_SaturationRange[0]  = ((mmaMaterial *)a)->m_SaturationRange[0];
  m_SaturationRange[1]  = ((mmaMaterial *)a)->m_SaturationRange[1];
  m_TableRange[0]       = ((mmaMaterial *)a)->m_TableRange[0];
  m_TableRange[1]       = ((mmaMaterial *)a)->m_TableRange[1];
  m_NumValues           = ((mmaMaterial *)a)->m_NumValues;
  
  m_MaterialType        = ((mmaMaterial *)a)->m_MaterialType;
  UpdateProp();
}
//----------------------------------------------------------------------------
bool mmaMaterial::Equals(const mafAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    return (m_MaterialName  == ((mmaMaterial *)a)->m_MaterialName       &&
      m_Value               == ((mmaMaterial *)a)->m_Value              &&
      m_Ambient[0]          == ((mmaMaterial *)a)->m_Ambient[0]         &&
      m_Ambient[1]          == ((mmaMaterial *)a)->m_Ambient[1]         &&
      m_Ambient[2]          == ((mmaMaterial *)a)->m_Ambient[2]         &&
      m_AmbientIntensity    == ((mmaMaterial *)a)->m_AmbientIntensity   &&
      m_Diffuse[0]          == ((mmaMaterial *)a)->m_Diffuse[0]         &&
      m_Diffuse[1]          == ((mmaMaterial *)a)->m_Diffuse[1]         &&
      m_Diffuse[2]          == ((mmaMaterial *)a)->m_Diffuse[2]         &&
      m_DiffuseIntensity    == ((mmaMaterial *)a)->m_DiffuseIntensity   &&
      m_Specular[0]         == ((mmaMaterial *)a)->m_Specular[0]        &&
      m_Specular[1]         == ((mmaMaterial *)a)->m_Specular[1]        &&
      m_Specular[2]         == ((mmaMaterial *)a)->m_Specular[2]        &&
      m_SpecularIntensity   == ((mmaMaterial *)a)->m_SpecularIntensity  &&
      m_SpecularPower       == ((mmaMaterial *)a)->m_SpecularPower      &&
      m_Opacity             == ((mmaMaterial *)a)->m_Opacity            &&
      m_TextureID           == ((mmaMaterial *)a)->m_TextureID          &&
      m_TextureImage        == ((mmaMaterial *)a)->m_TextureImage       &&
      m_HueRange[0]         == ((mmaMaterial *)a)->m_HueRange[0]        &&
      m_HueRange[1]         == ((mmaMaterial *)a)->m_HueRange[1]        &&
      m_SaturationRange[0]  == ((mmaMaterial *)a)->m_SaturationRange[0] &&
      m_SaturationRange[1]  == ((mmaMaterial *)a)->m_SaturationRange[1] &&
      m_TableRange[0]       == ((mmaMaterial *)a)->m_TableRange[0]      &&
      m_TableRange[1]       == ((mmaMaterial *)a)->m_TableRange[1]      &&
      m_NumValues           == ((mmaMaterial *)a)->m_NumValues          &&
      m_Representation      == ((mmaMaterial *)a)->m_Representation     &&
      m_MaterialType        == ((mmaMaterial *)a)->m_MaterialType       &&
      m_TextureMappingMode  == ((mmaMaterial *)a)->m_TextureMappingMode);
  }
  return false;
}
//-----------------------------------------------------------------------
int mmaMaterial::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    // property
    parent->StoreText(_R("MaterialName"),m_MaterialName);
    parent->StoreDouble(_R("Value"), m_Value);
    parent->StoreDouble(_R("Ambient0"), m_Ambient[0]);
    parent->StoreDouble(_R("Ambient1"), m_Ambient[1]);
    parent->StoreDouble(_R("Ambient2"), m_Ambient[2]);
    parent->StoreDouble(_R("AmbientIntensity"), m_AmbientIntensity);
    parent->StoreDouble(_R("Diffuse0"), m_Diffuse[0]);
    parent->StoreDouble(_R("Diffuse1"), m_Diffuse[1]);
    parent->StoreDouble(_R("Diffuse2"), m_Diffuse[2]);
    parent->StoreDouble(_R("DiffuseIntensity"), m_DiffuseIntensity);
    parent->StoreDouble(_R("Specular0"), m_Specular[0]);
    parent->StoreDouble(_R("Specular1"), m_Specular[1]);
    parent->StoreDouble(_R("Specular2"), m_Specular[2]);
    parent->StoreDouble(_R("SpecularIntensity"), m_SpecularIntensity);
    parent->StoreDouble(_R("SpecularPower"), m_SpecularPower);
    parent->StoreDouble(_R("Opacity"), m_Opacity);
    parent->StoreDouble(_R("Representation"), m_Representation);
    if (m_MaterialType == USE_LOOKUPTABLE)
    {
      // lut
      parent->StoreDouble(_R("HueRange0"), m_HueRange[0]);
      parent->StoreDouble(_R("HueRange1"), m_HueRange[1]);
      parent->StoreDouble(_R("SaturationRange0"), m_SaturationRange[0]);
      parent->StoreDouble(_R("SaturationRange1"), m_SaturationRange[1]);
      parent->StoreDouble(_R("TableRange0"), m_TableRange[0]);
      parent->StoreDouble(_R("TableRange1"), m_TableRange[1]);
      parent->StoreInteger(_R("NumValues"), m_NumValues);
      mafString lutvalues;
      for (int v = 0; v < m_NumValues; v++)
      {
				lutvalues = _R("LUT_VALUE_");
        lutvalues += mafToString(v);
        double *rgba = m_ColorLut->GetTableValue(v);
        parent->StoreVectorN(lutvalues,rgba,4);
      }
    }
    else if (m_MaterialType == USE_TEXTURE)
    {
      // texture
      parent->StoreInteger(_R("TextureID"), m_TextureID);
      parent->StoreInteger(_R("TextureMappingMode"), m_TextureMappingMode);
    }
    parent->StoreInteger(_R("MaterialType"), m_MaterialType);
    return MAF_OK;
  }
  return MAF_ERROR;
}
//----------------------------------------------------------------------------
int mmaMaterial::InternalRestore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    // property
    node->RestoreText(_R("MaterialName"),m_MaterialName);
    node->RestoreDouble(_R("Value"),m_Value);
    node->RestoreDouble(_R("Ambient0"), m_Ambient[0]);
    node->RestoreDouble(_R("Ambient1"), m_Ambient[1]);
    node->RestoreDouble(_R("Ambient2"), m_Ambient[2]);
    node->RestoreDouble(_R("AmbientIntensity"), m_AmbientIntensity);
    node->RestoreDouble(_R("Diffuse0"), m_Diffuse[0]);
    node->RestoreDouble(_R("Diffuse1"), m_Diffuse[1]);
    node->RestoreDouble(_R("Diffuse2"), m_Diffuse[2]);
    node->RestoreDouble(_R("DiffuseIntensity"), m_DiffuseIntensity);
    node->RestoreDouble(_R("Specular0"), m_Specular[0]);
    node->RestoreDouble(_R("Specular1"), m_Specular[1]);
    node->RestoreDouble(_R("Specular2"), m_Specular[2]);
    node->RestoreDouble(_R("SpecularIntensity"), m_SpecularIntensity);
    node->RestoreDouble(_R("SpecularPower"), m_SpecularPower);
    node->RestoreDouble(_R("Opacity"), m_Opacity);
    node->RestoreDouble(_R("Representation"), m_Representation);
    node->RestoreInteger(_R("MaterialType"), m_MaterialType);
    if (m_MaterialType == USE_LOOKUPTABLE)
    {
      // lut
      node->RestoreDouble(_R("HueRange0"), m_HueRange[0]);
      node->RestoreDouble(_R("HueRange1"), m_HueRange[1]);
      node->RestoreDouble(_R("SaturationRange0"), m_SaturationRange[0]);
      node->RestoreDouble(_R("SaturationRange1"), m_SaturationRange[1]);
      node->RestoreDouble(_R("TableRange0"), m_TableRange[0]);
      node->RestoreDouble(_R("TableRange1"), m_TableRange[1]);
      node->RestoreInteger(_R("NumValues"), m_NumValues);
      m_ColorLut->SetNumberOfTableValues(m_NumValues);
      mafString lutvalues;
      double rgba[4];
      for (int v = 0; v < m_NumValues; v++)
      {
				lutvalues = _R("LUT_VALUE_");
				lutvalues += mafToString(v);
				node->RestoreVectorN(lutvalues,rgba,4);
				m_ColorLut->SetTableValue(v,rgba);
      }
    }
    else if (m_MaterialType == USE_TEXTURE)
    {
      // texture
      node->RestoreInteger(_R("TextureID"), m_TextureID);
      node->RestoreInteger(_R("TextureMappingMode"), m_TextureMappingMode);
    }
    UpdateProp();
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void mmaMaterial::UpdateProp()
//-----------------------------------------------------------------------
{
  m_Prop->SetAmbientColor(m_Ambient);
  m_Prop->SetAmbient(m_AmbientIntensity);
  m_Prop->SetDiffuseColor(m_Diffuse);
  m_Prop->SetDiffuse(m_DiffuseIntensity);
  m_Prop->SetSpecularColor(m_Specular);
  m_Prop->SetSpecular(m_SpecularIntensity);
  m_Prop->SetSpecularPower(m_SpecularPower);
  m_Prop->SetOpacity(m_Opacity);
  m_Prop->SetRepresentation((int)m_Representation);
  m_Prop->Modified();

  m_ColorLut->SetHueRange(m_HueRange);
  m_ColorLut->SetSaturationRange(m_SaturationRange);
  m_ColorLut->SetNumberOfTableValues(m_NumValues);
  m_ColorLut->SetTableRange(m_TableRange);
  m_ColorLut->Build();
}
//-----------------------------------------------------------------------
void mmaMaterial::UpdateFromLut()
//-----------------------------------------------------------------------
{
  m_ColorLut->GetHueRange(m_HueRange);
  m_ColorLut->GetSaturationRange(m_SaturationRange);
  m_NumValues = m_ColorLut->GetNumberOfTableValues();
  m_ColorLut->GetTableRange(m_TableRange);
}
//-----------------------------------------------------------------------
void mmaMaterial::SetMaterialTexture(vtkImageData *tex)
//-----------------------------------------------------------------------
{
  m_TextureImage  = tex;
  m_TextureID     = -1;
}
//-----------------------------------------------------------------------
void mmaMaterial::SetMaterialTexture(int tex_id)
//-----------------------------------------------------------------------
{
  m_TextureID     = tex_id;
  m_TextureImage  = NULL;
}
//-----------------------------------------------------------------------
vtkImageData *mmaMaterial::GetMaterialTexture()
//-----------------------------------------------------------------------
{
  return m_TextureImage;
}
//-----------------------------------------------------------------------
int mmaMaterial::GetMaterialTextureID()
//-----------------------------------------------------------------------
{
  return m_TextureID;
}
//-----------------------------------------------------------------------
void mmaMaterial::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
