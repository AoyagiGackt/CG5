/**
 * @file Player.h
 * @brief プレイヤーの挙動、操作、および物理演算（当たり判定）を管理するファイル
 */
#pragma once
#include "Bullet.h"
#include "Camera.h"
#include "Condition.h"
#include "Collision.h"
#include "GameObject.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "Object3d.h"
#include <memory>
#include "SpecialMove.h"
#include "TeleportBomb.h"
#include "BeamSpecial.h"
#include "SpinBlade.h"

class EnemyManager;
/**
 * @brief プレイヤーキャラクターのクラス
 */
class Player : public GameObject{
public:
	/**
	 * @brief 初期化処理
	 * @param modelCommon 描画設定
	 * @param model プレイヤー用のモデルデータ
	 * @param input 入力管理へのポインタ
	 * @param mapField 衝突判定用のマップフィールド
	 */
	void Initialize(ModelCommon* modelCommon,Model* model,Input* input,MapChipField* mapField,Camera* camera);

	/**
	 * @brief 毎フレームの更新処理
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
	 * @brief 外部からの衝突イベント処理
	 */
	void OnCollision();

	//damageAmountはダメージ量（例：100）を引数として受け取る
	void TakeDamage(float damageAmount);

	void SetCameraPosX(float cameraPosX){ cameraPosX_ = cameraPosX; }

	Condition* GetCondition(){ return condition_.get(); }

	/**
	 * @brief 現在のワールド座標を取得
	 * @return Vector3 プレイヤーの座標
	 */
	Vector3 GetPosition() const{ return player_->GetTransform().translate; }

	float GetConditionSpeedMultiplier() const{
		return condition_->GetSpeedMultiplier();
	}

	float GetScrollSpeedMultiplier() const{
		return condition_->GetScrollSpeedMultiplier();
	}

	/**
	 * @brief 弾リストとの当たり判定チェック
	 * @param bullets GamePlaySceneが持つ弾のリスト
	 * @note ヒットした弾は OnCollision() が呼ばれ isDead_ が true になる
	 */
	void CheckBulletHit(std::list<std::unique_ptr<Bullet>>& bullets);

	/**
	 * @brief GamePlaySceneから計算済みのアニメーション行列を受け取って適用する
	 * @param mat MakeAffineMatrix(scale, rotate, translate) で作った localMatrix
	 */
	void SetAnimationMatrix(const Matrix4x4& mat){ animationMatrix_ = mat; }

	void OnEnemyDefeated();

	bool CheckSwipeInput();

	void OnSwipeSuccess();

	void SetPosition(const Vector3& pos){
		player_->SetPosition(pos);
	}
	void SetVelocity(const Vector3& vel){
		velocity_ = vel;
	}
	void SetInvincible(bool isInvincible){
		if(isInvincible){
			invincibleTimer_ = 9999;
		} else{
			invincibleTimer_ = 0;
		}
	}

	void SetSpecialMove(std::unique_ptr<SpecialMove> specialMove){
		specialMove_ = std::move(specialMove);
	}

	void CreateTeleportBomb(Model* model,ModelCommon* common){
		// Player自身の中で生成
		auto bomb = std::make_unique<TeleportBomb>();
		bomb->InitBomb(this,model,common);

		// メンバ変数に代入（これで所有権がPlayerに固定される）
		specialMove_ = std::move(bomb);
	}

	void CreateBeamSpecial(Model* model, ModelCommon* common){
		auto beam = std::make_unique<BeamSpecial>();
		beam->InitBeam(this, common, model);
		specialMove_ = std::move(beam);
	}


	void CreateSpinBlade(Model* model, ModelCommon* common) {
		auto blade = std::make_unique<SpinBlade>();
		blade->InitBlade(this, model, common);

		specialMove_ = std::move(blade);
	}

	/**
	 * @brief 当たり判定用のAABBを取得
	 * @return AABB プレイヤーの当たり判定ボックス
	 */
	AABB GetAABB() const{ return collider_.aabb; }

	void SetSpecialMoveModel(Model* model){ specialMoveModel_ = model; }


	Object3d* GetBaseObject() const{ return player_.get(); }

	// 初期化時にセットした元のモデル
	Model* GetOriginalModel() const{ return originalModel_; }

	// GamePlaySceneから受け取った必殺技用モデル
	Model* GetSpecialMoveModel() const{ return specialMoveModel_; }

	void SetEnemyManager(EnemyManager* enemyManager){ enemyManager_ = enemyManager; }
	EnemyManager* GetEnemyManager() const{ return enemyManager_; }

	TeleportBomb* GetTeleportBomb() const{
		return dynamic_cast<TeleportBomb*>(specialMove_.get());
	}

	BeamSpecial* GetBeam() const {
		return dynamic_cast<BeamSpecial*>(specialMove_.get());
	}

	bool IsInvincible() const{ return invincibleTimer_ > 0; }

	bool IsSwipeReady() const{ return swipeStock_ > 0; }
	int GetSwipeStock() const{ return swipeStock_; }
	int GetSwipeSuccessCount() const{ return swipeSuccessCount_; }

	/** @brief このフレームにスワイプが成功したか（GamePlaySceneが動画切替に使う） */
	bool ConsumeSwipeFired(){
		bool v = swipeFired_;
		swipeFired_ = false;
		return v;
	}

	bool IsDead() const{ return playerHp_ <= 0.0f; }

