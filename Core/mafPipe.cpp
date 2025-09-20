#include "mafPipe.h"

#include "mafNode.h"
#include "mafView.h"
#include "mafGUI.h"

BEGIN_FTK_NAMESPACE

mafPipe::mafPipe() = default;

void mafPipe::Create(mafNode* node, mafView* view)
{
	m_Node = node;
	m_View = view;
	m_Selected = false;
}

mafPipe::~mafPipe()
{
	SetListener(nullptr);
}

mafGUI* mafPipe::CreateGui()
{
	assert(!AccessGUI());
	auto gui = new mafGUI(this);

	mafString type = _R(GetTypeName());
	gui->Label(_R("type :"), type);
	gui->Divider();
	return gui;
}

END_FTK_NAMESPACE
