#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include <d3d12.h>
#include <wrl/client.h>

struct VignetteParams
{
    float intensity = 2.5f;
    float power     = 1.0f;
    float pad[2]    = {};
};

class PostProcessPass {
public:
    static PostProcessPass* GetInstance() { return s_instance_; }

    void Initialize(DirectXCommon* dxCommon);
    void Draw(ID3D12GraphicsCommandList* commandList, SrvManager* srvManager, uint32_t srvIndex);

    VignetteParams& GetVignetteParams() { return vignetteParams_; }

private:
    static PostProcessPass* s_instance_;

    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    Microsoft::WRL::ComPtr<ID3D12Resource> vignetteCB_;
    VignetteParams* vignetteMapped_ = nullptr;
    VignetteParams  vignetteParams_;
};
