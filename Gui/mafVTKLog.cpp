#include "mafVTKLog.h"

#include "ftk/Base/mfString.h"

mafVTKLog* mafVTKLog::New()
{
	return new mafVTKLog;
}

mafVTKLog::mafVTKLog() = default;

mafVTKLog::~mafVTKLog() = default;

void mafVTKLog::DisplayText(const char* text)
{
	if (!text)
	{
		return;
	}

	if (!Enabled)
	{
		return;
	}
	mafString message = _R("[VTK]");
	message += _R(text);

	// Strip CR
	for (; message.ends_with(_R("\n"));)
	{
		message.erase(message.length() - 1);
	}

	mafLogMessage(_M(message));
}
