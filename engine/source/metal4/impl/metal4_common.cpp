module mini.metal4;

import :common;

namespace mini::metal4 {

LogState::LogState(PtrView<Device> device, Logger::Level level, LogHandler logHandler, size_t bufferSize)
{
    ASSERT(device);

    SharedPtr<MTL::LogStateDescriptor> desc = TransferShared(MTL::LogStateDescriptor::alloc());
    ENSURE(desc, "failed to allocate MTL::LogStateDescriptor") {
        return;
    }

    desc->init();
    desc->setLevel(MTLLogLevel(level));
    desc->setBufferSize(static_cast<int64>(bufferSize));

    NS::Error* error;
    MTL::LogState* logState = device->MTLDevice()->newLogState(desc.Get(), memory::AddressOf(error));
    ENSURE(logState, error, "failed to create MTL::LogState") {
        return;
    }

    m_logLevel = level;
    m_handler = logHandler;
    m_logState = TransferShared(logState);

    auto blockHandler = ^(NS::String* subSystem, NS::String* category, MTL::LogLevel logLevel, NS::String* message) {
        HandleLog(subSystem, category, logLevel, message);
    };

    m_logState->addLogHandler(blockHandler);
}

void LogState::HandleLog(NS::String* subSystem, NS::String* category, MTL::LogLevel logLevel, NS::String* message)
{
    if (m_handler == nullptr) {
        return;
    }

    m_handler(ToString(subSystem), ToString(category), metal4::LogLevel(logLevel), ToString(message));
}

} // namespace mini::metal4