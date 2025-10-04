export module mini.macos:modules;

import mini.core;
import mini.platform;

namespace mini::macos {

export class MACOS_API Module final : public platform::Module {
private:
    String m_Name;
    void* m_Handle;

public:
    Module(StringView);
    ~Module() final;

    StringView GetName() const final { return m_Name; }

protected:
    void* LoadFunc(StringView) final;
};

} // namespace mini::macos