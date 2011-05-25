/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVisualPipeVolumeRayCasting.h,v $
  Language:  C++
  Date:      $Date: 2011-05-25 11:52:18 $
  Version:   $Revision: 1.2.2.1 $
  Authors:   Alexander Savenko - Paolo Quadrani (porting MAF2.2)
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafVisualPipeVolumeRayCasting_H_
#define __mafVisualPipeVolumeRayCasting_H_

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class vtkMAFAdaptiveVolumeMapper;

//----------------------------------------------------------------------------
// mafVisualPipeVolumeRayCasting :
//----------------------------------------------------------------------------
class MAF_EXPORT mafVisualPipeVolumeRayCasting : public mafPipe 
{
public:
  mafTypeMacro(mafVisualPipeVolumeRayCasting, mafPipe);

  mafVisualPipeVolumeRayCasting();
  virtual ~mafVisualPipeVolumeRayCasting();

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI  *CreateGui();

  vtkMAFAdaptiveVolumeMapper  *m_Mapper; 
  vtkVolume *m_Volume;
  vtkActor  *m_Box;
};  
#endif // __mafVisualPipeVolumeRayCasting_H_
