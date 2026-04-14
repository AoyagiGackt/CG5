/**
 * @file Bullet.h
 * @brief プレイヤーや敵が発射する弾の挙動を管理するファイル
 */
#pragma once
#include "GameObject.h"
#include "Object3d.h"
#include <memory>

/**
 * @brief 弾の発射主を識別するための列挙型
 */
enum class BulletOwner {
    Player, // プレイヤーが撃った弾
    Enemy, // 敵が撃った弾
};

/**
 * @brief 弾オブジェクトのクラス
 * @note 生成時に速度ベクトル（velocity）を与えることで、任意の方向に直進します
 */
class Bullet : public GameObject {
public:
    /**
     * @brief 弾の生成と初期化を同時に行う
     * @param modelCommon 共通描画設定
     * @param model 使用するモデルデータ
     * @param pos 発射開始座標
     * @param velocity 飛んでいく方向と速さ
     * @return 生成された Bullet のユニークポインタ
     */
    static std::unique_ptr<Bullet> Create(ModelCommon* modelCommon, Model* model, const Vector3& pos, const Vector3& velocity);

    /**
     * @brief 初期化処理
     */
    void Initialize(ModelCommon* modelCommon, Model* model,
        const Vector3& pos, const Vector3& velocity,
        BulletOwner owner = BulletOwner::Player);

    /**
     * @brief 更新処理（移動、寿命計算、当たり判定更新）
     */
    void Update() override;

    /**
     * @brief 描画処理
     */
    void Draw() override;

    /**
     * @brief シャドウパス用描画（深度のみ）
     */
    void DrawShadow();

    /**
     * @brief 衝突時の応答処理
     * @note 何かに当たった際に呼び出され、消滅フラグを立てます
     */
    void OnCollision();

    /**
     * @brief 消滅フラグの取得
     * @return true ならその弾は破棄対象
     */
    void SetDead() { isDead_ = true; }
    bool IsDead() const { return isDead_; }

    void SetOwner(BulletOwner owner){ owner_ = owner; }

    /** @brief 発射主の取得 */
    BulletOwner GetOwner() const { return owner_; }

    /** @brief ジャストガードで弾を跳ね返す */
    void Reflect() { velocity_.x = -velocity_.x; owner_ = BulletOwner::Player; lifeTimer_ = 120; }

    Vector3 GetPosition() const;

    int GetDamage() { return damage_; }

    float GetRadius() { return radius; }

    /**
     * @brief バフによる弾スケール設定（当たり判定も連動）
     * @param scale スケール倍率（1.0 がデフォルト 0.5 サイズ）
     */
    void SetBulletScale(float scale);

private:
    /** @brief 弾の見た目を管理する3Dオブジェクト */
    std::unique_ptr<Object3d> object_;

    /** @brief 弾の移動速度（1フレームあたりの移動量） */
    Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };

    /** @brief バフ適用後のスケール（Update の当たり判定に使用） */
    float scale_ = 0.5f;

    /** @brief 消滅フラグ */
    bool isDead_ = false;

    /** @brief 自動消滅までの残り時間（フレーム単位） */
    int lifeTimer_ = 120;

    /** @brief 誰が撃った弾か */
    BulletOwner owner_ = BulletOwner::Player;

    int damage_ = 60; // デフォルトのダメージ

    float radius = 0.2f; // Bulletの半径想定
};