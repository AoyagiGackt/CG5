#include "EnemyManager.h"
#include "EnemyDeathEffect.h"
#include "BulletHitEffect.h"
#include "ImGuiManager.h"
#include "MapChipField.h"
#include "ScoreManager.h"
#include "Camera.h"
#include "BeamSpecial.h"

// Conditionを文字列に変換
const char* GetConditionName(Condition::ConditionType condition) {
	switch (condition) {
		case Condition::ConditionType::Excellent: return "Excellent";
		case Condition::ConditionType::Good: return "Good";
		case Condition::ConditionType::Normal: return "Normal";
		case Condition::ConditionType::Bad: return "Bad";
		case Condition::ConditionType::Terrible: return "Terrible";
	}
	return "Unknown";
}

void EnemyManager::Initialize(ModelCommon* modelCommon, Model* model, Input* input, Model* bulletModel, MapChipField* mapChipField) {
	modelCommon_ = modelCommon;
	model_ = model;
	input_ = input;
	bulletModel_ = bulletModel;
	mapChipField_ = mapChipField;
}

void EnemyManager::SpawnEnemy(const Vector3& position) {
	if (!modelCommon_ || !model_ || !input_ || !bulletModel_ || !mapChipField_) {
		return;
	}

	auto enemy = std::make_unique<Enemy>();
	if (!enemy) {
		return;
	}

	// 修正：5つの引数を渡す
	enemy->Initialize(modelCommon_, model_, input_, bulletModel_, mapChipField_);

	enemy->SetPosition(position);

	Condition::ConditionType condition = DecideEnemyCondition();
	enemy->SetCondition(condition);

	enemies_.push_back(std::move(enemy));
}

void EnemyManager::SetPlayerCondition(Condition::ConditionType condition) {
	playerCondition_ = condition;
}

Condition::ConditionType EnemyManager::DecideEnemyCondition() {
	int r = rand() % 100;
	int pi = static_cast<int>(playerCondition_);
	int a = condThreshA_[pi];
	int b = condThreshB_[pi];

	switch (playerCondition_) {
		case Condition::ConditionType::Excellent:
			return (r < a) ? Condition::ConditionType::Good : Condition::ConditionType::Excellent;

		case Condition::ConditionType::Good:
			if (r < a) return Condition::ConditionType::Excellent;
			if (r < b) return Condition::ConditionType::Good;
			return Condition::ConditionType::Normal;

		case Condition::ConditionType::Normal:
			if (r < a) return Condition::ConditionType::Good;
			if (r < b) return Condition::ConditionType::Normal;
			return Condition::ConditionType::Bad;

		case Condition::ConditionType::Bad:
			if (r < a) return Condition::ConditionType::Normal;
			if (r < b) return Condition::ConditionType::Bad;
			return Condition::ConditionType::Terrible;

		case Condition::ConditionType::Terrible:
			return (r < a) ? Condition::ConditionType::Bad : Condition::ConditionType::Terrible;
	}

	return Condition::ConditionType::Normal;
}

void EnemyManager::Update(Camera* camera, std::list<std::unique_ptr<Bullet>>& playerBullets, Player* player, float scrollMultiplier) {

	if (player) {
		SetPlayerCondition(player->GetCondition()->GetCondition());
	}

	// スポーン管理
	if (!debugSpawnDisabled_) spawnTimer_++;
	if (!debugSpawnDisabled_ && spawnTimer_ > spawnInterval_ && enemies_.size() < (size_t)maxEnemy_) {
		float x = 20.0f + (rand() % 5);
		SpawnEnemy({ x,3,0 });
		spawnTimer_ = 0;
	}

	// 地面スクロール速度（MapChipField と同じ基準値）
	float scrollSpeed = 0.1f * scrollMultiplier;

	//敵の更新
	for (auto& enemy : enemies_) {
		enemy->Update(camera, scrollSpeed);
	}

	// 画面外（左端）に出た敵を削除
	float leftEdge = camera->GetTranslate().x - 16.0f;
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[leftEdge](const std::unique_ptr<Enemy>& e) {
				return e->GetPosition().x < leftEdge;
			}),
		enemies_.end()
	);

	CheckPlayerBulletCollisions(playerBullets);
	CheckPlayerEnemyBodyCollisions(player);
	CheckBeamCollision(player);
	RemoveDeadEnemies(player);
	DebugDraw();

}

