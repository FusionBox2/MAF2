#include "mafPipeVTK.h"

#include "mafSceneNode.h"
#include "mafSceneGraph.h"
#include "mafViewVTK.h"

mafPipeVTK::mafPipeVTK() = default;

mafPipeVTK::~mafPipeVTK() = default;

void mafPipeVTK::Create(mafNode* node, vtkRenderer** ren, vtkMAFAssembly** assembly)
{
	m_RenFront = ren[0];
	m_RenBack = ren[1];
	m_AlwaysVisibleRenderer = ren[2];

	m_AssemblyFront = assembly[0];
	m_AssemblyBack = assembly[1];
	m_AlwaysVisibleAssembly = assembly[2];

	Create(node, nullptr);
}

void mafPipeVTK::Create(mafNode* node, mafView* view)
{
	Superclass::Create(node, view);
	m_Vme = mafVME::SafeDownCast(m_Node);

	if (auto viewVTK = mafViewVTK::SafeDownCast(m_View))
	{
		m_RenFront = viewVTK->GetFrontRenderer();
		m_RenBack = viewVTK->GetBackRenderer();
		m_AlwaysVisibleRenderer = viewVTK->GetAlwaysVisibleRenderer();

		if (auto sg = viewVTK->GetSceneGraph())
		{
			if (auto n = sg->Vme2Node(m_Vme))
			{
				m_AssemblyFront = n->m_AssemblyFront;
				m_AssemblyBack = n->m_AssemblyBack;
				m_AlwaysVisibleAssembly = n->m_AlwaysVisibleAssembly;
			}
		}
	}
}
