#pragma once
#include "SpecialMove.h"
#include <memory>
#include <array>
#include "Object3d.h"
#include "ModelCommon.h"

class SpinBlade : public SpecialMove
{
public:

    enum class State
    {
        None,
        Charge,
        Active,
        Finish
    };

    void InitBlade(Player* player, Model* bladeModel, ModelCommon* modelCommon);

    void Initialize(Player* player) override;
    void Update() override;
    void Draw() override;
    void Activate() override;

private:

    //---------------------------------
    // 定数
    //---------------------------------

    static constexpr int kBladeCount = 4;

    static constexpr float kPI = 3.1415926535f;
    static constexpr float kAngleStep = (2.0f * kPI) / kBladeCount;

    static constexpr int kChargeTime = 60;
    static constexpr int kFinishTime = 30;
    static constexpr int kMaxActiveTime = 300;

    static constexpr float kMaxOrbitRadius = 5.0f;

    // motion
    static constexpr float kChargeRotateSpeed = 0.05f;
    static constexpr float kOrbitExpandSpeed = 0.01f;
    static constexpr float kFinishExpandSpeed = 0.4f;

    // animation
    static constexpr float kBladeWaveHeight = 0.6f;
    static constexpr float kBladeWaveSpeed = 3.0f;

    //---------------------------------
    // 状態
    //---------------------------------

    State state_ = State::None;

    //---------------------------------
    // オブジェクト
    //---------------------------------

    std::array<std::unique_ptr<Object3d>, kBladeCount> blades_;

    //---------------------------------
    // タイマー
    //---------------------------------

    int activeTimer_ = 0;
    int chargeTimer_ = 0;
    int finishTimer_ = 0;

    std::array<int, kBladeCount> damageTimer_ {};

    //---------------------------------
    // 回転
    //---------------------------------

    float rotationAngle_ = 0.0f;
    float rotationSpeed_ = 0.1f;
    float maxRotationSpeed_ = 1.2f;

    //---------------------------------
    // 軌道
    //---------------------------------

    float orbitRadius_ = 2.0f;

    //---------------------------------
    // 攻撃
    //---------------------------------

    float attackRadius_ = 1.5f;
    float finishRadius_ = 6.0f;

    //---------------------------------
    // 内部処理
    //---------------------------------

    void UpdateCharge(const Vector3& playerPos);
    void UpdateActive(const Vector3& playerPos);
    void UpdateFinish(const Vector3& playerPos);

    void UpdateBlade(int i, const Vector3& pos);

    Vector3 CalcOrbitPosition(const Vector3& center, float angle, float radius) const;
    bool IsHit(const Vector3& a, const Vector3& b, float radius) const;
};