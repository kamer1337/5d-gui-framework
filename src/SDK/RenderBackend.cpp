#include "../../include/SDK/RenderBackend.h"
#include "../../include/SDK/GDIRenderBackend.h"
#include "../../include/SDK/D2DRenderBackend.h"
#include <d2d1.h>

namespace SDK {

std::unique_ptr<RenderBackend> RenderBackend::Create(BackendType type) {
    if (type == BackendType::AUTO) {
        // Try Direct2D first (hardware accelerated)
        auto d2dBackend = std::make_unique<D2DRenderBackend>();
        // We'll initialize it later, but check if D2D is available
        
        // Check if Direct2D is available by trying to create factory
        ID2D1Factory* pFactory = nullptr;
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
        
        if (SUCCEEDED(hr) && pFactory) {
            pFactory->Release();
            // D2D is available, use it
            return d2dBackend;
        }
        
        // Fall back to GDI
        type = BackendType::GDI;
    }
    
    if (type == BackendType::DIRECT2D) {
        return std::make_unique<D2DRenderBackend>();
    } else {
        return std::make_unique<GDIRenderBackend>();
    }
}

} // namespace SDK
