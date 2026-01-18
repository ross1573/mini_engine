export module mini.d3d12:descriptor;

import mini.core;
import mini.windows;

export namespace mini::d3d12 {

struct Descriptor {
    typedef D3D12_CPU_DESCRIPTOR_HANDLE CPUOffsetT;

    CPUOffsetT offset;
    SizeT heapIndex;
};

class DescriptorHeap {
public:
    typedef D3D12_CPU_DESCRIPTOR_HANDLE CPUOffsetT;
    typedef D3D12_GPU_DESCRIPTOR_HANDLE GPUOffsetT;

private:
    SharedPtr<ID3D12DescriptorHeap> m_heap;
    CPUOffsetT m_cpuHeapStart;
    uint32 m_incrementSize;
    uint32 m_currentIndex;

public:
    DescriptorHeap(SharedPtr<ID3D12DescriptorHeap>&&, uint32);

    bool Allocate(CPUOffsetT&);
    void Deallocate(CPUOffsetT);

    CPUOffsetT GetOffset(uint32) const noexcept;
};

class DescriptorAllocator {
public:
    typedef D3D12_CPU_DESCRIPTOR_HANDLE CPUOffsetT;
    typedef D3D12_GPU_DESCRIPTOR_HANDLE GPUOffsetT;
    typedef D3D12_DESCRIPTOR_HEAP_TYPE HeapTypeT;
    typedef D3D12_DESCRIPTOR_HEAP_DESC HeapDescT;

private:
    SharedPtr<ID3D12Device> m_device;
    Array<DescriptorHeap> m_heapList;
    HeapDescT m_heapDesc;
    uint32 m_incrementSize;

public:
    DescriptorAllocator(HeapTypeT, uint32);

    bool Initialize(ID3D12Device*);

    Descriptor Allocate();
    void Deallocate(Descriptor);

    uint32 GetIncrementalSize() const { return m_incrementSize; }

private:
    bool AllocateHeap();
};

} // namespace mini::d3d12