	float GetHP() const{ return playerHp_; }
	void SetHP(float hp){ playerHp_ = hp; }

	bool IsBuffReady() const{ return isBuffReady_; }
	void ResetBuffReady(){ isBuffReady_ = false; }

	bool IsSpecialMoveActive() const{
		return specialMove_ && specialMove_->IsActive();
	}

	void SetSwipeStock(int stock){ swipeStock_ = stock; }

private:
	// ==========================================
	// 内部処理用の分割関数
	// ==========================================
	void HandleDash(const Input::Stick& stick);
	void HandleMovement(const Input::Stick& stick);
	void ResolveCollisions();

	/** @brief 画面外に出ないよう座標をクランプする */
	void ClampPosition();

private:
	/** @brief プレイヤーを描画するための3Dオブジェクト */
	std::unique_ptr<Object3d> player_;

	// 入力管理のポインタ
	Input* input_ = nullptr;

	Camera* camera_ = nullptr;

	// 必殺技用のモデルポインタ
	Model* specialMoveModel_ = nullptr;
	Model* originalModel_ = nullptr;

	EnemyManager* enemyManager_ = nullptr;

	// マップチップフィールドへのポインタ
	MapChipField* mapField_ = nullptr;

	// 調子管理のポインタ
	std::unique_ptr<Condition> condition_;

	// 1フレーム前の座標（衝突補正用）
	Vector3 prevPos_;

	// --- プレイヤーのステータス --- 

	float playerHp_ = 1000;

	// --- 物理演算用 ---

	// 現在の移動速度
	Vector3 velocity_ = {0.0f, 0.0f, 0.0f};

	float baseSpeed_ = 0.1f; // 基本移動速度

	// 着地しているか
	bool onGround_ = false;

	// ジャンプ直後の地面判定スキップ用フラグ
	bool justJumped_ = false;

	// 現在のジャンプ回数
	int jumpCount_ = 0;

	// コヨーテタイムのカウンタ（0より大きい間は空中でも1段目ジャンプ可）
	int coyoteTimer_ = 0;

	// ---パラメータ---

	// 重力の強さ
	const float kGravity = -0.015f;

	// ジャンプの初速度
	const float kJumpForce = 0.3f;

	// 落下速度の上限（これ以上速く落ちない）
	const float kTerminalVelocity = -1.5f;

	// コヨーテタイム（プラットフォームの端から落ちた直後もジャンプできる猶予フレーム数）
	const int kCoyoteTime = 8;

	// --- ダッシュ用 ---

	// ダッシュ中か
	bool isDashing_ = false;

	// ダッシュの継続時間タイマー
	int dashTimer_ = 0;

	// 次にダッシュできるまでの待ち時間
	int dashCooldownTimer_ = 0;

	// ダッシュ時の速度ベクトル
	Vector3 dashVelocity_ = {0.0f, 0.0f, 0.0f};

	// ダッシュ時の速さ
	const float kDashSpeed = 1.0f;

	// ダッシュが続く時間（フレーム数）
	const int kDashDuration = 1;

	// ダッシュ後の待ち時間（フレーム数）
	const int kDashCoolTime = 30;

	// --- 当たり判定用の定数 ---

	// ブロックの基本サイズ
	const float kBlockSize = 1.0f;

	// プレイヤーの当たり判定（半分の幅）
	const float kHalfWidth = 0.35f;

	// プレイヤーの当たり判定（半分の高さ）
	const float kHalfHeight = 0.45f;

	// めり込み補正の余裕
	const float kPushBack = 0.01f;

	// 着地スナップの許容誤差
	const float kSnapThreshold = 0.05f;

	const float kMinX = 0.5f; // 左端
	const float kMaxX = 28.0f; // 右端（カメラの画角に合わせて調整してください）
	const float kMinY = 0.0f; // 下端
	const float kMaxY = 20.0f; // 上端

	int invincibleTimer_ = 0;

	// --- ジャストガード ---
	int justGuardTimer_ = 0;           // 受付窓の残りフレーム
	const int kJustGuardWindow = 10;   // 受付窓の長さ（フレーム）

	float cameraPosX_ = 0.0f;
	const float kScreenHalfWidth = 13.0f;///個々の値を減らすと画面中央に向かって左端がずれるよ

	// --- スワイプ仕様に関する変数 ---
	int swipeStock_ = 0;         // 溜まった撃破ストック（最大3）
	int swipeSuccessCount_ = 0;  // スワイプ成功した累計数 (0~3)
	int swipeCooldown_ = 0;      // スワイプ連続発火を防ぐクールタイム
	bool swipeFired_ = false;    // このフレームにスワイプが成功したか
	bool prevWheelScrolled_ = false; // 前フレームのホイール状態（エッジ検出用）

	int totalSwipeCount_ = 0;   // バフ用の累計カウント
	bool isBuffReady_ = false;  // バフが発生したフラグ

	// --- 必殺技（SpecialMove）関連の追加 ---
	std::unique_ptr<SpecialMove> specialMove_ = nullptr; // 現在の必殺技
	bool hasReachedExcellent_ = false;                  // 初めて絶好調になったか
	bool isSelectingSkill_ = false;                     // スキル選択中か


	// --- アニメーション ---
   /// GamePlaySceneから毎フレームセットされるlocalMatrix（単位行列で初期化）
	Matrix4x4 animationMatrix_ = MakeIdentity4x4();
};