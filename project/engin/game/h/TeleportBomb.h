#pragma once
#include "SpecialMove.h"
#include <memory>
#include "Object3d.h"
#include "ModelCommon.h" 

class TeleportBomb : public SpecialMove{
public:
	// 技の内部状態
	enum class State{
		None,               // 待機中
		ProjectileFlying,    // 弾が飛んでいる
		TeleportExploding   // ワープ・爆発演出中
	};

	void InitBomb(Player* player,Model* bulletModel,ModelCommon* modelCommon);

	// 親クラスの Initialize は空のままにしておく
	void Initialize(Player* player) override;

	void Update() override;
	void Draw() override;
	void Activate() override;

	// 弾が飛んでいる状態か判定
	bool IsProjectileFlying() const{ return state_ == State::ProjectileFlying; }
	// 弾の現在座標を取得
	const Vector3& GetProjectilePos() const{ return projectilePos_; }
	// ブロックと衝突したときの消滅処理
	void OnCollisionBlock();

private:
	State state_ = State::None;

	// 弾の見た目となる実体
	std::unique_ptr<Object3d> projectileObject_;

	// --- 弾（プロジェクタイル）のデータ ---
	Vector3 projectilePos_;
	Vector3 projectileVelocity_;
	float projectileSpeed_ = 0.5f; // 弾の速さ
	int projectileLifetime_ = 0;   // 弾の寿命

	// --- 爆発のデータ ---
	int explosionTimer_ = 0;
	float explosionRadius_ = 3.0f; // 爆発の巻き込み範囲
	float hitDetectionRadius_ = 1.0f;
};