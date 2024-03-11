/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTextureOrientationFilter.cpp,v $
Language:  C++
Date:      $Date: 2009-01-29 17:17:54 $
Version:   $Revision: 1.1.2.2 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "lhpTextureOrientationFilter.h"
#include "lhpTextureOrientationCalculator.h"
#include "lhpTextureOrientationUseful.h"

#include "vtkObjectFactory.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCommand.h"

#include <assert.h>
#include <ostream>


//----------------------------------------------------------------------------
using namespace lhpTextureOrientation ;
//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
// mandatory vtk macro


//----------------------------------------------------------------------------
// standard New() method
vtkStandardNewMacro(lhpTextureOrientationFilter);



//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
lhpTextureOrientationFilter::lhpTextureOrientationFilter() : m_target_component(0), 
m_outputFormat(VectorFormat), m_texWinDimsSet(false), m_texWinSizeSet(false), 
m_texWinStepSet(false), m_texWinStepSizeSet(false), m_progress(NULL)
{
}



//----------------------------------------------------------------------------
// Destructor
lhpTextureOrientationFilter::~lhpTextureOrientationFilter() 
//----------------------------------------------------------------------------
{
}



//----------------------------------------------------------------------------
// Initialize method to refresh filter and output data
void lhpTextureOrientationFilter::Initialize()
//----------------------------------------------------------------------------
{
  m_output->Initialize() ;
}




//----------------------------------------------------------------------------
// set target component
void lhpTextureOrientationFilter::SetTargetComponent(int c)
//----------------------------------------------------------------------------
{
  if (c != m_target_component){
    m_target_component = c ;
    this->Modified() ;
  }
}



//----------------------------------------------------------------------------
// Set the dimensions of the texture window
void lhpTextureOrientationFilter::SetTexWinDimensions(int dimsx, int dimsy, int dimsz)
//----------------------------------------------------------------------------
{
  m_texWinDimensions[0] = dimsx ;
  m_texWinDimensions[1] = dimsy ;
  m_texWinDimensions[2] = dimsz ;

  m_texWinDimsSet = true ;
  m_texWinSizeSet = false ;
  this->Modified() ;
}


//----------------------------------------------------------------------------
// Set the dimensions of the texture window
void lhpTextureOrientationFilter::SetTexWinDimensions(const int dims[3])
//----------------------------------------------------------------------------
{
  SetTexWinDimensions(dims[0], dims[1], dims[2]) ;
}


//----------------------------------------------------------------------------
// Set the size of the texture window in world coords
void lhpTextureOrientationFilter::SetTexWinSize(double size)
//----------------------------------------------------------------------------
{
  m_texWinSize = size ;

  m_texWinSizeSet = true ;
  m_texWinDimsSet = false ;
  this->Modified() ;
}


//----------------------------------------------------------------------------
// Set the step increment of the texture window
void lhpTextureOrientationFilter::SetTexWinStep(int stepx, int stepy, int stepz)
//----------------------------------------------------------------------------
{
  m_texWinStep[0] = stepx ;
  m_texWinStep[1] = stepy ;
  m_texWinStep[2] = stepz ;

  m_texWinStepSet = true ;
  m_texWinStepSizeSet = false ;
  this->Modified() ;

}


//----------------------------------------------------------------------------
// Set the step increment of the texture window
void lhpTextureOrientationFilter::SetTexWinStep(const int step[3])
//----------------------------------------------------------------------------
{
  SetTexWinStep(step[0], step[1], step[2]) ;
}


//----------------------------------------------------------------------------
// Set the step increment of the texture window in world coords
void lhpTextureOrientationFilter::SetTexWinStepSize(double step)
//----------------------------------------------------------------------------
{
  m_texWinStepSize = step ;

  m_texWinStepSizeSet = true ;
  m_texWinStepSet = false ;
  this->Modified() ;
}




//----------------------------------------------------------------------------
// The Execute function calls the appropriate execute template for the input scalar type
void lhpTextureOrientationFilter::Execute()
//----------------------------------------------------------------------------
{
  m_input = (vtkImageData*)this->GetInput() ;
  m_output = this->GetOutput() ;

  // Refresh the filter and output data
  Initialize() ;

  // Run the filter
  FilterExecute() ;
}











