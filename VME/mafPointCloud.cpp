//
//  PointCloud.cpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 27/10/2018.
//

#include "mafPointCloud.hpp"
#include "vtkMAFSmartPointer.h"
//#include "../DataElementInfoPanel.hpp"
#include <wx/xml/xml.h>
#include <vtkGlyph3D.h>

#define const double M_PI = 3.1415926535;


mafdmPointCloud::mafdmPointCloud()
{
	makePointSet();
	this->name = "pointcloud";
}

// generate an incomplete ellipsoid with some noise added.
// rotates the ellispoid and shifts it away from 0
void mafdmPointCloud::makePointSet()
{
	vtkMAFSmartPointer<vtkPoints> points = vtkPoints::New();
	double pi = M_PI;

	// 3 main axis
	int a = 1;
	int b = 2;
	int c = 3;

	// rotate entire ellipsoid over ang
	double ang = 0.8 * M_PI / 3;
	ang = 0;
	double cosang = cos(ang);
	double sinang = sin(ang);

	double ang2 = M_PI / 6;
	ang2 = 0;
	double cosang2 = cos(ang2);
	double sinang2 = sin(ang2);
	// offset

	//   double shiftx = 4;
	//   double shifty = 5;
	//   double shiftz = 6;

	double shiftx = 0;
	double shifty = 0;
	double shiftz = 0;

	double noiseFactor = 0.1;

	for (double s = -pi / 2; s<pi / 2; s += 0.1)
	{
		for (double t = -3 * pi / 4; t<3 * pi / 4; t += 0.3)
		{
			// get ellipsoid point
			double x = a * cos(t) * cos(s);
			double y = b * sin(t) * cos(s);
			double z = c * sin(s);
			/*
			// rotate over ang
			double xt = x * cosang - y * sinang;
			double yt = x * sinang + y * cosang;
			double zt = z;
			double ytt = yt * cosang - zt * sinang;
			zt = yt * sinang + zt * cosang;
			yt = ytt;
			*/


			double xt = x * cosang - y * sinang;
			double yt = x * sinang + y * cosang;
			double zt = z;

			double xtt = xt * cosang2 + zt * sinang2;
			double ztt = xt * -sinang2 + zt * cosang2;

			xt = xtt;
			zt = ztt;


			//  double xt = x;
			// double yt = y;
			// double zt = z;

			// compute noise
			double noisex = noiseFactor * a * (rand() % 100) / 100.0;
			double noisey = noiseFactor * b * (rand() % 100) / 100.0;
			double noisez = noiseFactor * c * (rand() % 100) / 100.0;

			points->InsertNextPoint(noisex + shiftx + xt, noisey + shifty + yt, noisez + shiftz + zt);
		}
	}
}

mafdmPointCloud::mafdmPointCloud(vtkMAFSmartPointer<vtkPoints> points)
{
	this->points = points;
	this->name = "pointcloud";
}

void mafdmPointCloud::describe()
{
	cout << "dmPointCloud" << endl;
}


vtkMAFSmartPointer<vtkActor> mafdmPointCloud::makeRenderer()
{
	// Add the points to a polydata
	vtkMAFSmartPointer<vtkPolyData> polydata;
	polydata->SetPoints(points);


	vtkMAFSmartPointer<vtkGlyph3D> glyphFilter;
	glyphFilter->SetInput(polydata);
	glyphFilter->Update();

	// Visualize
	//vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

	vtkMAFSmartPointer<vtkPolyDataMapper> mapper ;
	mapper->SetInput(glyphFilter->GetOutput());
	vtkMAFSmartPointer<vtkActor> actor;
	//actor->setDataElement(this);
	
	int R = 100; int G = 53; int B = 150;

	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(R,G,B);
	actor->GetProperty()->SetPointSize(5);
	//actor->GetProperty()->EdgeVisibilityOn();
	//actor->GetProperty()->SetInterpolationToFlat();

	return actor;
}

wxXmlNode* mafdmPointCloud::getNode()
{
	wxXmlNode* node = new wxXmlNode();
	//wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, "dataElement");
	/*node->AddAttribute(_T("name"), this->name);
	node->AddAttribute(_T("type"), "dmPointCloud");
	Vector3d c = getCenter();
	node->AddAttribute(_T("X"), std::to_string(c(0)));
	node->AddAttribute(_T("Y"), std::to_string(c(1)));
	node->AddAttribute(_T("Z"), std::to_string(c(2)));*/
	return node;
}
