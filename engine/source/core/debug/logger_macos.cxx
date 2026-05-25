module;

#include <os/log.h>

export module mini.core:logger_platform;

import :type;
import :string;

namespace mini {

class CORE_API LoggerBase {
private:
    typedef os_log_t Logger;
    typedef os_log_type_t LogLevel;

    String m_category;
    Logger m_logger;

protected:
    LoggerBase(StringView);
    ~LoggerBase();

    void PrintMessage(byte, StringView);

private:
    LogLevel GetLogType(byte);
};

} // namespace mini