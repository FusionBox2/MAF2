#include "mafVMEScalar.h"
#include "mafGUI.h"

#include "mmuTimeSet.h"
#include "ftk/IO/StorageElement.h"
#include "mafVME.h"
#include "mafVMEOutputScalar.h"

//#include "mafDataPipeCustom.h"
#include "mafScalarVector.h"

mafVMEScalar::mafVMEScalar()
{
  m_ScalarVector = std::make_unique<mafScalarVector>();
  SetDataPipe(nullptr);
}

mafVMEScalar::~mafVMEScalar() = default;

int mafVMEScalar::DeepCopy(mafNode *a)
{
  if (Superclass::DeepCopy(a) == MAF_OK)
  {
    auto scalar = mafVMEScalar::SafeDownCast(a);
    if (scalar->GetScalarVector())
    {
      m_ScalarVector->DeepCopy(scalar->GetScalarVector()); // copy data
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}

bool mafVMEScalar::Equals(mafVME *vme)
{
  bool ret = false;
  if (Superclass::Equals(vme))
  {
    auto scalar = mafVMEScalar::SafeDownCast(vme);
    if (m_ScalarVector)
    {
      ret = m_ScalarVector->Equals(scalar->GetScalarVector());
    }
    else
      ret = true;
  }
  return ret;
}

bool mafVMEScalar::IsAnimated()
{
  bool anim = Superclass::IsAnimated();
  return anim || (m_ScalarVector->GetNumberOfItems() > 1);
}

mafVMEOutput *mafVMEScalar::GetOutput()
{
  // allocate the right type of output on demand
  if (!m_Output)
  {
    SetOutput(mafVMEOutputScalar::NewUPtr()); // create the output
  }
  return m_Output.get();
}

int mafVMEScalar::SetData(double data, mafTimeStamp t)
{
  m_ScalarVector->SetScalar(t, data);

  return MAF_OK;
}

void mafVMEScalar::GetDataTimeStamps(std::vector<mafTimeStamp> &kframes)
{
  if (m_ScalarVector)
  {
    m_ScalarVector->GetTimeStamps(kframes);
  }
}

void mafVMEScalar::GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes)
{
  std::vector<mafTimeStamp> datatimestamps;
  std::vector<mafTimeStamp> matrixtimestamps;

  if (m_ScalarVector)
  {
    m_ScalarVector->GetTimeStamps(datatimestamps);
  }
  Superclass::GetLocalTimeStamps(matrixtimestamps);

  kframes = mmuTimeSet::Merge(datatimestamps,matrixtimestamps);
}

void mafVMEScalar::GetLocalTimeBounds(mafTimeStamp tbounds[2])
{
  Superclass::GetLocalTimeBounds(tbounds);

  mafTimeStamp tmp[2];
  m_ScalarVector->GetTimeBounds(tmp);

  tmp[0] = tmp[0] < 0 ? 0 : tmp[0];
  tmp[1] = tmp[1] < 0 ? 0 : tmp[1];

  if (tmp[0]<tbounds[0]||tbounds[0]<0)
    tbounds[0]=tmp[0];

  if (tmp[1]>tbounds[1]||tbounds[1]<0)
    tbounds[1]=tmp[1];
}

void mafVMEScalar::InternalStore(mafStorageElementBuilder& parent)
{  
  Superclass::InternalStore(parent);
  if (m_ScalarVector)
  {
    {auto scalVec = parent[_R("ScalarVector")]; m_ScalarVector->Store(scalVec);}
  }
}

void mafVMEScalar::InternalRestore(const mafStorageElement& node)
{
  Superclass::InternalRestore(node);
  if (m_ScalarVector)
  {
    m_ScalarVector->Restore(node[_R("ScalarVector")]);
  }
}

void mafVMEScalar::InternalPreUpdate()
{
#ifdef MAF_USE_VTK
  GetScalarOutput()->UpdateVTKRepresentation();
#endif
}

void mafVMEScalar::SetTimeStamp(mafTimeStamp t)
{
  t = t < 0 ? 0 : t;
  bool update_vtk_data = t != m_CurrentTime;
  if (update_vtk_data)
  {
    Superclass::SetTimeStamp(t);

#ifdef MAF_USE_VTK
    GetScalarOutput()->UpdateVTKRepresentation();
#endif
  }
}

void mafVMEScalar::Print(std::ostream& os, const int tabs)
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);
}

const char** mafVMEScalar::GetIcon() 
{
  #include "mafVMEScalar.xpm"
  return mafVMEScalar_xpm;
}
