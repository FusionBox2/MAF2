/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpImporterC3D.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-28 19:35:02 $
  Version:   $Revision: 1.6 $
  Authors:   Daniele  Giunchi
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "lhpOpImporterC3D.h"

#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mmgGui.h"

#include "mafSmartPointer.h"
#include "mafVME.h"
#include "vtkMAFSmartPointer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafVMEVector.h"
#include "medVMEAnalog.h"
#include "mafTagArray.h"

#include <vtkCubeSource.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include "vtkMAFSmartPointer.h"
#include "vtkCellArray.h"
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include "C3D_Reader.h"

#include <vnl\vnl_matrix.h>

#include <iostream>
#include <fstream>

#define PLATFORM_THICKNESS 5.0
#define mafMax(a, b) (((a) >= (b)) ? (a) : (b))
#define mafMin(a, b) (((a) <= (b)) ? (a) : (b))

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpImporterC3D);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpImporterC3D::lhpOpImporterC3D(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_IMPORTER;
  m_Canundo = true;

  m_C3DInputFileNameFullPath = "";

	m_FileDir = (mafGetApplicationDirectory() + "/Data/External/").c_str();

  //vmes
  m_VmeCloud = NULL;
  m_VmeAnalog = NULL;
  
  //c3d filename
  m_FileName = "";

  //gui
  m_ImportTrajectoriesFlag = TRUE;
  m_ImportAnalogFlag = TRUE;
  m_ImportPlatformFlag = TRUE;
  m_ImportEventFlag = FALSE;

  //Aurion
  //m_Errcode = 0;
  m_AnalogRate = 0;
  m_VideoRate = 0;
  m_LengthMs = 0;

  //derived
  m_TrajectorySampleFrequency = 0;
  m_AnalogSampleFrequency = 0;
  m_VectogramSampleFrequency = 0;

  m_NumTotTrajectories = 0; 
  m_NumTrajectories = 0; 
  m_NumAngles = 0; 
  m_NumMoments = 0; 
  m_NumPowers = 0; 
  m_NumFrames = 0; 
  m_NumChannels = 0;
  m_NumSamples = 0;
  m_NumEvents = 0;
  m_NumPlatforms = 0;

  m_TrajectoryName = NULL;
  m_ChannelName = NULL;
  m_AngleName = NULL; 
  m_MomentName = NULL;
  m_PowerName = NULL;
  m_EventContext = NULL;

  m_TrajectoryUnit = NULL;
  m_ChannelUnit = NULL;
  m_AngleUnit = NULL;
  m_MomentUnit = NULL;
  m_PowerUnit = NULL;

  m_X = 0;
  m_Y = 0;
  m_Z = 0;

  m_AnalogValue = 0;
  m_EventValue  = 0;

  m_CopX = 0;
  m_CopY = 0;

  m_ForceX = 0;
  m_ForceY = 0; 
  m_ForceZ = 0;

  m_MomentX = 0;
  m_MomentY = 0;
  m_MomentZ = 0;

  m_CenterX = 0;
  m_CenterY = 0;
}

