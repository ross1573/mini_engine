export module @target@:log;

import mini.core;

namespace @prefix@::@api@ {

@api_define@::mini::Logger logger("@camel_case@");

using Level = ::mini::Logger::Level;
using MessageContext = ::mini::Logger::MessageContext;

export template <typename... Args>
inline void Log(Level level, MessageContext context, Args&&... args)
{
    logger.Log(level, context, ForwardArg<Args>(args)...);
}

export template <typename... Args>
inline void LogDebug(MessageContext context, Args&&... args)
{
    logger.Debug(context, ForwardArg<Args>(args)...);
}

export template <typename... Args>
inline void LogInfo(MessageContext context, Args&&... args)
{
    logger.Info(context, ForwardArg<Args>(args)...);
}

export template <typename... Args>
inline void LogWarning(MessageContext context, Args&&... args)
{
    logger.Warn(context, ForwardArg<Args>(args)...);
}

export template <typename... Args>
inline void LogError(MessageContext context, Args&&... args)
{
    logger.Error(context, ForwardArg<Args>(args)...);
}

export template <typename... Args>
inline void LogFatal(MessageContext context, Args&&... args)
{
    logger.Fatal(context, ForwardArg<Args>(args)...);
}

} // namespace @prefix@::@api@