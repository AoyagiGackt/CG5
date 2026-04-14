#include "Turret.h"
#include "MakeAffine.h"
#include <cfloat>

void Turret::Initialize(ModelCommon* modelCommon, Model* model, Model* bulletModel,
                        const Vector3& position, EnemyManager* enemyManager)
{
    modelCommon_  = modelCommon;
    bulletModel_  = bulletModel;
    enemyManager_ = enemyManager;

    object_ = std::make_unique<Object3d>();
    object_->Initialize(modelCommon_);
    object_->SetModel(model);
    object_->SetPosition(position);
    object_->SetScale({ 0.6f, 0.6f, 0.6f });
    object_->Update();
}

void Turret::Update(std::list<std::unique_ptr<Bullet>>& bulletList)
{
    if (isDead_) { return; }

    // --- 寿命カウントダウン ---
    --lifeTimer_;
    if (lifeTimer_ <= 0) {
        isDead_ = true;
        return;
    }

    // --- 自走フェーズ ---
    if (moveTimer_ < kMoveDuration) {
        Vector3 pos = object_->GetTransform().translate;
        pos.x += kMoveSpeed;
        object_->SetPosition(pos);
        ++moveTimer_;
    }

    // --- 射撃フェーズ（自走が終わったら開始） ---
    if (moveTimer_ >= kMoveDuration) {
        ++shootTimer_;
        if (shootTimer_ >= kShootInterval) {
            shootTimer_ = 0;

            Vector3 vel;
            if (FindNearestEnemy(vel)) {
                auto bullet = Bullet::Create(modelCommon_, bulletModel_,
                                             object_->GetTransform().translate, vel);
                bullet->SetOwner(BulletOwner::Player);
                bullet->SetBulletScale(0.4f);
                bullet->Update(); // WVP行列を初期化（未更新のまま描画されると点滅する）
                bulletList.push_back(std::move(bullet));
            }
        }
    }

    object_->Update();
}

void Turret::Draw()
{
    if (!isDead_) {
        object_->Draw();
    }
}

void Turret::DrawShadow()
{
    if (!isDead_) {
        object_->DrawShadow();
    }
}

Vector3 Turret::GetPosition() const
{
    return object_->GetTransform().translate;
}

bool Turret::FindNearestEnemy(Vector3& outVelocity) const
{
    if (!enemyManager_) { return false; }

    const auto& enemies = enemyManager_->GetEnemies();
    if (enemies.empty()) { return false; }

    Vector3 myPos = object_->GetTransform().translate;
    float   minDist = FLT_MAX;
    Vector3 nearestPos = {};
    bool    found = false;

    for (const auto& enemy : enemies) {
        if (enemy->IsDead()) { continue; }
        float d = Distance(myPos, enemy->GetPosition());
        if (d < minDist) {
            minDist     = d;
            nearestPos  = enemy->GetPosition();
            found       = true;
        }
    }

    if (!found) { return false; }

    // 正規化して速度ベクトルを作成
    Vector3 dir = Subtract(nearestPos, myPos);
    if (Length(dir) < 0.001f) { return false; }
    dir = Normalize(dir);
    outVelocity = { dir.x * kBulletSpeed, dir.y * kBulletSpeed, dir.z * kBulletSpeed };
    return true;
}
