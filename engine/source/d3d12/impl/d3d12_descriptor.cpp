module mini.d3d12;

import mini.core;
import mini.windows;
import :descriptor;

namespace mini::d3d12 {

DescriptorHeap::DescriptorHeap(SharedPtr<ID3D12DescriptorHeap>&& heap, uint32 incrementSize)
    : m_heap(MoveArg(heap))
    , m_cpuHeapStart{ .ptr = 0 }
    , m_incrementSize(incrementSize)
    , m_currentIndex(0)
{
    ASSERT(m_heap, "invalid heap with nullptr");
    ASSERT(m_incrementSize > 0, "invalid heap incremetal size");

    m_cpuHeapStart = m_heap->GetCPUDescriptorHandleForHeapStart();
}

bool DescriptorHeap::Allocate(CPUOffsetT& offset)
{
    offset = GetOffset(m_currentIndex++);
    return true;

    // if (m_freeList.size() > 0)
    //{
    // uint32 index = m_freeList.front();
    // m_freeList.pop();

    // offset = GetOffset(index, incrementSize);
    // return true;
    //}
}

void DescriptorHeap::Deallocate(CPUOffsetT)
{
    // TODO
}

DescriptorHeap::CPUOffsetT DescriptorHeap::GetOffset(uint32 index) const noexcept
{
    SIZE_T ptr = m_cpuHeapStart.ptr + ((SIZE_T)index * (SIZE_T)m_incrementSize);
    return D3D12_CPU_DESCRIPTOR_HANDLE{ .ptr = ptr };
}

DescriptorAllocator::DescriptorAllocator(HeapTypeT type, uint32 size)
    : m_device(nullptr)
    , m_heapList(1)
    , m_heapDesc{}
    , m_incrementSize(0)
{
    m_heapDesc.Type = type;
    m_heapDesc.NumDescriptors = size;
    m_heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
}

bool DescriptorAllocator::Initialize(ID3D12Device* device)
{
    ASSERT(device);

    m_device = SharedPtr<ID3D12Device>(device);
    m_incrementSize = m_device->GetDescriptorHandleIncrementSize(m_heapDesc.Type);

    return AllocateHeap();
}

Descriptor DescriptorAllocator::Allocate()
{
    CPUOffsetT offset = { .ptr = 0 };
    for (SizeT i = 0; i < m_heapList.Size(); ++i) {
        if (m_heapList[i].Allocate(offset)) {
            return { .offset = offset, .heapIndex = i };
        }
    }

    ASSERT(false, "failed to allocate descriptor");
    return { .offset = offset, .heapIndex = SizeT(-1) };
}

void DescriptorAllocator::Deallocate(Descriptor desc)
{
    ASSERT(m_heapList.Size() > desc.heapIndex);
    m_heapList[desc.heapIndex].Deallocate(desc.offset);
}

bool DescriptorAllocator::AllocateHeap()
{
    SharedPtr<ID3D12DescriptorHeap> heap;
    ENSURE(m_device->CreateDescriptorHeap(&m_heapDesc, IID_PPV_ARGS(&heap))) {
        return false;
    }

    m_heapList.Push(MoveArg(heap), m_incrementSize);
    return true;
}

} // namespace mini::d3d12