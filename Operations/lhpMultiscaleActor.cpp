/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpMultiscaleActor.cpp,v $
Language:  C++
Date:      $Date: 2007-11-26 12:39:56 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "lhpMultiscaleActor.h"
#include <assert.h>
#include <iostream>


//------------------------------------------------------------------------------
// Constructor
lhpMultiscaleActor::lhpMultiscaleActor(vtkActor* actor, vtkPolyDataMapper* mapper, MultiscaleActorType actortype)
: m_actor(actor), m_mapper(mapper), m_type(actortype), m_scaleStatus(UNKNOWN_SCALE), m_attention(true)
//------------------------------------------------------------------------------
{
  // screen size behaviour: set the data actors to variable and the tokens to limited
  switch(actortype){
    case MSCALE_DATA_ACTOR:
      SetScreenSizeMode(VARIABLE_SIZE) ;
      break ;
    case MSCALE_TOKEN:
      SetScreenSizeMode(LIMITED_SIZE) ;
      break ;
    default:
      std::cout << "error in lhpMultiscaleActor::lhpMultiscaleActor(): unrecognised multiscale actor type" << std::endl ;
      assert(false) ;
  }
}



//------------------------------------------------------------------------------
// print self
void lhpMultiscaleActor::PrintSelf(std::ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{
  os << indent ;

  os << "type = " ;
  switch(m_type){
    case MSCALE_DATA_ACTOR:
      os << "DATA " ;
      break ;
    case MSCALE_TOKEN:
      os << "TOKEN" ;
      break ;
    default:
      os << "?????" ;
      break ;
  }
  os << "\t" ;


  os << "size-mode = " ;
  switch(m_screenSizeMode){
    case VARIABLE_SIZE:
      os << "VAR    " ;
      break ;
    case FIXED_SIZE:
      os << "FIXED  " ;
      break ;
    case LIMITED_SIZE:
      os << "LIMITED" ;
      break ;
    default:
      os << "???????" ;
      break ;
  }
  os << "\t" ;

  os << "scale-status = " ;
  switch(m_scaleStatus){
    case UNKNOWN_SCALE:
      os << "UNKNOWN   " ;
      break ;
    case TOO_SMALL:
      os << "TOO SMALL " ;
      break ;
    case IN_SCALE:
      os << "IN SCALE  " ;
      break ;
    case TOO_LARGE:
      os << "TOO LARGE " ;
      break ;
    default:
      os << "??????????" ;
      break ;
  }
  os << "\t" ;

  os << "attention =  " << m_attention << "  " ;
  os << "visibility = " << GetActor()->GetVisibility() << std::endl ;
}
