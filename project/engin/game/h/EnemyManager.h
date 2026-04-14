#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>
#include <map>
#include "Condition.h"
#include "Bullet.h"
#include "Collision.h"
#include "Player.h"
#include "Sprite.h"
#include "SpriteCommon.h"

class MapChipField;

class EnemyManager{
public:

	void Initialize(ModelCommon* modelCommon,Model* model,Input* input,Model* bulletModel,MapChipField* mapChipField);

	//============================
	// 更新
	//============================
	void Update(Camera* camera,std::list<std::unique_ptr<Bullet>>& playerBullets,Player* player, float scrollMultiplier);

	//============================
	// 描画
	//============================
	void Draw();

	/** @brief 弾のみ描画（skyOverlay の後に呼ぶ） */
	void DrawBullets();

	/** @brief シャドウパス用描画（全敵 + 全敵弾） */
	void DrawShadow();

	//============================
	// Enemy生成
	//============================
	void SpawnEnemy(const Vector3& position);

	//============================
	// Getter（デバッグ用）
	//============================
	const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const{ return enemies_; }

	void SetPlayerCondition(Condition::ConditionType condition);

	void SetDebugSpawnDisabled(bool v){ debugSpawnDisabled_ = v; }

	// スポーン制御
	void SetSpawnInterval(int interval) { spawnInterval_ = interval; }
	int  GetSpawnInterval() const { return spawnInterval_; }
	void SetMaxEnemy(int max) { maxEnemy_ = max; }
	int  GetMaxEnemy() const { return maxEnemy_; }

	// 調子確率テーブル（[playerConditionIndex][0]=threshA, [1]=threshB）
	// Excellent/Terrible は threshA のみ使用（2択）
	// Good/Normal/Bad は threshA, threshB を使用（3択）
	int* GetCondThreshA() { return condThreshA_; }
	int* GetCondThreshB() { return condThreshB_; }

	// 全ての敵の弾をまとめて取得する関数
	std::list<std::unique_ptr<Bullet>>& GetBullets();

	//============================
	// 調子アイコン
	//============================
	/** @brief SpriteCommon をセットして調子アイコンを初期化する */
	void SetSpriteCommonAndInitIcons(SpriteCommon* spriteCommon);

	/** @brief 各敵の頭上に調子アイコンスプライトを描画する */
	void DrawConditionIcons(Camera* camera);

private:

	//============================
    // Enemy管理
    //============================
	std::vector<std::unique_ptr<Enemy>> enemies_;
	Model* bulletModel_ = nullptr;

	//============================
	// 全ての敵の弾をまとめて管理するリスト
	//============================
	std::list<std::unique_ptr<Bullet>> allEnemyBullets_;

	//============================
	// 共有リソース
	//============================
	ModelCommon* modelCommon_ = nullptr;
	Model* model_ = nullptr;
	Input* input_ = nullptr;
	MapChipField* mapChipField_;

	//============================
	// Player Condition
	//============================
	Condition::ConditionType playerCondition_ = Condition::ConditionType::Normal;

	Condition::ConditionType DecideEnemyCondition();

	//============================
	// 調子アイコン スプライト
	//============================
	SpriteCommon* spriteCommon_ = nullptr;

	/** @brief 敵スロットごとの独立スプライト（Constant Buffer を共有しないため個別に持つ） */
	std::vector<std::unique_ptr<Sprite>> iconPool_;

	/** @brief 各スロットに現在セットされている調子（テクスチャ変更の差分検出用） */
	std::vector<Condition::ConditionType> iconConditions_;

	/** @brief 調子 → テクスチャパス の対応表 */
	std::map<Condition::ConditionType, std::string> conditionPaths_;

	static constexpr float kIconSize = 40.0f;

	//============================
	// Spawn管理
	//============================
	int spawnTimer_    = 0;
	int spawnInterval_ = 180; // スポーン間隔（フレーム数）
	int maxEnemy_      = 10;
	bool debugSpawnDisabled_ = false;

	// 調子確率テーブル（playerCondition の enum int 値でインデックス）
	// Excellent=0, Good=1, Normal=2, Bad=3, Terrible=4
	// threshA: 0〜100  threshB: threshA〜100（0 = 2択のみ）
	int condThreshA_[5] = { 70, 20, 20, 20, 70 };
	int condThreshB_[5] = {  0, 70, 70, 70,  0 };

	//playerの弾とenemyの当たり判定
	void CheckPlayerBulletCollisions(std::list<std::unique_ptr<Bullet>>& playerBullets);
	//playerとenemyのボディ当たり判定
	void CheckPlayerEnemyBodyCollisions(Player* player);
	void CheckBeamCollision(Player* player);
	//死んだ敵の削除処理
	void RemoveDeadEnemies(Player* player);

	//imguiの処理
	void DebugDraw();

};