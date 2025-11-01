#pragma once

#include "ftkConfigure.h"

#include "vtkOutputWindow.h"

class MAF_EXPORT mafVTKLog : public vtkOutputWindow
{
public:

	static mafVTKLog* New();

	vtkTypeMacro(mafVTKLog, vtkOutputWindow);

	vtkSetMacro(Enabled, bool);
	vtkGetMacro(Enabled, bool);
	vtkBooleanMacro(Enabled, bool);

	void DisplayText(const char*) override;

protected:
	mafVTKLog();

	mafVTKLog(const mafVTKLog&) = delete;

	void operator=(const mafVTKLog&) = delete;

	~mafVTKLog() override;

	bool Enabled = true;
};