void EnemyManager::Draw() {

	for (auto& enemy : enemies_) {

		enemy->Draw();
	}
}

void EnemyManager::DrawBullets() {
	for (auto& enemy : enemies_) {
		enemy->DrawBullets();
	}
}

void EnemyManager::DrawShadow() {

	for (auto& enemy : enemies_) {

		enemy->DrawShadow();
	}
}

void EnemyManager::CheckPlayerBulletCollisions(std::list<std::unique_ptr<Bullet>>& playerBullets) {

	for (auto& enemy : enemies_) {

		auto it = playerBullets.begin();
		while (it != playerBullets.end()) {
			Bullet* bullet = it->get();

			Vector3 ePos = enemy->GetPosition();
			Vector3 bPos = bullet->GetPosition();

			bool hit =
				bPos.x + bullet->GetRadius() > ePos.x - enemy.get()->GetHalfWidth() &&
				bPos.x - bullet->GetRadius() < ePos.x + enemy.get()->GetHalfWidth() &&
				bPos.y + bullet->GetRadius() > ePos.y - enemy.get()->GetHalfHeight() &&
				bPos.y - bullet->GetRadius() < ePos.y + enemy.get()->GetHalfHeight();

			if (hit) {
				BulletHitEffect::Emit(bPos);
				if (enemy->GetCondition() == Condition::ConditionType::Excellent) {
					// 絶好調の敵に弾が当たると即死
					enemy->Damage(99999);
				} else {
					enemy->Damage(bullet->GetDamage());
					// 弾が当たるたびに敵の調子を1段階上げる
					enemy->RankUpCondition();
				}
				it = playerBullets.erase(it); // 当たった弾は消す
			} else {
				++it;
			}
		}
	}

}

void EnemyManager::CheckPlayerEnemyBodyCollisions(Player* player) {
	if (!player || player->IsInvincible()) {
		return;
	}

	AABB playerAABB = player->GetAABB();

	for (auto& enemy : enemies_) {
		Vector3 ePos = enemy->GetPosition();
		AABB enemyAABB = {
			{ ePos.x - enemy->GetHalfWidth(), ePos.y - enemy->GetHalfHeight(), ePos.z - 0.5f },
			{ ePos.x + enemy->GetHalfWidth(), ePos.y + enemy->GetHalfHeight(), ePos.z + 0.5f }
		};

		if (Collision::CheckCollision(playerAABB, enemyAABB)) {
			player->TakeDamage(20.0f);
			break; // 1フレームで複数敵から同時ダメージを防ぐ
		}
	}
}

void EnemyManager::CheckBeamCollision(Player* player)
{

	if (!player)
	{
		return;
	}

	auto* beam = player->GetBeam();
	// ↑ここはあとで「GetBeam()」作った方が綺麗

	if (!beam) return;
	if (!beam->IsActive()) return;

	

	// =========================
	// Enemyと当たり判定
	// =========================

	for (auto& enemy : enemies_)
	{
		if (!enemy || enemy->IsDead()) continue;

		if (Collision::CheckCollision(beam->GetAABB(), enemy->GetAABB()))
		{
			// ダメージ
			enemy->Damage(100);

		}
	}
}

void EnemyManager::RemoveDeadEnemies(Player* player) {
	// 1. 削除する前に、死んでいる敵を探してプレイヤーに通知する
	for (auto& enemy : enemies_) {
		if (enemy->IsDead()) {
			// 撃破エフェクト
			EnemyDeathEffect::Emit(enemy->GetPosition());
			// プレイヤーに撃破を伝える（ここでストック計算が行われる）
			player->OnEnemyDefeated();
			// プレイヤーの調子に応じたスコアを加算
			ScoreManager::GetInstance()->AddScore(playerCondition_);
		}
	}

	// 2. その後、実際にリストから削除する（既存の処理）
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<Enemy>& e)
	{
		return e->IsDead();
	}),
		enemies_.end()
	);
}

void EnemyManager::DebugDraw() {
    // 統合Inspectorパネルへ移行
}

std::list<std::unique_ptr<Bullet>>& EnemyManager::GetBullets() {
	// リストを一度空にする
	allEnemyBullets_.clear();

	// 現在生きている全敵の弾を一つのリストに集約する
	for (auto& enemy : enemies_) {
		auto& enemyBullets = enemy->GetBullets(); // Enemyクラスに作ったGetBulletsを呼ぶ
		for (auto& bullet : enemyBullets) {
		}
	}
	return allEnemyBullets_;
}

