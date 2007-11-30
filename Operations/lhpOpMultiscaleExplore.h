/*========================================================================= 
Program:   Multimod Application Framework
Module:    $RCSfile: lhpOpMultiscaleExplore.h,v $
Language:  C++
Date:      $Date: 2007-11-30 12:01:57 $
Version:   $Revision: 1.3 $
Authors:   Nigel McFarlane
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpOpMultiscaleExplore_H__
#define __lhpOpMultiscaleExplore_H__

#include "mafOp.h"
#include "mafRWI.h"
#include "mafEventBase.h"
#include "mafVME.h"
#include "mmgDialog.h"

#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include "lhpMultiscaleUtility.h"
#include "lhpMultiscaleVisualPipes.h"

#include <vector>
#include <iostream>


/*******************************************************************************
lhpOpMultiscaleExplore:
A multiscale "view", implemented as a modal operation.
This is an op for viewing surface data where the objects are on different scales.

The user selects an intial vme, on which to run the op.
Only one input vme can be selected because this is an op, not a view,
but additonal vme's can be added from the op dialog.

The op displays a surface view.
Vme's which are too small to see are replaced by tokens.  
Clicking the mouse on a token zooms in on the object.
Vme's which become too large for the view are made invisible, unless they were
the target of a zoom, in which case they have current "attention", and must remain 
visible.

When tokens become too close to resolve, they merge into one token, representing 
the group of actors.  Clicking on such a token zooms in on the group.

Operation and associated classes:
lhpOpMultiscaleExplore -     Main op, Responsible for gui, events and visual pipes.
lhpMultiscaleActor -         Container for an actor and mapper - can be data or token.
mafMultiscaleCoordsUtility - Methods for moving and sizing actors in world, view and display coords.
lhpMultiscaleCallbacks -     Callbacks which convert vtk events to maf events.
lhpMultiscaleCameraParams -  Class for saving camera parameters.
lhpMultiscaleCameraUtility - Methods for moving camera.
lhpMultiscaleUtility -       Blackboard and most of the methods for multiscale.
lhpMultiscaleVectorMath -    Methods for vector arithmetic.
lhpMultiscaleVisualPipes -   Visual pipes for data and tokens.  Create and delete vtk objects.

*******************************************************************************/


//------------------------------------------------------------------------------
// Dependency diagram:
//
//           ------------------------------------------------>
//         /                                                   \
//        /                                                     \                                     
//       /                      ----> CameraParams ------------->
//      /                     /                                   \       
// lhpOp --> MultiscaleUtility ------> CameraUtility --------------> VectorMath
//      \                     \                     \             /
//       \                     \                     \           /
//        \                      -----------> ActorCoordsUtility 
//         \                     \
//          \                     \
//            ---------------------> MultiscaleActor
//            \
//              ---> Visual Pipes
//              \
//                ----> Callbacks
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//class lhpOpMultiscaleExplore
//------------------------------------------------------------------------------
class lhpOpMultiscaleExplore : public mafOp
{
public:

  lhpOpMultiscaleExplore(wxString label = "Explore Multiscale");
  ~lhpOpMultiscaleExplore(); 

  mafTypeMacro(lhpOpMultiscaleExplore, mafOp);

  mafOp* Copy();

