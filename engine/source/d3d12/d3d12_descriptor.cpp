module;

#include "assertion.h"

module mini.d3d12;

import mini.core;
import mini.windows;
import :descriptor;

namespace mini::d3d12 {

DescriptorHeap::DescriptorHeap(SharedPtr<ID3D12DescriptorHeap>&& heap, uint32 incrementSize)
    : m_Heap(MoveArg(heap))
    , m_CpuHeapStart{ .ptr = 0 }
    , m_IncrementSize(incrementSize)
    , m_CurrentIndex(0)
{
    ASSERT(m_Heap, "invalid heap with nullptr");
    ASSERT(m_IncrementSize > 0, "invalid heap incremetal size");

    m_CpuHeapStart = m_Heap->GetCPUDescriptorHandleForHeapStart();
}

bool DescriptorHeap::Allocate(CPUOffsetT& offset)
{
    offset = GetOffset(m_CurrentIndex++);
    return true;

    // if (m_FreeList.size() > 0)
    //{
    // uint32 index = m_FreeList.front();
    // m_FreeList.pop();

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
    SIZE_T ptr = m_CpuHeapStart.ptr + ((SIZE_T)index * (SIZE_T)m_IncrementSize);
    return D3D12_CPU_DESCRIPTOR_HANDLE{ .ptr = ptr };
}

DescriptorAllocator::DescriptorAllocator(HeapTypeT type, uint32 size)
    : m_Device(nullptr)
    , m_HeapList(1)
    , m_HeapDesc{}
    , m_IncrementSize(0)
{
    m_HeapDesc.Type = type;
    m_HeapDesc.NumDescriptors = size;
    m_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
}

bool DescriptorAllocator::Initialize(ID3D12Device* device)
{
    ASSERT(device);

    m_Device = SharedPtr<ID3D12Device>(device);
    m_IncrementSize = m_Device->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);

    return AllocateHeap();
}

Descriptor DescriptorAllocator::Allocate()
{
    CPUOffsetT offset = { .ptr = 0 };
    for (SizeT i = 0; i < m_HeapList.Size(); ++i) {
        if (m_HeapList[i].Allocate(offset)) {
            return { .offset = offset, .heapIndex = i };
        }
    }

    ASSERT(false, "failed to allocate descriptor");
    return { .offset = offset, .heapIndex = SizeT(-1) };
}

void DescriptorAllocator::Deallocate(Descriptor desc)
{
    ASSERT(m_HeapList.Size() > desc.heapIndex);
    m_HeapList[desc.heapIndex].Deallocate(desc.offset);
}

bool DescriptorAllocator::AllocateHeap()
{
    SharedPtr<ID3D12DescriptorHeap> heap;
    ENSURE(m_Device->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(&heap)))
    {
        return false;
    }

    m_HeapList.Push(MoveArg(heap), m_IncrementSize);
    return true;
}

} // namespace mini::d3d12