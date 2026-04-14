#include "ConditionUpEffect.h"
#include "ParticleManager.h"
#include <cmath>
#include <algorithm>

const std::vector<std::string> ConditionUpEffect::kEmojiGroups = {
    "Excellent", "Good", "Normal", "Bad", "Terrible"
};

void ConditionUpEffect::CreateGroup(){
    ParticleManager::GetInstance()->CreateParticleGroup(kEmojiGroups[0],"Resources/emojiUI/AbsolutelyHappy.png");
    ParticleManager::GetInstance()->CreateParticleGroup(kEmojiGroups[1],"Resources/emojiUI/happy.png");
    ParticleManager::GetInstance()->CreateParticleGroup(kEmojiGroups[2],"Resources/emojiUI/Normal.png");
    ParticleManager::GetInstance()->CreateParticleGroup(kEmojiGroups[3],"Resources/emojiUI/Slump.png");
    ParticleManager::GetInstance()->CreateParticleGroup(kEmojiGroups[4],"Resources/emojiUI/AbsolutelySlump.png");
}

void ConditionUpEffect::Emit(Camera* camera){
    // ---------- 演出設定 ----------
    const int   kMaxParticleCount = 600;
    const float kZOffset = 15.0f;
    const float kWidthRange = 40.0f;
    const float kHeightRange = 15.0f;
    const float kCenterSpeed = 18.0f; // 中央の突き抜け速度
    const float kEdgeSpeed = 3.0f;  // 端の最低速度
    const float kLifeTime = 1.2f;  // 短い寿命でキレを出す
    const float kBaseScale = 0.6f;
    // ------------------------------

    Matrix4x4 camWorld = Inverse(camera->GetViewMatrix());
    Vector3 right = {camWorld.m[0][0], camWorld.m[0][1], camWorld.m[0][2]};
    Vector3 camForward = {camWorld.m[2][0], 0.0f, camWorld.m[2][2]};
    Vector3 worldUp = {0.0f, 1.0f, 0.0f};

    Vector3 camPos = camera->GetTranslate();
    Vector3 basePos = camPos + (camForward * kZOffset);

    for(int i = 0; i < kMaxParticleCount; ++i){
        // 横位置 (中央密度を高く)
        float r1 = ((float)rand() / RAND_MAX * 2.0f - 1.0f);
        float r2 = ((float)rand() / RAND_MAX * 2.0f - 1.0f);
        float nx = (r1 + r2) * 0.5f;
        float absX = std::abs(nx);

        // 速度計算 (6乗バイアスで中央を鋭く)
        float speedBias = std::pow(1.0f - absX,6.0f);
        float speed = kEdgeSpeed + (kCenterSpeed - kEdgeSpeed) * speedBias;
        speed *= (0.9f + (float)rand() / RAND_MAX * 0.2f);

        // 座標確定
        float x = nx * (kWidthRange * 0.5f);
        // 画面内にランダムに散らす（パッと現れる演出）
        float y = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * (kHeightRange * 0.5f);

        Vector3 spawnPos = basePos + (right * x) + (worldUp * y);
        Vector3 velocity = worldUp * speed;

        // サイズ (中央を大きく)
        float sizeBias = 1.0f - (absX * 0.4f);
        float randomScale = kBaseScale * sizeBias * (0.8f + (float)rand() / RAND_MAX * 0.4f);

        int index = rand() % kEmojiGroups.size();
        ParticleManager::GetInstance()->EmitWithColor(
            kEmojiGroups[index],
            spawnPos,
            velocity,
            {1.0f, 1.0f, 1.0f, 1.0f},
            kLifeTime,
            randomScale
        );
    }
}