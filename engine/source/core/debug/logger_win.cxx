export module mini.debug:base;

import mini.type;
import mini.string;

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