export module @target@:log;

import mini.core;

namespace @prefix@::@api@ {

@api_define@ ::mini::Logger logger("@camel_case@");

using Level = ::mini::Logger::Level;
using MessageContext = ::mini::Logger::MessageContext;

export template <typename... Args>
inline void log(Level level, MessageContext context, Args&&... args)
{
    logger.log(level, context, forwardCast<Args>(args)...);
}

export template <typename... Args>
inline void logDebug(MessageContext context, Args&&... args)
{
    logger.debug(context, forwardCast<Args>(args)...);
}

export template <typename... Args>
inline void logInfo(MessageContext context, Args&&... args)
{
    logger.info(context, forwardCast<Args>(args)...);
}

export template <typename... Args>
inline void logWarning(MessageContext context, Args&&... args)
{
    logger.warn(context, forwardCast<Args>(args)...);
}

export template <typename... Args>
inline void logError(MessageContext context, Args&&... args)
{
    logger.error(context, forwardCast<Args>(args)...);
}

export template <typename... Args>
inline void logFatal(MessageContext context, Args&&... args)
{
    logger.fatal(context, forwardCast<Args>(args)...);
}

} // namespace @prefix@::@api@