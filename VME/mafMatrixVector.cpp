/*=========================================================================

 Program: MAF2
 Module: mafMatrixVector
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafMatrixVector.h"
#include "mafIndent.h"

//-----------------------------------------------------------------------
mafCxxTypeMacro(mafMatrixVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
mafMatrixVector::mafMatrixVector()
//-----------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------
mafMatrixVector::~mafMatrixVector()
//-----------------------------------------------------------------------
{
}
//-----------------------------------------------------------------------
void mafMatrixVector::GetKeyMatrixVector(std::vector<mafMatrix *> &mvector)
//-----------------------------------------------------------------------
{
  mvector.clear();
  mvector.reserve(GetNumberOfMatrices());
  for (auto& elem : *this)
  {
    mvector.push_back(elem.second.get());
  }
}
//-----------------------------------------------------------------------
void mafMatrixVector::SetMatrix(const mafMatrix &mat)
//-----------------------------------------------------------------------
{
  mafAutoPointer<mafMatrix> tmp = mafMatrix::New();
  *tmp=mat;
  SetMatrix(tmp.get());
}
//-----------------------------------------------------------------------
void mafMatrixVector::AppendKeyMatrix(const mafMatrix &m)
//-----------------------------------------------------------------------
{
  mafAutoPointer<mafMatrix> tmp = mafMatrix::New();
  *tmp=m;
  AppendKeyMatrix(tmp.get());
}

//-----------------------------------------------------------------------
void mafMatrixVector::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{
  parent(_R("NumberOfItems")).SetValue(mafToString(GetNumberOfItems()));
  for (auto& elem : *this)
  {
    {auto vv = parent[_R("Matrix")]; serializer::Serialize(vv, *(elem.second));}
  }
}
//-----------------------------------------------------------------------
void mafMatrixVector::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  mafID num_items = node(_R("NumberOfItems")).As<mafID>();
  auto vector_elements = node[_R("Matrix")];

  assert(vector_elements.GetNumItems() == num_items);

  if (vector_elements.GetNumItems() != num_items)
    mafWarningMacro("Restore I/O error: found wrong number of matrices in restored MatrixVector.");

  for (int i = 0; i < vector_elements.GetNumItems(); i++)
  {
    mafAutoPointer<mafMatrix> mat = mafMatrix::New();
    *mat = vector_elements[i].As<mafMatrix>();
    //if (vector_elements[i].ReSetValue(*mat)!=MAF_OK)
    //  return MAF_ERROR;
    AppendItem(mat.get());
  }
}
