/*=========================================================================

 Program: MAF2
 Module: mafIndent
 Authors: originally based on vtkIndent (www.vtk.org), rewritten by Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafIndent_h
#define __mafIndent_h

#include "mafConfigure.h"
#include "mafBase.h" 
#include <ostream>


/** mafIndent - a simple class to control print indentation.
  mafIndent is used to control indentation during the chaining print 
  process. This way nested objects can correctly indent themselves.*/
class MAF_EXPORT mafIndent : public mafBase
{
public:
  mafIndent(int ind=0);
  /**
    Determine the next indentation level. Keep indenting by two until the 
    max of forty. */
  mafIndent GetNextIndent();
  int       GetIndent() const {return m_Indent;}
  operator int() const {return m_Indent;}
protected:
  int m_Indent;  
};

MAF_EXPORT std::ostream& operator<<(std::ostream& os, const mafIndent& o);

#endif

