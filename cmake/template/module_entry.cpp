import mini.core;
import mini.@api@;

@api_upper@_HIDDEN @class@* Create@camel_case@ModuleInterface()
{
    return new @class@();
}

#if @api_upper@_STATIC
    static StaticModuleInitializer<@class@> _("@api@", Create@camel_case@ModuleInterface());
#else
    extern "C" @api_upper@_API @class@* __start_module()
    {
        return Create@camel_case@ModuleInterface();
    }
#endif // @api_upper@_STATIC