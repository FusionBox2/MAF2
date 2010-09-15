/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoCrossRotate.h,v $
  Language:  C++
  Date:      $Date: 2010-09-15 16:11:32 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
 
#ifndef __medGizmoCrossRotate_H__
#define __medGizmoCrossRotate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGizmoInterface.h"
#include "mafGUIGizmoRotate.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class medGizmoCrossRotateAxis;
class medGizmoCrossRotateFan;
class mafInteractorGenericInterface;
class mafMatrix;

//----------------------------------------------------------------------------
/** Gizmo used to perform constrained rotation around an axis.
 This class create a rotation gizmo composed of:
  
 3 mafGizmoRotateCircle
 3 mafGizmoRotateFan
 
      y                             y
      ^                             ^  
      |                             |  
      | ___                         |     /|
      |     \              +        |   /  |
      |      |                      | /    | 
      z--------> x                  z--------->x
        
  3 x mafGizmoRotateCircle          3 x mafGizmoRotateFan
  (single axis rotation       (visual feedback for rotation angle on one axis) 
   constrain)

      
      ^  
      |-----
      |     /\
  =   |   /    \         
      | /       |         
       ---------->    
  
   1 x mafGizmoRotate
     

  and forward pose matrixes to the listener operation. 
  This object works by creating smaller components that are parented to the
  vme tree root ie works in global coordinates.
  in order to use it in your client create an instance of it and concatenate
  in post multiply modality matrix that this object is sending to the the
  vme you want to move.

*/

class medGizmoCrossRotate : public mafGizmoInterface 
{
public:
           medGizmoCrossRotate(mafVME *input, mafObserver* listener = NULL, bool buildGUI = true, int axis = X);
  virtual ~medGizmoCrossRotate(); 

  /** 
  Set input vme for the gizmo*/
  void SetInput(mafVME *vme);

  //----------------------------------------------------------------------------
  // events handling 
  //----------------------------------------------------------------------------
  
  /**
  Events handling*/        
  void OnEvent(mafEventBase *maf_event);
 
  //----------------------------------------------------------------------------
  // show 
  //----------------------------------------------------------------------------
  
  /**
  Show the gizmo*/
  void Show(bool show);

  //----------------------------------------------------------------------------

  /**
  Set the gizmo pose*/
  void SetAbsPose(mafMatrix *absPose, bool applyPoseToFans = true);
  mafMatrix *GetAbsPose();

  /**
  Get the Interactor asscociated to each axis*/
  mafInteractorGenericInterface *GetInteractor(int axis);

  //----------------------------------------------------------------------------
  // RefSys
  //----------------------------------------------------------------------------

  /** Set the vme to be used as reference system, the vme is referenced; default ref sys is vme abs matrix */
  void SetRefSys(mafVME *refSys);
  mafVME* GetRefSys();;

  mafGUI *GetGui() {return m_GuiGizmoRotate->GetGui();};

  /** Modify radius of circles*/
  void SetCircleFanRadius(double radius);
  double GetCircleFanRadius();

protected:
  mafGUIGizmoRotate *m_GuiGizmoRotate;

  /** Send matrix to postmultiply to listener */
  void SendTransformMatrixFromGui(mafEventBase *maf_event);

  /** Gizmo components events handling */
  void OnEventGizmoGui(mafEventBase *maf_event);

  /** Gizmo components events handling */
  void OnEventGizmoComponents(mafEventBase *maf_event);

  /** 
  gizmo components enum*/
  enum AXIS {X = 0, Y, Z};

  enum ACTIVE_COMPONENT {NONE = -1, X_AXIS = 0, Y_AXIS, Z_AXIS, NUM_COMPONENTS};

  /**
  Highlight one component and dehighlight other components*/
  void Highlight(bool highlight); 
  
  /** Array holding the three gizmo that performs rotation*/  
  medGizmoCrossRotateAxis *m_GRCircle;

  /** The rotating fan gizmo*/
  medGizmoCrossRotateFan *m_GRFan;

  /** Build Gizmo GUI */
  bool m_BuildGUI;

  /** test friend */
  friend class mafGizmoRotateTest;
  
  double m_CircleFanRadius;

};
#endif
