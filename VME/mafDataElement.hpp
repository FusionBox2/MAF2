//
//  DataElement.hpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 20/10/2018.
//

#ifndef __mafDataElement_hpp
#define __mafDataElement_hpp

#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkSmartPointer.h>

#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkSphereSource.h>
#include "mafVME.h"
//#include <vtkNamedColors.h>
#include <vtkPointSource.h>

//#include <vtkParametricEllipsoid.h>
//#include <vtkParametricFunctionSource.h>


//#include <vtkVersionMacros.h> // For version macros
//#include <vtkVertexGlyphFilter.h>
#include <vtkTransform.h>
//#include <vtkAxesActor.h>

//#include "../VTKContentActor.hpp"


#include "../Eigen/Eigen/Dense"
#include <iostream>

using Eigen::Vector3d;
using namespace std;

//class DataElementInfoPanel;
//class MyFrame;
class wxXmlNode;

class mafDataElement 
{
public:
	
    //DataElementInfoPanel* infoPanel;
    
   // virtual vtkSmartPointer<vtkContentActor> makeRenderer() = 0;
    virtual void describe() = 0;
    
    string name;
    
    virtual Vector3d getCenter(){return center;}
    virtual void setCenter(Vector3d c){center = c;}
    
    virtual wxXmlNode* getNode() = 0;
	//static bool VMEAccept(mafNode *node) { return(node != NULL && node->IsMAFType(mafVME)); };
	virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) = 0;
	virtual void SetMatrix(const mafMatrix &mat) = 0;


protected:
    Vector3d center;

	
};

#endif /* DataElement_hpp */
