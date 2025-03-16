#include "mafDataPipeCustomProber.h"

#include "mafEventBase.h"
#include "mafVME.h"

#include "mafTransformFrame.h"
#include "mafAbsMatrixPipe.h"
#include "vtkDataSet.h"
#include "vtkPolyDataNormals.h"
#include "vtkMAFDistanceFilter.h"

//------------------------------------------------------------------------------
mafDataPipeCustomProber::mafDataPipeCustomProber()
//------------------------------------------------------------------------------
{
  m_Surface = NULL;
  m_Volume  = NULL;

  m_DistThreshold   = -1.0;
  m_MaxDistance     = -1.0;
  m_DistanceModeType= -1;

  m_ProberMode  = -1;
  m_HighDensity = -1.0;
  m_LowDensity  = -1.0;

  vtkNEW(m_Normals);
  vtkNEW(m_Prober);
  m_Prober->SetInputConnection(m_Normals->GetOutputPort());
  SetInputConnection(m_Prober->GetOutputPort());
}

//------------------------------------------------------------------------------
mafDataPipeCustomProber::~mafDataPipeCustomProber()
//------------------------------------------------------------------------------
{
  vtkDEL(m_Normals);
  vtkDEL(m_Prober);
}
//------------------------------------------------------------------------------
void mafDataPipeCustomProber::SetSurface(mafNode *surface)
//------------------------------------------------------------------------------
{
  m_Surface = surface;
  if (m_Surface)
  {
    m_Normals->SetInputConnection(((mafVME*)m_Surface)->GetOutput()->GetVTKOutputPort());
  }
  else
  {
    m_Normals->SetInputConnection(nullptr);
  }
}
//------------------------------------------------------------------------------
void mafDataPipeCustomProber::SetVolume(mafNode *volume)
//------------------------------------------------------------------------------
{
  m_Volume = volume;
  if (m_Volume)
  {
    m_Prober->SetSourceConnection(((mafVME*)m_Volume)->GetOutput()->GetVTKOutputPort());
  }
  else
  {
    m_Prober->SetSourceConnection(nullptr);
  }
}
//------------------------------------------------------------------------------
int mafDataPipeCustomProber::DeepCopy(mafDataPipe *pipe)
//------------------------------------------------------------------------------
{
  if (mafDataPipe::DeepCopy(pipe) == MAF_OK)
  {
    mafDataPipeCustomProber *dpcp = mafDataPipeCustomProber::SafeDownCast(pipe);
    if (dpcp != NULL)
    {
      m_Surface = dpcp->GetSurface();
      m_Volume = dpcp->GetVolume();
      m_DistThreshold = dpcp->GetDistanceThreshold();
      m_MaxDistance = dpcp->GetMaxDistance();
      m_DistanceModeType = dpcp->GetDistanceMode();
      m_ProberMode = dpcp->GetMode();
      m_HighDensity = dpcp->GetHighDensity();
      m_LowDensity = dpcp->GetLowDensity();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//------------------------------------------------------------------------------
void mafDataPipeCustomProber::Execute()
//------------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(m_Volume);
  mafVME *surf = mafVME::SafeDownCast(m_Surface);

  if(vol && surf)
  {
    vol->GetOutput()->Update();
    surf->GetOutput()->Update();
    vtkDataSet *vol_data = vol->GetOutput()->GetVTKData();
    vtkDataSet *surf_data = surf->GetOutput()->GetVTKData();
    vtkAlgorithmOutput* vol_port = vol->GetOutput()->GetVTKOutputPort();
    vtkAlgorithmOutput* surf_port = surf->GetOutput()->GetVTKOutputPort();
    if(vol_data && surf_data)
    {
      m_Normals->SetInputConnection(surf_port);
      m_Normals->ComputePointNormalsOn();
      m_Normals->SplittingOff();
      m_Normals->Update();

      m_Prober->SetSourceConnection(vol_port);
      if(GetMode() == mafDataPipeCustomProber::DENSITY_MODE)
        m_Prober->SetFilterModeToDensity();
      else
      {
        m_Prober->SetFilterModeToDistance();
        m_Prober->SetThreshold(GetDistanceThreshold());
        m_Prober->SetMaxDistance(GetMaxDistance());
        if(GetDistanceMode() == mafDataPipeCustomProber::DISTANCE_MODE_SCALAR)
          m_Prober->SetDistanceModeToScalar();
        else
          m_Prober->SetDistanceModeToVector();
      }

      auto maps_to_volume = mafTransformFrame::NewSPtr();
      maps_to_volume->SetInput(*m_VME->GetAbsMatrixPipe()->GetMatrixPointer());
      maps_to_volume->SetTargetFrame(*vol->GetAbsMatrixPipe()->GetMatrixPointer());

      mafMatrix tmp_matrix = maps_to_volume->GetMatrix();

      m_Prober->SetInputMatrix(tmp_matrix.GetVTKMatrix());
    }
  }
  else
  {
    m_Prober->SetSourceConnection(nullptr);
  }
}
//-----------------------------------------------------------------------
void mafDataPipeCustomProber::SetModeToDensity()
//-----------------------------------------------------------------------
{
  this->SetMode(mafDataPipeCustomProber::DENSITY_MODE);
  Modified();
}
//-----------------------------------------------------------------------
void mafDataPipeCustomProber::SetModeToDistance()
//-----------------------------------------------------------------------
{
  this->SetMode(mafDataPipeCustomProber::DISTANCE_MODE);
  Modified();
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != mafDataPipeCustomProber::DENSITY_MODE && mode != mafDataPipeCustomProber::DISTANCE_MODE)
  {
    mafErrorMacro("trying to set the map mode to invalid type: allowed types are DENSITY_MODE & DISTANCE_MODE");
    return;
  }

  m_ProberMode = mode;
  this->Modified();
}
//-------------------------------------------------------------------------
int mafDataPipeCustomProber::GetMode()
//-------------------------------------------------------------------------
{
  return m_ProberMode;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetDistanceThreshold(float thr)
//-------------------------------------------------------------------------
{
  m_DistThreshold = thr;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetDistanceThreshold()
//-------------------------------------------------------------------------
{
  return m_DistThreshold;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetMaxDistance(float max_dist)
//-------------------------------------------------------------------------
{
  if (max_dist < 0)
  {
    mafErrorMacro("Bad max distance value.");
    return;
  }

  m_MaxDistance = max_dist;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetMaxDistance()
//-------------------------------------------------------------------------
{
  return m_MaxDistance;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetDistanceMode(int mode)
//-------------------------------------------------------------------------
{
  if (mode != mafDataPipeCustomProber::DISTANCE_MODE_SCALAR && mode != mafDataPipeCustomProber::DISTANCE_MODE_VECTOR)
  {
    mafErrorMacro("trying to set the map distance mode to invalid type: allowed types are DISTANCE_MODE_SCALAR & DISTANCE_MODE_VECTOR");
    return;
  }

  m_DistanceModeType = mode;
  this->Modified();
}
//-------------------------------------------------------------------------
int mafDataPipeCustomProber::GetDistanceMode()
//-------------------------------------------------------------------------
{
  return m_DistanceModeType;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetHighDensity(float high_dens)
//-------------------------------------------------------------------------
{
  m_HighDensity = high_dens;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetHighDensity()
//-------------------------------------------------------------------------
{
  return m_HighDensity;
}
//-------------------------------------------------------------------------
void mafDataPipeCustomProber::SetLowDensity(float low_dens)
//-------------------------------------------------------------------------
{
  m_LowDensity = low_dens;
  this->Modified();
}
//-------------------------------------------------------------------------
float mafDataPipeCustomProber::GetLowDensity()
//-------------------------------------------------------------------------
{
  return m_LowDensity;
}
