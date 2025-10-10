module;

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFString.h>
#include <os/log.h>

module mini.core;

import :string;
import :string_view;
import :format;
import :logger_base;

namespace mini {

String LoggerBase::m_Identifier = StringView::empty;

LoggerBase::LoggerBase(StringView category)
    : m_Category(category)
    , m_Logger(nullptr)
{
}

LoggerBase::~LoggerBase()
{
    os_release(m_Logger);
}

void LoggerBase::PrintMessage(byte level, StringView msg)
{
    // delay initialization of the logger until first message has been fetched
    if (m_Logger == nullptr) {
        StringView identifier = InitializeIdentifier();
        m_Logger = os_log_create(identifier.Data(), m_Category.Data());
    }

    LogLevel type = GetLogType(level);
    os_log_with_type(m_Logger, type, "%s", msg.Data());
}

LoggerBase::LogLevel LoggerBase::GetLogType(byte level)
{
    LogLevel type = OS_LOG_TYPE_DEFAULT;
    switch (level) {
        case 0: type = OS_LOG_TYPE_DEBUG; break;
        case 1: type = OS_LOG_TYPE_DEFAULT; break;
        case 2: type = OS_LOG_TYPE_ERROR; break;
        case 3:
        case 4: type = OS_LOG_TYPE_FAULT; break;
    }

    return type;
}

StringView LoggerBase::InitializeIdentifier()
{
    if (m_Identifier.IsEmpty() == false) {
        return m_Identifier;
    }

    String identifier = StringView::empty;
    uint32 encoding = kCFStringEncodingUTF8;
    bool result;

    CFIndex length, maxSize;
    CFBundleRef bundle = nullptr;
    CFStringRef bundleIdentifier = nullptr;

    bundle = CFBundleGetMainBundle();
    bundleIdentifier = bundle ? CFBundleGetIdentifier(bundle) : nullptr;
    if (bundleIdentifier == nullptr) {
        goto init_finish;
    }

    encoding = kCFStringEncodingUTF8;
    length = CFStringGetLength(bundleIdentifier);
    maxSize = CFStringGetMaximumSizeForEncoding(length, encoding) + 1;

    identifier.Resize(static_cast<SizeT>(maxSize));
    result = CFStringGetCString(bundleIdentifier, identifier.Data(), maxSize, encoding);
    if (result == false) {
        identifier.Clear();
    }

init_finish:
    if (bundle != nullptr) CFRelease(bundle);
    if (bundleIdentifier != nullptr) CFRelease(bundleIdentifier);

    if (identifier.IsEmpty() == false) {
        m_Identifier = MoveArg(identifier);
        return m_Identifier;
    }

    return ENGINE_PROJECT_NAME;
}

} // namespace mini