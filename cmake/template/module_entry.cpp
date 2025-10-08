import mini.core;
import @target@;

@api_upper@_HIDDEN @class@* __get_@api_full@_interface()
{
    static @class@* __module_interface = new @class@();
    return __module_interface;
}

#if @api_upper@_STATIC
void __@api_full@_init_module()
{
    using FactoryT = decltype([]() { return __get_@api_full@_interface(); });
    using DelT = mini::DefaultDeleter<mini::ModuleInterface>;
    mini::StaticModuleInitializer<@class@, FactoryT, DelT>::Register("@api@");
}
#else
extern "C" @api_upper@_API @class@* __start_module()
{
    return __get_@api_full@_interface();
}
#endif // @api_upper@_STATIC