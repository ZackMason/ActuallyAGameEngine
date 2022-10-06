// Author: Zackery Mason-Blaug
// Date: 2022-10-04
//////////////////////////////////////////////////////////


#include "Graphics/DX12/dx12_core.hpp"

namespace dx12_utl {
     inline void throw_failed(HRESULT hr) {
        if (FAILED(hr)) {
            throw std::exception();
        }
    }

     void enable_debug_layer() {
    #if defined(_DEBUG)
        // Always enable the debug layer before doing anything DX12 related
        // so all possible errors generated while creating DX12 objects
        // are caught by the debug layer.
        ComPtr<ID3D12Debug> debugInterface;
        throw_failed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
        debugInterface->EnableDebugLayer();
    #endif
    }


     ComPtr<IDXGIAdapter4> get_adapter(bool useWarp)
    {
        ComPtr<IDXGIFactory4> dxgiFactory;
        UINT createFactoryFlags = 0;
    #if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
    #endif
    
        throw_failed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

        ComPtr<IDXGIAdapter1> dxgiAdapter1;
        ComPtr<IDXGIAdapter4> dxgiAdapter4;
    
        if (useWarp)
        {
            throw_failed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
            throw_failed(dxgiAdapter1.As(&dxgiAdapter4));
        }
            else
        {
            SIZE_T maxDedicatedVideoMemory = 0;
            for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
                dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);
    
                // Check to see if the adapter can create a D3D12 device without actually 
                // creating it. The adapter with the largest dedicated video memory
                // is favored.
                if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                    SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), 
                        D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) && 
                    dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory )
                {
                    maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                    throw_failed(dxgiAdapter1.As(&dxgiAdapter4));
                }
            }
        }
    
        return dxgiAdapter4;
    }

     ComPtr<ID3D12Device2> create_device(bool use_warp) {
        ComPtr<IDXGIAdapter4> adapter = get_adapter(use_warp);
        ComPtr<ID3D12Device2> dx12_device_2;
        throw_failed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&dx12_device_2)));

           // Enable debug messages in debug mode.
    #if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> p_info_queue;
        if (SUCCEEDED(dx12_device_2.As(&p_info_queue)))
        {
            p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            p_info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
        
            D3D12_MESSAGE_SEVERITY Severities[] =
            {
                D3D12_MESSAGE_SEVERITY_INFO
            };
    
            // Suppress individual messages by their ID
            D3D12_MESSAGE_ID DenyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
            };
    
            D3D12_INFO_QUEUE_FILTER NewFilter = {};
            //NewFilter.DenyList.NumCategories = _countof(Categories);
            //NewFilter.DenyList.pCategoryList = Categories;
            NewFilter.DenyList.NumSeverities = _countof(Severities);
            NewFilter.DenyList.pSeverityList = Severities;
            NewFilter.DenyList.NumIDs = _countof(DenyIds);
            NewFilter.DenyList.pIDList = DenyIds;
    
            throw_failed(p_info_queue->PushStorageFilter(&NewFilter));
        }
    #endif
    
        return dx12_device_2;
    }

     ComPtr<ID3D12CommandQueue> create_command_queue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type ) {
        ComPtr<ID3D12CommandQueue> d3d12CommandQueue;
    
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type =     type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags =    D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;
    
        throw_failed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));
    
        return d3d12CommandQueue;
    }

     bool check_tearing_support() {
        BOOL allowTearing = FALSE;
    
        // Rather than create the DXGI 1.5 factory interface directly, we create the
        // DXGI 1.4 interface and query for the 1.5 interface. This is to enable the 
        // graphics debugging tools which will not support the 1.5 factory interface 
        // until a future update.
        ComPtr<IDXGIFactory4> factory4;
        if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
        {
            ComPtr<IDXGIFactory5> factory5;
            if (SUCCEEDED(factory4.As(&factory5)))
            {
                if (FAILED(factory5->CheckFeatureSupport(
                    DXGI_FEATURE_PRESENT_ALLOW_TEARING, 
                    &allowTearing, sizeof(allowTearing))))
                {
                    allowTearing = FALSE;
                }
            }
        }
    
        return allowTearing == TRUE;
    }

     ComPtr<IDXGISwapChain4> create_swap_chain(
        HWND hWnd, 
        ComPtr<ID3D12CommandQueue> commandQueue, 
        uint32_t width, uint32_t height, uint32_t bufferCount
    ) {
        ComPtr<IDXGISwapChain4> dxgiSwapChain4;
        ComPtr<IDXGIFactory4> dxgiFactory4;
        UINT createFactoryFlags = 0;
    #if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
    #endif
    
        throw_failed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc = { 1, 0 };
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = bufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        // It is recommended to always allow tearing if tearing support is available.
        swapChainDesc.Flags = check_tearing_support() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;


        ComPtr<IDXGISwapChain1> swapChain1;
        throw_failed(dxgiFactory4->CreateSwapChainForHwnd(
            commandQueue.Get(),
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1));
    
        // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
        // will be handled manually.
        throw_failed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
    
        throw_failed(swapChain1.As(&dxgiSwapChain4));
    
        return dxgiSwapChain4;
    }

     ComPtr<ID3D12DescriptorHeap> create_descriptor_heap(ComPtr<ID3D12Device2> device,
        D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors)
    {
        ComPtr<ID3D12DescriptorHeap> descriptorHeap;
    
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = numDescriptors;
        desc.Type = type;
    
        throw_failed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));
    
        return descriptorHeap;
    }

     void update_render_target_views(dx12_state_t& state)
    {
        auto rtvDescriptorSize = state.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(state.rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
    
        for (int i = 0; i < 3; ++i)
        {
            ComPtr<ID3D12Resource> backBuffer;
            throw_failed(state.swap_chain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
    
            state.device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
    
            state.back_buffers[i] = backBuffer;
    
            rtvHandle.Offset(rtvDescriptorSize);
        }
    }

     ComPtr<ID3D12CommandAllocator> create_command_allocator(ComPtr<ID3D12Device2> device,
        D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        throw_failed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
    
        return commandAllocator;
    }

     ComPtr<ID3D12GraphicsCommandList> create_command_list(ComPtr<ID3D12Device2> device,
        ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
    {
        ComPtr<ID3D12GraphicsCommandList> commandList;
        throw_failed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        
        throw_failed(commandList->Close());
    
        return commandList;
    }

     ComPtr<ID3D12Fence> create_fence(ComPtr<ID3D12Device2> device)
    {
        ComPtr<ID3D12Fence> fence;
    
        throw_failed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    
        return fence;
    }

        
     HANDLE create_event_handle()
    {
        HANDLE fenceEvent;
        
        fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(fenceEvent && "Failed to create fence event.");
    
        return fenceEvent;
    }

     uint64_t signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
        uint64_t& fenceValue)
    {
        uint64_t fenceValueForSignal = ++fenceValue;
        throw_failed(commandQueue->Signal(fence.Get(), fenceValueForSignal));
    
        return fenceValueForSignal;
    }

     void wait_fence(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
        std::chrono::milliseconds duration)
    {
        if (fence->GetCompletedValue() < fenceValue)
        {
            throw_failed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
            ::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
        }
    }

     void flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
        uint64_t& fenceValue, HANDLE fenceEvent )
    {
        uint64_t fenceValueForSignal = signal(commandQueue, fence, fenceValue);
        wait_fence(fence, fenceValueForSignal, fenceEvent);
    }

     void clear(
        v4f color, 
        dx12_state_t& state
    ) {
        auto command_allocator = state.command_allocators[state.current_back_buffer_index];
        auto back_buffer = state.back_buffers[state.current_back_buffer_index];
        
        command_allocator->Reset();
        state.command_list->Reset(command_allocator.Get(), nullptr);

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            back_buffer.Get(),
            D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
        );

        state.command_list->ResourceBarrier(1, &barrier);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(state.rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(),
            state.current_back_buffer_index, state.rtv_descriptor_size);
        state.command_list->ClearRenderTargetView(rtv, &color.r, 0, nullptr);
    }

     void present(dx12_state_t& state) {
        auto [allocator, back_buffer] = state.get_allocator_and_buffer();


        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            back_buffer.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
        );
        
        state.command_list->ResourceBarrier(1, &barrier);
        throw_failed(state.command_list->Close());
        
        ID3D12CommandList* const command_lists[] = {
            state.command_list.Get()
        };
        state.command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);
        state.frame_fence_values[state.current_back_buffer_index] = signal(state.command_queue, state.fence, state.fence_value);
 
        UINT syncInterval = state.vsync ? 1 : 0;
        UINT presentFlags = state.tearing && !state.vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;
        throw_failed(state.swap_chain->Present(syncInterval, presentFlags));
 
        state.current_back_buffer_index = state.swap_chain->GetCurrentBackBufferIndex();

        wait_fence(state.fence, state.frame_fence_values[state.current_back_buffer_index], state.fence_event);
    }

     void dx12_state_t::resize(u32 w, u32 h) {
         flush(command_queue, fence, fence_value, fence_event);
         for (int i = 0; i < 3; i++) {
             back_buffers[i].Reset();
             frame_fence_values[i] = frame_fence_values[current_back_buffer_index];
         }
         DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
         throw_failed(swap_chain->GetDesc(&swapChainDesc));
         throw_failed(swap_chain->ResizeBuffers(3, w, h,
             swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

         current_back_buffer_index = swap_chain->GetCurrentBackBufferIndex();

         update_render_target_views(*this);
     }

    dx12_state_t::dx12_state_t(HWND hWnd, u32 width, u32 height) {

        enable_debug_layer();

        device = create_device(true);
        command_queue = create_command_queue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);

 
        swap_chain = create_swap_chain(hWnd, command_queue, width, height, 3);

    
        current_back_buffer_index = swap_chain->GetCurrentBackBufferIndex();
    
        rtv_descriptor_heap = create_descriptor_heap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 3);
        rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
        update_render_target_views(*this);
        
        for (int i = 0; i < 3; i++) {
            command_allocators[i] = create_command_allocator(device, D3D12_COMMAND_LIST_TYPE_DIRECT);

        }
        command_list = create_command_list(device, command_allocators[current_back_buffer_index], D3D12_COMMAND_LIST_TYPE_DIRECT);

        fence = create_fence(device);
        fence_event = create_event_handle();
    }
}