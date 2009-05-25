/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDeviceButtonsPadTrackerP5Glove.cpp,v $
  Language:  C++
  Date:      $Date: 2009-05-25 14:48:12 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h"

#include "mafDeviceButtonsPadTrackerP5Glove.h"
#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafEvent.h"

#include "P5dll.h"
#include "P5Motion.h"
#include "P5Bend.h"

#include "mafMatrix.h"

mafCxxTypeMacro(mafDeviceButtonsPadTrackerP5Glove)

//------------------------------------------------------------------------------
// PIMPL declarations
//------------------------------------------------------------------------------
CP5DLL *mafDeviceButtonsPadTrackerP5Glove::m_P5=NULL;

//------------------------------------------------------------------------------
//vtkStandardNewMacro(mafDeviceButtonsPadTrackerP5Glove)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafDeviceButtonsPadTrackerP5Glove::mafDeviceButtonsPadTrackerP5Glove()
//------------------------------------------------------------------------------
{
  this->SetThreaded(1);
  mafNEW(m_TmpPose);
  m_IndexValue  = 0;
  m_MiddleValue = 0;
  m_RingValue   = 0;
  m_PinkyValue  = 0;
  m_ThumbValue  = 0;
  m_IndexSensitivity  = 0;
  m_MiddleSensitivity = 0;
  m_RingSensitivity   = 0;
  m_PinkySensitivity  = 0;
  m_ThumbSensitivity  = 0;
  SetNumberOfButtons(5);
}

//------------------------------------------------------------------------------
mafDeviceButtonsPadTrackerP5Glove::~mafDeviceButtonsPadTrackerP5Glove()
//------------------------------------------------------------------------------
{
  mafDEL(m_TmpPose);
}

//------------------------------------------------------------------------------
int mafDeviceButtonsPadTrackerP5Glove::InternalInitialize()
//------------------------------------------------------------------------------
{
  if (m_P5==NULL)
  {
    m_P5 = new CP5DLL;
  }

  if (!m_P5->P5_Init())
	{
	  mafErrorMacro("P5 Tracker Not Found");
    delete m_P5;
    m_P5=NULL;
    return -1;
	}
	else
	{
    if (m_P5->m_nNumP5==0)
    {
      mafErrorMacro("0 gloves connected to P5, shutdown...");
      m_P5->P5_Close();
      delete m_P5;
      m_P5=NULL;
      return -1;
    }
    
		RECT region;

		P5Motion_Init(m_P5, 0);

		P5Motion_InvertMouse(P5MOTION_INVERTAXIS, P5MOTION_NORMALAXIS, P5MOTION_NORMALAXIS);
#ifdef WIN32
    GetClipCursor(&region);
		P5Motion_SetClipRegion(region.left, region.right, region.top, region.bottom, region.top, region.bottom);
#endif

    // initilize the bend utility routine
    P5Bend_Init(m_P5, 0);

    // set sensitivity (notice P5 lib has a global sensitivity variable)
    SetThumbSensitivity(nBendSensitivity[P5_THUMB]);
	  SetIndexSensitivity(nBendSensitivity[P5_INDEX]);
	  SetMiddleSensitivity(nBendSensitivity[P5_MIDDLE]);
	  SetRingSensitivity(nBendSensitivity[P5_RING]);
	  SetPinkySensitivity(nBendSensitivity[P5_PINKY]);

    // disable Mouse modality for P5 0
		m_P5->P5_SetMouseState(0, FALSE);

    mafLogMessage("P5 Tracker Found & Initialized");
	}

  return (this->Superclass::InternalInitialize());
}

//------------------------------------------------------------------------------
void mafDeviceButtonsPadTrackerP5Glove::InternalShutdown()
//------------------------------------------------------------------------------
{
  Superclass::InternalShutdown(); // stop thread
  if (m_P5)
  {
    m_P5->P5_Close();
    delete m_P5;
    m_P5=NULL;
  }
}

//------------------------------------------------------------------------------
int mafDeviceButtonsPadTrackerP5Glove::InternalUpdate()
//------------------------------------------------------------------------------
{ 
  //float xyz[3];
  //float pyr[3];

	if( m_P5 && m_P5->m_P5Devices != NULL)
	{
    
    P5Data glove = m_P5->m_P5Devices[0];
    P5Motion_Process();

    m_TmpPose->Identity();
    mafTransform::SetPosition(*m_TmpPose,fFilterX, fFilterY, -fFilterZ);
    
    // todo: to be checked
    mafTransform::RotateZ(*m_TmpPose,-fAbsRollPos, PRE_MULTIPLY);
    mafTransform::RotateX(*m_TmpPose,fAbsPitchPos, PRE_MULTIPLY);
    mafTransform::RotateY(*m_TmpPose,-fAbsYawPos, PRE_MULTIPLY);
		
    
    // the matrix is automatically copied and timestamped
    SetLastPoseMatrix(*m_TmpPose);
    
    unsigned char value;

    value=((unsigned char)glove.m_byBendSensor_Data[P5_INDEX]);
		m_IndexValue = value;
		
		value=((unsigned char)glove.m_byBendSensor_Data[P5_MIDDLE]);
		m_MiddleValue = value;
		
		value=((unsigned char)glove.m_byBendSensor_Data[P5_RING]);
		m_RingValue = value;
		
		value=((unsigned char)glove.m_byBendSensor_Data[P5_PINKY]);
		m_PinkyValue = value;

		value=((unsigned char)glove.m_byBendSensor_Data[P5_THUMB]);
		m_ThumbValue = value;

    // update bend sensors sensitivity
		P5Bend_SetClickSensitivity(P5_THUMB, m_ThumbSensitivity);
		P5Bend_SetClickSensitivity(P5_INDEX, m_IndexSensitivity);
		P5Bend_SetClickSensitivity(P5_MIDDLE, m_MiddleSensitivity);
		P5Bend_SetClickSensitivity(P5_RING, m_RingSensitivity);
		P5Bend_SetClickSensitivity(P5_PINKY, m_PinkySensitivity);

		P5Bend_Process();

    SetButtonState(0,bP5ClickLevel[P5_INDEX]);
    SetButtonState(1,bP5ClickLevel[P5_MIDDLE]);
    SetButtonState(2,bP5ClickLevel[P5_RING]);
    SetButtonState(3,bP5ClickLevel[P5_PINKY]);
    SetButtonState(4,bP5ClickLevel[P5_THUMB]);
    
    Sleep(25);
    
    return 0;
	}
	else
  {
    return -1; // stop UpdateLoop
  }
}
