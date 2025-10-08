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
    mini::StaticModuleInitializer<@class@, FactoryT>::Register("@api@");
}

void __@api_full@_cleanup_module()
{
    mini::DefaultDeleter<@class@>{}(__get_@api_full@_interface());
}
#else
extern "C" @api_upper@_API @class@* __start_module()
{
    return __get_@api_full@_interface();
}
#endif // @api_upper@_STATIC