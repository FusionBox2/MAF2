/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpVMEKMInfo.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.13 $
  Authors:   Marco Petrone , Stefano Perticoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "lhpVMEKMInfo.h"
#include "mafGUI.h"

#include "mafTransform.h"
#include "mafMatrixVector.h"
#include "mafNode.h"
#include "mafVMEOutputNULL.h"

const int DEBUG_MODE = true;

//-------------------------------------------------------------------------
mafCxxTypeMacro(lhpVMEKMInfo)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
lhpVMEKMInfo::lhpVMEKMInfo()
//-------------------------------------------------------------------------
{
  for(int i = 0; i < 200; i++)
    m_values[i] = 0.;
  // The output is created on demand in GetOutput() to avoid
  // subclasses to have to destroy base class output
  mafNEW(m_Transform);
  m_MatrixVector->SetMatrix(m_Transform->GetMatrix());

  mafVMEOutputNULL *output=mafVMEOutputNULL::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);
}

//-------------------------------------------------------------------------
lhpVMEKMInfo::~lhpVMEKMInfo()
//-------------------------------------------------------------------------
{
  mafDEL(m_Transform);
  
  SetOutput(NULL);

  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGenericAbstract
}

//-------------------------------------------------------------------------
mafVMEOutput *lhpVMEKMInfo::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputNULL::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
mafGUI* lhpVMEKMInfo::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
	return m_Gui;
}

//-------------------------------------------------------------------------
void lhpVMEKMInfo::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  if (maf_event->GetChannel()==MCH_UP)
  {
    mafID eid = maf_event->GetId();
    if(eid == NODE_ATTACHED_TO_TREE)
    {
      mafNode *n = mafNode::SafeDownCast((mafObject *)maf_event->GetSender());
      if (n)
      {
        mafNode *parent = n->GetParent();
        if (parent == this)
        {
          //mafMessage("Ask for shared GUI!!");
        }
      }
    }
    else if(eid == NODE_DETACHED_FROM_TREE)
    {
      mafNode *n = mafNode::SafeDownCast((mafObject *)maf_event->GetSender());
      if (n)
      {
        mafNode *parent = n->GetParent();
        if (parent == this)
        {
          //mafMessage("Remove shared GUI!!");
        }
      }
    }
  }
  Superclass::OnEvent(maf_event);
}

//-------------------------------------------------------------------------
char** lhpVMEKMInfo::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEGroup.xpm"
  return mafVMEGroup_xpm;
}

//-------------------------------------------------------------------------
void lhpVMEKMInfo::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  Superclass::SetMatrix(mat);
  m_Transform->SetMatrix(mat);
}

//-----------------------------------------------------------------------
int lhpVMEKMInfo::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{ 
  if (DEBUG_MODE)
  {
    std::ostringstream stringStream;
    stringStream << "Storing matrix:"  << std::endl;
    m_Transform->Print(stringStream);
    mafLogMessage(_M(stringStream.str().c_str()));
  }

  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreMatrix(_R("Transform"),m_Transform->GetMatrix());
    return MAF_OK;
  }
  return MAF_ERROR;
  
  
}

//-----------------------------------------------------------------------
int lhpVMEKMInfo::InternalRestore(const mafStorageElement& node_)
//-----------------------------------------------------------------------
{
	auto node = &node_;

  if (Superclass::InternalRestore(node_)==MAF_OK)
  {
    mafMatrix matrix;
    if (node->RestoreMatrix(_R("Transform"),matrix)==MAF_OK)
    {

      if (DEBUG_MODE)
      {
        std::ostringstream stringStream;
        stringStream << "Restoring group matrix:"  << std::endl;
        matrix.Print(stringStream);
        mafLogMessage(_M(stringStream.str().c_str()));
      }

      this->SetMatrix(matrix);
      return MAF_OK;
    }
    else
    {
      // code handling for old msf without group pose matrix serialization
      this->SetMatrix(matrix);
      if (DEBUG_MODE)
        {
          std::ostringstream stringStream;
          stringStream << "BEWARE!!! Opening an old MSF without group matrix serialized:\
restoring group matrix as:"  << std::endl;
          matrix.Print(stringStream);
          stringStream << "Please report any problem with old MSF containing groups!!!"  << std::endl;
          mafLogMessage(_M(stringStream.str().c_str()));
      }
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}

//-----------------------------------------------------------------------
void lhpVMEKMInfo::Print(std::ostream& os, const int tabs)
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  mafMatrix m = m_Transform->GetMatrix();
  m.Print(os,indent.GetNextIndent());
}

//-------------------------------------------------------------------------
bool lhpVMEKMInfo::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    return ret = (m_Transform->GetMatrix()==((lhpVMEKMInfo *)vme)->m_Transform->GetMatrix());
  }
  return ret;
}

//-------------------------------------------------------------------------
int lhpVMEKMInfo::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    lhpVMEKMInfo *group = lhpVMEKMInfo::SafeDownCast(a);
    
    m_Transform->SetMatrix(group->m_Transform->GetMatrix());
    
    return MAF_OK;
  }  

  return MAF_ERROR;
}
