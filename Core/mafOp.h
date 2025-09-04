#pragma once

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"
#include "ftk/Base/Object.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
namespace model::data
{
	class Node;
}
using mafNode = model::data::Node;
class mafGUI;
class mafGUIHolder;
class mafGUISettings;
//----------------------------------------------------------------------------
// constants :
//----------------------------------------------------------------------------
enum OPERATIONS_TYPE_ID
{
	OPTYPE_OP = 0,
	OPTYPE_IMPORTER,
	OPTYPE_EXPORTER,
	OPTYPE_EDIT,
	OPTYPE_STATECHANGER,
	OPTYPE_MAX,
};
//----------------------------------------------------------------------------
// mafOp :
//----------------------------------------------------------------------------
/**
*/
class MAF_EXPORT mafOp: public mafBaseEventHandler, public mafEventSender
{
public:
  mafBaseTypeMacro(mafOp);

	mafOp();
	mafOp(const mafString &label);
	~mafOp() override; 

	/** Return the type of the operation: OPTYPE_OP, OPTYPE_IMPORTER, OPTYPE_EXPORTER, OPTYPE_EDIT or OPTYPE_STATECHANGER*/
	int GetType();

	void OnEvent(mafEventBase *maf_event) override;
	virtual mafOp* Copy();

	/** Builds operation's interface. */
	virtual void OpRun();

	/** Initialize operation's variables according to the parameter's list. */
	virtual void SetParameters(void *param) {}


	/** Return parameters used by operation. */
	virtual mafString GetParameters() {mafString parameters; return parameters;}

	/** Execute the operation. */
	virtual void OpDo();

	/** Makes the undo for the operation. */
	virtual void OpUndo();

	/** Return the operation's interface. */
	virtual mafGUI *GetGui()	{return m_Gui;}

  /** Set/Get the input vme for the operation. */
	void SetInput(std::shared_ptr<mafNode> vme);

	std::shared_ptr<mafNode> GetInput() const;

  /** Return the mafNode result of the operation.*/
	std::shared_ptr<mafNode> GetOutput() const;

	void SetOutput(std::shared_ptr<mafNode> output);

	/** Return true for the acceptable vme type. */
	virtual bool Accept(mafNode* vme);

	/** Return true if the operation is undoable. */
	virtual bool CanUndo();

	/** Return true if the operation preserve the input vme. */
	bool IsInputPreserving() const {return m_InputPreserving;}

	/** Puts the operation's interface into the gui holder and send the event to plug the interface on the side bar. */
	void ShowGui();

	/** Send the event to remove the interface from the side bar. */
	void HideGui();

	/** Return true if the OK button in operation's interface is enabled. */
	virtual bool OkEnabled();

	/** Stop operation with OK condition. */
	virtual void ForceStopWithOk();

	/** Stop operation with CANCEL condition. */
	virtual void ForceStopWithCancel();

  const mafString& GetLabel(){return m_Label;}
  void SetLabel(const mafString& label){m_Label = label;}

	int m_Id = -1; ///< Index of the operation referring to the operation list.
	mafOp *m_Next = nullptr; ///< Pointer to the next operation in the operation's list.

  //SIL 22/04/04
	long m_Compatibility = 0xFFFF;
	bool IsCompatible(long state);

  //MARCO 7/05/04
  virtual const char **GetActions() {return nullptr;}

  /** Turn On/Off the collaboration status. */
  void Collaborate(bool status);

  /** Return the collaborate status */
  bool GetCollaborateStatus(){return m_CollaborateStatus;}

  /** Turn On m_TestMode flag. 
  The m_TestMode flag is used to exclude the execution of splash screen or wxBusyInfo that conflicts with test machine.*/
  void TestModeOn() {m_TestMode = true;}

  /** Used to turn off m_TestMode flag.*/
  void TestModeOff() {m_TestMode = false;}

  /** Get TestMode*/
  bool GetTestMode(){return m_TestMode;}

  /** Set the Canundo flag for the operation when is plugged.*/
  void SetCanundo(bool can_undo = true) {m_Canundo = can_undo;}

  /** Return the Canundo flag for the operation.*/
  bool GetCanundo() {return m_Canundo;}

  /** Set the reference to the operation's setting panel.*/
  void SetSetting(mafGUISettings *setting) {m_SettingPanel = setting;}

  /** Get the reference to the operation's setting panel.*/
  mafGUISettings *GetSetting() {return m_SettingPanel;}

protected:
  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  virtual void OpStop(int result);

	mafGUI         *m_Gui = nullptr; ///< Pointer to the operation's GUI.
	mafGUIHolder   *m_Guih = nullptr;
	bool           m_Canundo = false; ///< Flag to establish if the operation define the UnDo method or not.
	int            m_OpType = OPTYPE_OP; ///< Store the type of the operation: OPTYPE_OP, OPTYPE_IMPORTER, OPTYPE_EXPORTER
	bool           m_InputPreserving = true; ///< Flag to say if the operation change the input data (m_InputPreserving = false) or not.
  bool           m_CollaborateStatus = false;
  mafGUISettings *m_SettingPanel = nullptr;
  bool           m_TestMode = false; ///< Flag used with cppunitTest: put this flag at true when executing tests to avoid busy-info or splash screen to be created, default is false.

private:
	std::shared_ptr<mafNode> m_Input; ///< Pointer to the Input VME.
	std::shared_ptr<mafNode> m_Output; ///< Pointer to the Output VME
	mafString      m_Label; ///< Label of the operation that will appear on the SideBar tab.
};
