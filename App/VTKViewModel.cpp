#include "VTKViewModel.h"

#include "IVTKViewNode.h"

#include "DocumentContext.h"
#include "ISelectionController.h"

#include "ftk/Core/Node.h"
#include "ftk/Core/PipeFactory.h"

#include "vtkMAFAssembly.h"

#include "mafVME.h"
#include "mafPipeVTK.h"

#include <deque>

BEGIN_FTK_NAMESPACE

namespace
{
	vtkLinearTransform* getNodeTransform(model::data::Node* node)
	{
		if (node->IsA("mafVME"))
		{
			if (auto v = mafVME::StaticDownCast(node))
			{
				if (auto o = v->GetOutput())
				{
					if (auto t = o->GetTransform())
					{
						auto transform = t->GetVTKTransform();
						assert(transform);
						return transform;
					}
					else
					{
						assert(false);
					}
				}
				else
				{
					assert(false);
				}
			}
		}
		return nullptr;
	}

	class VTKSceneNode : public IVTKViewNode
	{
	public:
		VTKSceneNode(model::data::Node* node, VTKSceneNode* parent)
			: m_model(node)
			, m_parent(parent)
		{
			auto transform = getNodeTransform(node);

			for (size_t i = 0; i < std::size(m_assemblies); i++)
			{
				auto assembly = vtkSmartPointer<vtkMAFAssembly>::New();
				assembly->SetVme(m_model);
				assembly->SetUserTransform(transform);
				m_assemblies[i] = assembly;

				if (parent)
				{
					parent->m_assemblies[i]->AddPart(m_assemblies[i]);
				}
			}
		}

		~VTKSceneNode() override
		{
			m_children.clear();
			for (size_t i = 0; i < std::size(m_assemblies); i++)
			{
				if (m_parent)
				{
					m_parent->m_assemblies[i]->RemovePart(m_assemblies[i]);
				}
			}
		}

		VTKSceneNode* parent() const override
		{
			return m_parent;
		}

		std::vector<IVTKViewNode*> children() const override
		{
			std::vector<IVTKViewNode*> out;
			for (auto& c : m_children)
			{
				out.push_back(c.get());
			}
			return out;
		}

		vtkSmartPointer<vtkAssembly> getAssembly(size_t i) const override
		{
			return m_assemblies[i];
		}

		model::data::Node* m_model = nullptr;
		VTKSceneNode* m_parent = nullptr;
		
		std::vector<std::unique_ptr<VTKSceneNode>> m_children;
		
		vtkSmartPointer<vtkAssembly> m_assemblies[3];
		std::shared_ptr<mafPipe> m_pipe;
		
		base::Connection m_onPipeValuesChanged;
		base::Connection m_onPipePropertiesChanged;

		base::Signal<> m_pipeValuesChanged;
		base::Signal<> m_pipePropertiesChanged;
	};

	void removeFromMapRecursive(VTKSceneNode* node, std::unordered_map<model::data::Node*, IVTKViewNode*>& nodeMap)
	{
		nodeMap.erase(node->m_model);
		for (auto& child : node->m_children)
		{
			removeFromMapRecursive(child.get(), nodeMap);
		}
	}
}


VTKViewModel::VTKViewModel(DocumentContext& context)
	: m_context(context)
{
	addTree(m_context.getDocument()->getRoot().get());
	subscribeToContext();
}

VTKViewModel::~VTKViewModel() = default;

IVTKViewModel::NodeId VTKViewModel::root() const
{
	return m_root.get();
}

VTKViewModel::NodeVisibility VTKViewModel::getVisibility(model::data::Node* node) const
{
	auto vn = static_cast<VTKSceneNode*>(getViewNode(node));

	if (!vn)
	{
		return NodeVisibility::NODE_NON_VISIBLE;
	}

	if (auto it = m_pipeMap.find(_R(node->GetTypeName())); it != m_pipeMap.end())
	{
		if (it->second.visibility == VisibilityMode::NON_VISIBLE)
		{
			return NodeVisibility::NODE_NON_VISIBLE;
		}
		if (it->second.visibility == VisibilityMode::MUTEX)
		{
			return vn->m_pipe ? NodeVisibility::NODE_MUTEX_ON : NodeVisibility::NODE_MUTEX_OFF;
		}
		return vn->m_pipe ? NodeVisibility::NODE_VISIBLE_ON : NodeVisibility::NODE_VISIBLE_OFF;
	}

	if (auto vme = mafVME::SafeDownCast(node))
	{
		if (!vme->GetVisualPipe().empty())
		{
			return vn->m_pipe ? NodeVisibility::NODE_VISIBLE_ON : NodeVisibility::NODE_VISIBLE_OFF;
		}
	}
	return NodeVisibility::NODE_NON_VISIBLE;
}

