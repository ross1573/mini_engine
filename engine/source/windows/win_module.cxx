export module mini.windows:modules;

import mini.core;
import mini.platform;

namespace mini::windows {

export class WINDOWS_API Module final : public platform::Module {
private:
    String m_Name;
    HMODULE m_Handle;

public:
    Module(StringView);
    ~Module() final;

    StringView GetName() const final { return m_Name; }
    HMODULE GetNativeHandle() const { return m_Handle; }

protected:
    void* LoadFunc(StringView) final;
};

} // namespace mini::windows