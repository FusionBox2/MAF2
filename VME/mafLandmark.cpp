//
//  Marker.cpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 25/10/2018.
//
#include "mafDefines.h" 
#include "mafLandmark.hpp"
//#include "../DataElementInfoPanel.hpp"
#include <wx/xml/xml.h>

//class MyFrame;

int mafdmLandmark::landmarkID = 0;

mafdmLandmark::mafdmLandmark(double x, double y, double z,string name)
{
    this->center = Vector3d(x,y,z);
    this->name = name;
}
mafdmLandmark::mafdmLandmark(double x, double y, double z)
{
    this->name = "landmark" + std::to_string(landmarkID++);
    this->center = Vector3d(x,y,z);
}

void mafdmLandmark::describe()
{
    cout << "Marker1" << endl;
}



/*vtkSmartPointer<vtkContentActor> dmLandmark::makeRenderer()
{
    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    Vector3d center = this->getCenter();
    sphere->SetCenter(center(0),center(1),center(2));
    sphere->SetRadius(4);
    sphere->SetPhiResolution(32);
    sphere->SetThetaResolution(32);
    vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    sphereMapper->SetInputConnection(sphere->GetOutputPort());
    vtkSmartPointer<vtkContentActor> sphereActor = vtkSmartPointer<vtkContentActor>::New();
    sphereActor->setDataElement(this);
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetColor(0, 0, 1);
    
     return sphereActor;
}
*/

wxXmlNode* mafdmLandmark::getNode()
{
	wxXmlNode* node;
		/*= new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "dataElement");
    
    node->AddAttribute(_T("name"), this->name);
    node->AddAttribute(_T("type"), "dmLandmark");
    Vector3d c = getCenter();
    
    node->AddAttribute(_T("X"), std::to_string(c(0)));
    node->AddAttribute(_T("Y"), std::to_string(c(1)));
    node->AddAttribute(_T("Z"), std::to_string(c(2)));*/
    return node;
}
