module @target@;

import mini.core;

namespace {

struct InterfaceFactory {
private:
    typedef @class@ Interface;

    static_assert(mini::DerivedFromT<Interface, mini::Module::Interface>,
                  "module interface must inherit from mini::Module::Interface class.");

    static_assert(mini::AbstractT<Interface> == false, 
                  "module interface cannot be an abstract type. " 
                  "consider passing NO_MODULE_ENTRY when adding the module");

    static_assert(mini::DefaultConstructibleT<Interface>,
                  "non default constructible module inteface must be generated manually. "
                  "consider passing NO_MODULE_ENTRY when adding the module.");

public:
    mini::Module::Interface* operator()() 
    { 
        if constexpr (@null_interface@) return nullptr;
        return new Interface(); 
    }
};

} // namespace

#if @api_upper@_STATIC
extern "C" void __@api_full@_start_module()
{
    mini::StaticModuleInitializer<InterfaceFactory>::Register("@api@");
}
#else
extern "C" @api_upper@_API mini::Module::Interface* __start_module()
{
    return InterfaceFactory{}();
}
#endif // @api_upper@_STATIC