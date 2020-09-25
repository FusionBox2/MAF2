//
//  LineSegment.hpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 29/10/2018.
//

#ifndef LineSegment_hpp
#define LineSegment_hpp

#include "mafDataElement.hpp"
#include "mafLandmark.hpp"
#include "vtkMAFSmartPointer.h"
class mafdmLineSegment : public mafDataElement
{
private:
	mafdmLandmark* p1;
	mafdmLandmark* p2;
public:
	mafdmLineSegment(mafdmLandmark* m1, mafdmLandmark* m2);
	vtkMAFSmartPointer<vtkActor> makeRenderer();
	wxXmlNode* getNode();
	void describe();
	void SetMatrix(const mafMatrix &mat){};
	void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes){};
};
#endif /* LineSegment_hpp */
