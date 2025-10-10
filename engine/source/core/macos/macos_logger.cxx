module;

#include <os/log.h>

export module mini.core:logger_base;

import :type;
import :utility;
import :string;
import :string_view;

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