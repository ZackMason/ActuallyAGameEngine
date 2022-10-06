// Author: Zackery Mason-Blaug
// Date: 2022-10-04
//////////////////////////////////////////////////////////


#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if defined(CreateWindow)
#undef CreateWindow
#endif
 
// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>    

// D3D12 extension library.
#include "d3dx12.h"

#include <chrono>

#include "Util/exceptions.hpp"

#include "types.hpp"

namespace dx12_utl {
    struct dx12_state_t {
        explicit dx12_state_t(HWND hWnd, uint32_t w, uint32_t h);
        ComPtr<ID3D12Device2> device;
        ComPtr<ID3D12CommandQueue> command_queue;
        ComPtr<IDXGISwapChain4> swap_chain;
        ComPtr<ID3D12Resource> back_buffers[3];
        ComPtr<ID3D12GraphicsCommandList> command_list;
        ComPtr<ID3D12CommandAllocator> command_allocators[3];
        ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap;
        ComPtr<ID3D12Fence> fence;

        uint64_t fence_value = 0;
        uint64_t frame_fence_values[3] = {};
        HANDLE fence_event;

        uint32_t vsync{1};
        uint32_t tearing{0};
        uint32_t fullscreen{0};

        UINT rtv_descriptor_size;
        UINT current_back_buffer_index;

        decltype(auto) get_allocator_and_buffer() {
            return std::pair{command_allocators[current_back_buffer_index], back_buffers[current_back_buffer_index]};
        }

        void resize(uint32_t w, uint32_t h);
    };

    inline void throw_failed(HRESULT hr);

    void enable_debug_layer() ;
    ComPtr<IDXGIAdapter4> get_adapter(bool useWarp);

    ComPtr<ID3D12Device2> create_device(bool use_warp);

    ComPtr<ID3D12CommandQueue> create_command_queue(
        ComPtr<ID3D12Device2> device, 
        D3D12_COMMAND_LIST_TYPE type 
    );

    bool check_tearing_support();

    ComPtr<IDXGISwapChain4> create_swap_chain(
        HWND hWnd, 
        ComPtr<ID3D12CommandQueue> commandQueue, 
        uint32_t width, uint32_t height, uint32_t bufferCount
    );

    ComPtr<ID3D12DescriptorHeap> create_descriptor_heap(ComPtr<ID3D12Device2> device,
        D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors);

    void update_render_target_views(dx12_state_t& state);

    ComPtr<ID3D12CommandAllocator> create_command_allocator(ComPtr<ID3D12Device2> device,
        D3D12_COMMAND_LIST_TYPE type);

    ComPtr<ID3D12GraphicsCommandList> create_command_list(ComPtr<ID3D12Device2> device,
        ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);

    ComPtr<ID3D12Fence> create_fence(ComPtr<ID3D12Device2> device);

    HANDLE create_event_handle();

    uint64_t signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
        uint64_t& fenceValue);

    void wait_fence(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
        std::chrono::milliseconds duration = std::chrono::milliseconds::max() );

    void flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
        uint64_t& fenceValue, HANDLE fenceEvent);

    void clear(v4f color, dx12_state_t& state);

    void present(dx12_state_t& state);
}