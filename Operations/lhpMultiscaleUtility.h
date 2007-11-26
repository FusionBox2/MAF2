/*========================================================================= 
Program:   Multimod Application Framework
Module:    $RCSfile: lhpMultiscaleUtility.h,v $
Language:  C++
Date:      $Date: 2007-11-26 12:39:56 $
Version:   $Revision: 1.1 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpMultiscaleUtility_H__
#define __lhpMultiscaleUtility_H__

#include "vtkObject.h"
#include "vtkCommand.h"

#include "lhpMultiscaleActor.h"
#include "lhpMultiscaleCameraParams.h"
#include "lhpMultiscaleActorCoordsUtility.h"
#include "lhpMultiscaleCameraUtility.h"

#include <vector>
#include <iostream>


/*******************************************************************************
Utility methods for multiscale:
typedef ViewState           - helper struct
class ActorIdPair           - helper class
class lhpMultiscaleUtility  - methods and info blackboard for managing multiscale view
*******************************************************************************/

/*******************************************************************************
Helper struct to store state of view
*******************************************************************************/
typedef struct{
  lhpMultiscaleCameraParams cameraParams ;               // camera position
  std::vector<bool> attentionFlags ;        // attention flags of the actors (nb beware of vector<bool>)
} ViewState ;


/*******************************************************************************
Helper class containing pair of id's.
*******************************************************************************/
class ActorIdPair{
public:
  ActorIdPair(int id0, int id1) : m_id0(id0), m_id1(id1) {}
  int GetId0() {return m_id0 ;}
  int GetId1() {return m_id1 ;}

  // equality operator
  bool operator==(const ActorIdPair& idPair1) const 
  {
    return ((this->m_id0==idPair1.m_id0 && this->m_id1==idPair1.m_id1) ||
      (this->m_id0==idPair1.m_id1 && this->m_id1==idPair1.m_id0)) ;
  }
private:
  int m_id0 ;
  int m_id1 ;
} ;



/*******************************************************************************
lhpMultiscaleUtility:
Methods and blackboard for managing multiscale view
*******************************************************************************/

class lhpMultiscaleUtility
{
public:
  lhpMultiscaleUtility() ;
  ~lhpMultiscaleUtility() ;

  /** Print self */
  void PrintSelf(std::ostream& os, vtkIndent indent) ;

  /** lhpMultiscaleActorCoordsUtility provides methods for manipulating the coords and size of actors */
  lhpMultiscaleActorCoordsUtility* GetActorCoordsUtility() {return m_actorCoordsUtility ;}

  /** lhpMultiscaleCameraUtility provides methods for manipulating the camera */
  lhpMultiscaleCameraUtility* GetCameraUtility() {return m_cameraUtility ;}

  /** Create multiscale actor from given actor and mapper, and add to list
  Type is data actor or token
  NB If you add actors after having saved the camera view, you must call SaveInitialView() again. */
  void AddMultiscaleActor(vtkActor* actor, vtkPolyDataMapper* mapper, MultiscaleActorType type) ;

  /** Get ith multiscale actor */
  lhpMultiscaleActor* GetMultiscaleActor(int i) ;

  /** Get number of actors */
  int GetNumberOfActors() {return (int)m_multiscaleActors.size() ;}

  /** Get the token actor which corresponds to data-actor i */
  int GetTokenCorrespondingToDataActor(int i) ;

  /** Get the data-actor which corresponds to token i */
  int GetDataActorCorrespondingToToken(int i) ;

  /** Set actors i and j as a data-actor/token pair.
  You can only call this once for each i and j. */
  void SetActorTokenPair(int i, int j) ;

  /** Save current view state.  
  Saves camera parameters and attention flags so that we can return to a previous state. */
  void SaveView(vtkRenderer *renderer) ;

  /** Save current view state as intial view
  Saves camera parameters and attention flags so that we can return to a previous state.
  If you add more actors, you must call this again. */
  void SaveInitialView(vtkRenderer *renderer) ;

  /** Restore view to previous state
  If the states stored are O, A, B, C, and our current state is C' (ie C plus camera movement)
  this will delete C and return the view to B. */
  void RestoreView(vtkRenderer* renderer) ;

  /** Restore view to last state without deleting saved state
  If the states stored are O, A, B, C, and our current state is C' (ie C plus camera movement)
  this will return the state to C, without deleting C. */
  void RestoreViewWithoutDelete(vtkRenderer* renderer) ;

  /** Clear all camera saves except the intial state */
  void ClearViewSaves() ;

  /** Has camera changed since last save */
  bool CameraSameSinceLastSave(vtkRenderer *renderer) ;

  /** Reset camera to fit all actors */
  void ResetCameraFitAll(vtkRenderer *renderer) ;

  /** convert scale to tidy units, eg 0.342 m -> 300 mm */
  void ConvertScaleToTidyUnits(double scale, int *iscale, std::ostrstream& units) ;

  /** set attention of all actors to true or false */
  void SetAttentionAll(bool attention) ;

  /** set attention to all actors in list */
  void SetAttentionToList(const std::vector<int> &idlist, bool attention) ;

  /** Add pair of touching tokens to list */
  void AddTouchingTokens(int tokenid0, int tokenid1) ;

  /** Find and remove pair of touching tokens from list */
  void RemoveTouchingTokens(int tokenid0, int vid1) ;

  /** Get number of tokens which touch this one */
  int GetNumberOfTouchingTokens(int tokenId) ;

  /** Get number of touching tokens whose data actors are larger
  If actors are the same size, the lower token index is considered larger. */
  int GetNumberOfTouchingTokensLarger(int tokenId) ;

  /** Are tokens in list of touching pairs */
  bool TokensListedAsTouching(int tokenid0, int tokenid1) ;

  /** Print list of touching tokens */
  void PrintTouchingTokensList() ;

  /** Return actor collection consisting of the data actors of the tokens which touch this one
  The collection includes the input token */
  void GetTouchingDataActorsAsCollection(int tokenId, vtkActorCollection *AC) ;

  /** Return id list consisting of all the data actors of the tokens which touch this one
  The list includes the input token */
  void GetTouchingDataActorsAsIdList(int tokenId, std::vector<int> &idlist) ;

  /** Get distance apart of two actors in pixel units
  This is not the 2D separation - it is the 3D distance in pixels, independent of view direction */
  double DistanceApartPixelUnits(vtkActor *actor1, vtkActor *actor2, vtkRenderer *renderer) ;


private:
  /** Find pair of touching tokens in the list
  Returns -1 if not found */
  int FindTouchingTokens(int tokenId0, int tokenId1) ;

  /** list of multiscale actors */
  std::vector<lhpMultiscaleActor> m_multiscaleActors ;

  /** table of correspondences between actors and tokens.
  if actor i is a data-actor,  m_actorPairs[i] is its token. 
  "    "   "  " " token,       m_actorPairs[i] is its data actor. */
  int m_actorPairs[100] ;

  /** table of pairs of touching tokens */
  std::vector<ActorIdPair> m_touchingTokens ;

  /** utilities (these are created and deleted by the class)
  it is convenient but not essential for this class to have pointers to these classes */
  lhpMultiscaleActorCoordsUtility *m_actorCoordsUtility ;
  lhpMultiscaleCameraUtility *m_cameraUtility ;

  /** stack of saved states */
  std::vector<ViewState> m_savedViewStates ;
} ;

#endif
