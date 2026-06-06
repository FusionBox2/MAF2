#pragma once

#include "mafDataPipe.h"
//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVMEItem;

/** superclass for computing the VME output data out of the internal VME-Items.
  mafDataPipeInterpolator is a data pipe producing as output a dataset interpolated
  against the ones stored in the array associated to the VME itself (only for VMEs
  with data array like mafVMEGeneric). The class also provides the mechanism to 
  obtain the 3D bounds for any time.
  @sa mflDataPipe mafVMEGeneric
*/
class MAF_EXPORT mafDataPipeInterpolator : public mafDataPipe
{
public:
  mafAbstractTypeMacroN(mafDataPipeInterpolator);

  /** This DataPipe accepts only VME's with internal DataArray. */
  bool Accept(mafVME *vme) override;

  void OnUpdate() override;
  void OnPreUpdate() override;

  /**
    This function makes the current bounds to be updated. It's optimized
    to not require data updating, so that data bounds can be evaluated 
    without loading all the data (dataset bounds are replicated in the
    VME item data structure)*/
  void UpdateBounds() override;

  /**
  Get the output of the interpolator item*/
  std::shared_ptr<mafVMEItem> GetCurrentItem() {return m_CurrentItem;}

  /**
  Set the current time. Overidden to allow the output not to change whem 
  not necessary, e.g. constant data (in time) being interpolated should not
  produce a change in the output data when time is changed.*/
  virtual void SetTimeStamp(mafTimeStamp time);

  /**
  Get the MTime: this is the bit of magic that makes everything work.*/
  MTimeType GetMTime() override;

protected:
  mafDataPipeInterpolator();
  mafDataPipeInterpolator(const mafDataPipeInterpolator&) = delete;
  mafDataPipeInterpolator(mafDataPipeInterpolator&&) = delete;
  mafDataPipeInterpolator& operator=(const mafDataPipeInterpolator&) = delete;
  mafDataPipeInterpolator& operator=(mafDataPipeInterpolator&&) = delete;
  ~mafDataPipeInterpolator() override;

  void Execute() override =0;

  /** This function is called internally to updated the output data according
  interpolator rules. It should be reimplemented in sub-classes.*/
  virtual void InternalItemUpdate();

  /** Internally used to set the current item member variable*/
  void SetCurrentItem(std::shared_ptr<mafVMEItem> data);

  /** Internally used update the current item variable to a new value (only if necessary)*/
  void UpdateCurrentItem(std::shared_ptr<mafVMEItem> item);

  /** Set a flag making the interpolator to release data of unnecessary items */
  void SetReleaseDataFlag(bool flag);
  void ReleaseDataFlagOn();
  void ReleaseDataFlagOff();

  std::shared_ptr<mafVMEItem> m_CurrentItem; ///< the item currently selected for the current time

  mafVMEItem *m_OldItem = nullptr;     ///< the item previously selected

  mafTimeStamp  m_OldTimeStamp; ///< previous time

  mafMTime      m_UpdateTime;   ///< the modification time of last data update

  bool          m_ReleaseDataFlag; ///< this flag forces the interpolator to release unnecessary data (default == false)
};
