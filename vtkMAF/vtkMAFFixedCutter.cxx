/*=========================================================================

 Program: MAF2
 Module: vtkMAFFixedCutter
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkMAFFixedCutter.h"
#include "vtkViewport.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkDataSet.h"

vtkCxxRevisionMacro(vtkMAFFixedCutter, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFFixedCutter);

//----------------------------------------------------------------------------
vtkMAFFixedCutter::vtkMAFFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
vtkMAFFixedCutter::~vtkMAFFixedCutter()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int vtkMAFFixedCutter::RequestData(
  vtkInformation *request,
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
    if (this->Superclass::RequestData(request, inputVector, outputVector) == 0)
    {
        return 0;
    }

  // get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkDataSet *input = vtkDataSet::SafeDownCast(
    inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  if(output->GetNumberOfPoints() == 0)
  {
	  vtkPoints *pts = vtkPoints::New();
	  pts->InsertNextPoint(input->GetCenter());
    output->SetPoints(pts);
    pts->Delete();
  }
  output->GetPointData()->SetNormals(NULL);
  return 1;
}
