#pragma once

#include "ftkConfigure.h"

#include <cstdint>
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

namespace logging
{
    namespace impl
    {
        struct Noop {};
    }

    enum class Level : std::uint8_t
	{
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warning = 3,
        Error = 4,
        Critical = 5,
        None = 6
    };

    void GetDefaultLogger();

	class LogStreamer
    {
    public:
        LogStreamer() = default;
        LogStreamer(LogStreamer&&) = delete;
        LogStreamer(const LogStreamer&) = delete;
        LogStreamer& operator=(LogStreamer&&) = delete;
        LogStreamer& operator=(const LogStreamer&) = delete;

        ~LogStreamer() = default;

        // Helper function that could be called on LogStreamer&& to get LogStreamer&.
        LogStreamer& AsLvalue() noexcept { return *this; }

		template<typename T>
        LogStreamer& operator<<(const T&) { return *this; }

		operator impl::Noop() const noexcept { return {}; }
    };
}

END_FTK_NAMESPACE

#define IMPL_LOG_TO(logger, level, ...)                                   \
    logging::LogStreamer().AsLvalue(__VA_ARGS__)

#define LOG_TO(logger, lvl, ...) true ? logging::impl::Noop{} : IMPL_LOG_TO((logger), (lvl), __VA_ARGS__)

#define LOG(lvl, ...) LOG_TO(logging::GetDefaultLogger(), (lvl), __VA_ARGS__)

#define LOG_TRACE(...) LOG(logging::Level::Trace, __VA_ARGS__)

#define LOG_DEBUG(...) LOG(logging::Level::Debug, __VA_ARGS__)

#define LOG_INFO(...) LOG(logging::Level::Info, __VA_ARGS__)

#define LOG_WARNING(...) LOG(logging::Level::Warning, __VA_ARGS__)

#define LOG_ERROR(...) LOG(logging::Level::Error, __VA_ARGS__)

#define LOG_CRITICAL(...) LOG(logging::Level::Critical, __VA_ARGS__)

#define LOG_TRACE_TO(logger, ...) LOG_TO(logger, logging::Level::Trace, __VA_ARGS__)

#define LOG_DEBUG_TO(logger, ...) LOG_TO(logger, logging::Level::Debug, __VA_ARGS__)

#define LOG_INFO_TO(logger, ...) LOG_TO(logger, logging::Level::Info, __VA_ARGS__)

#define LOG_WARNING_TO(logger, ...) LOG_TO(logger, logging::Level::Warning, __VA_ARGS__)

#define LOG_ERROR_TO(logger, ...) LOG_TO(logger, logging::Level::Error, __VA_ARGS__)

#define LOG_CRITICAL_TO(logger, ...) LOG_TO(logger, logging::Level::Critical, __VA_ARGS__)
