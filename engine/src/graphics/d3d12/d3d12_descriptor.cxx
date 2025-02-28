module;

#include <d3d12.h>

export module mini.d3d12:descriptor;

import mini.core;
import mini.windows;
import mini.container;

export namespace mini::d3d12
{

struct Descriptor
{
    typedef D3D12_CPU_DESCRIPTOR_HANDLE CPUOffsetT;

    CPUOffsetT offset;
    SizeT heapIndex;
};

class DescriptorHeap
{
public:
    typedef D3D12_CPU_DESCRIPTOR_HANDLE CPUOffsetT;
    typedef D3D12_GPU_DESCRIPTOR_HANDLE GPUOffsetT;

private:
    SharedPtr<ID3D12DescriptorHeap> m_Heap;
    CPUOffsetT m_CpuHeapStart;
    SizeT m_IncrementSize;
    SizeT m_CurrentIndex;

public:
    DescriptorHeap(SharedPtr<ID3D12DescriptorHeap>&&, uint32);

    bool Allocate(CPUOffsetT&);
    void Deallocate(CPUOffsetT);

    CPUOffsetT GetOffset(uint32) const noexcept;
};

class DescriptorAllocator
{
public:
    typedef D3D12_CPU_DESCRIPTOR_HANDLE CPUOffsetT;
    typedef D3D12_GPU_DESCRIPTOR_HANDLE GPUOffsetT;
    typedef D3D12_DESCRIPTOR_HEAP_TYPE HeapTypeT;
    typedef D3D12_DESCRIPTOR_HEAP_DESC HeapDescT;

private:
    SharedPtr<ID3D12Device> m_Device;
    Array<DescriptorHeap> m_HeapList;
    HeapDescT m_HeapDesc;
    uint32 m_IncrementSize;

public:
    DescriptorAllocator(HeapTypeT, uint32);

    bool Initialize(ID3D12Device*);

    Descriptor Allocate();
    void Deallocate(Descriptor);

    inline uint32 GetIncrementalSize() const { return m_IncrementSize; }

private:
    bool AllocateHeap();
};

} // namespace mini::d3d12
