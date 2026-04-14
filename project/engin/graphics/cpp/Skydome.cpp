#include "Skydome.h"
#include <numbers>

void Skydome::Initialize(ModelCommon* modelCommon, Model* model)
{
    object_ = std::make_unique<Object3d>();
    object_->Initialize(modelCommon);
    object_->SetModel(model);

    object_->SetScale({ 50.0f, 50.0f, 50.0f });

    object_->SetEnableLighting(false);
}

void Skydome::Update(Camera* camera, float timeRatio)
{
    if (camera != nullptr) {
        Vector3 camPos = camera->GetTransform().translate;

        object_->SetPosition(camPos);
    }

    // 時間に合わせてY軸回転（0.0=18:00 → 1.0=翌6:00 → 半周π）
    // ゲームは12時間分(18:00→6:00)なので、24時間テクスチャの半周分だけ回す
    float rotY = timeRatio * std::numbers::pi_v<float>;
    object_->SetRotation({ 0.0f, -rotY, 0.0f });

    // 行列の更新
    object_->Update();
}

void Skydome::Draw()
{
     if (object_) {
        object_->Draw();
    }
}