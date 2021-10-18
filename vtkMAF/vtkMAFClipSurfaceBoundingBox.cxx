/*=========================================================================

 Program: MAF2
 Module: vtkMAFClipSurfaceBoundingBox
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"

#include "vtkMAFClipSurfaceBoundingBox.h"
#include "vtkMath.h"
#include "vtkClipPolyData.h"
#include "vtkLinearExtrusionFilter.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkLinearSubdivisionFilter.h"
#include "vtkClipPolyData.h"

vtkStandardNewMacro(vtkMAFClipSurfaceBoundingBox);

//-------------------------------------------------------------------------
vtkMAFClipSurfaceBoundingBox::vtkMAFClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
	ClipInside = 0;
    this->SetNumberOfInputPorts(2);
    this->SetNumberOfOutputPorts(1);
}
//-------------------------------------------------------------------------
vtkMAFClipSurfaceBoundingBox::~vtkMAFClipSurfaceBoundingBox()
//-------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int vtkMAFClipSurfaceBoundingBox::FillInputPortInformation(
    int vtkNotUsed(port), vtkInformation* info)
{
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
}

void vtkMAFClipSurfaceBoundingBox::SetMaskData(vtkDataSet* input)
{
    this->Superclass::SetInputData(1, input);
}

//----------------------------------------------------------------------------
// Specify the input data or filter. New style.
void vtkMAFClipSurfaceBoundingBox::SetMaskConnection(vtkAlgorithmOutput* algOutput)
{
    this->Superclass::SetInputConnection(1, algOutput);
}


//-------------------------------------------------------------------------
int vtkMAFClipSurfaceBoundingBox::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPolyData *input = vtkPolyData::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

	double scale_factor=2*input->GetLength();

	vtkLinearExtrusionFilter *extrusionFilter = vtkLinearExtrusionFilter::New();
	extrusionFilter->SetInputConnection(GetInputConnection(1, 0));
	extrusionFilter->SetScaleFactor(scale_factor);
	extrusionFilter->Modified();
	extrusionFilter->Update();

	vtkMAFImplicitPolyData *implicitPolyData = vtkMAFImplicitPolyData::New();
	implicitPolyData->SetInput(extrusionFilter->GetOutput());

	vtkClipPolyData *clipFilter = vtkClipPolyData::New();
	clipFilter->SetInputConnection(GetInputConnection(0, 0));
	clipFilter->SetGenerateClipScalars(0);
	clipFilter->SetClipFunction(implicitPolyData);
	clipFilter->SetInsideOut(ClipInside);
	clipFilter->SetValue(0);
	clipFilter->Update();

	output->DeepCopy(clipFilter->GetOutputDataObject(0));

	clipFilter->Delete();
	implicitPolyData->Delete();
	extrusionFilter->Delete();

	return 1;
}
