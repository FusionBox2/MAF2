/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPolyline.cpp,v $
  Language:  C++
  Date:      $Date: 2010-05-31 10:35:13 $
  Version:   $Revision: 1.7.4.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEPolyline.h"
#include "mmaMaterial.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include <sstream>
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEPolyline)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEPolyline::mafVMEPolyline()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEPolyline::~mafVMEPolyline()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric
}


//-------------------------------------------------------------------------
mafVMEOutput *mafVMEPolyline::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputPolyline::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
char** mafVMEPolyline::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEFem.xpm"
	return mafVMEFem_xpm;
}
//-------------------------------------------------------------------------
int mafVMEPolyline::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
int mafVMEPolyline::SetData(vtkPolyData *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  return Superclass::SetData(data,t,mode);
}
//-------------------------------------------------------------------------
int mafVMEPolyline::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  vtkPolyData *polydata = vtkPolyData::SafeDownCast(data);
  
  if (polydata)
    polydata->Update();

  // check this is a polydata containing only lines
  if (polydata && polydata->GetPolys()->GetNumberOfCells()==0 && \
      polydata->GetStrips()->GetNumberOfCells()==0 && \
      polydata->GetVerts()->GetNumberOfCells()==0)
  {
    return Superclass::SetData(data,t,mode);
  }
  
  mafErrorMacro("Trying to set the wrong type of fata inside a VME Polyline :"<< (data?data->GetClassName():"NULL"));
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mmaMaterial *mafVMEPolyline::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    SetAttribute("MaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputPolyline *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}
