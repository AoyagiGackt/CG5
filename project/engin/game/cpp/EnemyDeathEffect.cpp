#include "EnemyDeathEffect.h"
#include "ParticleManager.h"
#include <cmath>
#include <numbers>
#include <random>

static std::mt19937 sRng{std::random_device{}()};
static const std::string kGroupName = "enemyDeath";

void EnemyDeathEffect::CreateGroup(){
    ParticleManager::GetInstance()->CreateParticleGroup(kGroupName,"Resources/emojiUI/happy.png");
}

void EnemyDeathEffect::Emit(const Vector3& position){
    const Vector4 kColor = {1.0f, 1.0f, 1.0f, 1.0f};
    const int kParticleCount = 5;
    const float kExpandSpeed = 3.0f; // 広がる初速
    const float kScale = 0.6f;
    const float kLifeTime = 0.5f;

    float angleStep = (2.0f * std::numbers::pi_v<float>) / static_cast<float>(kParticleCount);

    for(int i = 0; i < kParticleCount; ++i){
        float angle = angleStep * static_cast<float>(i);

        // 中心から外側へ向かう速度
        Vector3 vel = {
            std::cosf(angle) * kExpandSpeed,
            std::sinf(angle) * kExpandSpeed,
            0.0f
        };

        ParticleManager::GetInstance()->EmitWithColor(
            kGroupName,position,vel,kColor,kLifeTime,kScale
        );
    }
}