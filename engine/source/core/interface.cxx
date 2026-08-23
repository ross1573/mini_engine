export module mini.core:interface;

import :module_system;
import :module_loader;

namespace mini {

export class CORE_API Core final : public ModuleInterface {
public:
    ~Core() noexcept
    {
        [[maybe_unused]] size_t remainingCount = g_moduleLoader.Count();
        ASSERT(remainingCount == 0, "{} module remaining", remainingCount);
    }
};

} // namespace mini