//----------------------------------------------------------------------------
lhpOpImporterC3D::~lhpOpImporterC3D()
//----------------------------------------------------------------------------
{
  mafDEL(m_VmeCloud);

  mafDEL(m_VmeAnalog);

  for(int currentPlatForm=0; currentPlatForm< m_PlatformList.size();currentPlatForm++)
  {
    mafDEL(m_PlatformList[currentPlatForm]);
    mafDEL(m_ForceList[currentPlatForm]);
    mafDEL(m_MomentList[currentPlatForm]);
  }
  m_PlatformList.clear();
  m_ForceList.clear();
  m_MomentList.clear();
}
//----------------------------------------------------------------------------
bool lhpOpImporterC3D::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return true;
}
//----------------------------------------------------------------------------
mafOp* lhpOpImporterC3D::Copy()   
//----------------------------------------------------------------------------
{
  lhpOpImporterC3D *cp = new lhpOpImporterC3D(m_Label);
  cp->m_Canundo = m_Canundo;
  cp->m_OpType = m_OpType;
  cp->m_Listener = m_Listener;
  cp->m_VmeCloud = m_VmeCloud;
  return cp;
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::OpRun()   
//----------------------------------------------------------------------------
{
  CreateGui();
  ShowGui();
}
//----------------------------------------------------------------------------
int lhpOpImporterC3D::OpenC3D()
//----------------------------------------------------------------------------
{
  mafLogMessage("C3D_Open");
	int errcode=C3D_Open(const_cast<char *> (m_C3DInputFileNameFullPath.GetCStr()));
	if(errcode != NOERROR)
	{
		if( errcode == ERROR_NOT_LICENSE)
			wxMessageBox("Not registered Product. Contact Aurion S.r.l.");
		if( errcode == ERROR_OPEN_FILE)
			wxMessageBox("Error on opening file");
		if( errcode == ERROR_READING_PROC_TYPE)
			wxMessageBox("Error on reading type (PC, DEC, MIPS)");

		errcode = -1;
	}
  return errcode;
}
//----------------------------------------------------------------------------
int lhpOpImporterC3D::ReadHeaderC3D()
//----------------------------------------------------------------------------
{
  mafLogMessage("C3D_Read_Header");
	int errcode=C3D_Read_Header(&m_LengthMs, &m_VideoRate, &m_AnalogRate);
	if( errcode != NOERROR)
	{
		switch(errcode)
		{
		case ERROR_READING_HEADER:
			wxMessageBox("Error reading header");
			break;
		case ERROR_READING_PARAM:
			wxMessageBox("Error reading parameters");
			break;
		case ERROR_READING_TRIAL_PARAM:
			wxMessageBox("Error reading parameter of trial section");
			break;
		}

		errcode = -1;
	}	
  return errcode;
}
//----------------------------------------------------------------------------
int lhpOpImporterC3D::ReadDataC3D()
//----------------------------------------------------------------------------
{
  mafLogMessage("C3D_Read_Data");
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, loading file");
  }

	int errcode=C3D_Read_Data();	
	if( errcode != NOERROR)
	{
		switch(errcode)
		{
		case ERROR_READING_PARAM:
			wxMessageBox("Error reading parameters");
			break;
		case ERROR_READING_TRIAL_PARAM:
			wxMessageBox("Error reading parameters of trial section");
			break;
		case ERROR_READING_VIDEO_PARAM:
			wxMessageBox("Error reading parameters of cinematic section");
			break;
		case ERROR_READING_ANALOG_PARAM:
			wxMessageBox("Error reading parameters of analog section");
			break;
		case ERROR_READING_FORCE_PLATFORM_PARAM:
			wxMessageBox("Error reading parameters of force plate section");
			break;
		case ERROR_READING_EVENT_PARAM:
			wxMessageBox("Error reading parameters of events section");
			break;
		case ERROR_READING_DATA:
			wxMessageBox("Error reading data");
			break;
		case ERROR_READING_VIDEO_DATA:
			wxMessageBox("Error reading cinematic data");
			break;			
		case ERROR_READING_ANALOG_DATA:
			wxMessageBox("Error reading analog data");
			break;	
		default:
			break;
		}

		errcode = -1;

    if(!m_TestMode)
    {
      delete wait;
    }

    return errcode;
	}

  mafLogMessage("C3D_Calculate_Data");
  errcode=C3D_Calculate_Data();

  if(!m_TestMode)
  {
    delete wait;
  }

	return errcode;
}
//----------------------------------------------------------------------------
int lhpOpImporterC3D::CloseC3D()
//----------------------------------------------------------------------------
{
  mafLogMessage("C3D_Close");
	int errcode=C3D_Close();
	if( errcode == ERROR_CLOSE_FILE)
	{
		wxMessageBox("Error on closing file");		
		errcode = -1;
	}	
  return errcode;
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::Initialize()
//----------------------------------------------------------------------------
{
	//initialize class members with read data 
  //Trajectories
  m_NumTotTrajectories = getNumTraj();		//number of total trajectories(with angles, moments, powers)
  m_NumFrames = getTotalFrameTraj();		  //number of frames

  //Analog
  m_NumChannels = getChannelsAnalog();    //channels number
  m_NumSamples  = getTotalSamplesAnalog();//samples number

  //Platforms
  m_NumPlatforms = getPlatforms();        // platforms number

  //Events
  m_NumEvents = getEvents();              //events number

  //derived
  m_TrajectorySampleFrequency = ((double)m_LengthMs/(double)m_NumFrames) / 1000.0;
  m_AnalogSampleFrequency = ((double)m_LengthMs/(double)m_NumSamples) / 1000.0;
  m_VectogramSampleFrequency = m_AnalogSampleFrequency;

  wxString fileName = m_C3DInputFileNameFullPath.GetCStr();
  fileName = fileName.AfterLast('\\').BeforeLast('.');

  m_FileName = fileName;
}
//----------------------------------------------------------------------------
bool lhpOpImporterC3D::Import()
//----------------------------------------------------------------------------
{
	if(OpenC3D()==NOERROR)
	{
		//c3d read data
		if(ReadHeaderC3D()==NOERROR && ReadDataC3D() == NOERROR)
		{
      Initialize();

			//fill data structures
      if(m_ImportTrajectoriesFlag) ImportTrajectories();
			if(m_ImportAnalogFlag) ImportAnalog();	
			if(m_ImportPlatformFlag) ImportPlatform();
      if(m_ImportEventFlag) ImportEvent();

			CloseC3D();
			return true;
		}
		else
		{
			CloseC3D();
			return false;
		}
    
	}
	else
	{
    return false; //however there is a precedent error
	}
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::ImportTrajectories()
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, import trajectories");
  }

  mafNEW(m_VmeCloud);
  mafString vmeCloudName;
  vmeCloudName.Append(m_FileName);
  vmeCloudName.Append("_TRAJECTORIES");

  m_VmeCloud->SetName(vmeCloudName);

  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  
  long progress = 0;
  for(int currentFrame = 0; currentFrame < m_NumFrames; currentFrame++)
  {
    m_NumTrajectories=0;
    m_NumAngles=0;
    m_NumMoments=0;
    m_NumPowers=0;
    //For every trajectory
    for(int currentTrajectory=0; currentTrajectory<m_NumTotTrajectories; currentTrajectory++)
    {
      switch(getTypeTraj(currentTrajectory))
      {
      case TRAJECTORY:
        {
          m_TrajectoryName=getNameTraj(currentTrajectory);		//trajectory name
          //control if m_Trajectory is not a phantom landmark(camera reflexes)
          if(m_TrajectoryName[0] != '*')
          {
            m_TrajectoryUnit=getUnitTraj(currentTrajectory);		//unit measure of trajectory
            bool visibility = isDefinedTraj(currentTrajectory, currentFrame);
            if(visibility)
            {
              m_X = getXTraj(currentTrajectory, currentFrame);					//component x of the trajectory
              m_Y = getYTraj(currentTrajectory, currentFrame);					//component y of the trajectory
              m_Z = getZTraj(currentTrajectory, currentFrame);					//component z of the trajectory
            }
            else
            {
              m_X = m_Y = m_Z = 0.0;
            }


            if(currentFrame == 0)
            {
              m_VmeCloud->AppendLandmark(m_X,m_Y,m_Z,m_TrajectoryName);
            }
            else
            {
              m_VmeCloud->SetLandmark(m_TrajectoryName,m_X,m_Y,m_Z,currentFrame * m_TrajectorySampleFrequency);
            }

            m_VmeCloud->SetLandmarkVisibility(m_TrajectoryName,visibility,currentFrame * m_TrajectorySampleFrequency);

            m_NumTrajectories++;
          }
        }
        break;
      case ANGLE:
        m_AngleName=getNameTraj(currentTrajectory);		//angle name
        m_AngleUnit=getUnitTraj(currentTrajectory);		//unit measure of angle
        if(isDefinedTraj(currentTrajectory, currentFrame))
        {
          m_X = getXTraj(currentTrajectory, currentFrame);					//component x of angle
          m_Y = getYTraj(currentTrajectory, currentFrame);					//component y of angle
          m_Z = getZTraj(currentTrajectory, currentFrame);					//component z of angle
        }
        m_NumAngles++;
        break;
      case MOMENT:
        m_MomentName=getNameTraj(currentTrajectory);		//moment name
        m_MomentUnit=getUnitTraj(currentTrajectory);		//unit measure of moment
        if(isDefinedTraj(currentTrajectory, currentFrame))
        {
          m_X = getXTraj(currentTrajectory, currentFrame);					//component x of the moment
          m_Y = getYTraj(currentTrajectory, currentFrame);					//component y of the moment
          m_Z = getZTraj(currentTrajectory, currentFrame);					//component z of the moment
        }
        m_NumMoments++;
        break;
      case POWER:
        m_PowerName=getNameTraj(currentTrajectory);		//power name
        m_PowerUnit=getUnitTraj(currentTrajectory);		//unit measure of power
        if(isDefinedTraj(currentTrajectory, currentFrame))
        {
          m_X = getXTraj(currentTrajectory, currentFrame);					//component x of power
          m_Y = getYTraj(currentTrajectory, currentFrame);					//component y of power
          m_Z = getZTraj(currentTrajectory, currentFrame);					//component z of power
        }
        m_NumPowers++;
        break;
      }
    }

    progress = (currentFrame + 1) * 100 / m_NumFrames;
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
  }
  
  m_VmeCloud->Modified();
  m_VmeCloud->Update();

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  if(!m_TestMode)
  {
    delete wait;
  }
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::ImportAnalog()
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, import analog data");
  }
  
  long progress = 0;
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  //name analog vme
  mafNEW(m_VmeAnalog);
  mafString analogVmeName;
  analogVmeName.Append(m_FileName);
  analogVmeName.Append("_ANALOG");
  m_VmeAnalog->SetName(analogVmeName);

  vnl_matrix<double> analogMatrix;
  analogMatrix.set_size(m_NumChannels+1 , m_NumSamples); //set dimensions

  std::vector<mafString> channelsNameList; //string array for channel name

  //For every Sample
  for(int currentSample=0; currentSample<m_NumSamples; currentSample++)
  {
    int currentTime = currentSample * m_AnalogSampleFrequency;
    
    analogMatrix.put(0,currentSample, currentTime); //fill first row with timeframe, every column is a time

    //For every channel
    for(int currentChannel=0; currentChannel<m_NumChannels; currentChannel++)
    {
      m_ChannelName=getNameAnalog(currentChannel);				            //channel name
      m_AnalogValue=getValueAnalog(currentChannel, currentSample);	  //trajectory value
      m_ChannelUnit=getUnitAnalog(currentChannel);				            //unit measure of analogic channel

      if(currentSample == 0) channelsNameList.push_back(m_ChannelName);

      analogMatrix.put(currentChannel+1,currentSample, m_AnalogValue); //fill following rows with values, every channel is a row
    }

    progress = (currentSample +1 ) * 100 / (m_NumSamples);
    mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
  }

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  m_VmeAnalog->SetData(analogMatrix, 0);

  mafTagItem tag_Sig;
  tag_Sig.SetName("SIGNALS_NAME");
  tag_Sig.SetNumberOfComponents(m_NumSamples);
  m_VmeAnalog->GetTagArray()->SetTag(tag_Sig);

  mafTagItem *tag_Signals = m_VmeAnalog->GetTagArray()->GetTag("SIGNALS_NAME");
  for (int n = 0; n < channelsNameList.size(); n++)
  {
    tag_Signals->SetValue(channelsNameList[n], n);
  }

  if(!m_TestMode)
  {
    delete wait;
  }
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::ImportPlatform()
//----------------------------------------------------------------------------
{
  wxBusyInfo *wait;
  if(!m_TestMode)
  {
    wait = new wxBusyInfo("Please wait, import Force Plate Data");
  }

  long progress = 0;
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  //For every platform
  for(int currentPlatform=0; currentPlatform<m_NumPlatforms; currentPlatform++)
  {
    getCenterPlatform(currentPlatform, &m_CenterX, &m_CenterY);	//geometric center coordinate of the platform
    getCornerPlatform(currentPlatform, 1, &m_X, &m_Y);			//corner coordinate  1
    double platformCorner1[2];
    platformCorner1[0] = m_X;
    platformCorner1[1] = m_Y;
    getCornerPlatform(currentPlatform, 2, &m_X, &m_Y);			//corner coordinate  2
    double platformCorner2[2];
    platformCorner2[0] = m_X;
    platformCorner2[1] = m_Y;
    getCornerPlatform(currentPlatform, 3, &m_X, &m_Y);			//corner coordinate  3
    double platformCorner3[2];
    platformCorner3[0] = m_X;
    platformCorner3[1] = m_Y;
    getCornerPlatform(currentPlatform, 4, &m_X, &m_Y);			//corner coordinate  4
    double platformCorner4[2];
    platformCorner4[0] = m_X;
    platformCorner4[1] = m_Y;

    double minX,maxX;
    double minY,maxY;
    minX = VTK_DOUBLE_MAX;
    maxX = VTK_DOUBLE_MIN;
    minY = VTK_DOUBLE_MAX;
    maxY = VTK_DOUBLE_MIN;

    minX = mafMin(mafMin(mafMin(mafMin(minX,platformCorner1[0]),platformCorner2[0]),platformCorner3[0]),platformCorner4[0]);
    maxX = mafMax(mafMax(mafMax(mafMax(maxX,platformCorner1[0]),platformCorner2[0]),platformCorner3[0]),platformCorner4[0]);
    minY = mafMin(mafMin(mafMin(mafMin(minY,platformCorner1[1]),platformCorner2[1]),platformCorner3[1]),platformCorner4[1]);
    maxY = mafMax(mafMax(mafMax(mafMax(maxY,platformCorner1[1]),platformCorner2[1]),platformCorner3[1]),platformCorner4[1]);

    vtkMAFSmartPointer<vtkCubeSource> cube;

    mafVMESurface *platform;
    mafNEW(platform);
    m_PlatformList.push_back(platform);
    mafString platformNumber;
    platformNumber << (currentPlatform + 1) ;
    mafString platformName;
    platformName.Append(m_FileName);
    platformName.Append("_FORCE_PLATFORM_");
    platformName.Append(platformNumber);
    m_PlatformList[currentPlatform]->SetName(platformName);

    double z = 0;
    double thickness = z - PLATFORM_THICKNESS;

    cube->SetBounds(minX,maxX,minY,maxY,thickness,z);
    
    //Create the mafVMESurface for the platforms
    m_PlatformList[currentPlatform]->SetData(cube->GetOutput(), 0);

    //force vector
    mafVMEVector *force;
    mafNEW(force);
    m_ForceList.push_back(force);
    mafString forceName;
    forceName.Append(m_FileName);
    forceName.Append("_GRF_");
    forceName.Append(platformNumber);
    m_ForceList[currentPlatform]->SetName(forceName);

    //moment vector
    mafVMEVector *moment;
    mafNEW(moment);
    m_MomentList.push_back(moment);
    mafString momentName;
    momentName.Append(m_FileName);
    momentName.Append("_MOMENT_");
    momentName.Append(platformNumber);
    m_MomentList[currentPlatform]->SetName(momentName);

    vtkMAFSmartPointer<vtkPolyData> vectorForce;
    vtkMAFSmartPointer<vtkPoints> pointsForce;
    vtkMAFSmartPointer<vtkCellArray> cellArrayForce;
    int pointIdForce[2] = {0,1};
    vectorForce->SetPoints(pointsForce);
    vectorForce->SetLines(cellArrayForce);

    vtkMAFSmartPointer<vtkPolyData> vectorMoment;
    vtkMAFSmartPointer<vtkPoints> pointsMoment;
    vtkMAFSmartPointer<vtkCellArray> cellArrayMoment;
    int pointIdMoment[2] = {0,1};
    vectorMoment->SetPoints(pointsMoment);
    vectorMoment->SetLines(cellArrayMoment);

    //For every sample
    int currentTime = 0;
    for(int currentSample=0; currentSample<m_NumSamples; currentSample++)
    {
      m_CopX=getCOPX(currentPlatform, currentSample);			//x coordinate of COP
      m_CopY=getCOPY(currentPlatform, currentSample);			//y coordinate of COP

      m_ForceX=getFx(currentPlatform, currentSample);				//x component of force
      m_ForceY=getFy(currentPlatform, currentSample);				//y component of force
      m_ForceZ=getFz(currentPlatform, currentSample);				//z component  of force

      m_MomentX=getMx(currentPlatform, currentSample);				//x component of moment
      m_MomentY=getMy(currentPlatform, currentSample);				//y component of moment
      m_MomentZ=getMz(currentPlatform, currentSample);				//z component of moment

      currentTime = currentSample * m_VectogramSampleFrequency;

      //force      
      pointsForce->Reset();
      pointsForce->InsertPoint(0, 0, 0, 0);
      pointsForce->InsertPoint(1, m_ForceX, m_ForceY, m_ForceZ);
      cellArrayForce->Reset();
      cellArrayForce->InsertNextCell(2, pointIdForce);
      vectorForce->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfVecForce;
      vtkMAFSmartPointer<vtkTransform> transfForce;

      transfForce->Translate(m_CopX, m_CopY, z); //z = 0
      transfVecForce->SetTransform(transfForce);
      transfVecForce->SetInput(vectorForce);
      transfVecForce->Update();

      
      m_ForceList[currentPlatform]->SetData(transfVecForce->GetOutput(), currentTime); //look here times

      m_ForceList[currentPlatform]->Modified();
      m_ForceList[currentPlatform]->Update();
      m_ForceList[currentPlatform]->GetOutput()->GetVTKData()->Update();

      //moment
      
      
      pointsMoment->Reset();
      pointsMoment->InsertPoint(0, 0, 0, 0);
      pointsMoment->InsertPoint(1, m_MomentX, m_MomentY, m_MomentZ);

      cellArrayMoment->Reset();
      cellArrayMoment->InsertNextCell(2, pointIdMoment);  
      vectorMoment->Update();

      vtkMAFSmartPointer<vtkTransformPolyDataFilter> transfVecMoment;
      vtkMAFSmartPointer<vtkTransform> transfMoment;

      transfMoment->Translate(m_CopX, m_CopY, z); //z = 0
      transfVecMoment->SetTransform(transfMoment);
      transfVecMoment->SetInput(vectorMoment);
      transfVecMoment->Update();


      m_MomentList[currentPlatform]->SetData(transfVecMoment->GetOutput(), currentTime); //look here times

      m_MomentList[currentPlatform]->Modified();
      m_MomentList[currentPlatform]->Update();
      m_MomentList[currentPlatform]->GetOutput()->GetVTKData()->Update();

      progress = (currentSample + 1 + (currentPlatform * m_NumSamples )) * 100 / (m_NumSamples * m_NumPlatforms);
      mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,progress));
    }
  }
  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

  if(!m_TestMode)
  {
    delete wait;
  }
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::ImportEvent()
//----------------------------------------------------------------------------
{
  //For every event
  for(int currentEvent=0; currentEvent<m_NumEvents; currentEvent++)
  {
    m_EventContext=getContextEvent(currentEvent);		//event context
    m_EventValue=getValueEvent(currentEvent);				      //event value in seconds
  }
}
//----------------------------------------------------------------------------
// Operation constants
//----------------------------------------------------------------------------
enum C3D_IMPORTER_ID
{
  ID_FIRST = MINID,
  ID_IMPORT_TRAJECTORIES,
  ID_IMPORT_ANALOG,
  ID_IMPORT_PLATFORM,
  ID_IMPORT_EVENT,
  ID_OK,
  ID_CANCEL,
};
//----------------------------------------------------------------------------
void lhpOpImporterC3D::CreateGui()
//----------------------------------------------------------------------------
{
  mafString wildcard = "c3d files (*.c3d)|*.c3d";

  //int result = OP_RUN_CANCEL;
  m_C3DInputFileNameFullPath = "";
  
  wxString f;
  f = mafGetOpenFile(m_FileDir,wildcard).c_str(); 
  if(!f.IsEmpty() && wxFileExists(f))
  {
    m_C3DInputFileNameFullPath = f;
    //result = OP_RUN_OK;
  }
  //mafEventMacro(mafEvent(this,result));
	m_Gui = new mmgGui(this);
	m_Gui->Label("Select:", true);

  m_Gui->Bool(ID_IMPORT_TRAJECTORIES,_("Trajectories"),&m_ImportTrajectoriesFlag,1);
  m_Gui->Bool(ID_IMPORT_ANALOG,_("Analog Data"),&m_ImportAnalogFlag,1);
  m_Gui->Bool(ID_IMPORT_PLATFORM,_("Force Plate Data"),&m_ImportPlatformFlag,1);
  //m_Gui->Bool(ID_IMPORT_EVENT,_("Auto Crop"),&m_ImportEventFlag,1);

	m_Gui->OkCancel();
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case wxOK:
      {
				if(Import())
				{
					this->OpStop(OP_RUN_OK);
				}
				else
				{
					this->OpStop(OP_RUN_CANCEL);
				}
        
      }
      break;
      case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}