// =====================================================
// 調子アイコン 初期化
// =====================================================

void EnemyManager::SetSpriteCommonAndInitIcons(SpriteCommon* spriteCommon) {
	spriteCommon_ = spriteCommon;
	if (!spriteCommon_) { return; }

	// 調子 → テクスチャパス の対応を登録するだけ（Sprite生成はプール経由）
	conditionPaths_ = {
		{ Condition::ConditionType::Excellent, "Resources/emojiUI/AbsolutelyHappy.png" },
		{ Condition::ConditionType::Good,      "Resources/emojiUI/happy.png"           },
		{ Condition::ConditionType::Normal,    "Resources/emojiUI/Normal.png"          },
		{ Condition::ConditionType::Bad,       "Resources/emojiUI/Slump.png"           },
		{ Condition::ConditionType::Terrible,  "Resources/emojiUI/AbsolutelySlump.png" },
	};
}

// =====================================================
// 調子アイコン 描画
// =====================================================

void EnemyManager::DrawConditionIcons(Camera* camera) {
	if (!spriteCommon_ || conditionPaths_.empty() || !camera) { return; }

	// ---- プール拡張 ----
	// スプライトの Constant Buffer は1インスタンス1バッファなので
	// 敵スロットごとに別インスタンスが必要（共有すると最後の位置でGPUが描画してしまう）
	while (iconPool_.size() < enemies_.size()) {
		const size_t idx = iconPool_.size();
		// 初期テクスチャは Normal（後で条件に応じて SetTexture で変更）
		auto sprite = std::make_unique<Sprite>();
		sprite->Initialize(spriteCommon_, conditionPaths_.at(Condition::ConditionType::Normal));
		sprite->SetSize({ kIconSize, kIconSize });
		sprite->SetAnchorPoint({ 0.5f, 0.5f });
		iconPool_.push_back(std::move(sprite));
		iconConditions_.push_back(Condition::ConditionType::Normal);
	}

	// View * Projection 合成行列（行ベクトル左乗算 P * M の convention）
	const Matrix4x4& view = camera->GetViewMatrix();
	const Matrix4x4& proj = camera->GetProjectionMatrix();
	Matrix4x4 vp = Multiply(view, proj);

	static constexpr float kScreenW = 1280.0f;
	static constexpr float kScreenH = 720.0f;

	for (size_t i = 0; i < enemies_.size(); ++i) {
		const auto& enemy = enemies_[i];
		if (enemy->IsDead()) { continue; }

		// 調子が変わったときだけテクスチャを切り替える
		Condition::ConditionType cond = enemy->GetCondition();
		if (iconConditions_[i] != cond) {
			auto it = conditionPaths_.find(cond);
			if (it != conditionPaths_.end()) {
				iconPool_[i]->SetTexture(it->second);
			}
			iconConditions_[i] = cond;
		}

		// 敵の頭上にアイコンを置く（halfHeight + 余白）
		Vector3 wp = enemy->GetPosition();
		wp.y += enemy->GetHalfHeight() + 0.6f;

		// クリップ空間へ変換
		float wx = wp.x, wy = wp.y, wz = wp.z;
		float cx = wx * vp.m[0][0] + wy * vp.m[1][0] + wz * vp.m[2][0] + vp.m[3][0];
		float cy = wx * vp.m[0][1] + wy * vp.m[1][1] + wz * vp.m[2][1] + vp.m[3][1];
		float cw = wx * vp.m[0][3] + wy * vp.m[1][3] + wz * vp.m[2][3] + vp.m[3][3];

		if (cw <= 0.0f) { continue; } // カメラ後方はスキップ

		float ndcX = cx / cw;
		float ndcY = cy / cw;
		float screenX = (ndcX + 1.0f) * 0.5f * kScreenW;
		float screenY = (1.0f - ndcY) * 0.5f * kScreenH;

		// 画面外はスキップ
		if (screenX < -kIconSize || screenX > kScreenW + kIconSize) { continue; }
		if (screenY < -kIconSize || screenY > kScreenH + kIconSize) { continue; }

		iconPool_[i]->SetPosition({ screenX, screenY });
		iconPool_[i]->Update();
		iconPool_[i]->Draw();
	}
}