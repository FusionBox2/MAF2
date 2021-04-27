/*=========================================================================

 Program: MAF2
 Module: mafVMEScalarMatrix
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


#include "mafVMEScalarMatrix.h"
#include "mafGUI.h"

#include "mafStorageElement.h"
#include "mafVME.h"
#include "mafVMEOutputScalarMatrix.h"

#include "mafDataPipeInterpolatorScalarMatrix.h"
#include "mafDataVector.h"
#include "mafVMEItemScalarMatrix.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEScalarMatrix)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEScalarMatrix::mafVMEScalarMatrix()
//-------------------------------------------------------------------------
{
  m_DataVector = mafDataVector::New();
  m_DataVector->SetItemTypeName(mafVMEItemScalarMatrix::GetStaticTypeName());  
  m_DataVector->SetListener(this);
  SetDataPipe(mafDataPipeInterpolatorScalarMatrix::New()); // interpolator data pipe

  m_ScalarArrayOrientationInMatrix = ROWS;
  m_Xtype = USE_TIME;
  m_Ytype = USE_PROGRESS_NUMBER;
  m_Ztype = USE_SCALAR;
  m_XID = 0;
  m_YID = 0;
  m_ZID = 0;
  m_ActiveScalar = -1;
}

//-------------------------------------------------------------------------
mafVMEScalarMatrix::~mafVMEScalarMatrix()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(a);
    this->SetTypeForXCoordinates(scalar->GetTypeForXCoordinates());
    this->SetTypeForYCoordinates(scalar->GetTypeForYCoordinates());
    this->SetTypeForZCoordinates(scalar->GetTypeForZCoordinates());
    this->SetScalarArrayOrientation(scalar->GetScalarArrayOrientation());
    this->SetScalarIdForXCoordinate(scalar->GetScalarIdForXCoordinate());
    this->SetScalarIdForYCoordinate(scalar->GetScalarIdForYCoordinate());
    this->SetScalarIdForZCoordinate(scalar->GetScalarIdForZCoordinate());
    this->SetActiveScalarOnGeometry(scalar->GetActiveScalarOnGeometry());
    return MAF_OK;
  }
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool mafVMEScalarMatrix::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    mafVMEScalarMatrix *scalar = mafVMEScalarMatrix::SafeDownCast(vme);
    ret = this->m_Xtype == scalar->GetTypeForXCoordinates() && \
          this->m_Ytype == scalar->GetTypeForYCoordinates() && \
          this->m_Ztype == scalar->GetTypeForZCoordinates() && \
          this->m_XID == scalar->GetScalarIdForXCoordinate() && \
          this->m_YID == scalar->GetScalarIdForYCoordinate() && \
          this->m_ZID == scalar->GetScalarIdForZCoordinate() && \
          this->m_ActiveScalar == scalar->GetActiveScalarOnGeometry() && \
          this->m_ScalarArrayOrientationInMatrix == scalar->GetScalarArrayOrientation();
  }
  return ret;
}
//-------------------------------------------------------------------------
mafVMEOutput *mafVMEScalarMatrix::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputScalarMatrix::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEScalarMatrix::SetData(vnl_matrix<double> &data, mafTimeStamp t)
//-------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEItemScalarMatrix> item;
  item->SetData(data);
  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  return MAF_OK;
}
//-------------------------------------------------------------------------
mafGUI* mafVMEScalarMatrix::CreateGui()
//-------------------------------------------------------------------------
{
  mafString type_array[3] = {_L("time"), _L("progress number"), _L("scalar ID")};
  
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->SetListener(this);
  m_Gui->Divider();
  m_Gui->Label(_L("Parameters used for"));
  m_Gui->Label(_L("VTK representation"));
  m_Gui->Divider(2);
  m_Gui->Label(_L("x coord"));
  m_Gui->Combo(ID_TYPE_FOR_X,_L("type"),&m_Xtype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_X,_L("scalar id"),&m_XID,0,MAXINT,_L("Scalar ID associated to the X coordinate \nof the VTK representation"));
  m_Gui->Label(_L("y coord"));
  m_Gui->Combo(ID_TYPE_FOR_Y,_L("type"),&m_Ytype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_Y,_L("scalar id"),&m_YID,0,MAXINT,_L("Scalar ID associated to the Y coordinate \nof the VTK representation"));
  m_Gui->Label(_L("z coord"));
  m_Gui->Combo(ID_TYPE_FOR_Z,_L("type"),&m_Ztype,3,type_array);
  m_Gui->Integer(ID_SCALAR_FOR_Z,_L("scalar id"),&m_ZID,0,MAXINT,_L("Scalar ID associated to the Z coordinate \nof the VTK representation"));
  m_Gui->Divider();
  m_Gui->Label(_L("scalar for geometry."));
  m_Gui->Integer(ID_ACTIVE_SCALAR,_L("scalat id"),&m_ActiveScalar,-1,MAXINT,_L("Active scalar to use with geometry;\n -1 means no scalar active."));
  m_Gui->Divider();

  EnableWidgets();
  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::EnableWidgets()
//-------------------------------------------------------------------------
{
  m_Gui->Enable(ID_SCALAR_FOR_X, m_Xtype == USE_SCALAR);
  m_Gui->Enable(ID_SCALAR_FOR_Y, m_Ytype == USE_SCALAR);
  m_Gui->Enable(ID_SCALAR_FOR_Z, m_Ztype == USE_SCALAR);
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_SCALAR_FOR_X:
      case ID_SCALAR_FOR_Y:
      case ID_SCALAR_FOR_Z:
      case ID_ACTIVE_SCALAR:
      break;
      case ID_TYPE_FOR_X:
      case ID_TYPE_FOR_Y:
      case ID_TYPE_FOR_Z:
        EnableWidgets();
      break;
      default:
        mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-----------------------------------------------------------------------
int mafVMEScalarMatrix::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    if (parent->StoreInteger(_R("XID"),m_XID)==MAF_OK &&
        parent->StoreInteger(_R("YID"),m_YID)==MAF_OK &&
        parent->StoreInteger(_R("ZID"),m_ZID)==MAF_OK &&
        parent->StoreInteger(_R("Xtype"),m_Xtype)==MAF_OK &&
        parent->StoreInteger(_R("Ytype"),m_Ytype)==MAF_OK &&
        parent->StoreInteger(_R("Ztype"),m_Ztype)==MAF_OK &&
        parent->StoreInteger(_R("ActiveScalar"),m_ActiveScalar)==MAF_OK &&
        parent->StoreInteger(_R("ScalarArrayOrientationInMatrix"),m_ScalarArrayOrientationInMatrix)==MAF_OK)
      return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int mafVMEScalarMatrix::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node)==MAF_OK)
  {
    if (node->RestoreInteger(_R("XID"),m_XID) == MAF_OK &&
        node->RestoreInteger(_R("YID"),m_YID) == MAF_OK &&
        node->RestoreInteger(_R("ZID"),m_ZID) == MAF_OK &&
        node->RestoreInteger(_R("Xtype"),m_Xtype) == MAF_OK &&
        node->RestoreInteger(_R("Ytype"),m_Ytype) == MAF_OK &&
        node->RestoreInteger(_R("Ztype"),m_Ztype) == MAF_OK &&
        node->RestoreInteger(_R("ActiveScalar"),m_ActiveScalar)==MAF_OK &&
        node->RestoreInteger(_R("ScalarArrayOrientationInMatrix"),m_ScalarArrayOrientationInMatrix) == MAF_OK)
    {
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarArrayOrientation(int o)
//-------------------------------------------------------------------------
{
  m_ScalarArrayOrientationInMatrix = o;
  m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix < ROWS ? ROWS : m_ScalarArrayOrientationInMatrix;
  m_ScalarArrayOrientationInMatrix = m_ScalarArrayOrientationInMatrix > COLUMNS ? COLUMNS : m_ScalarArrayOrientationInMatrix;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetActiveScalarOnGeometry(int scalar)
//-------------------------------------------------------------------------
{
  if (scalar == m_ActiveScalar)
  {
    return;
  }
  m_ActiveScalar = scalar < -1 ? -1 : scalar;
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetActiveScalarOnGeometry()
//-------------------------------------------------------------------------
{
  return m_ActiveScalar;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForXCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_XID == id)
  {
    return;
  }
  m_XID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForXCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalarMatrix *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_XID = m_XID > data.columns() ? data.columns() : m_XID;
    }
    else
    {
      m_XID = m_XID > data.rows() ? data.rows() : m_XID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_XID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForXCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Xtype == t)
  {
    return;
  }
  m_Xtype = t;
  m_Xtype = m_Xtype < USE_TIME ? USE_TIME : m_Xtype;
  m_Xtype = m_Xtype > USE_SCALAR ? USE_SCALAR : m_Xtype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForYCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_YID == id)
  {
    return;
  }
  m_YID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForYCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalarMatrix *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_YID = m_YID > data.columns() ? data.columns() : m_YID;
    }
    else
    {
      m_YID = m_YID > data.rows() ? data.rows() : m_YID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_YID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForYCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Ytype == t)
  {
    return;
  }
  m_Ytype = t;
  m_Ytype = m_Ytype < USE_TIME ? USE_TIME : m_Ytype;
  m_Ytype = m_Ytype > USE_SCALAR ? USE_SCALAR : m_Ytype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetScalarIdForZCoordinate(int id)
//-------------------------------------------------------------------------
{
  if (m_ZID == id)
  {
    return;
  }
  m_ZID = id;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-------------------------------------------------------------------------
int mafVMEScalarMatrix::GetScalarIdForZCoordinate()
//-------------------------------------------------------------------------
{
  mafVMEOutputScalarMatrix *output = GetScalarOutput();
  vnl_matrix<double> data = output->GetScalarData();
  if (!data.empty())
  {
    if (m_ScalarArrayOrientationInMatrix == COLUMNS)
    {
      m_ZID = m_ZID > data.columns() ? data.columns() : m_ZID;
    }
    else
    {
      m_ZID = m_ZID > data.rows() ? data.rows() : m_ZID;
    }
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
  return m_ZID;
}
//-------------------------------------------------------------------------
void mafVMEScalarMatrix::SetTypeForZCoordinates(int t)
//-------------------------------------------------------------------------
{
  if (m_Ztype == t)
  {
    return;
  }
  m_Ztype = t;
  m_Ztype = m_Ztype < USE_TIME ? USE_TIME : m_Ztype;
  m_Ztype = m_Ztype > USE_SCALAR ? USE_SCALAR : m_Ztype;
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
  Modified();
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::InternalPreUpdate()
//-----------------------------------------------------------------------
{
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::SetTimeStamp(mafTimeStamp t)
//-----------------------------------------------------------------------
{
  t = t < 0 ? 0 : t;
  bool update_vtk_data = t != m_CurrentTime;
  if (update_vtk_data)
  {
    Superclass::SetTimeStamp(t);

#ifdef MAF_USE_VTK
    GetScalarOutput()->UpdateVTKRepresentation();
#endif
  }
}
//-----------------------------------------------------------------------
void mafVMEScalarMatrix::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << _L("Scalar Array orientation in matrix: ");
  os << (m_ScalarArrayOrientationInMatrix == 0) ? _L("Rows") : _L("Columns");
  os << _L("\n");

  mafString type_scalar[3] = {_L("Time"), _L("Progress numbers"), _L("Scalars")};

  os << indent << _L("Scalar type for X coordinate: ");
  os << type_scalar[m_Xtype].GetCStr();
  os << _L("\n");

  if (m_Xtype == USE_SCALAR)
  {
    os << indent << _L("Scalar ID for X coordinate: ");
    os << m_XID;
    os << _L("\n");
  }

  os << indent << _L("Scalar type for Y coordinate: ");
  os << type_scalar[m_Ytype].GetCStr();
  os << _L("\n");

  if (m_Ytype == USE_SCALAR)
  {
    os << indent << _L("Scalar ID for Y coordinate: ");
    os << m_YID;
    os << _L("\n");
  }

  os << indent << _L("Scalar type for Z coordinate: ");
  os << type_scalar[m_Ztype].GetCStr();
  os << _L("\n");

  if (m_Ztype == USE_SCALAR)
  {
    os << indent << _L("Scalar ID for Z coordinate: ");
    os << m_ZID;
    os << _L("\n");
  }
}
//-------------------------------------------------------------------------
char** mafVMEScalarMatrix::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEScalar.xpm"
  return mafVMEScalar_xpm;
}
