/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputMuscleWrapper
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEOutputMuscleWrapper.h"
#include "mafVMEMuscleWrapper.h"
#include "mafGUI.h"

#include <assert.h>

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEOutputMuscleWrapperAQ)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEOutputMuscleWrapperAQ::mafVMEOutputMuscleWrapperAQ()
//-------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
mafVMEOutputMuscleWrapperAQ::~mafVMEOutputMuscleWrapperAQ()
//-------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
mafGUI *mafVMEOutputMuscleWrapperAQ::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafVMEOutput::CreateGui();
  
  m_Distance = ((mafVMEMuscleWrapperAQ *)m_VME)->GetDistance();
  m_Gui->Label(_("distance: "), &m_Distance, true);

  m_Angle = ((mafVMEMuscleWrapperAQ *)m_VME)->GetAngle();
  m_Gui->Label(_("angle: "), &m_Angle, true);
	m_Gui->Divider();

  return m_Gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputMuscleWrapperAQ::Update()
//-------------------------------------------------------------------------
{
  assert(m_VME);
  m_VME->Update();

  if (((mafVMEMuscleWrapperAQ *)m_VME)->GetMeterMode() == mafVMEMuscleWrapperAQ::POINT_DISTANCE || ((mafVMEMuscleWrapperAQ *)m_VME)->GetMeterMode() == mafVMEMuscleWrapperAQ::LINE_DISTANCE)
  {
	  m_Distance = ((mafVMEMuscleWrapperAQ *)m_VME)->GetDistance();
  m_Angle ="";
  }
  else if (((mafVMEMuscleWrapperAQ *)m_VME)->GetMeterMode() == mafVMEMuscleWrapperAQ::LINE_ANGLE)
  {
    m_Distance ="";
	m_Angle = ((mafVMEMuscleWrapperAQ *)m_VME)->GetAngle();
  }
  if (m_Gui)
  {
    m_Gui->Update();
  }
}
