import mini.modules;
import @target@;

namespace {

struct InterfaceFactory {
private:
    typedef @class@ Interface;

    static_assert(mini::DerivedFromT<Interface, mini::ModuleInterface>,
                  "module interface must inherit from mini::ModuleInterface class.");

    static_assert(mini::DefaultConstructibleT<Interface>,
                  "non default constructible module inteface must be generated manually. "
                  "consider passing NO_MODULE_ENTRY when adding the module.");

public:
    mini::ModuleInterface* operator()() 
    { 
        if constexpr (@null_interface@) return nullptr;
        return new Interface(); 
    }
};

} // namespace

#if @api_upper@_STATIC
void __@api_full@_start_module()
{
    mini::StaticModuleInitializer<InterfaceFactory>::Register("@api@");
}
#else
extern "C" @api_upper@_API mini::ModuleInterface* __start_module()
{
    return InterfaceFactory{}();
}
#endif // @api_upper@_STATIC