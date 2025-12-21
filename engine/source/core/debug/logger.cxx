export module mini.debug:logger;

import mini.type;
import mini.utility;
import mini.string;
import :base;

namespace mini {

export class CORE_API Logger final : public LoggerBase {
public:
    enum class Level {
        info = 1,
        warn = 2,
        error = 3,
        fatal = 4,
    };

    struct MessageContext {
        StringView const message;
        SourceLocation const location;

        template <StringLikeT<char> T>
        MessageContext(T const&, SourceLocation = SourceLocation::current());
    };

public:
    Logger(StringView);

    template <typename... Args>
    void Message(Level, MessageContext, Args&&...);

    template <typename... Args>
    void Debug(MessageContext, Args&&...);

    template <typename... Args>
    void Log(MessageContext, Args&&...);

    template <typename... Args>
    void Warn(MessageContext, Args&&...);

    template <typename... Args>
    void Error(MessageContext, Args&&...);

    template <typename... Args>
    void Fatal(MessageContext, Args&&...);
};

template <StringLikeT<char> T>
Logger::MessageContext::MessageContext(T const& msg, SourceLocation loc)
    : message(msg)
    , location(loc)
{
}

Logger::Logger(StringView category)
    : LoggerBase(category)
{
}

template <typename... Args>
inline void Logger::Message(Level level, MessageContext context, Args&&... args)
{
    String log;
    if constexpr (sizeof...(args) == 0) {
        log.Append(context.message);
    }
    else {
        FormatTo(log, context.message, ForwardArg<Args>(args)...);
    }

    LoggerBase::PrintMessage(static_cast<byte>(level), log);
}

template <typename... Args>
inline void Logger::Debug([[maybe_unused]] MessageContext context, [[maybe_unused]] Args&&... args)
{
#if DEBUG
    Message(Level(0), context, ForwardArg<Args>(args)...);
#endif
}

template <typename... Args>
inline void Logger::Log(MessageContext context, Args&&... args)
{
    Message(Level::info, context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void Logger::Warn(MessageContext context, Args&&... args)
{
    Message(Level::warn, context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void Logger::Error(MessageContext context, Args&&... args)
{
    Message(Level::error, context, ForwardArg<Args>(args)...);
}

template <typename... Args>
inline void Logger::Fatal(MessageContext context, Args&&... args)
{
    Message(Level::fatal, context, ForwardArg<Args>(args)...);
}

} // namespace mini
