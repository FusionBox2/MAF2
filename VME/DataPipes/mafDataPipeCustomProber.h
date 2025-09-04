#pragma once

#include "mafDataPipeCustom.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class vtkMAFDistanceFilter;
class vtkPolyDataNormals;
namespace model::data
{
    class Node;
}
using mafNode = model::data::Node;

/** a data pipe which calculate new data depending on parameters given by the VME.
  This data pipe creates an internal vtkSource objects and links to it to 
  receive update events and calculate new data for the VME.
  @sa mafDataPipeCustom
*/
class MAF_EXPORT mafDataPipeCustomProber : public mafDataPipeCustom
{
public:
  mafTypeMacroN(mafDataPipeCustomProber);

  enum PROBING_MODALITY
  {
    DENSITY_MODE = 0,
    DISTANCE_MODE,
    DISTANCE_MODE_SCALAR = 0,
    DISTANCE_MODE_VECTOR
  };

  /** Make a copy of itself.*/
  int DeepCopy(mafDataPipe *pipe) override;

  /** Set the surface to probe volume data*/
  void SetSurface(mafNode *surface);

  /** Return the probing surface node*/
  mafNode *GetSurface() {return m_Surface;};

  /** Set the volume to probe*/
  void SetVolume(mafNode *volume);

  /** Return the probed volume*/
  mafNode *GetVolume() {return m_Volume;};

  /** Set the mode to Density or Distance Map.*/
  void SetMode(int mode);

  /** Set the Density mode Map.*/
  void SetModeToDensity();

  /** Set the Distance mode Map.*/
  void SetModeToDistance();

  /** Get the mode.*/
  int GetMode();

  /** Set the threshold for Distance mode Map.*/
  void SetDistanceThreshold(float thr);

  /** Get the threshold of Distance mode Map.*/
  float GetDistanceThreshold();

  /** Set/Get the max distance for Distance mode Map.*/
  float GetMaxDistance();

  /** Set/Get the max distance for Distance mode Map.*/
  void SetMaxDistance(float max_dist);

  /** Set the Distance mode to produce scalar data or vector data.*/
  void SetDistanceMode(int mode);

  /** Set the Distance mode to produce scalar data.*/
  void SetDistanceModeToScalar() {this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_SCALAR);};

  /** Set the Distance mode to produce vector data.*/
  void SetDistanceModeToVector() {this->SetDistanceMode(mafDataPipeCustomProber::DISTANCE_MODE_VECTOR);};

  /** Get the distance mode type.*/
  int GetDistanceMode();

  /** Set/Get the High Density Threshold for Density mode Map.*/
  float GetHighDensity();

  /** Set/Get the High Density Threshold for Density mode Map.*/
  void SetHighDensity(float high_dens);

  /** Set/Get the Low Density Threshold for Density mode Map.*/
  float GetLowDensity();

  /** Set/Get the Low Density Threshold for Density mode Map.*/
  void SetLowDensity(float low_dens);

protected:
  mafDataPipeCustomProber();
  mafDataPipeCustomProber(const mafDataPipeCustomProber&) = delete;
  mafDataPipeCustomProber(mafDataPipeCustomProber&&) = delete;
  mafDataPipeCustomProber& operator=(const mafDataPipeCustomProber&) = delete;
  mafDataPipeCustomProber& operator=(mafDataPipeCustomProber&&) = delete;
  ~mafDataPipeCustomProber() override;

  /** function called before of data pipe execution */
  void Execute() override;

  mafNode *m_Surface;
  mafNode *m_Volume;

  vtkPolyDataNormals *m_Normals;
  vtkMAFDistanceFilter  *m_Prober;

  float  m_DistThreshold;
  float  m_MaxDistance;
  int		 m_DistanceModeType;
  int		 m_ProberMode;
  float  m_HighDensity;
  float  m_LowDensity;
};
