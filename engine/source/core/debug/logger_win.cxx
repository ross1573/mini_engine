export module mini.core:logger_platform;

import :type;
import :string;

namespace mini {

class CORE_API LoggerBase {
private:
    String m_Category;

protected:
    LoggerBase(StringView);
    ~LoggerBase() noexcept = default;

    void PrintMessage(byte, StringView);
};

} // namespace mini