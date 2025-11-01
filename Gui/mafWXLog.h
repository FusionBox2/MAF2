#pragma once

#include "ftkConfigure.h"

class MAF_EXPORT mafWXLog : public wxLog
{
public:
	mafWXLog(wxTextCtrl* pTextCtrl);

	~mafWXLog() override;

	/** Set the filename for the log file. */
	int SetFileName(wxString filename);

	/** Set the flag to rodirect the log on a file. */
	void LogToFile(bool on);

private:
	/** Implement sink function. */
	void DoLogText(const wxString& msg) override;

	// the control we use
	wxTextCtrl* m_PTextCtrl = nullptr;
	FILE* m_Fp = nullptr;
	bool m_LogToFile = false;
};
