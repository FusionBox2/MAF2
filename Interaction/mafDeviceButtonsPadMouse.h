/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceButtonsPadMouse.h,v $
  Language:  C++
  Date:      $Date: 2009-05-25 14:48:12 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafDeviceButtonsPadMouse_h
#define __mafDeviceButtonsPadMouse_h

#include "mafDeviceButtonsPad.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafView;
class mafRWIBase;
class vtkRenderer;
class vtkRenderWindowInteractor;

/** Tracking 2D device, i.e. "Mouse".
  mafDeviceButtonsPadMouse is a class implementing interface for a Mouse. Current implementation 
  receive its inputs by means of ProcessEvent and forward to all listeners. Any 
  widget or windows toolkit can inject events into this centralized mouse.
  @sa mafDeviceButtonsPad mafRWI
*/
class mafDeviceButtonsPadMouse : public mafDeviceButtonsPad
{
public:
  mafTypeMacro(mafDeviceButtonsPadMouse,mafDeviceButtonsPad); 

  //------------------------------------------------------------------------------
  // Events
  //------------------------------------------------------------------------------
  /** @ingroup Events
      Issued when a mouse is moved */
  MAF_ID_DEC(MOUSE_2D_MOVE)
  /** @ingroup Events
      Issued when a keyboard char is pressed */
  MAF_ID_DEC(MOUSE_CHAR_EVENT)
  /** @ingroup Events
      Issued on double click mouse button */
  MAF_ID_DEC(MOUSE_DCLICK)
    
  /** Reimplemented to manage fusion of move events. */
  virtual void OnEvent(mafEventBase *event);

  /** Set the last mouse position and send a 2D MoveEvent. */
  void SetLastPosition(double x,double y,unsigned long modifiers = 0);

  /**  Return the last position */
  const double *GetLastPosition() {return m_LastPosition;}
  void GetLastPosition(double pos[2]) {pos[0]=m_LastPosition[0];pos[1]=m_LastPosition[1];}

  /** Return the renderer of the selected view. */
  vtkRenderer *GetRenderer();

  /** Return the selected view. */
  mafView *GetView();

  /** Return the Interactor. */
  vtkRenderWindowInteractor *GetInteractor();

  /** Return the RenderWindowInteractor used by mouse device */
  mafRWIBase *GetRWI();

  /** Used to set the flag for updating the m_SelectedRWI during mouse motion and not only on ViewSelected event.*/
  void SetUpdateRWIDuringMotion(bool update_on_motion) {m_UpdateRwiInOnMoveFlag = update_on_motion;};
  void UpdateRWIDuringMotionOff() {m_UpdateRwiInOnMoveFlag = true;};
  void UpdateRWIDuringMotionOn() {m_UpdateRwiInOnMoveFlag = true;};

  /** Return the status of the m_UpdateRwiInOnMoveFlag flag.*/
  bool IsUpdateRWIDuringMotion() {return m_UpdateRwiInOnMoveFlag;};

  void DisplayToNormalizedDisplay(double display[2]);
  void NormalizedDisplayToDisplay(double normalized[2]);

  /** Turn On/Off the collaboration status. */
  void Collaborate(bool status) {m_CollaborateStatus = status;};

  bool m_FromRemote;  ///< Flag used from RemoteLogic to avoid loop

protected:
  mafDeviceButtonsPadMouse();
  virtual ~mafDeviceButtonsPadMouse();

  /** add position to the event */ 
  virtual void SendButtonEvent(mafEventInteraction *event);

  double      m_LastPosition[2];///< stores the last position
  bool        m_UpdateRwiInOnMoveFlag; ///< If this flag true the m_SelectedRWI is updated in when the mouse move inside the view (used in compounded views)
  bool        m_CollaborateStatus;
  bool        m_ButtonPressed;

  mafView    *m_SelectedView;   ///< store the selected view to perform the mouse picking
  mafRWIBase *m_SelectedRWI;

private:
  mafDeviceButtonsPadMouse(const mafDeviceButtonsPadMouse&);  // Not implemented.
  void operator=(const mafDeviceButtonsPadMouse&);  // Not implemented.
};
#endif 
