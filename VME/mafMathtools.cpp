//
//  Mathtools.cpp
//  CylinderRenderingProperties
//
//  Created by Bart Jansen on 30/10/2018.
//
#include "mafDefines.h" 
#include "mafMathtools.hpp"

/*
%DIST2LINE
% computes the closest point from q to the line segment p_pp and returns
% the closest point qq, the distance d, and the location along the line
% segment s (s \in [0,1]).
%
% Ian Stavness -- 14/Feb/2012

function [d, qq, s] = dist2line(p,pp,q)
vp = (pp-p);
l = vp'*vp;
if (l == 0)
qq = p;
s = 0;
d = norm(qq-q);
return;
end

s = (q-p)'*vp/l;

qq = (s<=0)*p + (s>0&&s<1)*(p + vp*s) + (s>=1)*pp;
d = norm(qq-q);
*/

void dist2line(Vector3d p, Vector3d pp, Vector3d q, double* d, Vector3d* qq,double* s)
{
    Vector3d vp = pp-p;
    double l = vp.norm();
    if(l == 0)
    {
        *qq = p;
        *s = 0;
        *d = (p - q).norm();
        //return;
    }
    else
    {
        *s = (q-p).dot(vp) / l;
        double sc = *s;
        *qq = (sc<=0)*p + (sc>0&&sc<1)*(p + vp*sc) + (sc>=1)*pp;
        *d = (*qq-q).norm();
    }
}

double dist2line(Vector3d p, Vector3d pp, Vector3d q)
{
    Vector3d vp = pp-p;
    double l = vp.norm();
    if(l == 0)
    {
        return (p - q).norm();
    }
    else
    {
        double sc = (q-p).dot(vp) / l;
        Vector3d qq = (sc<=0)*p + (sc>0&&sc<1)*(p + vp*sc) + (sc>=1)*pp;
        return (qq-q).norm();
    }
}

// convert a 3x3 matrix from Eigen into a 4x4 matrix in vtk
vtkSmartPointer<vtkMatrix4x4> convertEigenToVTK(Matrix3d inputMatrix)
{
	vtkSmartPointer<vtkMatrix4x4> matrix;
    matrix->Identity();
    for (int i = 0; i < 3; i++)
    {
        for(int j=0;j<3;j++)
        {
            matrix->SetElement(i, j, inputMatrix(i,j));
        }
    }
	
    return matrix;
}
