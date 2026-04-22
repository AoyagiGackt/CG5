#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include <d3d12.h>
#include <wrl/client.h>

class PostProcessPass {
public:
    void Initialize(DirectXCommon* dxCommon);
    void Draw(ID3D12GraphicsCommandList* commandList, SrvManager* srvManager, uint32_t srvIndex);

private:
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
};
