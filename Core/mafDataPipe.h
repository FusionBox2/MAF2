#pragma once

#include "ftkConfigure.h"

#include "mafTimeStamped.h"
#include "mafOBB.h"

class mafVMEItem;
class mafVME;

#ifdef MAF_USE_VTK
  class vtkDataSet;
  class vtkAlgorithmOutput;
#endif

/** abstract class for process objects producing data as output of a VME.
  mafDataPipe is the base class for process objects producing data as output 
  of a VME. This basic class does not define the kind of output data. The class
  also implements the mechanism to compute the 3D bounds
  at any time...
 
  @sa mafDataPipeInterpolator
 
  @todo
  - rewrite comments
  - modify the GetCurrentBounds to make it call UpdateCurentBounds explicitly!
  - rewrite GetOutput()
  - reerite UpdateBounds()
*/
class FTK_CORE_EXPORT mafDataPipe:public mafTimeStamped
{
public:
  mafDataPipe();

  mafDataPipe(const mafDataPipe&) = delete;
	mafDataPipe& operator=(const mafDataPipe&) = delete;

  mafDataPipe(mafDataPipe&&) = delete;
  mafDataPipe& operator=(mafDataPipe&&) = delete;

	virtual ~mafDataPipe();

	mafBaseTypeMacro(mafDataPipe)

  /**
    This function makes the current bounds to be updated. It should be optimized
    to not require data loading, i.e. data bounds to be evaluated 
    without actually loading the data, but reading dataset bounds from 
    VME item meta-data structure (@sa mafDataPipeInterpolator)*/
  virtual void UpdateBounds() {}
 
  /** update the output of the data pipe */
  virtual void Update() {}

  /**
    Return the current dataset bounds, updated by UpdateBounds() and
    Update() when CurrentTime is changed. The Update() functions
    must be called before calling this function, otherwise the bounds
    are invalid.*/
  mafOBB *GetBounds() {return &m_Bounds;}

#ifdef MAF_USE_VTK
  /**
    Return a VTK dataset corresponding to the current time.*/
  virtual vtkAlgorithmOutput *GetVTKOutputPort() {return nullptr;}
  vtkDataSet *GetVTKData();
#endif

  /** Set/Get the current time */
  void SetTimeStamp(mafTimeStamp t);
  mafTimeStamp GetTimeStamp() {return m_CurrentTime;}

  /** return modification time */
  MTimeType GetMTime() override;

  /** This function returns true if the VME is accepted by this Pipe. */
  virtual bool Accept(mafVME *vme) {return vme != nullptr;}

  std::shared_ptr<mafDataPipe> MakeACopy();

  /**
    Copy from another pipe, the function return VTK_ERROR if the specified pipe 
    is not compatible. */
  virtual int DeepCopy(mafDataPipe *pipe);

  /** Set/Get the VME to be used as input for this PIPE */
  int SetVME(mafVME *vme);
  mafVME *GetVME() {return m_VME;}

  /** enable/disable the dependence of this data pipe from VME's pose matrix */
  void SetDependOnPose(int flag) {m_DependOnPose=flag;Modified();}
  int GetDependOnPose() {return m_DependOnPose;}

  /** enable/disable the dependence of this data pipe from VME's pose matrix */
  void SetDependOnVMETime(int flag) {m_DependOnVMETime=flag;Modified();}
  int GetDependOnVMETime() {return m_DependOnVMETime;}

  /** enable/disable the dependence of this data pipe from VME's ABS pose matrix */
  void SetDependOnAbsPose(int flag) {m_DependOnAbsPose=flag;Modified();}
  int GetDependOnAbsPose() {return m_DependOnAbsPose;}

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  virtual void OnPreUpdate1();
  virtual void OnPreUpdate2();
  virtual void OnUpdate();

protected:
  /** function called before of data pipe execution */
  virtual void PreExecute1();
  /** function called before of data pipe execution */
  virtual void PreExecute2();

  /** function called to updated the data pipe output */
  virtual void Execute();

  mafOBB        m_Bounds;///< bounds of the output data
  mafTimeStamp  m_CurrentTime;  ///< time for which data is computed
  mafVME        *m_VME;         ///< pointer to the VME for which output is computed
  mafMTime      m_PreExecuteTime;///< use to reduce the number of time PreExecute is called

  int           m_DependOnPose;
  int           m_DependOnAbsPose;
  int           m_DependOnVMETime;
};
