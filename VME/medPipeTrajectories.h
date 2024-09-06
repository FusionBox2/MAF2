/*=========================================================================

 Program: MAF2Medical
 Module: medPipeTrajectories
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipeTrajectories_H__
#define __medPipeTrajectories_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------

#include "mafPipeVTK.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkPolyData;
class vtkSphereSource;
class mafVMELandmark;
class vtkAppendPolyData;
class mafMatrixVector;
class vtkCaptionActor2D;


//----------------------------------------------------------------------------
// medPipeTrajectories :
//----------------------------------------------------------------------------
class MED_VME_EXPORT medPipeTrajectories : public mafPipeVTK
{
public:
  mafTypeMacro(medPipeTrajectories,mafPipeVTK);

  medPipeTrajectories();
  ~medPipeTrajectories () override;

  /** process events coming from gui */
  void OnEvent(mafEventBase *maf_event) override;

  void Create(mafNode *node, mafView *view) override;
  void Select(bool select) override; 

  /** Set trajectories interval */
  void SetInterval(int interval) {m_Interval = interval;};

  /**Function to update trajectory */
  void UpdateProperty(bool fromTag = false) override;


  /** IDs for the GUI */
  enum PIPE_POLYLINE_WIDGET_ID
  {
    ID_INTERVAL = Superclass::ID_LAST,
    ID_LABELS,
        ID_LAST
  };

  
protected:
  vtkCaptionActor2D      *m_Caption;
  vtkAppendPolyData      *m_Traj;
  vtkSphereSource        *m_Sphere;
  vtkPolyDataMapper	     *m_Mapper;
  vtkActor               *m_Actor;
  vtkOutlineCornerFilter *m_OutlineBox;
  vtkPolyDataMapper      *m_OutlineMapper;
  vtkProperty            *m_OutlineProperty;
  vtkActor               *m_OutlineActor;
  mafVMELandmark         *m_Landmark;

  std::vector<mafTimeStamp> m_TimeVector;
  mafMatrixVector *m_MatrixVector;

  int m_Interval;
  int m_Labels;

  mafGUI  *CreateGui() override;

  
};  
#endif // __mafPipeTrajectories_H__
