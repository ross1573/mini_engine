module mini.metal4;

import :common;

namespace mini::metal4 {

LogState::LogState(PtrView<Device> device, Logger::Level level, LogHandler logHandler, size_t bufferSize)
{
    ASSERT(device);

    SharedPtr<MTL::LogStateDescriptor> desc = transfer(MTL::LogStateDescriptor::alloc());
    ENSURE(desc, "failed to allocate MTL::LogStateDescriptor") {
        return;
    }

    desc->init();
    desc->setLevel(mtlLogLevel(level));
    desc->setBufferSize(static_cast<int64>(bufferSize));

    NS::Error* error;
    MTL::LogState* logState = device->mtlDevice()->newLogState(desc.get(), addressof(error));
    ENSURE(logState, error, "failed to create MTL::LogState") {
        return;
    }

    m_logLevel = level;
    m_handler = logHandler;
    m_logState = transfer(logState);

    auto blockHandler = ^(NS::String* subSystem, NS::String* category, MTL::LogLevel logLevel, NS::String* message) {
        handleLog(subSystem, category, logLevel, message);
    };

    m_logState->addLogHandler(blockHandler);
}

void LogState::handleLog(NS::String* subSystem, NS::String* category, MTL::LogLevel logLevel, NS::String* message)
{
    if (m_handler == nullptr) {
        return;
    }

    m_handler(toString(subSystem), toString(category), metal4::logLevel(logLevel), toString(message));
}

} // namespace mini::metal4