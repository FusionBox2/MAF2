/*=========================================================================

 Program: MAF2
 Module: mafInteractor6DOFCameraMove
 Authors: Michele Diegoli & Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafInteractor6DOFCameraMove_h
#define __mafInteractor6DOFCameraMove_h

#include "mafInteractor6DOF.h"
#include "mafMTime.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkCamera;

/** implements 6DOF move of camera in the scene.
  This class implements a 6DOF move of the renderer camera in the scene. The interaction
  modality is a direct manipulation, where movement of the tracker are mapped
  into movements of the camera. More details to be written...
*/
class MAF_EXPORT mafInteractor6DOFCameraMove : public mafInteractor6DOF
{
public:
  mafTypeMacro(mafInteractor6DOFCameraMove,mafInteractor6DOF);

  /** Start the interaction with the selected object */
  virtual int StartInteraction(mafDeviceButtonsPadTracker *tracker,mafMatrix *pose=NULL);

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

protected:
  mafInteractor6DOFCameraMove();
  virtual ~mafInteractor6DOFCameraMove();

  virtual void Update();

  //double               m_ObjectOrientation[3]; ///< orientation of the object at last Update
  //double               m_ObjectPosition[3]; ///< position of the object at last Update
  double               m_StartCameraPosition[4]; ///< position of the camera at start of interaction
  double               m_StartFocalPoint[4]; ///< focal point of the camera at start of interaction
  double               m_StartViewUp[4]; ///< viewUp of the camera at start of interaction
  double               m_StartOrientation[4];
  double               m_OldZ;

//  bool                 m_ActiveCameraToCurrentCameraFlag;
  vtkCamera*           m_CurrentCamera; ///< Stores camera to which the interaction is currently assigned
  mafMTime             m_UpdateTime; ///< Timestamp of the last update of the tracker to world transformation 

private:
  mafInteractor6DOFCameraMove(const mafInteractor6DOFCameraMove&);  // Not implemented.
  void operator=(const mafInteractor6DOFCameraMove&);  // Not implemented.

  /** Test friend */
  friend class mafInteractor6DOFCameraMoveTest;

};

#endif 
