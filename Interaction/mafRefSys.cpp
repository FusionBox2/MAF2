/*=========================================================================

 Program: MAF2
 Module: mafRefSys
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"


#include "mafRefSys.h"

#include "mafVME.h"
#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafAbsMatrixPipe.h"
#include "mafIndent.h"

#include "vtkTransform.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafRefSys::mafRefSys()
//------------------------------------------------------------------------------
{
  Initialize();
}

//------------------------------------------------------------------------------
mafRefSys::mafRefSys(int type)
//------------------------------------------------------------------------------
{
  Initialize();
  SetType(type);
}
//------------------------------------------------------------------------------
mafRefSys::mafRefSys(mafVME *vme)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToLocal(vme);
}

//------------------------------------------------------------------------------
mafRefSys::mafRefSys(vtkMatrix4x4 *matrix)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToCustom(matrix);
}
//------------------------------------------------------------------------------
mafRefSys::mafRefSys(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  Initialize();
  SetTypeToView(ren);
}

//------------------------------------------------------------------------------
mafRefSys::~mafRefSys()
//------------------------------------------------------------------------------
{
  vtkDEL(m_Renderer);
}

//----------------------------------------------------------------------------
void mafRefSys::operator =(const mafRefSys &source)
//----------------------------------------------------------------------------
{
  m_Type = source.m_Type;
  m_Transform=source.m_Transform;
  m_Renderer=source.m_Renderer; 
  m_VME=source.m_VME;
}

//----------------------------------------------------------------------------
void mafRefSys::DeepCopy(const mafRefSys *source)
//----------------------------------------------------------------------------
{
  /*Initialize();
  m_Type = source->m_Type;
  m_Transform=source->m_Transform;
  m_Renderer=source->m_Renderer;
  m_VME=source->m_VME;
  */
} 

//----------------------------------------------------------------------------
void mafRefSys::Initialize()
//----------------------------------------------------------------------------
{
  m_Identity = mafTransform::NewSPtr();
  SetTypeToGlobal();
  m_Renderer  = NULL; 
  m_Transform = NULL;
  m_VME       = NULL;
}

//----------------------------------------------------------------------------
void mafRefSys::Reset()
//----------------------------------------------------------------------------
{
  m_Identity.reset();
  Initialize();
}

//----------------------------------------------------------------------------
void mafRefSys::SetTransform(std::shared_ptr<mafTransformBase> transform)
//----------------------------------------------------------------------------
{
  m_Transform = transform;
}

//----------------------------------------------------------------------------
std::shared_ptr<mafTransformBase> mafRefSys::GetTransform()
//----------------------------------------------------------------------------
{
  switch (m_Type)
  {
  case CUSTOM:
    return m_Transform ? m_Transform : m_Identity;
  case GLOBAL: 
    return m_Identity;
  case PARENT: 
    if (m_VME.get() && m_VME->GetParent())
    {
      return m_VME->GetParent()->GetAbsMatrixPipe();
    }
    return m_Identity;
  case LOCAL:
    if (m_VME.get())
      return m_VME->GetAbsMatrixPipe();
    
    return m_Identity;
  case VIEW:
    /*
    if (m_Renderer)
          return m_Renderer->GetActiveCamera()->GetViewTransformObject();
        else
          return m_Identity;
    //return m_Renderer?m_Renderer->GetActiveCamera()->GetViewTransformObject():m_Identity;
    // 

    */
    return m_Transform ? m_Transform : m_Identity;
  }
  return m_Identity;
}

//----------------------------------------------------------------------------
void mafRefSys::SetMatrix(vtkMatrix4x4 *matrix)
//---------------------------------------------------------------------------- 
{
  if (matrix)
  {
    auto mat = mafMatrix::NewSPtr();
    mat->SetVTKMatrix(matrix);
    SetMatrix(mat);
  }
  else
  {
    SetTransform(nullptr);
  }
}

//----------------------------------------------------------------------------
void mafRefSys::SetMatrix(std::shared_ptr<mafMatrix> matrix)
//---------------------------------------------------------------------------- 
{
  if (matrix)
  {
    auto trans = mafTransform::NewSPtr(); 
    trans->SetMatrix(*matrix);
    SetTransform(trans);
  }
  else
  {
    SetTransform(nullptr);
  }
}

//----------------------------------------------------------------------------
std::shared_ptr<mafMatrix> mafRefSys::GetMatrix()
//----------------------------------------------------------------------------
{
  return GetTransform() ? GetTransform()->GetMatrixPointer() : NULL;
}

//----------------------------------------------------------------------------
void mafRefSys::SetVME(mafVME *vme)
//----------------------------------------------------------------------------
{
  m_VME = vme;
}

//----------------------------------------------------------------------------
void mafRefSys::SetRenderer(vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
  if (m_Renderer!=renderer)
  {
    if(m_Renderer)
    {
      m_Renderer->UnRegister(NULL);
    }
    m_Renderer = renderer;
		if(m_Renderer)
			m_Renderer->Register(NULL);
  }
}
//----------------------------------------------------------------------------
void mafRefSys::SetTypeToCustom(vtkMatrix4x4 *matrix)
//----------------------------------------------------------------------------
{
  SetMatrix(matrix);
  SetType(CUSTOM);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToCustom(std::shared_ptr<mafMatrix> matrix)
//----------------------------------------------------------------------------
{
  SetMatrix(matrix);
  SetType(CUSTOM);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToCustom(std::shared_ptr<mafTransformBase> transform)
//----------------------------------------------------------------------------
{
  SetTransform(transform);
  SetType(CUSTOM);
}
//----------------------------------------------------------------------------
void mafRefSys::SetTypeToView(vtkRenderer *renderer)
//----------------------------------------------------------------------------
{
	SetRenderer(renderer);
  SetType(VIEW);
}
 
//----------------------------------------------------------------------------
void mafRefSys::SetTypeToParent(mafVME *vme)
//----------------------------------------------------------------------------
{
  assert(vme);
  SetVME(vme);
  SetType(PARENT);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToLocal(mafVME *vme)
//----------------------------------------------------------------------------
{
  //assert(vme);
  SetVME(vme);
  SetType(LOCAL);
}

//----------------------------------------------------------------------------
void mafRefSys::SetTypeToGlobal()
//----------------------------------------------------------------------------
{
  SetType(GLOBAL);
}

//----------------------------------------------------------------------------
void mafRefSys::Print(std::ostream& os, const int tabs)
//----------------------------------------------------------------------------
{
  mafIndent indent(tabs);

  os << indent << "m_Type: ";
  switch (m_Type)
  {
    case PARENT:
      os << "PARENT\n";
      break; 
    case GLOBAL:
      os << "GLOBAL\n";
      break;
    case CUSTOM:
      os << "CUSTOM\n";
      break;
    case VIEW:
      os << "VIEW\n";
      break;
    default:
      os << "UNKONOWN\n";
  }
  
  os << indent << "Reference System Matrix:\n";
  GetTransform()->GetMatrixPointer()->Print(os,indent.GetNextIndent());

  os << indent << "m_Renderer used as Reference System:"<<m_Renderer;

  os << indent << "m_VME: \""<<(m_VME.get() ?m_VME->GetName().GetCStr() :_R("NULL"))<<"\"\n";

}
