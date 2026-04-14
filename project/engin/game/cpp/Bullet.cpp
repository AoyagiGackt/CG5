#include "Bullet.h"

// 1行でインスタンス化＋初期化を終わらせる関数
std::unique_ptr<Bullet> Bullet::Create(ModelCommon* modelCommon, Model* model, const Vector3& pos, const Vector3& velocity)
{
    auto bullet = std::make_unique<Bullet>();
    bullet->Initialize(modelCommon, model, pos, velocity);
    return bullet;
}

void Bullet::Initialize(
    ModelCommon* modelCommon, Model* model,
    const Vector3& pos, const Vector3& velocity,
    BulletOwner owner)
{
    object_ = std::make_unique<Object3d>();
    object_->Initialize(modelCommon);
    object_->SetModel(model);
    object_->SetPosition(pos);
    object_->SetScale({ 0.5f, 0.5f, 0.5f });
    object_->SetEnableLighting(false); // 夜でも暗くならないよう

    velocity_ = velocity; // 飛んでいく「向き」と「速さ」を記憶
    owner_ = owner;
}

void Bullet::Update()
{
    if (isDead_) {
        return;
    }

    // 寿命タイマーを減らす
    lifeTimer_--;
    if (lifeTimer_ <= 0) {
        isDead_ = true;
        return;
    }

    // 記憶している速度ベクトルを足して移動
    Vector3 pos = object_->GetTransform().translate;
    pos.x += velocity_.x;
    pos.y += velocity_.y;
    object_->SetPosition(pos);

    // 当たり判定(AABB)を今の座標に合わせる（スケール連動）
    float half = scale_ * 0.5f;
    collider_.aabb.min = { pos.x - half, pos.y - half, pos.z - half };
    collider_.aabb.max = { pos.x + half, pos.y + half, pos.z + half };

    object_->Update();
}

void Bullet::OnCollision()
{
    isDead_ = true; // 何かにぶつかったら消滅フラグを立てる
}

void Bullet::Draw()
{
    if (!isDead_) {
        object_->Draw();
    }
}

void Bullet::DrawShadow()
{
    if (!isDead_) {
        object_->DrawShadow();
    }
}

Vector3 Bullet::GetPosition() const
{
    return object_->GetTransform().translate;
}

void Bullet::SetBulletScale(float scale)
{
    scale_ = scale;
    radius = scale * 0.5f;
    object_->SetScale({ scale, scale, scale });
}