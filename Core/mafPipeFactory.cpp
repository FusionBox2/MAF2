/*=========================================================================

 Program: MAF2
 Module: mafPipeFactory
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


#include "mafPipeFactory.h"
#include "mafVersion.h"
#include "mafIndent.h"
#include "mafPipe.h"
#ifdef MAF_USE_VTK
  #include "mafPipeBox.h"
#endif
#include <string>
#include <ostream>
#include <algorithm>

bool mafPipeFactory::m_Initialized=false;
// mafPipeFactory *mafPipeFactory::m_Instance=NULL;

mafCxxTypeMacro(mafPipeFactory);

// std::vector<std::string> mafPipeFactory::m_PipeNames;

//----------------------------------------------------------------------------
// This is used to register the factory when linking statically
int mafPipeFactory::Initialize()
//----------------------------------------------------------------------------
{
  if (!m_Initialized)
  {
    // m_Instance=mafPipeFactory::New();

    m_Initialized=true;
    if (GetInstance())
    {
      GetInstance()->RegisterFactory(GetInstance());
      return MAF_OK;  
    }
    else
    {
      m_Initialized=false;
      return MAF_ERROR;
    }
  }
  
  return MAF_OK;
}

//------------------------------------------------------------------------
mafPipeFactory::mafPipeFactory()
//------------------------------------------------------------------------------
{
  // m_Instance = NULL;
  
  //
  // Plug here Pipes in this factory
  //
  //mafPlugPipeMacro(mafPipeRoot,"root for generic pipes tree");
  //mafPlugPipeMacro(mafPipeGeneric,"a generic pipe with only basic features");
#ifdef MAF_USE_VTK
  mafPlugPipeMacro(mafPipeBox,"General pipe to show box for vtk data");
#endif
}

//------------------------------------------------------------------------------
const char* mafPipeFactory::GetMAFSourceVersion() const
//------------------------------------------------------------------------------
{
  return MAF_SOURCE_VERSION;
}

//------------------------------------------------------------------------------
const char* mafPipeFactory::GetDescription() const
//------------------------------------------------------------------------------
{
  return "Factory for MAF Pipes";
}

//------------------------------------------------------------------------------
mafPipe *mafPipeFactory::CreatePipeInstance(const char *type_name)
//------------------------------------------------------------------------------
{
  return mafPipe::SafeDownCast(Superclass::CreateInstance(type_name));
}

//------------------------------------------------------------------------------
void mafPipeFactory::RegisterNewPipe(const char* pipe_name, const char* description, mafCreateObjectFunction createFunction)
//------------------------------------------------------------------------------
{
	std::vector<std::string, std::allocator<std::string> >::const_iterator it = std::find(GetPipeNames().begin (), GetPipeNames().end (), std::string(pipe_name));
	if(it != GetPipeNames().end() )
	{
    return;
	}
  GetPipeNames().push_back(pipe_name);
  RegisterNewObject(pipe_name,description,createFunction);
}
//------------------------------------------------------------------------------
mafPipeFactory* mafPipeFactory::GetInstance()
//------------------------------------------------------------------------------
{
  static mafPipeFactory &istance = *(mafPipeFactory::New());
  Initialize();
  return &istance;
}
//------------------------------------------------------------------------------
std::vector<std::string> &mafPipeFactory::GetPipeNames()
//------------------------------------------------------------------------------
{
  static std::vector<std::string> pipeNames;
  return pipeNames;
}
