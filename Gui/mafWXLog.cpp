#include "mafDefines.h" 

#include "mafWXLog.h"

#include <wx/datetime.h>

mafWXLog::mafWXLog(wxTextCtrl* pTextCtrl)
	: m_PTextCtrl(pTextCtrl)
{
}

mafWXLog::~mafWXLog()
{
	if (m_Fp)
	{
		fclose(m_Fp);
	}
}

void mafWXLog::DoLogText(const wxString& msg)
{
	wxDateTime log_time = wxDateTime::UNow();
	wxString logMsg = wxString::Format("%02d:%02d:%02d:%03d ", log_time.GetHour(), log_time.GetMinute(), log_time.GetSecond(), log_time.GetMillisecond());
	logMsg << msg << wxT('\n');
	m_PTextCtrl->AppendText(logMsg);

	if (m_Fp && m_LogToFile)
	{
		fputs(logMsg.mb_str(), m_Fp);
		fflush(m_Fp);
	}
}

void mafWXLog::LogToFile(bool on)
{
	m_LogToFile = on;
}

int mafWXLog::SetFileName(wxString filename)
{
	if (m_Fp)
	{
		fclose(m_Fp);
		m_Fp = nullptr;
	}
	m_Fp = fopen(filename.c_str(), "w");
	return m_Fp != nullptr ? MAF_OK : MAF_ERROR;
}
