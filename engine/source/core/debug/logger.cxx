export module mini.core:logger;

import :type;
import :utility_operation;
import :source_location;
import :string;
import :logger_platform;

namespace mini {

export class CORE_API Logger final : public LoggerBase {
public:
    enum class Level : byte {
        debug = 0,
        info = 1,
        warn = 2,
        error = 3,
        fatal = 4,
    };

    struct MessageContext {
        StringView const message;
        SourceLocation const location;

        template <StringLikeT<char> T>
        MessageContext(T const& msg, SourceLocation loc = SourceLocation::current()) noexcept;
    };

public:
    Logger(StringView category) noexcept;

    template <typename... Args>
    void log(Level level, MessageContext context, Args&&... args);

    template <typename... Args>
    void debug(MessageContext context, Args&&... args);

    template <typename... Args>
    void info(MessageContext context, Args&&... args);

    template <typename... Args>
    void warn(MessageContext context, Args&&... args);

    template <typename... Args>
    void error(MessageContext context, Args&&... args);

    template <typename... Args>
    void fatal(MessageContext context, Args&&... args);
};

template <StringLikeT<char> T>
Logger::MessageContext::MessageContext(T const& msg, SourceLocation loc) noexcept
    : message(msg)
    , location(loc)
{
}

Logger::Logger(StringView category) noexcept
    : LoggerBase(category)
{
}

template <typename... Args>
inline void Logger::log(Level level, MessageContext context, Args&&... args)
{
    String log;
    if constexpr (sizeof...(args) == 0) {
        log.append(context.message);
    } else {
        formatTo(log, context.message, forwardCast<Args>(args)...);
    }

    LoggerBase::printMessage(static_cast<byte>(level), log);
}

template <typename... Args>
inline void Logger::debug([[maybe_unused]] MessageContext context, [[maybe_unused]] Args&&... args)
{
#if !NODEBUGLOG
    log(Level::debug, context, forwardCast<Args>(args)...);
#endif
}

template <typename... Args>
inline void Logger::info(MessageContext context, Args&&... args)
{
    log(Level::info, context, forwardCast<Args>(args)...);
}

template <typename... Args>
inline void Logger::warn(MessageContext context, Args&&... args)
{
    log(Level::warn, context, forwardCast<Args>(args)...);
}

template <typename... Args>
inline void Logger::error(MessageContext context, Args&&... args)
{
    log(Level::error, context, forwardCast<Args>(args)...);
}

template <typename... Args>
inline void Logger::fatal(MessageContext context, Args&&... args)
{
    log(Level::fatal, context, forwardCast<Args>(args)...);
}

} // namespace mini
