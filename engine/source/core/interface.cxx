export module mini.core:interface;

import :module_system;
import :module_loader;

namespace mini::core {

class CORE_API Interface final : public Module::Interface {
public:
    Interface() noexcept;
    ~Interface() noexcept final;

private:
    bool Initialize() final;
};

Interface::Interface() noexcept
{
}

Interface::~Interface() noexcept
{
    ASSERT(g_ModuleLoader.Count() == 0, "module remaining");
}

bool Interface::Initialize()
{
    return true;
}

} // namespace mini::core