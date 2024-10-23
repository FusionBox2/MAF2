/*=========================================================================

 Program: MAF2Medical
 Module: medVMESegmentationVolume
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
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

#include "medVMESegmentationVolume.h"
#include "mafGUI.h"
#include "mmaVolumeMaterial.h"

#include "mafTransform.h"
#include "ftk/IO/StorageElement.h"
#include "medDataPipeCustomSegmentationVolume.h"
#include "mafVMEOutputVolume.h"
#include "mafMatrix.h"
#include "medAttributeSegmentationVolume.h"


#include "vtkSmartPointer.h"

//-------------------------------------------------------------------------
mafCxxTypeMacro(medVMESegmentationVolume)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
medVMESegmentationVolume::medVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  m_Transform = mafTransform::NewSPtr();
  mafVMEOutputVolume *output = mafVMEOutputVolume::New(); // an output with no data
  output->SetTransform(m_Transform); // force my transform in the output
  SetOutput(output);

  //DependsOnLinkedNodeOn();

  // attach a data pipe which creates a bridge between VTK and MAF
  m_SegmentingDataPipe = medDataPipeCustomSegmentationVolume::NewSPtr();
  SetDataPipe(m_SegmentingDataPipe);
}

//-------------------------------------------------------------------------
medVMESegmentationVolume::~medVMESegmentationVolume()
//-------------------------------------------------------------------------
{
  // these links are children, thus it's not our responsibility to
  // destroy them, it's part of the vtkTree one's
  SetOutput(NULL);
}

//-------------------------------------------------------------------------
std::shared_ptr<mmaVolumeMaterial> medVMESegmentationVolume::GetMaterial()
//-------------------------------------------------------------------------
{
  auto material = mmaVolumeMaterial::SafeDownCast(GetAttribute(_R("VolumeMaterialAttributes")));
  if (!material)
  {
    material = mmaVolumeMaterial::NewSPtr();
    SetAttribute(_R("VolumeMaterialAttributes"), material);
  }
  return material;
}
//-------------------------------------------------------------------------
std::shared_ptr<medAttributeSegmentationVolume> medVMESegmentationVolume::GetVolumeAttribute()
//-------------------------------------------------------------------------
{
  auto volumeAttribute = medAttributeSegmentationVolume::SafeDownCast(this->GetAttribute(_R("SegmentationVolumeData")));
  if (!volumeAttribute)
  {
    volumeAttribute = medAttributeSegmentationVolume::NewSPtr();
    this->SetAttribute(_R("SegmentationVolumeData"),volumeAttribute);
  }
  return volumeAttribute;

}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::InternalInitialize()
//-------------------------------------------------------------------------
{
  if (Superclass::InternalInitialize()==MAF_OK)
  {
    // force material allocation
    GetMaterial();
    
    auto volumeAttribute = GetVolumeAttribute();
    
    // attach a data pipe which creates a bridge between VTK and MAF
    m_SegmentingDataPipe->SetVolume(this->GetVolumeLink());
    m_SegmentingDataPipe->SetManualVolumeMask(this->GetManualVolumeMask());
    m_SegmentingDataPipe->SetDependOnVMETime(false);
    m_SegmentingDataPipe->SetAutomaticSegmentationThresholdModality(volumeAttribute->GetAutomaticSegmentationThresholdModality());
    m_SegmentingDataPipe->SetDoubleThresholdModality(volumeAttribute->GetDoubleThresholdModality());
    m_SegmentingDataPipe->SetAutomaticSegmentationGlobalThreshold(volumeAttribute->GetAutomaticSegmentationGlobalThreshold());
    for (int i=0;i<volumeAttribute->GetNumberOfRanges();i++)
    {
      int startSlice,endSlice;
      double threshold;
      volumeAttribute->GetRange(i,startSlice,endSlice,threshold);
      m_SegmentingDataPipe->AddRange(startSlice,endSlice,threshold);
    }

    return MAF_OK;
  }
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::InternalPreUpdate()
//-----------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  if (m_SegmentingDataPipe->GetVolume() != vol)
  {
    m_SegmentingDataPipe->SetVolume(vol);
  }

  if (m_Gui)
  {
    m_Gui->Update();
  }
}

//-------------------------------------------------------------------------
int medVMESegmentationVolume::DeepCopy(mafNode *a)
//-------------------------------------------------------------------------
{ 
  if (Superclass::DeepCopy(a)==MAF_OK)
  {
    ((medVMESegmentationVolume *)a)->SetVolumeLink(this->GetVolumeLink());
    ((medVMESegmentationVolume *)a)->SetManualVolumeMask(this->GetManualVolumeMask());
    return MAF_OK;
  }  
  return MAF_ERROR;
}
//-------------------------------------------------------------------------
bool medVMESegmentationVolume::Equals(mafVME *vme)
//-------------------------------------------------------------------------
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    ret = true;
    if (((medVMESegmentationVolume *)vme)->GetVolumeLink() != this->GetVolumeLink())
    {
      ret = false;
    }
    if (((medVMESegmentationVolume *)vme)->GetManualVolumeMask() != this->GetManualVolumeMask())
    {
      ret = false;
    }
    
  }
  return ret;
}

//-------------------------------------------------------------------------
void medVMESegmentationVolume::SetMatrix(const mafMatrix &mat)
//-------------------------------------------------------------------------
{
  m_Transform->SetMatrix(mat);
  Modified();
}

//-------------------------------------------------------------------------
void medVMESegmentationVolume::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
//-------------------------------------------------------------------------
{
  kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
bool medVMESegmentationVolume::IsAnimated()
//-------------------------------------------------------------------------
{
  return false;
}

//-------------------------------------------------------------------------
bool medVMESegmentationVolume::IsDataAvailable()
//-------------------------------------------------------------------------
{
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  return (vol && vol->IsDataAvailable());
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);
  parent[_R("Transform")].SetValue(m_Transform->GetMatrix());
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::InternalRestore(const mafStorageElement& node)
//-----------------------------------------------------------------------
{
  Superclass::InternalRestore(node);
  m_Transform->SetMatrix(node[_R("Transform")].As<mafMatrix>());
}
//-------------------------------------------------------------------------
mafGUI* medVMESegmentationVolume::CreateGui()
//-------------------------------------------------------------------------
{
  m_Gui = mafNode::CreateGui(); // Called to show info about vmes' type and name
  m_Gui->Divider();
  mafVME *vol = mafVME::SafeDownCast(GetVolumeLink());
  m_VolumeName = vol ? vol->GetName() : _L("none");
  m_Gui->Button(ID_VOLUME_LINK,&m_VolumeName,_L("Volume"), _L("Select the volume to be segmented"));

  return m_Gui;
}
//-------------------------------------------------------------------------
void medVMESegmentationVolume::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
  // events to be sent up or down in the tree are simply forwarded
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_VOLUME_LINK:
      {
        mafString title = _L("Choose volume vme");
        e->SetId(VME_CHOOSE);
        e->SetArg((intptr_t)&medVMESegmentationVolume::VolumeAccept);
        e->SetString(&title);
        ForwardUpEvent(e);
        mafNode *n = e->GetVme();
        if (n != NULL)
        {
          SetVolumeLink(n);
          m_VolumeName = n->GetName();
          m_Gui->Update();
        }
      }
      break;
    default:
      mafNode::OnEvent(maf_event);
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::AddRange(int startSlice,int endSlice,double threshold, double upperThreshold)
//-------------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->AddRange(startSlice,endSlice,threshold, upperThreshold);
  if (result == MAF_OK)
  {
    result = GetVolumeAttribute()->AddRange(startSlice, endSlice, threshold, upperThreshold);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }
  
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold, double &upperThreshold)
//-----------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetRange(index, startSlice, endSlice, threshold, upperThreshold);
}

//-----------------------------------------------------------------------
int medVMESegmentationVolume::GetRange(int index,int &startSlice, int &endSlice, double &threshold)
//-----------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetRange(index, startSlice, endSlice, threshold);
}


//-----------------------------------------------------------------------
bool medVMESegmentationVolume::CheckNumberOfThresholds()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->CheckNumberOfThresholds();
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::UpdateRange(int index,int startSlice, int endSlice, double threshold, double upperThershold)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->UpdateRange(index,startSlice,endSlice,threshold, upperThershold);
  if (result == MAF_OK)
  {
    result = GetVolumeAttribute()->UpdateRange(index, startSlice, endSlice, threshold, upperThershold);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;  
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::RemoveAllRanges()
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->RemoveAllRanges();
  if (result == MAF_OK)
  {
    result = GetVolumeAttribute()->RemoveAllRanges();
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::RemoveAllSeeds()
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->RemoveAllSeeds();
  if (result == MAF_OK)
  {
    result = GetVolumeAttribute()->RemoveAllSeeds();
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::DeleteSeed(int index)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->DeleteSeed(index);
  if (result == MAF_OK)
  {
    result = GetVolumeAttribute()->DeleteSeed(index);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::DeleteRange(int index)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->DeleteRange(index);
  if (result == MAF_OK)
  {
    result = GetVolumeAttribute()->DeleteRange(index);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }
  
  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::SetVolumeLink(mafNode *volume)
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->SetVolume(volume);

  if (result == MAF_ERROR)
  {
    return MAF_ERROR;
  }

  SetLink(_R("Volume"), volume);
  Modified();

  return MAF_OK;
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetManualVolumeMask(mafNode *volume)
//-----------------------------------------------------------------------
{
  SetLink(_R("ManualVolumeMask"), volume);
  m_SegmentingDataPipe->SetManualVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
mafNode *medVMESegmentationVolume::GetManualVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink(_R("ManualVolumeMask"));
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetRefinementVolumeMask(mafNode *volume)
//-----------------------------------------------------------------------
{
  SetLink(_R("RefinementVolumeMask"), volume);
  m_SegmentingDataPipe->SetRefinementVolumeMask(volume);
  Modified();
}
//-----------------------------------------------------------------------
mafNode *medVMESegmentationVolume::GetRefinementVolumeMask()
//-----------------------------------------------------------------------
{
  return GetLink(_R("RefinementVolumeMask"));
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetAutomaticOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetAutomaticOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetRegionGrowingOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetRefinementOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRefinementOutput();
}
//-----------------------------------------------------------------------
vtkDataSet *medVMESegmentationVolume::GetManualOutput()
//-----------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetManualOutput();
}
//-----------------------------------------------------------------------
mafNode *medVMESegmentationVolume::GetVolumeLink()
//-----------------------------------------------------------------------
{
  return GetLink(_R("Volume"));
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::AddSeed(int seed[3])
//-----------------------------------------------------------------------
{
  int result = m_SegmentingDataPipe->AddSeed(seed);
  if (result == MAF_OK)
  {
    result = GetVolumeAttribute()->AddSeed(seed);
    if (result == MAF_OK)
    {
      Modified();
      return MAF_OK;
    }
  }

  return MAF_ERROR;
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::GetSeed(int index,int seed[3])
//-----------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetSeed(index,seed);
}
//-------------------------------------------------------------------------
double medVMESegmentationVolume::GetRegionGrowingLowerThreshold()
//-------------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetRegionGrowingLowerThreshold();
}
//-------------------------------------------------------------------------
double medVMESegmentationVolume::GetRegionGrowingUpperThreshold()
//-------------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetRegionGrowingUpperThreshold();
}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::GetNumberOfRanges()
//-------------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetNumberOfRanges();
}
//-------------------------------------------------------------------------
int medVMESegmentationVolume::GetNumberOfSeeds()
//-------------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetNumberOfSeeds();
}
//-------------------------------------------------------------------------
void medVMESegmentationVolume::SetRegionGrowingLowerThreshold(double value)
//-------------------------------------------------------------------------
{
  if (GetVolumeAttribute() && m_SegmentingDataPipe)
  {
    GetVolumeAttribute()->SetRegionGrowingLowerThreshold(value);
	  m_SegmentingDataPipe->SetRegionGrowingLowerThreshold(value);
  }
}
//-------------------------------------------------------------------------
void medVMESegmentationVolume::SetRegionGrowingUpperThreshold(double value)
//-------------------------------------------------------------------------
{
  if (GetVolumeAttribute() && m_SegmentingDataPipe)
  {
    GetVolumeAttribute()->SetRegionGrowingUpperThreshold(value);
    m_SegmentingDataPipe->SetRegionGrowingUpperThreshold(value);
  }
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetAutomaticSegmentationThresholdModality(int modality)
//-----------------------------------------------------------------------
{
  GetVolumeAttribute()->SetAutomaticSegmentationThresholdModality(modality);
  m_SegmentingDataPipe->SetAutomaticSegmentationThresholdModality(modality);
  Modified();
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::GetAutomaticSegmentationThresholdModality()
//-----------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetAutomaticSegmentationThresholdModality();
}

//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetDoubleThresholdModality(int modality)
//-----------------------------------------------------------------------
{
  GetVolumeAttribute()->SetDoubleThresholdModality(modality);
  m_SegmentingDataPipe->SetDoubleThresholdModality(modality);
  Modified();
}
//-----------------------------------------------------------------------
int medVMESegmentationVolume::GetDoubleThresholdModality()
//-----------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetDoubleThresholdModality();
}
//-----------------------------------------------------------------------
void medVMESegmentationVolume::SetAutomaticSegmentationGlobalThreshold(double lowerThreshold, double uppperThreshold)
//-----------------------------------------------------------------------
{
  GetVolumeAttribute()->SetAutomaticSegmentationGlobalThreshold(lowerThreshold,uppperThreshold);
  m_SegmentingDataPipe->SetAutomaticSegmentationGlobalThreshold(lowerThreshold,uppperThreshold);
  Modified();
}
//-----------------------------------------------------------------------
double medVMESegmentationVolume::GetAutomaticSegmentationGlobalThreshold()
//-----------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetAutomaticSegmentationGlobalThreshold();
}

//-----------------------------------------------------------------------
double medVMESegmentationVolume::GetAutomaticSegmentationGlobalUpperThreshold()
//-----------------------------------------------------------------------
{
  return GetVolumeAttribute()->GetAutomaticSegmentationGlobalUpperThreshold();
}

//------------------------------------------------------------------------
void medVMESegmentationVolume::SetRegionGrowingSliceRange(int startSlice, int endSlice)
//------------------------------------------------------------------------
{
  m_SegmentingDataPipe->SetRegionGrowingSliceRange(startSlice, endSlice);
}

//------------------------------------------------------------------------
int medVMESegmentationVolume::GetRegionGrowingSliceRange(int &startSlice, int &endSlice)
//------------------------------------------------------------------------
{
  return m_SegmentingDataPipe->GetRegionGrowingSliceRange(startSlice, endSlice);
}

//-------------------------------------------------------------------------
mafVMEOutput *medVMESegmentationVolume::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputVolume::New()); // create the output
  }
  return m_Output;
}
//-------------------------------------------------------------------------
char** medVMESegmentationVolume::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMEVolume.xpm"
  return mafVMEVolume_xpm;
}
