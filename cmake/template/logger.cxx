export module @target@:log;

@core_import@

namespace @prefix@::@api@ {

export @api_upper@_API ::mini::Logger logger("@camel_case@");

using Level = ::mini::Logger::Level;
using MessageContext = ::mini::Logger::MessageContext;

template <typename... Args>
inline void Log(Level level, MessageContext context, Args&&... args)
{
    logger.Message(level, context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void LogDebug(MessageContext context, Args&&... args)
{
    logger.Debug(context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void Log(MessageContext context, Args&&... args)
{
    logger.Log(context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void LogWarning(MessageContext context, Args&&... args)
{
    logger.Warn(context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void LogError(MessageContext context, Args&&... args)
{
    logger.Error(context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void LogFatal(MessageContext context, Args&&... args)
{
    logger.Fatal(context, ForwardArg<Args>(args)...);
}

} // namespace @prefix@::@api@