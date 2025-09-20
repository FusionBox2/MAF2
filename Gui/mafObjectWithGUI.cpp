#include "mafObjectWithGUI.h"

#include "mafGUI.h"

#include <assert.h>

mafObjectWithGUI::mafObjectWithGUI() = default;

mafObjectWithGUI::~mafObjectWithGUI() = default;

mafGUI* mafObjectWithGUI::GetGui()
{
	if (!m_GUI)
		m_GUI.reset(CreateGui());
	assert(m_GUI);
	return m_GUI.get();
}

void mafObjectWithGUI::UpdateGUI()
{
	if (m_GUI)
		m_GUI->Update();
}

mafGUI* mafObjectWithGUI::AccessGUI() const
{
	return m_GUI.get();
}

void mafObjectWithGUI::DeleteGUI()
{
	m_GUI.reset();
}
