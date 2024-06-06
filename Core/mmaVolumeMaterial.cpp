/*=========================================================================

 Program: MAF2
 Module: mmaVolumeMaterial
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

#include "mmaVolumeMaterial.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafGUILutPreset.h"

#include "mafStorageElement.h"
#include "mafIndent.h"

#include "mafNode.h"

#include "vtkSmartPointer.h"
#include "vtkLookupTable.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkMAFTransferFunction2D.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmaVolumeMaterial)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmaVolumeMaterial::mmaVolumeMaterial()
//----------------------------------------------------------------------------
{  
  m_Name        = _R("VolumeMaterialAttributes");
  m_MaterialName= _R("new material");
  vtkNEW(m_ColorLut);

  vtkNEW(m_OpacityTransferFunction);
  vtkNEW(m_GradientTransferFunction);
  vtkNEW(m_ColorTransferFunction);
  vtkNEW(m_VolumeProperty);
  vtkNEW(m_VolumeProperty2);

  m_Level_LUT           = 0.5;
  m_Window_LUT          = 1.0;
  m_HueRange[0]         = 0.0;
  m_HueRange[1]         = 0.6667;
  m_SaturationRange[0]  = 0;
  m_SaturationRange[1]  = 1;
  m_TableRange[0]       = 0.0;
  m_TableRange[1]       = -1.0; // this is an invalid range, needed to be checked by visual pipe, 
                                // that if them found it invalid, initialize with data range
  m_GammaCorrection     = 1.0;
  m_InterpolationType   = 0;
  m_Shade               = 0;

  lutPreset(4,m_ColorLut);
  m_NumValues = m_ColorLut->GetNumberOfTableValues();
}
//----------------------------------------------------------------------------
mmaVolumeMaterial::~mmaVolumeMaterial()
//----------------------------------------------------------------------------
{
  vtkDEL(m_ColorLut);
  vtkDEL(m_VolumeProperty2); 

  vtkDEL(m_OpacityTransferFunction);
  vtkDEL(m_GradientTransferFunction);
  vtkDEL(m_ColorTransferFunction);
  vtkDEL(m_VolumeProperty);
}
//-------------------------------------------------------------------------
void mmaVolumeMaterial::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  // property
  m_MaterialName        = ((mmaVolumeMaterial *)a)->m_MaterialName;
  // lut
  m_Level_LUT           = ((mmaVolumeMaterial *)a)->m_Level_LUT;
  m_Window_LUT          = ((mmaVolumeMaterial *)a)->m_Window_LUT;
  m_HueRange[0]         = ((mmaVolumeMaterial *)a)->m_HueRange[0];
  m_HueRange[1]         = ((mmaVolumeMaterial *)a)->m_HueRange[1];
  m_SaturationRange[0]  = ((mmaVolumeMaterial *)a)->m_SaturationRange[0];
  m_SaturationRange[1]  = ((mmaVolumeMaterial *)a)->m_SaturationRange[1];
  m_TableRange[0]       = ((mmaVolumeMaterial *)a)->m_TableRange[0];
  m_TableRange[1]       = ((mmaVolumeMaterial *)a)->m_TableRange[1];
  m_GammaCorrection     = ((mmaVolumeMaterial *)a)->m_GammaCorrection;
  m_NumValues           = ((mmaVolumeMaterial *)a)->m_NumValues;
  m_InterpolationType   = ((mmaVolumeMaterial *)a)->m_InterpolationType;
  m_Shade               = ((mmaVolumeMaterial *)a)->m_Shade;
  UpdateProp();
}
//----------------------------------------------------------------------------
bool mmaVolumeMaterial::Equals(const mafAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    return (m_MaterialName  == ((mmaVolumeMaterial *)a)->m_MaterialName       &&
      m_Level_LUT           == ((mmaVolumeMaterial *)a)->m_Level_LUT          &&
      m_Window_LUT          == ((mmaVolumeMaterial *)a)->m_Window_LUT         &&
      m_HueRange[0]         == ((mmaVolumeMaterial *)a)->m_HueRange[0]        &&
      m_HueRange[1]         == ((mmaVolumeMaterial *)a)->m_HueRange[1]        &&
      m_SaturationRange[0]  == ((mmaVolumeMaterial *)a)->m_SaturationRange[0] &&
      m_SaturationRange[1]  == ((mmaVolumeMaterial *)a)->m_SaturationRange[1] &&
      m_TableRange[0]       == ((mmaVolumeMaterial *)a)->m_TableRange[0]      &&
      m_TableRange[1]       == ((mmaVolumeMaterial *)a)->m_TableRange[1]      &&
      m_GammaCorrection     == ((mmaVolumeMaterial *)a)->m_GammaCorrection    &&
      m_InterpolationType   == ((mmaVolumeMaterial *)a)->m_InterpolationType  &&
      m_Shade               == ((mmaVolumeMaterial *)a)->m_Shade              &&
      m_NumValues           == ((mmaVolumeMaterial *)a)->m_NumValues);
  }
  return false;
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);
  UpdateFromTables();
  parent[_R("MaterialName")].SetValue(m_MaterialName);
  parent[_R("Level_LUT")].SetValue(m_Level_LUT);
  parent[_R("Window_LUT")].SetValue(m_Window_LUT);
  parent[_R("HueRange0")].SetValue(m_HueRange[0]);
  parent[_R("HueRange1")].SetValue(m_HueRange[1]);
  parent[_R("SaturationRange0")].SetValue(m_SaturationRange[0]);
  parent[_R("SaturationRange1")].SetValue(m_SaturationRange[1]);
  parent[_R("TableRange0")].SetValue(m_TableRange[0]);
  parent[_R("TableRange1")].SetValue(m_TableRange[1]);
  parent[_R("GammaCorrection")].SetValue(m_GammaCorrection);
  parent[_R("NumValues")].SetValue(m_NumValues);

  mafString lutvalues;
  double* rgba;
  for (int v = 0; v < m_NumValues; v++)
  {
    lutvalues = _R("LUT_VALUE_");
    lutvalues += mafToString(v);
    rgba = m_ColorLut->GetTableValue(v);
    parent[lutvalues].SetValue(mafToString(rgba, 4));
  }
  parent[_R("InterpolationType")].SetValue(m_InterpolationType);
  parent[_R("Shade")].SetValue(m_Shade);
  int numOpacityValues = m_OpacityTransferFunction->GetSize();
  parent[_R("NumOpacityValues")].SetValue(m_OpacityTransferFunction->GetSize());
  double* data_values = m_OpacityTransferFunction->GetDataPointer();
  double point[2];
  int p;
  for (p = 0; p < numOpacityValues; p++)
  {
    lutvalues = _R("OPACITY_VALUE_") + mafToString(p);
    point[0] = data_values[2 * p];
    point[1] = data_values[2 * p + 1];
    parent[lutvalues].SetValue(mafToString(point, 2));
  }
  int numGradientValues = m_GradientTransferFunction->GetSize();
  parent[_R("NumGradientValues")].SetValue(numGradientValues);
  data_values = m_GradientTransferFunction->GetDataPointer();
  for (p = 0; p < numGradientValues; p++)
  {
    lutvalues = _R("GRADIENT_VALUE_") + mafToString(p);
    point[0] = data_values[2 * p];
    point[1] = data_values[2 * p + 1];
    parent[lutvalues].SetValue(mafToString(point, 2));
  }
}
//----------------------------------------------------------------------------
void mmaVolumeMaterial::InternalRestore(const mafStorageElement& node)
//----------------------------------------------------------------------------
{
  Superclass::InternalRestore(node);
  m_MaterialName = node[_R("MaterialName")].As<mafString>();
  m_Level_LUT = node[_R("Level_LUT")].As<double>();
  m_Window_LUT = node[_R("Window_LUT")].As<double>();
  m_HueRange[0] = node[_R("HueRange0")].As<double>();
  m_HueRange[1] = node[_R("HueRange1")].As<double>();
  m_SaturationRange[0] = node[_R("SaturationRange0")].As<double>();
  m_SaturationRange[1] = node[_R("SaturationRange1")].As<double>();
  m_TableRange[0] = node[_R("TableRange0")].As<double>();
  m_TableRange[1] = node[_R("TableRange1")].As<double>();
  m_GammaCorrection = node[_R("GammaCorrection")].As<double>();
  m_NumValues = node[_R("NumValues")].As<int>();

  m_ColorLut->SetNumberOfTableValues(m_NumValues);
  mafString lutvalues;
  double rgba[4];
  for (int v = 0; v < m_NumValues; v++)
  {
    lutvalues = _R("LUT_VALUE_");
    lutvalues += mafToString(v);
    mafParseVector(node[lutvalues].As<mafString>(), rgba, 4);
    m_ColorLut->SetTableValue(v, rgba);
  }
  m_ColorLut->SetTableRange(m_TableRange);
  m_ColorLut->SetRange(m_TableRange);
  m_ColorLut->Build();
  m_InterpolationType = node[_R("InterpolationType")].As<int>();
  m_Shade = node[_R("Shade")].As<int>();
  int numOpacityValues = node[_R("NumOpacityValues")].As<int>();

  double point[2];
  int p;
  for (p = 0; p < numOpacityValues; p++)
  {
    lutvalues = _R("OPACITY_VALUE_");
    lutvalues += mafToString(p);
    mafParseVector(node[lutvalues].As<mafString>(), point, 2);
    m_OpacityTransferFunction->AddPoint(point[0], point[1]);
  }
  int numGradientValues = node[_R("NumGradientValues")].As<int>();
  for (p = 0; p < numGradientValues; p++)
  {
    lutvalues = _R("GRADIENT_VALUE_");
    lutvalues += mafToString(p);
    mafParseVector(node[lutvalues].As<mafString>(), point, 2);
    m_GradientTransferFunction->AddPoint(point[0], point[1]);
  }
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::UpdateProp()
//-----------------------------------------------------------------------
{
  m_VolumeProperty->SetScalarOpacity(m_OpacityTransferFunction);
  m_VolumeProperty->SetGradientOpacity(m_GradientTransferFunction);
  m_VolumeProperty->SetColor(m_ColorTransferFunction);
  m_VolumeProperty->SetInterpolationType(m_InterpolationType);
  m_VolumeProperty->SetShade(m_Shade);

  m_ColorLut->SetHueRange(m_HueRange);
  m_ColorLut->SetSaturationRange(m_SaturationRange);
  m_ColorLut->SetNumberOfTableValues(m_NumValues);
  m_ColorLut->SetTableRange(m_TableRange);
  m_ColorLut->SetRange(m_TableRange);
  m_ColorLut->Build();

  m_Window_LUT = m_TableRange[1] - m_TableRange[0];
  m_Level_LUT  = (m_TableRange[1] + m_TableRange[0])* .5;
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::UpdateFromTables()
//-----------------------------------------------------------------------
{
  m_ColorLut->GetHueRange(m_HueRange);
  m_ColorLut->GetSaturationRange(m_SaturationRange);
  m_NumValues = m_ColorLut->GetNumberOfTableValues();

  // Added by Losi 09.23.09
  // BUG 1809 Fix
  // When storing the material if m_TableRange attribute represents the invalid range [0, -1] (set in the constructor)
  // it mustn't be updated from m_ColorLut's GetTableRange method that returns a valid range [0, 1]
  // This avoids storing a valid range when the invalid range is set

  if(!(m_TableRange[0] == 0 && m_TableRange[1] == -1))
  {
	  m_ColorLut->GetTableRange(m_TableRange);
  }
  m_Window_LUT = m_TableRange[1] - m_TableRange[0];
  m_Level_LUT  = (m_TableRange[1] + m_TableRange[0])* .5;
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::ApplyGammaCorrection(const int preset)
//-----------------------------------------------------------------------
{
  if (-1 != preset)
  {
    lutPreset(preset, m_ColorLut);
  }
  
  if(1.0 == m_GammaCorrection)
  {
    //useless calculation
    return;
  }
  for(int i=0; i<m_NumValues;i++)
  {
    double rgba[4];
    m_ColorLut->GetTableValue(i,rgba);
    rgba[0] = pow(rgba[0], 1./m_GammaCorrection);
    rgba[0] = rgba[0] < 0. ?  0.: rgba[0];
    rgba[0] = rgba[0] > 1. ?  1.: rgba[0];
    rgba[1] = pow(rgba[1], 1./m_GammaCorrection);
    rgba[1] = rgba[1] < 0. ?  0.: rgba[1];
    rgba[1] = rgba[1] > 1. ?  1.: rgba[1];
    rgba[2] = pow(rgba[2], 1./m_GammaCorrection);
    rgba[2] = rgba[2] < 0. ?  0.: rgba[2];
    rgba[2] = rgba[2] > 1. ?  1.: rgba[2];
    m_ColorLut->SetTableValue(i,rgba);
  }
}
//-----------------------------------------------------------------------
void mmaVolumeMaterial::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}
