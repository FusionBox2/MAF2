#include "VTKViewModel.h"

#include "IVTKViewNode.h"

#include "DocumentContext.h"
#include "ISelectionController.h"

#include "ftk/Core/Node.h"

#include "vtkMAFAssembly.h"

#include "mafVME.h"
#include "mafPipeVTK.h"
#include "ftk/Core/PipeFactory.h"

BEGIN_FTK_NAMESPACE

namespace
{
	class VTKSceneNode : public IVTKViewNode
	{
	public:

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

		/*std::shared_ptr<mafNode> m_Vme;
		mafSceneNode* m_Parent;
		std::shared_ptr<mafPipe> m_Pipe;
		bool               m_PipeCreatable;
		bool               m_Mutex;
		vtkRenderer* m_RenFront;
		vtkMAFAssembly* m_AssemblyFront;
		vtkRenderer* m_RenBack;
		vtkMAFAssembly* m_AssemblyBack;
		vtkRenderer* m_AlwaysVisibleRenderer;
		vtkMAFAssembly* m_AlwaysVisibleAssembly;
		mafSceneNode* m_Next;
		mafSceneGraph* m_Sg;*/

	};

	std::unique_ptr<VTKSceneNode> buildRecursive(model::data::Node* node, VTKSceneNode* parent, std::unordered_map<model::data::Node*, IVTKViewNode*>& nodeMap)
	{
		auto vn = std::make_unique<VTKSceneNode>();

		vn->m_model = node;
		vn->m_parent = parent;

		vtkLinearTransform* transform = nullptr;
		if (node->IsA("mafVME"))
		{
			auto v = mafVME::StaticDownCast(node);
			assert(v->GetOutput());
			assert(v->GetOutput()->GetTransform());
			assert(v->GetOutput()->GetTransform()->GetVTKTransform());
			transform = v->GetOutput()->GetTransform()->GetVTKTransform();
		}

		for (size_t i = 0; i < std::size(vn->m_assemblies); i++)
		{
			auto assembly = vtkSmartPointer<vtkMAFAssembly>::New();
			assembly->SetVme(vn->m_model);
			assembly->SetUserTransform(transform);
			vn->m_assemblies[i] = assembly;

			if (parent)
			{
				parent->m_assemblies[i]->AddPart(vn->m_assemblies[i]);
			}
		}

		nodeMap[node] = vn.get();
		for (size_t i = 0; i < node->GetNumberOfChildren(); ++i)
		{
			vn->m_children.push_back(buildRecursive(node->GetChild(i).get(), vn.get(), nodeMap));
		}
		return vn;
	}
}


VTKViewModel::VTKViewModel(DocumentContext& context)
	: m_context(context)
{
	buildTree();
	subscribeToContext();
}

VTKViewModel::~VTKViewModel() = default;

IVTKViewModel::NodeId VTKViewModel::root() const
{
	return m_root.get();
}

VTKViewModel::NodeVisibility VTKViewModel::getVisibility(model::data::Node* node) const
{
	auto nodeIt = m_nodeMap.find(node);

	if (nodeIt == m_nodeMap.end())
	{
		return NodeVisibility::NODE_NON_VISIBLE;
	}

	auto vn = static_cast<VTKSceneNode*>(nodeIt->second);

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
	auto nodeIt = m_nodeMap.find(node);

	if (nodeIt == m_nodeMap.end())
	{
		return;
	}

	auto vn = static_cast<VTKSceneNode*>(nodeIt->second);

	if (vn->m_pipe)
	{
		vn->m_pipe = nullptr;
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

IVTKViewModel::NodeId VTKViewModel::getViewNode(model::data::Node* node) const
{
	if (auto it = m_nodeMap.find(node); it != m_nodeMap.end())
	{
		return it->second;
	}
	return nullptr;
}

void VTKViewModel::buildTree()
{
	m_root = buildRecursive(m_context.getDocument()->getRoot().get(), nullptr, m_nodeMap);
}

void VTKViewModel::subscribeToContext()
{
	m_connections.push_back(m_context.getDocument()->connectNodeAdded(
		[this](const NodeAdded& e)
		{
			auto parentVN = static_cast<VTKSceneNode*>(m_nodeMap[e.node->GetParent()]);
			auto newVN = buildRecursive(e.node, parentVN, m_nodeMap);
			parentVN->m_children.push_back(buildRecursive(e.node, parentVN, m_nodeMap));
			auto raw = parentVN->m_children.back().get();
			m_nodeMap[e.node] = raw;
			//nodeAdded.emit(raw);
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeRemoved(
		[this](const NodeRemoved& e)
		{
			auto vn = static_cast<VTKSceneNode*>(m_nodeMap[e.node]);
			auto parentVN = vn->parent();
			auto it = std::find_if(begin(parentVN->m_children), end(parentVN->m_children), [vn](auto& p) {return p.get() == vn; });
			auto extracted = std::move(*it);//retain it till notification handled
			parentVN->m_children.erase(it);
			//nodeRemoved.emit(vn);
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeMoved(
		[this](const NodeMoved& e)
		{
			auto vn = static_cast<VTKSceneNode*>(m_nodeMap[e.node]);
			auto oldParentVN = vn->parent();
			auto newParentVN = static_cast<VTKSceneNode*>(m_nodeMap[e.node->GetParent()]);
			auto it = std::find_if(begin(oldParentVN->m_children), end(oldParentVN->m_children), [vn](auto& p) {return p.get() == vn; });
			newParentVN->m_children.push_back(std::move(*it));
			auto raw = newParentVN->m_children.back().get();
			//nodeMoved.emit(raw);
		}));

	m_connections.push_back(m_context.getDocument()->connectNodeChanged(
		[this](const NodeChanged& e)
		{
			auto vn = static_cast<VTKSceneNode*>(m_nodeMap[e.node]);
			if (vn->m_pipe)
			{
				m_sceneUpdated.emit();
			}
		}));

	m_connections.push_back(m_context.connectStatusChanged(
		[this](const NodeStatusChanged& e)
		{
			auto vn = static_cast<VTKSceneNode*>(m_nodeMap[e.node]);
			//nodeChanged.emit(vn);
		}));

	m_connections.push_back(m_context.getSelectionController().connectSelectionChanged(
		[this](model::data::Node* node)
		{
			if (auto vn = static_cast<VTKSceneNode*>(getViewNode(node)))
			{
				if (vn->m_pipe)
				{
					vn->m_pipe->Select(m_context.getSelectionController().isSelected(node));
					m_sceneUpdated.emit();
				}
			}
		}));
}

END_FTK_NAMESPACE
