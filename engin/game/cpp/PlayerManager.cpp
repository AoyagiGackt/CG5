#include "PlayerManager.h"

void PlayerManager::Initialize(ModelCommon* modelCommon, Model* model, Input* input, MapChipField* mapField,Camera* camera)
{
    player_ = std::make_unique<Player>();
    player_->Initialize(modelCommon, model, input, mapField,camera);
}

void PlayerManager::Update(float cameraPosX)
{
    if (player_) {

        player_->SetCameraPosX(cameraPosX);
        player_->Update();
    }
}

void PlayerManager::Draw()
{
    if (player_) {
        player_->Draw();
    }
}

void PlayerManager::DrawShadow()
{
    if (player_) {
        player_->DrawShadow();
    }
}