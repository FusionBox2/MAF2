//
//  PointCloud.hpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 27/10/2018.
//

#ifndef __PointCloud_hpp
#define __PointCloud_hpp

#include "mafDataElement.hpp"
#include "vtkMAFSmartPointer.h"


class mafdmPointCloud : public mafDataElement
{
public:
    mafdmPointCloud();
    mafdmPointCloud(vtkMAFSmartPointer<vtkPoints> points);
    
    vtkMAFSmartPointer<vtkActor> makeRenderer();
    void describe();
        wxXmlNode* getNode();
		void SetMatrix(const mafMatrix &mat){};
		void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};
private:
    vtkMAFSmartPointer <vtkPoints> points;
    void makePointSet();
};

#endif /* PointCloud_hpp */
