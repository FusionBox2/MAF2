#include "ftk/Base/Log.h"

#ifdef MAF_USE_WX
#ifdef WX_PRECOMP
#include "wx/wxprec.h"  //originally was the first line of any lal cpp file 
#else
#include "wx/wx.h"      //originally was the first line of any lal header file 
#endif
#endif


#include <mutex>

void GetDefaultLogger()
{
	
}

#define BUFFER_DIMENSION 4096

static std::mutex mafLogMutex;
static char mafLogBuffer[BUFFER_DIMENSION];

mafMessageBuf _M(const char* s)
{
    return mafMessageBuf(s);
}

//------------------------------------------------------------------------------
void mafLogMessage(mafMessageBuf msg)
//------------------------------------------------------------------------------
{

    mafLogMutex.lock();
    wxSnprintf(mafLogBuffer, BUFFER_DIMENSION, wxT("%s"), msg.GetBuf());

#ifdef MAF_USE_WX
    wxString logStr;
    logStr.Append(wxString::Format(wxT("%s"), msg.GetBuf()));
    if (logStr.size() < sizeof(mafLogBuffer))
        wxLogMessage(mafLogBuffer);
    else
    {
        long times;
        times = logStr.size() / BUFFER_DIMENSION;
        for (int count = 0; count < times + 1; count++)
        {
            wxString temporary;
            temporary.Append(logStr.SubString(count * BUFFER_DIMENSION, (count + 1) * BUFFER_DIMENSION));
            wxLogMessage(temporary);
        }
    }
#else
    cerr << mafLogBuffer;
#endif

    mafLogMutex.unlock();
}
//------------------------------------------------------------------------------
// open a warning dialog and write a message
void mafWarningMessage(mafMessageBuf msg)
//------------------------------------------------------------------------------
{
    mafLogMutex.lock();
    wxSnprintf(mafLogBuffer, BUFFER_DIMENSION, wxT("%s"), msg.GetBuf());

#ifdef MAF_USE_WX
    wxMessageBox(mafLogBuffer, wxT("Warning Message"), wxOK | wxICON_WARNING);
#else
    cerr << "Warning: " << mafLogBuffer;
#endif

    mafLogMutex.unlock();
}

//------------------------------------------------------------------------------
// open an error dialog and write a message
void mafErrorMessage(mafMessageBuf msg)
//------------------------------------------------------------------------------
{
    mafLogMutex.lock();
    wxSnprintf(mafLogBuffer, BUFFER_DIMENSION, wxT("%s"), msg.GetBuf());

#ifdef MAF_USE_WX
    wxMessageBox(mafLogBuffer, wxT("Error Message"), wxOK | wxICON_ERROR);
#else
    cerr << "Error:" << mafLogBuffer;
#endif

    mafLogMutex.unlock();
}

//------------------------------------------------------------------------------
// open a message dialog and write a message
void mafMessage(mafMessageBuf msg)
//------------------------------------------------------------------------------
{
    mafLogMutex.lock();
    wxSnprintf(mafLogBuffer, BUFFER_DIMENSION, wxT("%s"), msg.GetBuf());

#ifdef MAF_USE_WX
    wxMessageBox(mafLogBuffer, wxT("Information"), wxOK | wxICON_INFORMATION);
#else
    cerr << mafLogBuffer;
#endif

    mafLogMutex.unlock();
}


