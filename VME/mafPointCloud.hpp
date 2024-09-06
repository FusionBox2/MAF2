//
//  PointCloud.hpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 27/10/2018.
//

#ifndef __PointCloud_hpp
#define __PointCloud_hpp

#include "mafDataElement.hpp"
#include "vtkSmartPointer.h"


class mafdmPointCloud : public mafDataElement
{
public:
    mafdmPointCloud();
    mafdmPointCloud(vtkSmartPointer<vtkPoints> points);
    
    vtkNew<vtkActor> makeRenderer();
    void describe() override;
        wxXmlNode* getNode() override;
		void SetMatrix(const mafMatrix &mat) override {};
		void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override {};
private:
    vtkSmartPointer <vtkPoints> points;
    void makePointSet();
};

#endif /* PointCloud_hpp */