  void OnEvent(mafEventBase *maf_event);

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode* vme);

  /** Static copy of Accept(), required so that we can pass the function
  pointer to the VME_CHOOSE event */
  static bool AcceptStatic(mafNode* vme);

  /** Builds operation's interface by calling CreateOpDialog() method. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

protected:
  //----------------------------------------------------------------------------
  // methods for operation's workflow
  //----------------------------------------------------------------------------

  /** Builds operation's interface and visualization pipeline. */
  void CreateOpDialog();

  /** Remove operation's interface. */
  void DeleteOpDialog();

  /** Visual pipe for surface data 
  Joins vme to renderer */
  void CreateSurfacePipeline(mafVME* vme, vtkRenderer *renderer);

  /** Visual pipe for polydata token */
  void CreateTokenPipeline(vtkRenderer *renderer);

  /** Add new vme to scene
  Adds vme to list, creates multiscale actors for data and tokens and creates visual pipes. */
  void AddVmeToScene(mafVME* vme) ;


  //----------------------------------------------------------------------------
  // methods for maintaining list of vme's
  //----------------------------------------------------------------------------

  /** Add vme to list */
  void AddVME(mafVME* vme) {m_VMEs.push_back(vme) ;}

  /** Get pointer to vme */
  mafVME* GetVME(int i) {return m_VMEs.at(i) ;}

  /** Remove vme from list */
  void RemoveVME(int i) {m_VMEs.erase(m_VMEs.begin()+i) ;}

  /** Get number of selected vme's */
  int GetNumberOfVMEs() {return (int)m_VMEs.size() ;}


  //----------------------------------------------------------------------------
  // methods for handling callbacks
  //----------------------------------------------------------------------------

  /** Constants for size thresholds
  SIZELOWER and SIZEUPPER are different to provide hysteresis, so the actor doesn't flicker between states.
  They should be different enough such that the state should not change under a complete camera rotation.
  */
  static const int SIZEUPPER = 60 ;         ///< upper size threshold for actor, ie screen size when it changes from token to visible
  static const int SIZELOWER = 40 ;         ///< lower size threshold for actor, ie screen size when it becomes a token
  static const int TOKENSIZE = 10 ;         ///< standard size of token
  static const int TOKENSIZEMAX = 15 ;      ///< maximum size of visible token
  static const int TOKENSIZEMIN = 5 ;       ///< minimum size of visible token
  static const int TOO_LARGE_FACTOR = 3 ;   ///< how many x larger than scale before an actor is deemed too large

  /** structure for actor and depth info
  used in pick callback */
  typedef struct {
    int actorId ;
    double depth ;
  } ACTORINFO ;

  /** set token size */
  void SetTokenSize(vtkRenderer* renderer, int actorId, int tokenSize) ;          

  /** Get renderer */
  vtkRenderer* GetRenderer() {return m_Rwi->m_RenFront ;}

  /** Get interactor */
  vtkRenderWindowInteractor* GetInteractor() {return vtkRenderWindowInteractor::SafeDownCast(m_Rwi->m_RwiBase) ;}

  /** Get Multiscale utility */
  lhpMultiscaleUtility* GetMultiscaleUtility() {return m_MultiscaleUtility ;}

  /** Zoom out handler */
  void OnZoomOut(vtkRenderer *renderer) ;

  /** Camera reset handler */
  void OnCameraReset(vtkRenderer *renderer) ;

  /** Go back handler */
  void OnGoBack(vtkRenderer *renderer) ;

  /** Start Render handler.
  This is where we look at the scene and decide which actors and tokens should be visible.
  Nowhere else should change the visibility of the actors. */
  void OnStartRender(vtkRenderer *renderer) ;
  void OnActorTooSmall(vtkRenderer* renderer, int actorId) ;               ///< on actor becoming too small
  void OnActorInScale(vtkRenderer* renderer, int actorId) ;                ///< on actor coming back into scale
  void OnActorTooLarge(vtkRenderer* renderer, int actorId) ;               ///< on actor becoming too large
  void OnTokensOverlap(vtkRenderer* renderer, int token1, int token2) ;    ///< on tokens overlapping
  void OnTokensSeparated(vtkRenderer* renderer, int token1, int token2) ;  ///< on tokens separating

  /** Mouse click handler */
  void OnMouseClick() ;
  void OnPick(vtkRenderer* renderer, int tokenId) ;        ///< on token picked by mouse click

  /** Debug handler
  This prints diagnostic info */
  void OnDebug(std::ostream& os, vtkRenderer *renderer) ;



  //----------------------------------------------------------------------------
  // member variables
  //----------------------------------------------------------------------------

  mmgDialog		*m_Dialog;
  mafRWI      *m_Rwi;

  lhpMultiscaleUtility *m_MultiscaleUtility ;                   ///< multiscale utility 

  std::vector <mafVME*> m_VMEs ;                                ///< list of vme's selected by the op

  std::vector <lhpMultiscaleSurfacePipeline*> m_surfacePipes ;  ///< list of surface pipes
  std::vector <lhpMultiscaleTokenPipeline*> m_tokenPipes ;      ///< list of token pipes

  int m_nextTokenColor ;                                        ///< color id of next token
};



#endif
