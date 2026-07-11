#pragma once

#include "ftkConfigure.h"

#include "mafEvent.h"
#include "mafEventSender.h"
#include "mafBaseEventHandler.h"

#include <list>
#include <memory>

namespace model::data
{
	class Node;
}
using mafNode = model::data::Node;
class mafView;
class mafGUIViewPanel;
class wxVTKWindow;
class vtkRenderWindow;
class mafDeviceButtonsPadMouse;

//----------------------------------------------------------------------------
// mafViewManager :
//----------------------------------------------------------------------------
/** An implementation of mafViewManager with [D]ynamic view [C]reation feature */
class MAF_EXPORT mafViewManager : public mafBaseEventHandler, public mafEventSender
{

public:
	mafViewManager();
	~mafViewManager() override;

	void SetRemoteListener(mafBaseEventHandler* Listener) { m_RemoteListener = Listener; }

	void OnEvent(mafEventBase* maf_event) override;

	/** Add the vme to all views. */
	void VmeAdd(std::shared_ptr<mafNode> node);

	/** Remove the vme from all views. */
	void VmeRemove(mafNode* node);

	/** Show the selection of the vme in all views. */
	void VmeSelect(mafNode* node);

	/** Show/Hide the vme in the selected view. */
	void VmeShow(mafNode* node, bool show);

	/** inform the views that a vme was modified */
	void VmeModified(mafNode* node); //SIL. 9-3-2005: 

	/** Add the View to the view-list. */
	virtual long ViewAdd(std::unique_ptr<mafView> view);

	/** Pass the selected render window to the mouse device. */
	void ViewSelected(mafView* view /*, wxVTKWindow *rwi*/);

	/** Activate the view from software to simulate the click on the view (used by remote). */
	void Activate(mafView* view);

	/** Create a view at index 'id' of the view-list. */
	virtual mafView* ViewCreate(int id);

	/** Create a view given the type. */
	virtual mafView* ViewCreate(const mafString& label);

	/** Insert the view into the view-list, fill the view with the vme tree and select the selected vme. */
	void ViewInsert(mafView* view);

	/** Remove the view from the view-list and delete it. */
	virtual void ViewDelete(mafView* view);

	/** Delete all view. */
	virtual void ViewDeleteAll();

	/** Reset the camera to fit the selected vme (if sel = true) or all (if sel = false). apply to the selected view only. */
	void CameraReset(bool sel = false);

	/** Reset the camera to fit the specified vme. apply to the selected view only. */
	void CameraReset(mafNode* node);

	/** Update the camera for all opened views; optionally the Update can be limited for the selected view. */
	void CameraUpdate(bool only_selected = false);

	/** Set the FlyTo mode for the selected view. */
	void CameraFlyToMode();

	/** Call PropertyUpdate for all views. */
	void PropertyUpdate(bool fromTag = false);

	/** Return the selected view. */
	mafView* GetSelectedView() const;

	/** Return the root of the vme tree. */
	mafNode* GetCurrentRoot() const { return m_RootVme.get(); }

	/** Return the created view-list. */
	const auto& GetList() { return m_ViewList; }

	/** Return a view (defined by label) if is present on open wiews list*/
	mafView* GetFromList(const mafString& label) const;

	/** Return the plugged view-list. */
	const auto& GetListTemplate() { return m_ViewTemplate; }

	/** Empty. */
	void OnQuit();

	/** Return the view pointer from view's id and multiplicity. */
	mafView* GetView(int id, int mult) const;

	/** Initialize the action for the mouse device.*/
	void SetMouse(mafDeviceButtonsPadMouse* mouse);

	/** Turn On/Off the collaboration status.*/
	void Collaborate(bool status) { m_CollaborateStatus = status; }

	bool m_FromRemote = false;  ///< Flag used from RemoteLogic to avoid loop

protected:

	std::vector<std::unique_ptr<mafView> > m_ViewTemplate; // view template vector

	mafDeviceButtonsPadMouse* m_Mouse = nullptr;

	std::list<mafView*> m_ViewList; // created view list

	mafBaseEventHandler* m_RemoteListener = nullptr;
	std::shared_ptr<mafNode> m_RootVme;
	mafNode* m_SelectedVme = nullptr;
	mafView* m_SelectedView = nullptr;
	vtkRenderWindow* m_SelectedRWI = nullptr;
	mafView* m_ViewBeingCreated = nullptr;
	std::vector<std::vector<mafView*> > m_ViewMatrixID; ///< Matrix to access views directly by (id, multiplicity)
	bool m_CollaborateStatus = false;
};