//----------------------------------------------------------------------------
// Execute method, normally templated but no need in this case
void lhpTextureOrientationFilter::FilterExecute()
//----------------------------------------------------------------------------
{
  int x, y, z ;
  int inputExt[6], inputWholeExt[6], dims[3] ;
  double origin[3], spacing[3] ;


  // Get no. of components
  int ncompsIn = m_input->GetNumberOfScalarComponents() ;

  // Get properties of the input image
  //m_input->GetWholeExtent(inputWholeExt) ;
  m_input->GetExtent(inputExt) ;
  m_input->GetOrigin(origin) ;
  m_input->GetSpacing(spacing) ;
  m_input->GetDimensions(dims) ;



  //--------------------------------------------------------------------------
  // Define the dimensions and increment step of the moving texture window
  //--------------------------------------------------------------------------
  if (!m_texWinDimsSet){
    if (m_texWinSizeSet){
      // calculate dimensions from size in world coords
      int d0 = lhpTextureOrientationUseful::Round(this->m_texWinSize / spacing[0]) ;
      int d1 = lhpTextureOrientationUseful::Round(this->m_texWinSize / spacing[1]) ;
      int d2 = lhpTextureOrientationUseful::Round(this->m_texWinSize / spacing[2]) ;
      SetTexWinDimensions(d0, d1, d2) ;
    }
    else{
      // default is whole image
      SetTexWinDimensions(dims) ;
    }
  }

  if (!m_texWinStepSet){
    if (m_texWinStepSizeSet){
      // calculate step from step size in world coords
      int s0 = lhpTextureOrientationUseful::Round(this->m_texWinStepSize / spacing[0]) ;
      int s1 = lhpTextureOrientationUseful::Round(this->m_texWinStepSize / spacing[1]) ;
      int s2 = lhpTextureOrientationUseful::Round(this->m_texWinStepSize / spacing[2]) ;
      SetTexWinStep(s0, s1, s2) ;
    }
    else{
      // default is zero step (no movement at all)
      SetTexWinStep(0,0,0) ;
    }
  }

  //--------------------------------------------------------------------------
  // Calculate how many whole texture windows you can get into the image
  // and calculate the start and finish positions of the texture windows
  //--------------------------------------------------------------------------
  if (m_texWinDimensions[0] == 0)
    m_numx = 1 ;
  else
    m_numx = (dims[0] - m_texWinDimensions[0]) / m_texWinStep[0] + 1 ;
  int xstart = inputWholeExt[0] ;
  int xlast = xstart + (m_numx-1)*m_texWinStep[0] ;
  assert(xlast + m_texWinDimensions[0] - 1 <= inputWholeExt[1]) ;

  if (m_texWinDimensions[1] == 0)
    m_numy = 1 ;
  else
    m_numy = (dims[1] - m_texWinDimensions[1]) / m_texWinStep[1] + 1 ;
  int ystart = inputWholeExt[2] ;
  int ylast = ystart + (m_numy-1)*m_texWinStep[1] ;
  assert(ylast + m_texWinDimensions[1] - 1 <= inputWholeExt[3]) ;

  if (m_texWinDimensions[2] == 0)
    m_numz = 1 ;
  else
    m_numz = (dims[2] - m_texWinDimensions[2]) / m_texWinStep[2] + 1 ;
  int zstart = inputWholeExt[4] ;
  int zlast = zstart + (m_numz-1)*m_texWinStep[2] ;
  assert(zlast + m_texWinDimensions[2] - 1 <= inputWholeExt[5]) ;





  //--------------------------------------------------------------------------
  // Create points and attribute arrays (vector or tensor as requested by user)
  //--------------------------------------------------------------------------
  vtkPoints *points = vtkPoints::New() ;

  vtkFloatArray *scalars ;
  vtkFloatArray *vectors ;
  vtkFloatArray *tensors ;

  scalars = vtkFloatArray::New() ;
  scalars->SetNumberOfComponents(1) ;
  scalars->SetName("scalars") ;

  if (m_outputFormat == VectorFormat){
    vectors = vtkFloatArray::New() ;
    vectors->SetNumberOfComponents(3) ;
    vectors->SetName("vectors") ;
  }
  if (m_outputFormat == TensorFormat){
    tensors = vtkFloatArray::New() ;
    tensors->SetNumberOfComponents(9) ;
    tensors->SetName("tensors") ;
  }



  //--------------------------------------------------------------------------
  // Set up the orientation calculator
  //--------------------------------------------------------------------------
  lhpTextureOrientationCalculator *texCalc = new lhpTextureOrientationCalculator() ;
  texCalc->SetInputImage(m_input) ;
  texCalc->SetVoiDimensions(m_texWinDimensions) ;
  texCalc->SetTextureStatistic(CooCorrelation) ;
  texCalc->SetResolutionOfSamples(7, 12, 15) ;


  //--------------------------------------------------------------------------
  // set up progress reporting
  //--------------------------------------------------------------------------
  int numTotal = m_numx * m_numy * m_numz ;
  double progressInc = 100.0 / (double)numTotal ;
  m_progress = 0.0 ;
  InvokeEvent(ProgressEventId, &m_progress) ;

  


  //--------------------------------------------------------------------------
  // Process the image
  //--------------------------------------------------------------------------
  for (z = zstart;  z <= zlast ;  z += m_texWinStep[2]){
    for (y = ystart;  y <= ylast ;  y += m_texWinStep[1]){
      for (x = xstart ;  x <= xlast ;  x += m_texWinStep[0]){
        //------------------------------------------------------
        // Run the orientation calculator
        //------------------------------------------------------
        texCalc->SetVoiPosition(x,y,z) ;
        texCalc->Execute() ;

        //------------------------------------------------------
        // Create new polydata point in the centre of the window and transfer data to it
        //------------------------------------------------------
        double xcoords[3] ;
        xcoords[0] = origin[0] + ((double)x + (double)m_texWinDimensions[0]/2.0) * spacing[0] ;
        xcoords[1] = origin[1] + ((double)y + (double)m_texWinDimensions[1]/2.0) * spacing[1] ;
        xcoords[2] = origin[2] + ((double)z + (double)m_texWinDimensions[2]/2.0) * spacing[2] ;
        points->InsertNextPoint(xcoords) ;

        double evals[3] ;
        texCalc->GetEigenvalues(evals) ;
        double scal = evals[0] / (evals[0] + evals[1] + evals[2]) ;
        scal = (3.0*scal - 1.0) / 2.0 ;
        scalars->InsertNextTuple(&scal) ;

        if (m_outputFormat == VectorFormat){
          double Vec[3] ;
          texCalc->GetEigenvectorScaled(Vec, 0) ;
          vectors->InsertNextTuple(Vec) ;
        }
        else if (m_outputFormat == TensorFormat){
          double Vmat[9] ;
          texCalc->GetEigenvectorMatrixScaled(Vmat) ;
          tensors->InsertNextTuple(Vmat) ;
        }
      } // for x


      //------------------------------------------------------
      // progress tracking
      //------------------------------------------------------
      m_progress += (double)m_numx * progressInc ;
      InvokeEvent(ProgressEventId, &m_progress) ;

    } // for y
  } // for z



  // attach points and tensors to output polydata
  m_output->SetPoints(points) ;
  points->Delete() ;

  m_output->GetPointData()->SetScalars(scalars) ;
  scalars->Delete() ;

  if (m_outputFormat == VectorFormat){
    m_output->GetPointData()->SetVectors(vectors) ;
    vectors->Delete() ;
  }
  else{
    m_output->GetPointData()->SetTensors(tensors) ;
    tensors->Delete() ;
  }

  delete texCalc ;
}





