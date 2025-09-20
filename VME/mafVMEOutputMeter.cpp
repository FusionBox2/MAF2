/*=========================================================================

 Program: MAF2
 Module: mafVMEOutputMeter
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

#include "mafVMEOutputMeter.h"
#include "mafVMEMeter.h"
#include "mafGUI.h"

#include <assert.h>

mafVMEOutputMeter::mafVMEOutputMeter() = default;

mafVMEOutputMeter::~mafVMEOutputMeter() = default;

//-------------------------------------------------------------------------
mafGUI* mafVMEOutputMeter::CreateGui()
//-------------------------------------------------------------------------
{
	assert(!AccessGUI());
	auto gui = mafVMEOutput::CreateGui();

	m_Distance = mafToString(((mafVMEMeter*)m_VME)->GetDistance());
	gui->Label(_L("distance: "), &m_Distance, true);

	m_Angle = mafToString(((mafVMEMeter*)m_VME)->GetAngle());
	gui->Label(_L("angle: "), &m_Angle, true);
	gui->Divider();

	return gui;
}
//-------------------------------------------------------------------------
void mafVMEOutputMeter::Update()
//-------------------------------------------------------------------------
{
	assert(m_VME);
	m_VME->Update();
	auto meter = mafVMEMeter::StaticDownCast(m_VME);
	if (meter->GetMeterMode() == mafVMEMeter::POINT_DISTANCE || meter->GetMeterMode() == mafVMEMeter::LINE_DISTANCE)
	{
		m_Distance = mafToString(meter->GetDistance());
		m_Angle = _R("");
	}
	else if (meter->GetMeterMode() == mafVMEMeter::LINE_ANGLE)
	{
		m_Distance = _R("");
		m_Angle = mafToString(meter->GetAngle());
	}
	UpdateGUI();
}
