export module mini.core:interface;

import :module_system;
import :module_loader;

namespace mini {

class CORE_API Core final : public ModuleInterface {
public:
    ~Core() noexcept final { ASSERT(g_ModuleLoader.Count() == 0, "module remaining"); }
};

} // namespace mini