//----------------------------------------------------------------------------
void lhpOpImporterC3D::OpDo()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Please wait, create all VMEs in tree");
  
  //trajectories -> landmark cloud (with landmarks) vme
  if(m_ImportTrajectoriesFlag) m_VmeCloud->ReparentTo(m_Input);

  //analog -> analog vme
  if(m_ImportAnalogFlag) m_VmeAnalog->ReparentTo(m_Input);

  //GRF -> platform , vectors (force, moment) vme
  if(m_ImportPlatformFlag)
  {
    for(int currentPlatform = 0; currentPlatform<m_PlatformList.size(); currentPlatform++)
    {
      m_PlatformList[currentPlatform]->ReparentTo(m_Input);
      m_ForceList[currentPlatform]->ReparentTo(m_PlatformList[currentPlatform]);
      m_MomentList[currentPlatform]->ReparentTo(m_PlatformList[currentPlatform]);
    }
  }
  
  //if(m_ImportEventFlag);
}
//----------------------------------------------------------------------------
void lhpOpImporterC3D::OpUndo()
//----------------------------------------------------------------------------
{   
  if(m_VmeCloud != NULL)
    mafEventMacro(mafEvent(this,VME_REMOVE,m_VmeCloud));

  if(m_VmeAnalog != NULL)
    mafEventMacro(mafEvent(this,VME_REMOVE,m_VmeAnalog));

  for(int currentPlatform = 0; currentPlatform<m_PlatformList.size(); currentPlatform++)
  {
    if(m_PlatformList[currentPlatform] != NULL)
      mafEventMacro(mafEvent(this,VME_REMOVE,m_PlatformList[currentPlatform]));

    if(m_ForceList[currentPlatform] != NULL)
      mafEventMacro(mafEvent(this,VME_REMOVE,m_ForceList[currentPlatform]));

    if(m_MomentList[currentPlatform] != NULL)
      mafEventMacro(mafEvent(this,VME_REMOVE,m_MomentList[currentPlatform]));
  }
  
}