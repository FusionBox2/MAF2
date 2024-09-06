/*=========================================================================

 Program: MAF2
 Module: mafVisualPipeVolumeRayCasting
 Authors: Alexander Savenko - Paolo Quadrani (porting MAF2.2)
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafVisualPipeVolumeRayCasting_H_
#define __mafVisualPipeVolumeRayCasting_H_

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipeVTK.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class vtkMAFAdaptiveVolumeMapper;

//----------------------------------------------------------------------------
// mafVisualPipeVolumeRayCasting :
//----------------------------------------------------------------------------
class MAF_EXPORT mafVisualPipeVolumeRayCasting : public mafPipeVTK 
{
public:
  mafTypeMacro(mafVisualPipeVolumeRayCasting, mafPipeVTK);

  mafVisualPipeVolumeRayCasting();
  ~mafVisualPipeVolumeRayCasting() override;

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  void Select(bool select) override; 

  /** Create the VTK rendering pipeline*/
  void Create(mafNode *node, mafView *view) override;

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  mafGUI  *CreateGui() override;

  vtkMAFAdaptiveVolumeMapper  *m_Mapper; 
  vtkVolume *m_Volume;
  vtkActor  *m_Box;
};  
#endif // __mafVisualPipeVolumeRayCasting_H_
