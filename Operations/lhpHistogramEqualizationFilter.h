#ifndef __lhpHistogramEqualizationFilter_H__
#define __lhpHistogramEqualizationFilter_H__

#include <vtkImageAlgorithm.h>


//------------------------------------------------------------------------------
// Image to image filter whose output is histogram equalized.
// Output image is one component, unsigned char.
//------------------------------------------------------------------------------

class lhpHistogramEqualizationFilter : public vtkImageAlgorithm
{
public:
  static lhpHistogramEqualizationFilter *New();
  vtkTypeMacro(lhpHistogramEqualizationFilter,vtkImageAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  // set/get which scalar component to use (0, 1 or 2)
  void SetTargetComponent(int) ;
  int GetTargetComponent() const ;

  void SetNumberOfGreyLevels(int numberOfGreyLevels) ;
  int GetNumberOfGreyLevels() const {return m_numberOfGreyLevels ;}

protected:

  int m_target_component ;	// image component
  int m_dims[3] ;
  double m_spacing[3], m_origin[3] ;
  int m_numberOfGreyLevels ;


  lhpHistogramEqualizationFilter();
  ~lhpHistogramEqualizationFilter() override;

  // This gives the subclass a chance to configure the output
  // see VTK user's guide p 246
  void ExecuteInformation(vtkImageData *inData, vtkImageData *outData);
  void ExecuteInformation(){}//this->vtkImageToImageFilter::ExecuteInformation();};
  void ComputeInputUpdateExtent(int inExt[6], int outExt[6]);

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
    int extent[6], int id);


  // Declare this function as a friend so that self->func() can access private members
  // Otherwise all methods have to be public
  template <class IT, class OT>
  friend void ImageFilterExecute(
    lhpHistogramEqualizationFilter *self, 
    vtkImageData *inData,
    IT *inPtr,
    vtkImageData *outData, 
    OT *outPtr,
    int outExt[6], 
    int id
    ) ;
} ;



#endif