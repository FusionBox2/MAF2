/*========================================================================= 
Program:   Multimod Application Framework
Module:    $RCSfile: lhpTextureOrientationCallbacks.h,v $
Language:  C++
Date:      $Date: 2009-01-29 16:36:58 $
Version:   $Revision: 1.1.2.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpTextureOrientationCallbacks_H__
#define __lhpTextureOrientationCallbacks_H__

#include "vtkCommand.h"
#include "vtkObject.h"
#include "mafBaseEventHandler.h"
#include "mafEventSender.h"
#include "mafInteractor.h"


/*******************************************************************************
Callback to convert vtk event into a maf event.
Plugs into lhpTextureOrientationFilter and responds to user progress event.
Throws maf event id back to listener->OnEvent().
*******************************************************************************/
class lhpTextureOrientationProgressCallback : public vtkCommand, public mafEventSender
{
public:
  static lhpTextureOrientationProgressCallback *New() { return new lhpTextureOrientationProgressCallback ; }
  void Execute(vtkObject *caller, unsigned long eventId, void* calldata) ;

  /** This sets the id of the maf event to be sent to the listener. */
  // NB since there is only one listener, we let the listener set the id.
  // This way the listener can choose a valid id, and define it in the
  // same place as the rest of its id's.
  void SetMafEventId(int id) {m_id = id ;}

private:
  int m_id ;                // id of maf event
};








#endif
