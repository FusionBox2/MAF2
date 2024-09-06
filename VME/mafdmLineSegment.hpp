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
#include "vtkSmartPointer.h"
class mafdmLineSegment : public mafDataElement
{
private:
	mafdmLandmark* p1;
	mafdmLandmark* p2;
public:
	mafdmLineSegment(mafdmLandmark* m1, mafdmLandmark* m2);
	vtkSmartPointer<vtkActor> makeRenderer();
	wxXmlNode* getNode() override;
	void describe() override;
	void SetMatrix(const mafMatrix &mat) override {};
	void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes) override {};
};
#endif /* LineSegment_hpp */
