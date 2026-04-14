/**
 * @file Turret.h
 * @brief 一定時間自走した後、最も近い敵へ弾を発射し続けるタレット
 */
#pragma once
#include "Bullet.h"
#include "EnemyManager.h"
#include "ModelCommon.h"
#include "Object3d.h"
#include <list>
#include <memory>

/**
 * @brief 自走式タレットクラス
 * @note
 *  - 召喚後 moveDuration_ フレーム右方向へ自走する
 *  - 停止後は shootInterval_ フレームごとに最近傍の敵へ弾を発射する
 *  - lifeTimer_ フレームで自動消滅する
 */
class Turret {
public:
    /**
     * @brief 初期化
     * @param modelCommon  共通描画設定
     * @param model        タレット本体のモデル
     * @param bulletModel  タレットが発射する弾のモデル
     * @param position     召喚座標
     * @param enemyManager 敵マネージャー（ターゲット探索用）
     */
    void Initialize(ModelCommon* modelCommon, Model* model, Model* bulletModel,
                    const Vector3& position, EnemyManager* enemyManager);

    /**
     * @brief 更新処理
     * @param bulletList 弾を追加する共有リスト（GamePlayScene::bullets_）
     */
    void Update(std::list<std::unique_ptr<Bullet>>& bulletList);

    /** @brief 通常描画 */
    void Draw();

    /** @brief シャドウパス描画 */
    void DrawShadow();

    /** @brief 消滅フラグの取得 */
    bool IsDead() const { return isDead_; }

    /** @brief 現在の座標を取得 */
    Vector3 GetPosition() const;

    /** @brief 座標を直接設定（ブロック押し出し用） */
    void SetPosition(const Vector3& pos) { object_->SetPosition(pos); }

    /** @brief ブロック押し出し判定に使う当たり判定半径 */
    static constexpr float kHalfSize = 0.3f;

private:
    /** @brief 最も近い敵を向いた速度ベクトルを返す（敵がいなければ nullopt） */
    bool FindNearestEnemy(Vector3& outVelocity) const;

    std::unique_ptr<Object3d> object_;
    ModelCommon* modelCommon_ = nullptr;
    Model*       bulletModel_ = nullptr;
    EnemyManager* enemyManager_ = nullptr;

    // --- 自走フェーズ ---
    static constexpr int kMoveDuration  = 120;   // 2 秒間自走
    static constexpr float kMoveSpeed   = 0.08f; // 1 フレームあたりの移動量
    int moveTimer_ = 0;

    // --- 射撃フェーズ ---
    static constexpr int   kShootInterval = 60;   // 1 秒ごとに射撃
    static constexpr float kBulletSpeed   = 0.25f;
    int shootTimer_ = 0;

    // --- 寿命 ---
    static constexpr int kLifeDuration = 600;    // 10 秒
    int lifeTimer_ = kLifeDuration;

    bool isDead_ = false;
};
