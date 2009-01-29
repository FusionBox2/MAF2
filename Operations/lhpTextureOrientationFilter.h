#ifndef __lhpTextureOrientationFilter_H__
#define __lhpTextureOrientationFilter_H__

#include "vtkStructuredPointsToPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkCommand.h"

#include <ostream>


namespace lhpTextureOrientation
{
  // output format of filter
  enum OutputFormat{
    VectorFormat = 0,
    TensorFormat
  };

  // this defines the id of the vtk user callback 
  enum UserEvent{
    ProgressEventId = vtkCommand::UserEvent+0
  };
}


//------------------------------------------------------------------------------
// Texture orientation filter
// This returns a grid of polydata points across the image.
//
// The attributes of the polydata are tensors containing the texture direction.
// The user can choose the output attributes to be tensor or vector only.
// 
// The scalar attribute gives a value from 0-1 (red - blue on glyphs)
// which indicates how directional the tensor is, ie the ratio of the 
// principal eigenvalue to the total.
// 
//------------------------------------------------------------------------------

class lhpTextureOrientationFilter : public vtkStructuredPointsToPolyDataFilter
{
public:
  static lhpTextureOrientationFilter *New();
  vtkTypeRevisionMacro(lhpTextureOrientationFilter, vtkStructuredPointsToPolyDataFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  // print results
  void PrintResults(ostream& os);

  // set/get which scalar component to use (0, 1 or 2)
  void SetTargetComponent(int) ;
  int GetTargetComponent() const {return m_target_component ;}

  // set output to vector format (this is the default)
  // output attributes are normals containing principal direction and scalars containing magnitude
  void SetOutputToVectorFormat() {m_outputFormat = lhpTextureOrientation::VectorFormat ;  this->Modified() ;}

  // set output to tensor format
  // output attributes are tensors containing all direction information
  void SetOutputToTensorFormat() {m_outputFormat = lhpTextureOrientation::TensorFormat ;  this->Modified() ;}

  // set output format to vector or tensor
  void SetOutputFormat(lhpTextureOrientation::OutputFormat outputFormat) {m_outputFormat = outputFormat ;  this->Modified() ;}

  // Set the dimensions of the texture window
  void SetTexWinDimensions(int dimsx, int dimsy, int dimsz) ;

  // Set the dimensions of the texture window
  void SetTexWinDimensions(const int dims[3]) ;

  // Set the size of the texture window in world coords
  void SetTexWinSize(double size) ;

  // Set the step increment of the texture window
  void SetTexWinStep(int stepx, int stepy, int stepz) ;

  // Set the step increment of the texture window
  void SetTexWinStep(const int step[3]) ;

  // Set the step increment of the texture window in world coords
  void SetTexWinStepSize(double step) ;

  // Execute Method
  void Execute() ;

protected:
  int m_target_component ;	          // input image component to use

  // constructor and destructor
  lhpTextureOrientationFilter();
  ~lhpTextureOrientationFilter();

  // refresh the filter
  void Initialize() ;

  // run the filter
  void FilterExecute() ;


  // format of output
  lhpTextureOrientation::OutputFormat m_outputFormat ;

  // texture window dimensions and step
  bool m_texWinDimsSet ;
  bool m_texWinSizeSet ;
  bool m_texWinStepSet ;
  bool m_texWinStepSizeSet ;
  int m_texWinDimensions[3] ;
  double m_texWinSize ;
  int m_texWinStep[3] ;
  double m_texWinStepSize ;
  int m_numx, m_numy, m_numz ;

  // progress tracker
  double m_progress ;

  vtkImageData *m_input ;
  vtkPolyData *m_output ;

} ;

#endif