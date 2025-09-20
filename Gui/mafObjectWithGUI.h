#pragma once

#include "ftkConfigure.h"

#include <memory>

BEGIN_FTK_NAMESPACE

class mafGUI;

class MAF_EXPORT mafObjectWithGUI
{
public:
	mafObjectWithGUI();

	virtual ~mafObjectWithGUI();

	mafGUI* GetGui();

	void UpdateGUI();

	void DeleteGUI();

protected:
	mafGUI* AccessGUI() const;

	virtual mafGUI* CreateGui() = 0;

private:

	std::unique_ptr<mafGUI> m_GUI;
};

END_FTK_NAMESPACE