void VTKViewModel::toggleVisibility(model::data::Node* node)
{
	auto vn = static_cast<VTKSceneNode*>(getViewNode(node));

	if (!vn)
	{
		return;
	}

	if (vn->m_pipe)
	{
		vn->m_pipe = nullptr;
		vn->m_onPipeValuesChanged = {};
		vn->m_onPipePropertiesChanged = {};
		--m_pipeCount;
		m_sceneUpdated.emit();
		m_visibilityChanged.emit(node);
		return;
	}

	auto vme = mafVME::SafeDownCast(node);

	base::String pipeName;

	if (auto it = m_pipeMap.find(_R(node->GetTypeName())); it != m_pipeMap.end())
	{
		if (it->second.visibility == VisibilityMode::NON_VISIBLE)
		{
			return;
		}
		if (it->second.visibility == VisibilityMode::MUTEX)
		{
			return;
		}
		pipeName = it->second.pipeName;
	}

	if (pipeName.empty())
	{
		if (vme)
		{
			pipeName = vme->GetVisualPipe();
		}
	}

	if (pipeName.empty())
	{
		return;
	}

	if (auto pipe = mafPipeVTK::SafeDownCast(PipeFactory::CreatePipe(pipeName.c_str())))
	{
		vtkRenderer* renderers[] = { nullptr, nullptr, nullptr };// m_RWI->m_RenFront, m_RWI->m_RenBack, m_RWI->m_AlwaysVisibleRenderer
		vtkMAFAssembly* assemblies[] = {
			static_cast<vtkMAFAssembly*>(vn->m_assemblies[0].Get()),
			static_cast<vtkMAFAssembly*>(vn->m_assemblies[1].Get()),
			static_cast<vtkMAFAssembly*>(vn->m_assemblies[2].Get())
		};
		pipe->Create(node, renderers, assemblies);
		pipe->Select(m_context.getSelectionController().isSelected(node));
		vn->m_pipe = pipe;
		vn->m_onPipeValuesChanged = vn->m_pipe->connectValuesChanged([this, vn]() {m_sceneUpdated.emit(); vn->m_pipeValuesChanged.emit(); });
		vn->m_onPipePropertiesChanged = vn->m_pipe->connectPropertiesChanged([this, vn]() {m_sceneUpdated.emit(); vn->m_pipePropertiesChanged.emit(); });
		++m_pipeCount;
		if (vme && m_pipeCount == 1)
		{
			std::array<double, 6> bounds;
			vme->GetOutput()->GetVMEBounds(bounds.data());
			m_sceneReset.emit(bounds);
		}
		m_sceneUpdated.emit();
		m_visibilityChanged.emit(node);
	}
}

base::Connection VTKViewModel::connectSceneUpdated(std::function<void()> fn)
{
	return m_sceneUpdated.connect(fn);
}

base::Connection VTKViewModel::connectSceneReset(std::function<void(const std::array<double, 6>&)> fn)
{
	return m_sceneReset.connect(fn);
}

base::Connection VTKViewModel::connectVisibilityChanged(std::function<void(model::data::Node*)> fn)
{
	return m_visibilityChanged.connect(fn);
}

base::Connection VTKViewModel::connectVisualValuesChanged(model::data::Node* node, std::function<void()> fn)
{
	auto vn = static_cast<VTKSceneNode*>(getViewNode(node));

	if (!vn)
	{
		return {};
	}

	return vn->m_pipeValuesChanged.connect(std::move(fn));
}

base::Connection VTKViewModel::connectVisualPropertiesChanged(model::data::Node* node, std::function<void()> fn)
{
	auto vn = static_cast<VTKSceneNode*>(getViewNode(node));

	if (!vn)
	{
		return {};
	}

	return vn->m_pipePropertiesChanged.connect(std::move(fn));
}

void VTKViewModel::plugVisualPipe(const base::String& nodeType, const base::String& pipeType, VisibilityMode visibility)
{
	VisualPipeInfo pluggedPipe;
	pluggedPipe.pipeName = std::move(pipeType);
	pluggedPipe.visibility = visibility;
	m_pipeMap.emplace(std::move(nodeType), std::move(pluggedPipe));
}

core::WithProperties::PropertyList VTKViewModel::getProperties()
{
	auto result = IVTKViewModel::getProperties();
	return result;
}

core::WithProperties::PropertyList VTKViewModel::getVisualProperties(model::data::Node* node)
{
	if (auto vn  = static_cast<VTKSceneNode*>(getViewNode(node)); vn && vn->m_pipe)
	{
		return vn->m_pipe->getProperties();
	}

	return {};
}

