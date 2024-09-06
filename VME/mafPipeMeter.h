/*=========================================================================

 Program: MAF2
 Module: mafPipeMeter
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeMeter_H__
#define __mafPipeMeter_H__

#include "mafPipeVTK.h"
//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkTubeFilter;
class mafVMEMeter;
class vtkLookupTable;
class vtkCaptionActor2D;
class mafGUIMaterialButton;

//----------------------------------------------------------------------------
// mafPipeMeter :
//----------------------------------------------------------------------------
class MAF_EXPORT mafPipeMeter : public mafPipeVTK
{
public:
  mafTypeMacro(mafPipeMeter,mafPipeVTK);

               mafPipeMeter();
  ~mafPipeMeter () override;

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
  mafVMEMeter       *m_MeterVME;
  mafGUIMaterialButton *m_MaterialButton;

  /** Update visual properties*/
  void UpdateProperty(bool fromTag = false) override;
  mafGUI  *CreateGui() override;
};
#endif // __mafPipeMeter_H__
