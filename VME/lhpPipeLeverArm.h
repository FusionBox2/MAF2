/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeLeverArm.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:05:59 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __lhpPipeLeverArm_H__
#define __lhpPipeLeverArm_H__

#include "mafDefines.h"

#include "mafPipeVTK.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkTubeFilter;
class lhpVMELeverArm;
class vtkLookupTable;
class vtkCaptionActor2D;
class mafGUIMaterialButton;

//----------------------------------------------------------------------------
// lhpPipeLeverArm :
//----------------------------------------------------------------------------
class lhpPipeLeverArm : public mafPipeVTK
{
public:
  mafTypeMacro(lhpPipeLeverArm,mafPipeVTK);

               lhpPipeLeverArm();
  ~lhpPipeLeverArm () override;

  /** process events coming from gui */
  void OnEvent(mafEventBase *maf_event) override;

  void Create(mafNode *node, mafView *view /*,bool use_axes = true*/ ) override; //Can't add parameters - is Virtual
  void Select(bool select) override; 

  /** IDs for the GUI */
  enum PIPE_METER_WIDGET_ID
  {
    ID_SHOW_LABEL = Superclass::ID_LAST,
    ID_COLOR_MODE,
    ID_DISTANCE_RANGE,
    ID_METER_REPRESENTATION,
    ID_TUBE_RADIUS,
    ID_TUBE_CAPPING,
    ID_METER_MEASURE_TYPE,
    ID_INIT_MEASURE,
    ID_GENERATE_EVENT,
    ID_DELTA_PERCENT,
    ID_LAST
  };

  vtkCaptionActor2D      *m_Caption;
  vtkTubeFilter          *m_Tube;
  vtkLookupTable         *m_Lut;
  vtkPolyDataMapper	     *m_DataMapper;
  vtkActor               *m_DataActor;
  vtkOutlineCornerFilter *m_SelectionBox;
	vtkPolyDataMapper      *m_SelectionMapper;
  vtkProperty            *m_SelectionProperty;
  vtkActor               *m_SelectionActor;

protected:
  lhpVMELeverArm       *m_MeterVME;
  mafGUIMaterialButton *m_MaterialButton;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false) override;
  mafGUI  *CreateGui() override;
};
#endif // __lhpPipeLeverArm_H__
