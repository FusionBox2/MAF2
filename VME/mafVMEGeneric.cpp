#include "mafVMEGeneric.h"
#include "mafDataVector.h"
#include "mafDataPipeInterpolatorVTK.h"
#include "mafVMEItemVTK.h"
#include "vtkDataSet.h"
#include "mafEvent.h"
//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEGeneric)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEGeneric::mafVMEGeneric()
//-------------------------------------------------------------------------
{
  m_DataVector = mafDataVector::NewUPtr();
  m_DataVector->SetItemTypeName(mafVMEItemVTK::GetStaticTypeName());  
  m_DataVector->SetListener(this);
  SetDataPipe(mafDataPipeInterpolatorVTK::NewSPtr()); // interpolator data pipe

  // The output is created on demand in GetOutput() to avoid
  // subclasses to have to destroy base class output
}

//-------------------------------------------------------------------------
mafVMEGeneric::~mafVMEGeneric()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGenericAbstract
}

//-------------------------------------------------------------------------
mafVMEOutput *mafVMEGeneric::GetOutput()
//-------------------------------------------------------------------------
{
  // allocate the right type of output on demand
  if (m_Output==NULL)
  {
    SetOutput(mafVMEOutputVTK::New()); // create the output
  }
  return m_Output;
}

//-------------------------------------------------------------------------
int mafVMEGeneric::SetData(vtkDataSet *data, mafTimeStamp t, int mode)
//-------------------------------------------------------------------------
{
  assert(data);
  auto item = mafVMEItemVTK::NewSPtr();
  
  switch (mode)
  {
  case MAF_VME_COPY_DATA:
  {
    // update and make a copy of the VTK dataset
    //data->Update();
    vtkDataSet *new_data=data->NewInstance();
    new_data->DeepCopy(data);
    item->SetData(new_data);
    new_data->Delete();
  }
  break;
  case MAF_VME_REFERENCE_DATA:
  case MAF_VME_DETACH_DATA:  
    // simply reference the VTK dataset (keep it attached to the source) 
    item->SetData(data);
  break;
  }

  item->SetTimeStamp(t);
  GetDataVector()->InsertItem(item);

  Modified();
  if(m_EnableModifiedEvent)
  {
    {mafEvent evUnq(this, VME_MODIFIED, this); ForwardUpEvent(&evUnq);}
  }

  return MAF_OK;
}

//-------------------------------------------------------------------------
const char** mafVMEGeneric::GetIcon() 
//-------------------------------------------------------------------------
{
  #include "mafVMEGeneric.xpm"
  return mafVMEGeneric_xpm;
}
