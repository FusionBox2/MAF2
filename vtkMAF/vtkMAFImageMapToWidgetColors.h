/*=========================================================================

 Program: MAF2
 Module: vtkMAFImageMapToWidgetColors
 Authors: Alexander Savenko, Mel Krokos
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMAFImageMapToWidgetColors - a special filter for demonstrating which part of the image is covered by a 2D transfer function widget
// .SECTION Description

// .SECTION See Also
// vtkVolumeProperty2 vtkMAFTransferFunction2D

#ifndef __vtkMAFImageMapToWidgetColors_h
#define __vtkMAFImageMapToWidgetColors_h

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------
#include "vtkMAFConfigure.h"

#include "vtkImageMapToColors.h"
#include "vtkMAFTransferFunction2D.h"

//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
class vtkDataObject;
class vtkImageData;
class vtkCharArray;


class VTK_vtkMAF_EXPORT vtkMAFImageMapToWidgetColors : public vtkImageMapToColors 
{
public:
  static vtkMAFImageMapToWidgetColors *New();
  vtkTypeMacro(vtkMAFImageMapToWidgetColors,vtkImageMapToColors);

  /**
  Set / Get transfer function*/
  vtkSetObjectMacro(TransferFunction, vtkMAFTransferFunction2D);
  vtkGetObjectMacro(TransferFunction, vtkMAFTransferFunction2D);

  /**
  Set / Get widget index*/
  vtkSetMacro(WidgetIndex, int);
  vtkGetMacro(WidgetIndex, int);

  /**
  Set / Get the Window to use -> modulation will be performed on the 
  color based on (S - (L - W/2))/W where S is the scalar value, L is
  the level and W is the window.*/
  vtkSetMacro( Window, double );
  vtkGetMacro( Window, double );
  
  /**
  Set / Get the Level to use -> modulation will be performed on the 
  color based on (S - (L - W/2))/W where S is the scalar value, L is
  the level and W is the window.*/
  vtkSetMacro( Level, double );
  vtkGetMacro( Level, double );
  
  vtkMTimeType GetMTime();

protected:
  vtkMAFImageMapToWidgetColors();
  ~vtkMAFImageMapToWidgetColors() override;

  int RequestInformation(vtkInformation *,
                                 vtkInformationVector **,
                                 vtkInformationVector *) override;

  void ThreadedRequestData(vtkInformation *request,
                           vtkInformationVector **inputVector,
                           vtkInformationVector *outputVector,
                           vtkImageData ***inData, vtkImageData **outData,
                           int extent[6], int id) override;

  int RequestData(vtkInformation *request,
                          vtkInformationVector **inputVector,
                          vtkInformationVector *outputVector) override;
  template<class T> void UpdateGradientCache(vtkImageData* imageData, T *dataPointer);

  vtkMAFTransferFunction2D *TransferFunction;

  int   WidgetIndex;

  double Window;
  double Level;

  double       *GradientCache;
  int          GradientCacheSize;
  vtkTimeStamp GradientCacheMTime;
  int          GradientExtent[6];

  void Execute(vtkImageData* inData, vtkDataArray* inArray, vtkCharArray* maskArray,
      vtkImageData* outData, vtkDataArray* outArray, int outExt[6]);

private:
  vtkMAFImageMapToWidgetColors(const vtkMAFImageMapToWidgetColors&) = delete;
  void operator=(const vtkMAFImageMapToWidgetColors&) = delete;
};
#endif
