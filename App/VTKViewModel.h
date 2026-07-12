#pragma once

#include "ftkConfigure.h"

#include "IVTKViewModel.h"

#include "ftk/Base/Signal.h"
#include "ftk/Base/String.h"

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

BEGIN_FTK_NAMESPACE

class DocumentContext;
namespace model::data
{
	class Node;
}

class VTKViewModel : public IVTKViewModel
{
public:
	enum class NodeVisibility
	{
		NODE_NON_VISIBLE,
		NODE_VISIBLE_OFF,
		NODE_VISIBLE_ON,
		NODE_MUTEX_OFF,
		NODE_MUTEX_ON
	};

	enum class VisibilityMode
	{
		NON_VISIBLE,
		VISIBLE,
		MUTEX
	};

	VTKViewModel(DocumentContext& context);

	~VTKViewModel() override;

	NodeId root() const override;

	NodeVisibility getVisibility(model::data::Node* node) const;

	void toggleVisibility(model::data::Node* node);

	base::Connection connectSceneUpdated(std::function<void()>) override;

	base::Connection connectSceneReset(std::function<void(const std::array<double, 6>&)>) override;

	base::Connection connectVisibilityChanged(std::function<void(model::data::Node*)> fn);

	base::Connection connectVisualValuesChanged(model::data::Node* node, std::function<void()> fn);

	base::Connection connectVisualPropertiesChanged(model::data::Node* node, std::function<void()> fn);

	void plugVisualPipe(const base::String& nodeType, const base::String& pipeType, VisibilityMode visibility = VisibilityMode::VISIBLE);

	PropertyList getProperties() override;

	PropertyList getVisualProperties(model::data::Node* node);

private:
	NodeId getViewNode(model::data::Node* node) const;

	void addNode(model::data::Node* node);

	void addTree(model::data::Node* node);

	void subscribeToContext();

	DocumentContext& m_context;
	std::vector<base::Connection> m_connections;
	std::unique_ptr<IVTKViewNode> m_root;
	std::size_t m_pipeCount = 0;
	std::unordered_map<model::data::Node*, NodeId> m_nodeMap;
	base::Signal<> m_sceneUpdated;
	base::Signal<const std::array<double, 6>& > m_sceneReset;
	base::Signal<model::data::Node*> m_visibilityChanged;

	struct VisualPipeInfo
	{
		base::String pipeName;
		VisibilityMode visibility = VisibilityMode::VISIBLE;
	};
	std::unordered_map<base::String, VisualPipeInfo> m_pipeMap;
};


#ifdef kjhkjhkjhkjhkjh

// forward references :
//----------------------------------------------------------------------------
namespace model::data
{
	class Node;
}
using mafNode = model::data::Node;
class mafGUI;
class mafView;
class mafSceneNode;
class vtkRenderer;

//----------------------------------------------------------------------------
// mafSceneGraph :
//----------------------------------------------------------------------------
class MAF_EXPORT mafSceneGraph :public mafEventSender /*: public mafObserver*/
{
public:
	mafSceneGraph(mafView* view, vtkRenderer* ren1, vtkRenderer* ren2 = NULL, vtkRenderer* ren3 = NULL);
	~mafSceneGraph() override;
	//virtual void	OnEvent(mafEvent& e);

	/** Add a vme to the scene graph. */
	virtual void VmeAdd(std::shared_ptr<mafNode> vme);

	/** Remove a vme from the scene graph. */
	virtual void VmeRemove(mafNode* vme);

	/** Select a vme and store a reference to it into m_SelectedVme variable. */
	virtual void VmeSelect(mafNode* vme, bool select);

	/** Change the visibility of the vme and if necessary create the vme related pipe. */
	virtual void VmeShow(mafNode* vme, bool show);

	/** Show/Hide the Vme with the same type of the passed vme */
	virtual void VmeShowByType(mafNode* vme, bool show);

	/**
  Show all the same vme type. */
  //@@@ virtual void					VmeShowByType			(mafNodeBaseTypes type,bool show);

  /** Show all the vme subtree. */
	virtual void VmeShowSubTree(mafNode* vme, bool show);

	/** Update the vme's properties. */
	virtual void VmeUpdateProperty(mafNode* vme, bool fromTag = false);

	/** Find the corresponding vme's node. */
	virtual mafSceneNode* Vme2Node(mafNode* vme);

	/** Return the list of node that are added to the view.*/
	mafSceneNode* GetNodeList() { return m_List; }

	vtkRenderer* m_RenFront;  ///< pointer to the front renderer
	vtkRenderer* m_RenBack;   ///< pointer to the back renderer
	vtkRenderer* m_AlwaysVisibleRenderer; /// < Renderer used to superimpose utility stuff to main render window

	mafView* m_View;      ///< pointer to the view

	// Set the flags for creatable vmes type.
	  //@@@ virtual void SetCreatableFlag	(mafNodeBaseTypes type,  bool flag = true);

	  // Set the flags for mutex vmes.
	//@@@ virtual void SetMutexFlag			(mafNodeBaseTypes type,  bool flag = true);

	  // Set the flags for automatic show vme's type.
	//@@@ virtual void SetAutoShowFlag	(mafNodeBaseTypes type,  bool flag = true);

	// Return the gui with the autoshow widgets.
	  //@@@ virtual mafGUI *GetGui();

	/**
	Return the selected vme.*/
	virtual mafNode* GetSelectedVme() { return m_SelectedVme; };

	/**
	Used by the mafGUICheckTree - return the status of a SceneNode*/
	virtual int  GetNodeStatus(mafNode* node);

	// Event to show/hide the LandmarkClouds
	//@@@ virtual void	OnOpenCloseEvent( mafSceneNode *node );

  /** print a dump of this object */
	virtual void Print(std::ostream& os, const int tabs = 0);// const;


protected:
	/**
	recursively kill all node - starting from the tail of the list. */
	virtual void DeleteNodeList(mafSceneNode* n);
	/*
		bool m_creatable[NUM_OF_BASETYPE];
		bool m_mutex		[NUM_OF_BASETYPE];
		int  m_autoshow	[NUM_OF_BASETYPE];
		mafNode  *m_shown_mutex_vme[NUM_OF_BASETYPE];
	*/

	mafSceneNode* NodeAdd(std::shared_ptr<mafNode> vme);

	mafSceneNode* m_List;      ///< list of visualized node
	mafGUI* m_Gui;
	mafNode* m_SelectedVme;
};
#endif

END_FTK_NAMESPACE
