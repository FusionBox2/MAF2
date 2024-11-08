#pragma once

#include "ftkConfigure.h"

#include "mafEventBase.h"
#include "mafMatrix.h"
#include "ftk/Base/RegisteringPointer.h"

/** Event class to transport a triggering button and a position from pointing devices.
  Evant issue by 2D and 3D tracking devices in correspondace to
  MoveEvent, ButtonDownEvent and ButtonUpevent. 2D devices set the 
  X and Y coordinates, while 3D devices set the pose matrix. The event
  can also store an optional button argument, used to store the button 
  indexs when events for button down and button up are issued. Also an
  optional word of modifiers can be used.
  @sa mafEventBase mafDeviceButtonsPadTracker MoveEvent ButtonDownEvent ButtonUpEvent
*/
class MAF_EXPORT mafEventInteraction : public mafEventBase
{
	mafTypeMacroN(mafEventInteraction);
public:
	mafEventInteraction(void* sender = NULL, mafID id = -1, int button = 0, unsigned long modifiers = 0) :
		mafEventBase(sender, id), m_Button(button), m_Modifiers(modifiers) {}

	mafEventInteraction(void* sender, mafID id, std::shared_ptr<mafMatrix> matrix, int button = 0, unsigned long modifiers = 0) :
		mafEventBase(sender, id), m_Button(button), m_Modifiers(modifiers), m_Matrix(matrix) {}

	mafEventInteraction(void* sender, mafID id, double x, double y, int button = 0, unsigned long modifiers = 0) :
		mafEventBase(sender, id), m_X(x), m_Y(y), m_XYFlag(true), m_Button(button), m_Modifiers(modifiers) {}

	~mafEventInteraction() override = default;

  /** Set screen position, for 2D tracking devices */
  void Set2DPosition(double x,double y);
  /** Set screen position, for 2D tracking devices */
  void Set2DPosition(const double pos[2]);

  /** Get screen position, for 2D tracking devices */
  void Get2DPosition(double pos[2]);

  /** set the flag for 2D coords */
  void SetXYFlag(bool val);

  /** get the flag for 2D coords */
  bool GetXYFlag();
  
  /** Set the triggering button */
  void SetButton(int button);
  /** Get the triggering button */
  int GetButton() {return m_Button;}

  /** Set the optional key argument */
  void SetKey(unsigned char key);
  /** Get the optional key argument */
  unsigned char GetKey() {return m_Key;}

  /** Get the pose matrix, for 3D tracking devices */
  std::shared_ptr<mafMatrix> GetMatrix();
  /** Set the pose matrix, for 3D tracking devices */
  void SetMatrix(std::shared_ptr<mafMatrix> matrix);

  /** Set the given modifier value*/
  void SetModifier(unsigned long idx,bool value=true);
  /** Get the given modifier value*/
  bool GetModifier(unsigned long idx);
  /** Set the given modifiers value*/
  void SetModifiers(unsigned long modifiers);
  /** Get the given modifiers value*/
  unsigned long GetModifiers() {return m_Modifiers;}

  /** Deep Copy of the object*/
  void DeepCopy(const mafEventBase *event) override;

protected:

  int           m_Button = 0;     ///< Optional button which triggered the event
  unsigned long m_Modifiers = 0;  ///< Optional modifiers for the button 
  unsigned char m_Key = 0;        ///< Optional Key 
  double        m_X = 0.0;        ///< X coordinate, used by mouse device
  double        m_Y = 0.0;        ///< Y coordinate, used by mouse device
  bool          m_XYFlag = false; ///< Used to signal a 2D coordinate is present

  std::shared_ptr<mafMatrix>  m_Matrix;    ///< Pose matrix, used by 3D trackers
};
