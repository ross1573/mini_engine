module mini.metal4;

import :common;

namespace mini::metal4 {

LogState::LogState(Device* device, Logger::Level logLevel, LogHandler logHandler, size_t bufferSize)
{
    Init(device, logLevel, bufferSize);

    m_logState->addLogHandler(logHandler);
}

void LogState::Init(Device* device, Logger::Level logLevel, size_t bufferSize)
{
    ASSERT(device);

    SharedPtr<MTL::LogStateDescriptor> desc = TransferShared(MTL::LogStateDescriptor::alloc());
    ENSURE(desc, "failed to allocate MTL::LogStateDescriptor") {
        return;
    }

    desc->init();
    desc->setLevel(MTLLogLevel(logLevel));
    desc->setBufferSize(static_cast<int64>(bufferSize));

    NS::Error* error;
    MTL::LogState* logState = device->MTLDevice()->newLogState(desc.Get(), memory::AddressOf(error));
    ENSURE(logState, error, "failed to create MTL::LogState") {
        return;
    }

    m_logLevel = logLevel;
    m_logState = TransferShared(logState);
}

} // namespace mini::metal4