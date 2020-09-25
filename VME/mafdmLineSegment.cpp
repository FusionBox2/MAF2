//
//  LineSegment.cpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 29/10/2018.
//
#include <vtkLineSource.h>
#include "mafdmLineSegment.hpp"
#include "vtkMAFSmartPointer.h"
//#include "../DataElementInfoPanel.hpp"
#include <wx/xml/xml.h>

mafdmLineSegment::mafdmLineSegment(mafdmLandmark* m1, mafdmLandmark* m2)
{
	p1 = m1;
	p2 = m2;
	center = (p1->getCenter() + p2->getCenter()) / 2;
	this->name = "linesegment";
}

void mafdmLineSegment::describe()
{
	cout << "LineSegment" << endl;
}



vtkMAFSmartPointer<vtkActor> mafdmLineSegment::makeRenderer()
{
	vtkMAFSmartPointer<vtkLineSource> lineSource = vtkLineSource::New();

	Vector3d c1 = p1->getCenter();
	Vector3d c2 = p2->getCenter();

	lineSource->SetPoint1(c1(0), c1(1), c1(2));
	lineSource->SetPoint2(c2(0), c2(1), c2(2));
	lineSource->Update();

	// Visualize
	vtkMAFSmartPointer<vtkPolyDataMapper> mapper = vtkPolyDataMapper::New();

	mapper->SetInput(lineSource->GetOutput());
	vtkMAFSmartPointer<vtkActor> actor = vtkActor::New();
//	actor->setDataElement(this);
	actor->SetMapper(mapper);
	actor->GetProperty()->SetLineWidth(4);

	return actor;
}

wxXmlNode* mafdmLineSegment::getNode()
{
	wxXmlNode* node = new wxXmlNode();
	//wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "dataElement");
	/*node->AddAttribute(_T("name"), this->name);
	node->AddAttribute(_T("type"), "dmLineSegment");
	Vector3d c = getCenter();
	node->AddAttribute(_T("X"), std::to_string(c(0)));
	node->AddAttribute(_T("Y"), std::to_string(c(1)));
	node->AddAttribute(_T("Z"), std::to_string(c(2)));*/
	return node;
}
