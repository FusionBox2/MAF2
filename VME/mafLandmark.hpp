//
//  Marker.hpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 25/10/2018.
//

#ifndef Marker_hpp
#define Marker_hpp

#include "mafDataElement.hpp"

class mafDataElement;

class mafdmLandmark: public mafDataElement
{
    
public:

    
    mafdmLandmark(double x,double y, double z, string name);
    mafdmLandmark(double x,double y, double z);
    mafdmLandmark() : mafdmLandmark(0,0,0){};
    void describe();

    //vtkSmartPointer<vtkContentActor> makeRenderer();
    wxXmlNode* getNode();
	void SetMatrix(const mafMatrix &mat){};
	void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};
    static int landmarkID;
};





#endif /* Marker_hpp */
