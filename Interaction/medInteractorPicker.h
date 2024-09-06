/*=========================================================================

 Program: MAF2Medical
 Module: medInteractorPicker
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medInteractorPicker_h
#define __medInteractorPicker_h


#include "mafInteractorCameraMove.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafView;

/** 
Class medInteractorPicker:

When attached to a vme this behavior performs the following actions:

Left Mouse button => camera rotate
Middle Mouse button => camera pan
Right Mouse button  => camera zoom

CTRL + Left Mouse button => send picked vme abs coordinates to the listener

@sa medViewArbitraryOrthoSlice as an example on how to use this picker
*/
class MED_INTERACTION_EXPORT medInteractorPicker : public mafInteractorCameraMove
{
public:
  mafTypeMacro(medInteractorPicker,mafInteractorCameraMove);

  /** Redefined to send picking events if continuous picking is enabled */
  void OnEvent(mafEventBase *event) override;

   /** Enable/disable continuous picking in OnEvent. */
  void EnableContinuousPicking(bool enable);

protected:
  void OnButtonDown   (mafEventInteraction *e) override;
  void OnButtonUp     (mafEventInteraction *e) override;

  /** 
  Send to the listener picked point coordinate through vtkPoint and the corresponding scalar value found in that position. */
  void SendPickingInformation(mafView *v, double *mouse_pos = NULL, int msg_id = VME_PICKED, mafMatrix *tracker_pos = NULL, bool mouse_flag = true);

  bool m_ContinuousPickingFlag;

  medInteractorPicker();
  ~medInteractorPicker() override;

private:
  medInteractorPicker(const medInteractorPicker&);  // Not implemented.
  void operator=(const medInteractorPicker&);  // Not implemented.
};
#endif 