IVTKViewModel::NodeId VTKViewModel::getViewNode(model::data::Node* node) const
{
	if (auto it = m_nodeMap.find(node); it != m_nodeMap.end())
	{
		return it->second;
	}
	return nullptr;
}

void VTKViewModel::addNode(model::data::Node* node)
{
	if (!node)
	{
		return;
	}

	if (auto parentVN = static_cast<VTKSceneNode*>(getViewNode(node->GetParent())))
	{
		parentVN->m_children.push_back(std::make_unique<VTKSceneNode>(node, parentVN));
		m_nodeMap.emplace(node, parentVN->m_children.back().get());
	}
	else
	{
		m_root = std::make_unique<VTKSceneNode>(node, nullptr);
		m_nodeMap.emplace(node, m_root.get());
	}
}

void VTKViewModel::addTree(model::data::Node* node)
{
	if (!node)
	{
		return;
	}

	for (auto queue = std::deque<model::data::Node*>(1, node); !queue.empty(); )
	{
		auto nextNode = queue.front();
		queue.pop_front();

		addNode(nextNode);

		for (size_t i = 0; i < nextNode->GetNumberOfChildren(); i++)
		{
			queue.push_back(nextNode->GetChild(i).get());
		}
	}
}

void VTKViewModel::subscribeToContext()
{
	m_connections.push_back(m_context.getDocument()->connectTreeAdded(
		[this](const NodeAdded& e)
		{
			addTree(e.node);
		}));

	m_connections.push_back(m_context.getDocument()->connectTreeRemoved(
		[this](const NodeRemoved& e)
		{
			auto vn = static_cast<VTKSceneNode*>(getViewNode(e.node));
			removeFromMapRecursive(vn, m_nodeMap);
			if (auto parentVN = vn->parent())//vn != m_root.get()
			{
				auto it = std::find_if(begin(parentVN->m_children), end(parentVN->m_children), [vn](auto& p) {return p.get() == vn; });
				auto extracted = std::move(*it);//retain it till notification handled
				parentVN->m_children.erase(it);
			}
			else
			{
				m_root.reset();
			}
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeAdded(
		[this](const NodeAdded& e)
		{
			addNode(e.node);
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeRemoved(
		[this](const NodeRemoved& e)
		{
			auto vn = static_cast<VTKSceneNode*>(getViewNode(e.node));
			m_nodeMap.erase(e.node);
			if (auto parentVN = vn->parent())//vn != m_root.get()
			{
				auto it = std::find_if(begin(parentVN->m_children), end(parentVN->m_children), [vn](auto& p) {return p.get() == vn; });
				auto extracted = std::move(*it);//retain it till notification handled
				parentVN->m_children.erase(it);
			}
			else
			{
				m_root.reset();
			}
		}));

	m_connections.push_back(m_context.getDocument()->connectTreeMoved(
		[this](const NodeMoved& e)
		{
			auto vn = static_cast<VTKSceneNode*>(getViewNode(e.node));
			auto oldParentVN = vn->parent();
			auto newParentVN = static_cast<VTKSceneNode*>(getViewNode(e.node->GetParent()));

			auto it = std::find_if(begin(oldParentVN->m_children), end(oldParentVN->m_children), [vn](auto& p) {return p.get() == vn; });
			
			newParentVN->m_children.push_back(std::move(*it));
			oldParentVN->m_children.erase(it);
			for (size_t i = 0; i < std::size(vn->m_assemblies); i++)
			{
				if (oldParentVN)
				{
					oldParentVN->m_assemblies[i]->RemovePart(vn->m_assemblies[i]);
				}
				if (newParentVN)
				{
					newParentVN->m_assemblies[i]->AddPart(vn->m_assemblies[i]);
				}
			}

			auto raw = newParentVN->m_children.back().get();
			raw->m_parent = newParentVN;
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeChanged(
		[this](const NodeChanged& e)
		{
			if (auto vn = static_cast<VTKSceneNode*>(getViewNode(e.node)); vn && vn->m_pipe)
			{
				m_sceneUpdated.emit();
			}
		}));

	//m_connections.push_back(m_context.connectStatusChanged(
		//[this](const NodeStatusChanged& e)
		//{
		//	auto vn = static_cast<VTKSceneNode*>(getViewNode(e.node));
		//}));

	m_connections.push_back(m_context.getSelectionController().connectSelectionChanged(
		[this](model::data::Node* node)
		{
			if (auto vn = static_cast<VTKSceneNode*>(getViewNode(node)); vn && vn->m_pipe)
			{
				vn->m_pipe->Select(m_context.getSelectionController().isSelected(node));
				m_sceneUpdated.emit();
			}
		}));
}

END_FTK_NAMESPACE
