//
//  Mathtools.hpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 30/10/2018.
//

#ifndef mafMathtools_hpp
#define mafMathtools_hpp

#include "vtkSmartPointer.h"
#include <vtkMatrix4x4.h>

#include "Eigen/Dense"
using Eigen::Vector3d;
using Eigen::Matrix3d;

void dist2line(Vector3d p, Vector3d pp, Vector3d q,double* d, Vector3d* qq,double* s);
double dist2line(Vector3d p, Vector3d pp, Vector3d q);

vtkSmartPointer<vtkMatrix4x4> convertEigenToVTK(Matrix3d inputMatrix);


#endif /* Mathtools_hpp */
