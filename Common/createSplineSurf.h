/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: createSplineSurf.h,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:00:28 $
  Version:   $Revision: 1.2 $
  Authors:   Fedor Moiseev
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#ifndef CREATESPLINESURF_HEADER
#define CREATESPLINESURF_HEADER

#include "forarray.h"
#include "vectors.h"
#include "splines.h"
#include <limits>


//building of splined surface
template <class Type>
void createSurface(const std::vector<V3d<Type> > &gridValue, unsigned numlines, unsigned numpnts, unsigned splinetypex, unsigned splinetypey, unsigned pointsOut, unsigned linesOut, Type rhoParam, Type sgmParam, vtkPoints *newPts, vtkCellArray *newPolys)
{
  {
    //set of working arrays for each 3D vector component
    ForArray<Type> x[3];
    ForArray<Type> y[3];
    ForArray<Type> z[3];
    ForArray<Type> zl[3];
    ForArray<Type> zr[3];
    ForArray<Type> zu[3];
    ForArray<Type> zd[3];
    ForArray<Type> zxy[3];
    ForArray<Type> rho[3];
    ForArray<Type> sgm[3];
    ForArray<Type> zx[3];
    ForArray<Type> zy[3];
    assert(gridValue.size() == numlines * numpnts);

    for(int i = 0; i < 3; i++)
    {
      x[i].assign(numpnts, Type(0));
      y[i].assign(numlines, Type(0));
      z[i].assign(numpnts * numlines, Type(0));
      zl[i].assign(numlines, Type(0));
      zr[i].assign(numlines, Type(0));
      zu[i].assign(numpnts, Type(0));
      zd[i].assign(numpnts, Type(0));
      zxy[i].assign(numpnts * numlines, Type(0));
      rho[i].assign(numpnts, Type(0));
      sgm[i].assign(numlines, Type(0));
      zx[i].assign(numpnts * numlines, Type(0));
      zy[i].assign(numpnts * numlines, Type(0));
    }

    //initialization with input params
    for(int j = 0; j < 3; j++)
    {
      for(unsigned i = 0; i < numpnts; i++)
      {
        x[j][i] = i;
        zu[j][i] = 0.;
        zd[j][i] = 0.;
        rho[j][i] = rhoParam;
      }
      for(unsigned i = 0; i < numlines; i++)
      {
        y[j][i] = i;
        zl[j][i] = 0.;
        zr[j][i] = 0.;
        sgm[j][i] = sgmParam;
      }
      for(unsigned i = 0; i < gridValue.size(); i++)
      {
        z[j][i] = gridValue[i][j];
      }
      for(int i = 0; i < 4; i++)
        zxy[j][i] = 0.;
    }

    //creating smoothing 2D splines for each 3D vector component
    for(int k = 0; k < 3; k++)
    {
      Type        argx = 0;
      Type        argy = 0;
      V3d<Type> resval;
      //Splint2(numpnts, x[k], (unsigned)numlines, y[k], z[k], zl[k], zr[k], zu[k], zd[k], zx[k], zy[k], zxy[k], 
      //        0, 3, 2, argx, argy, resval[k]);
      Smspl2(numpnts, x[k], (unsigned)numlines, y[k], z[k], zl[k], zr[k], zu[k], zd[k], rho[k], sgm[k], zx[k], zy[k], zxy[k], 
        0, splinetypex, splinetypey, argx, argy, resval[k]);
    }

    //preparing output array
    std::vector<std::vector<V3d<Type> > > output;
    output.resize(linesOut);
    for(unsigned i = 0; i < linesOut; i++)
      output[i].assign(pointsOut, V3d<Type>(Type(0), Type(0), Type(0)));


    //producing new tesselation
    Type stepX = 1.0 * (numpnts - 1) / (pointsOut - 1);
    Type stepY = 1.0 * (numlines - 1) / (linesOut - 1);
    for(unsigned j = 0; j < linesOut; j++)
    {
      for(unsigned i = 0; i < pointsOut; i++)
      {
        Type        argx = i * stepX;
        Type        argy   = j * stepY;
        V3d<Type>   resval;
        for(int k = 0; k < 3; k++)
        {
          /*Splint2(numpnts, x[k], (unsigned)numlines, y[k], z[k], zl[k], zr[k], zu[k], zd[k], zx[k], zy[k], zxy[k], 
          1, 3, 2, argx, argy, resval[k]);*/
          Smspl2(numpnts, x[k], (unsigned)numlines, y[k], z[k], zl[k], zr[k], zu[k], zd[k], rho[k], sgm[k], zx[k], zy[k], zxy[k], 1, splinetypex, splinetypey, argx, argy, resval[k]);
        }
        output[j][i] = resval;
      }
    }
    //looping spline if needed
    if(splinetypex == 3)
    {
      for(unsigned i = 0; i < linesOut; i++)
        output[i][output[i].size() - 1] = output[i][0];
    }
    //looping spline if needed
    if(splinetypey == 3)
    {
      for(unsigned i = 0; i < pointsOut; i++)
        output[linesOut - 1][i] = output[0][i];
    }
    //filling output data
    for(unsigned j = 0; j < linesOut; j++)
    {
      for(unsigned i = 0; i < pointsOut; i++)
      {
        newPts->InsertNextPoint(output[j][i].components);
      }
    }
  }

  //filling output triangles
  int triidx = 0;
  for(unsigned i = 0; i < linesOut - 1; i++)
  {
    for(unsigned j = 0; j < pointsOut - 1; j++)
    {
      vtkIdType                pts[3];
      pts[0] = (i) * (pointsOut) + (j);
      pts[1] = (i + 1) * (pointsOut) + (j);
      pts[2] = (i + 1) * (pointsOut) + (j + 1);
      newPolys->InsertNextCell(3, pts);
      pts[0] = (i) * (pointsOut) + (j);
      pts[1] = (i + 1) * (pointsOut) + (j + 1);
      pts[2] = (i) * (pointsOut) + (j + 1);
      newPolys->InsertNextCell(3, pts);
    }
  }
}

#endif