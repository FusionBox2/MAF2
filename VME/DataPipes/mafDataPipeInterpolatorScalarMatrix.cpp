#include "mafDataPipeInterpolatorScalarMatrix.h"

#include "mafVME.h"
#include "mafVMEItemScalarMatrix.h"
#include "mafVMEScalarMatrix.h"

mafDataPipeInterpolatorScalarMatrix::mafDataPipeInterpolatorScalarMatrix() = default;

//------------------------------------------------------------------------------
mafDataPipeInterpolatorScalarMatrix::~mafDataPipeInterpolatorScalarMatrix() = default;

//------------------------------------------------------------------------------
bool mafDataPipeInterpolatorScalarMatrix::Accept(mafVME *vme)
//------------------------------------------------------------------------------
{
  return Superclass::Accept(vme) && vme->IsA(mafVMEScalarMatrix::GetStaticTypeId());
}

//------------------------------------------------------------------------------
vnl_matrix<double> &mafDataPipeInterpolatorScalarMatrix::GetScalarData()
//------------------------------------------------------------------------------
{
  OnPreUpdate();
  return m_ScalarData;
}

//------------------------------------------------------------------------------
void mafDataPipeInterpolatorScalarMatrix::Execute()
//------------------------------------------------------------------------------
{
  Superclass::Execute();

  auto mtime = this->GetMTime();

  // if the current item is changed set the data inside new item as input for the interpolator
  // more specialized interpolators could redefine this to have more inputs (e.g. when 
  // interpolating different items)
  if ( m_CurrentItem && (m_CurrentItem != m_OldItem || \
    mtime > m_UpdateTime.GetMTime() ||
    !m_CurrentItem->IsDataPresent() ))
  {
    vnl_matrix<double> scalar = GetCurrentItemScalarMatrix()->GetData();
    if (scalar.size() != 0)
    {
      m_ScalarData = GetCurrentItemScalarMatrix()->GetData();
      m_UpdateTime.Modified();
    }
  } 
}
