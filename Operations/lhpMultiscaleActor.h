/*========================================================================= 
Program:   Multimod Application Framework
Module:    $RCSfile: lhpMultiscaleActor.h,v $
Language:  C++
Date:      $Date: 2007-11-26 12:39:56 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpMultiscaleActor_H__
#define __lhpMultiscaleActor_H__

#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include <iostream>


/** types of multiscale actor */
enum MultiscaleActorType {
  MSCALE_DATA_ACTOR = 0,   ///< visual data actor
  MSCALE_TOKEN             ///< token representing single data actor
} ;

/** screen size mode for tokens */
enum ScreenSizeMode {
  VARIABLE_SIZE = 0,             ///< size fully variable
  FIXED_SIZE,                    ///< size constant
  LIMITED_SIZE                   ///< size varies between max and min
} ;

/** status of size relative to scale thresholds */
enum ScaleStatus {
  UNKNOWN_SCALE = 0,              ///< unknown
  TOO_SMALL,                      ///< actor below size threshold
  IN_SCALE,                       ///< actor size in visible range
  TOO_LARGE                       ///< actor size larger than scale
} ;



/*******************************************************************************
lhpMultiscaleActor:

Utility class for lhpOpMultiscaleExplore.
This is a container for a vtk actor and its mapper.

The class also contains flags for:
(a) the type of multiscale actor - visualization data or token
(b) the current status of the actor relative to the scale thresholds
(c) the behaviour of the screen size (tokens only)
(d) whether the actor has current attention (ie needs to stay visible)

*******************************************************************************/

class lhpMultiscaleActor
{
public:
  /** Constructor */
  lhpMultiscaleActor(vtkActor* actor, vtkPolyDataMapper* mapper, MultiscaleActorType actortype) ;

  /** Print self */
  void PrintSelf(std::ostream& os, vtkIndent indent) ;

  /** Get the vtk actor */
  vtkActor* GetActor() {return m_actor ;}

  /** Get the vtk mapper */
  vtkPolyDataMapper* GetMapper() {return m_mapper ;}

  /** Get the type of actor - data or token */
  MultiscaleActorType GetActorType() {return m_type ;}

  /** Get/set the status of the actor relative to the size threshold */
  ScaleStatus GetScaleStatus() {return m_scaleStatus ;}
  void SetScaleStatus(ScaleStatus scaleStatus) {m_scaleStatus = scaleStatus ;}

  /** Get/set the behaviour of the actor's screen size to fixed, variable or limited */
  ScreenSizeMode GetScreenSizeMode() {return m_screenSizeMode ;}
  void SetScreenSizeMode(ScreenSizeMode mode) {m_screenSizeMode = mode ;}

  /** Get/set attention flag.
  The attention flag distinguishes actors which you are currently interested in from those you are not.
  If true, it stops the actor disappearing when it becomes too large. */
  bool GetAttention() {return m_attention ;}
  void SetAttention(bool attention) {m_attention = attention ;}

private:
  vtkActor* m_actor ;
  vtkPolyDataMapper* m_mapper ;
  MultiscaleActorType m_type ;                  ///< type of actor - data-actor, token or group
  ScaleStatus m_scaleStatus ;                   ///< status of size relative to scale thresholds
  ScreenSizeMode m_screenSizeMode ;             ///< controls behaviour of screen size
  bool m_attention ;                            ///< flag indicating if actor has current attention
} ;

#endif