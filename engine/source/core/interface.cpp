module mini.core;

import :module_system;
import :module_initializer;

#if CORE_STATIC
extern "C" void __mini_core_start_module()
{
    auto loader = []() constexpr static -> mini::ModuleInterface* { return new mini::Core(); };
    mini::StaticModuleInitializer::Register("mini.core", loader);
}
#else
extern "C" CORE_API mini::ModuleInterface* __start_module()
{
    return new mini::Core();
}
#endif