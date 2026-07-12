#pragma once

#include "ftkConfigure.h"

#include <sstream>

class wxString;
BEGIN_FTK_NAMESPACE

namespace logging
{
    class FTK_BASE_EXPORT MessageBuf
    {
    public:
        explicit MessageBuf(const char* buf)
            : m_buf(buf)
        {
        }
        const char* GetBuf() const { return m_buf; }
        MessageBuf& operator=(const MessageBuf& other)
        {
            m_buf = other.m_buf;
            return *this;
        }
    private:
        MessageBuf(const wxString& str);
        MessageBuf& operator=(const wxString& str);
        MessageBuf& operator=(const char* buf);
        const char* m_buf;
    };
}

END_FTK_NAMESPACE

using mafMessageBuf = logging::MessageBuf;
FTK_BASE_EXPORT mafMessageBuf _M(const char* s);

/** write a message in the log area */
FTK_BASE_EXPORT void mafLogMessage(mafMessageBuf msg);
/** open a warning dialog and write a message */
FTK_BASE_EXPORT void mafWarningMessage(mafMessageBuf msg);
/** open an error dialog and write a message */
FTK_BASE_EXPORT void mafErrorMessage(mafMessageBuf msg);
/** open a message dialog and write a message */
FTK_BASE_EXPORT void mafMessage(mafMessageBuf msg);

/**
  Macro for printing Warning messages in log area. This macro also
  displays line at which error was printed. */
#define mafWarningMacro(x) \
{ \
  std::stringstream msg; \
  msg << "Warning in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
  mafLogMessage(_M(msg.str().c_str()));\
}

  /**
    Macro for printing Error messages in log area. This macro also
    displays line at which error was printed. */
#define mafErrorMacro(x) \
{ \
  std::stringstream msg; \
  msg << "Error in: " __FILE__ ", line " << __LINE__ << "\n" x \
    << "\n"; \
  mafLogMessage(_M(msg.str().c_str()));\
}

    /**
      Macro for displaying Warning messages.*/
#define mafWarningMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  mafWarningMessage(_M(msg.str().c_str()));\
}

      /**
        Macro for displaying Error messages. */
#define mafErrorMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  mafErrorMessage(_M(msg.str().c_str()));\
}

        /**
          Macro for displaying messages. */
#define mafMessageMacro(x) \
{ \
  std::stringstream msg; \
  msg << x << "\n"; \
  mafMessage(_M(msg.str().c_str()));\
}
