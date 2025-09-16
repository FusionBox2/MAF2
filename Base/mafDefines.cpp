/*=========================================================================

 Program: MAF2
 Module: mafDefines
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafDefines.h"
#include <mutex>

#include "wx/string.h"

#include <stdio.h>
#include <math.h>

#ifndef WIN32
#include <unistd.h>  
#endif

#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
bool mafEquals(double x, double y)
//------------------------------------------------------------------------------
{
   double diff=fabs(x - y);
   double max_err=fabs(x / pow((double)10,(double)15));
   if (diff > max_err)
     return false;
   return  ( diff <= max_err);
}

//------------------------------------------------------------------------------
bool mafFloatEquals(float x, float y)
//------------------------------------------------------------------------------
{
   float diff=fabs(x - y);
   float max_err=fabs(x / pow((double)10,(double)7));
   return  ( diff <= max_err);
}
//------------------------------------------------------------------------------
double mafRoundToPrecision(double val, unsigned prec)
//------------------------------------------------------------------------------
{
  double k = pow((double)10,(double)prec);
  return floor(val*k+0.5)/k; 
} 
//------------------------------------------------------------------------------
void mafSleep(int msec)
//------------------------------------------------------------------------------
{
#ifdef WIN32
  Sleep(msec);
#else

  usleep(msec*1000);
#endif
}
//------------------------------------------------------------------------------
bool mafIsLittleEndian(void)
//------------------------------------------------------------------------------
{ 
  /* 
   * Variables definition 
   */ 
  short magic, test; 
  char * ptr;    
  magic = 0xABCD;                     /* endianity test number */ 
  ptr = (char *) &magic;               
  test = (ptr[1]<<8) + (ptr[0]&0xFF); /* build value byte by byte */ 
  return (magic == test);             /* if the same is little endian */  
}
