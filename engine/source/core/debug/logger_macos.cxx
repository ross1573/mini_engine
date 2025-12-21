module;

#include <os/log.h>

export module mini.debug:base;

import mini.type;
import mini.string;

namespace mini {

class CORE_API LoggerBase {
private:
    typedef os_log_t Logger;
    typedef os_log_type_t LogLevel;

    String m_Category;
    Logger m_Logger;

    static CORE_API String m_Identifier;

protected:
    LoggerBase(StringView);
    ~LoggerBase();

    void PrintMessage(byte, StringView);

private:
    LogLevel GetLogType(byte);
    StringView InitializeIdentifier();
};

} // namespace mini