export module mini.core:logger_base;

import :type;
import :string;
import :string_view;

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