//----------------------------------------------------------------------------
// PrintSelf method
void lhpTextureOrientationFilter::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  // target component
  os << "Target component = " << this->GetTargetComponent() << std::endl ;

  // size and step or texture window
  if (m_texWinDimsSet)
    os << "tex win dims: " << m_texWinDimensions[0] << " " << m_texWinDimensions[1] << " " << m_texWinDimensions[2] << std::endl ;
  else
    os << "tex win dims: not set" << std::endl ;

  if (m_texWinStepSet)
    os << "tex win step: " << m_texWinStep[0] << " " << m_texWinStep[1] << " " << m_texWinStep[2] << std::endl ;
  else
    os << "tex win step: not set" << std::endl ;

  if (m_texWinSizeSet)
    os << "tex win size: " << m_texWinSize << std::endl ;
  else
    os << "tex win size: not set " << std::endl ;

  if (m_texWinStepSizeSet)
    os << "tex win step size: " << m_texWinStepSize << std::endl ;
  else
    os << "tex win step size: not set " << std::endl ;

  if ((m_texWinDimsSet) && (m_texWinStepSet))
    os << "no. of tex wins: " << m_numx << " " << m_numy << " " << m_numz << std::endl ;

  os << "output format: " << m_outputFormat << std::endl ;
  os << std::endl ;

}




//----------------------------------------------------------------------------
// Print results
void lhpTextureOrientationFilter::PrintResults(ostream& os)
//----------------------------------------------------------------------------
{
  //m_output->Update() ;

  PrintSelf(os, vtkIndent()) ;

  vtkPoints *points = m_output->GetPoints() ;
  vtkPointData *PD = m_output->GetPointData() ;

  switch(m_outputFormat){
    case VectorFormat:
      {
        vtkDataArray *DA = PD->GetArray("vectors") ;

        int ituple = 0 ;
        for (int iz = 0 ;  iz < this->m_numz ;  iz++){
          for (int iy = 0 ;  iy < this->m_numy ;  iy++){
            for (int ix = 0 ;  ix < this->m_numx ;  ix++, ituple++){
              double *x = points->GetPoint(ituple) ;
              double *vec = DA->GetTuple3(ituple) ;
              os << "x, " << x[0] << ", " << x[1] << ", " << x[2] << ",\t" 
                << "vector, " << vec[0] << ", " << vec[1] << ", " << vec[2] << std::endl ;
            }
            os << std::endl ;
          }
          os << std::endl ;
        }
      }
      break ;
    case TensorFormat:
      break ;
  }
}