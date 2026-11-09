module mini.d3d12;

import :event;

namespace mini::d3d12 {

Event::Event(PtrView<Device> device)
    : m_signaledValue(0)
{
    ID3D12Fence* fence;
    ENSURE(device->d3d12Device()->CreateFence(0, D3D12_FENCE_FLAG_NONE, COM_ARG(&fence)),
           "failed to create fence object") {
        return;
    }

    m_fence = transfer(fence);
}

bool Event::signal(uint64 value)
{
    if (m_signaledValue >= value) {
        return false;
    }

    m_fence->Signal(value);
    m_signaledValue = value;
    return true;
}

uint64 Event::signaledValue()
{
    m_signaledValue = m_fence->GetCompletedValue();
    return m_signaledValue;
}

SharedEvent::SharedEvent(PtrView<Device> device)
    : Event(device)
{
    m_eventHandle = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    ENSURE(m_eventHandle, "failed to create event object") {
        return;
    }
}

SharedEvent::~SharedEvent() noexcept
{
    if (m_eventHandle == nullptr) {
        return;
    }

    CloseHandle(m_eventHandle);
    m_eventHandle = nullptr;
}

void SharedEvent::wait(uint64 value, Milliseconds timeout)
{
    if (m_signaledValue >= value) {
        return;
    }

    if (signaledValue() >= value) {
        return;
    }

    DWORD tick = static_cast<DWORD>(max(timeout.count(), 0));
    m_fence->SetEventOnCompletion(value, m_eventHandle);
    WaitForSingleObject(m_eventHandle, tick);
}

} // namespace mini::d3d12