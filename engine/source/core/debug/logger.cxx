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
    void Log(Level level, MessageContext context, Args&&... args);

    template <typename... Args>
    void Debug(MessageContext context, Args&&... args);

    template <typename... Args>
    void Info(MessageContext context, Args&&... args);

    template <typename... Args>
    void Warn(MessageContext context, Args&&... args);

    template <typename... Args>
    void Error(MessageContext context, Args&&... args);

    template <typename... Args>
    void Fatal(MessageContext context, Args&&... args);
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
inline void Logger::Log(Level level, MessageContext context, Args&&... args)
{
    String log;
    if constexpr (sizeof...(args) == 0) {
        log.Append(context.message);
    } else {
        FormatTo(log, context.message, ForwardArg<Args>(args)...);
    }

    LoggerBase::PrintMessage(static_cast<byte>(level), log);
}

template <typename... Args>
inline void Logger::Debug([[maybe_unused]] MessageContext context, [[maybe_unused]] Args&&... args)
{
#if !NODEBUGLOG
    Log(Level::debug, context, ForwardArg<Args>(args)...);
#endif
}

template <typename... Args>
inline void Logger::Info(MessageContext context, Args&&... args)
{
    Log(Level::info, context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void Logger::Warn(MessageContext context, Args&&... args)
{
    Log(Level::warn, context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void Logger::Error(MessageContext context, Args&&... args)
{
    Log(Level::error, context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void Logger::Fatal(MessageContext context, Args&&... args)
{
    Log(Level::fatal, context, ForwardArg<Args>(args)...);
}

} // namespace mini
