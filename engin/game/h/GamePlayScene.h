/**
 * @file GamePlayScene.h
 * @brief ゲームプレイ本編のシーンロジックを管理するファイル
 */
#pragma once

 // --- 標準ライブラリ ---
#include <list>
#include <memory>
#include <string>
#include <vector>

// --- エンジンシステム・基盤 ---
#include "Animation.h"
#include "Audio.h"
#include "BaseScene.h"
#include "Camera.h"
#include "CollisionManager.h"
#include "DirectXCommon.h"
#include "GameObject.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Model.h"
#include "ModelCommon.h"
#include "Object3dCommon.h"
#include "ShadowManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "SrvManager.h"
#include "VideoPlayer.h"
#include "Fade.h"

// --- ゲームロジック・オブジェクト ---
#include "BuffManager.h"
#include "Bullet.h"
#include "EmojiUI.h"
#include "EnemyManager.h"
#include "GameTime.h"
#include "MapChipField.h"
#include "PauseScene.h"
#include "Player.h"
#include "PlayerManager.h"
#include "ScoreDisplay.h"
#include "Skydome.h"
#include "TimeDisplay.h"
#include "Turret.h"
#include "hoge.h"

// 雲1枚分のデータ
struct CloudData{
	std::unique_ptr<Sprite> sprite;
	float posX;
	float posY;
	float speed;
	float width;
	float height;
};

// チュートリアルの進行ステップ
enum class TutorialStep{
	Move,           // 移動：地面停止、基本操作
	Shoot,          // 射撃：地面停止、的撃ち
	ScrollStart,    // スクロール開始
	BattleTrain,    // 実戦練習：走りながら敵を3体倒す
	SwipeTrain,     // スワイプ練習：調子を上げる
	SpecialMove,    // 必殺技
	Epilogue,       // 合格演出
	None            // 本編開始
};

/**
 * @brief ゲームプレイ本編のシーンクラス
 */
class GamePlayScene : public BaseScene{
public:
	// --- 基本関数 ---
	void Initialize(DirectXCommon* dxCommon,Input* input,Audio* audio) override;
	void Finalize() override;
	void Update() override;
	void Draw() override;

	// --- パブリックメソッド ---
	void SetImGuiManager(ImGuiManager* imgui){ imguiManager_ = imgui; }
	void UpdateTutorial();

private:
	// --- 内部処理関数 ---
	void UpdateDebugUI();
	void DrawShadowPass();

	// --- データセーブ・ロード関数 ---
	void SaveCameraParams();
	void LoadCameraParams();
	void SaveEnemyParams();
	void LoadEnemyParams();
	void SaveModelPaths();
	void LoadModelPaths();
	void SaveTurretData();
	void LoadTurretData();
	void SaveUILayout();
	void LoadUILayout();

	// =================================================
	// メンバ変数
	// =================================================

	// --- 外部システムポインタ ---
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	ImGuiManager* imguiManager_ = nullptr;

	// --- 描画・共通基盤リソース ---
	std::unique_ptr<SpriteCommon> spriteCommon_;
	std::unique_ptr<ModelCommon> modelCommon_;
	std::unique_ptr<Object3dCommon> objectCommon_;
	std::unique_ptr<ShadowManager> shadowManager_;
	std::unique_ptr<Camera> camera_;
	std::unique_ptr<CollisionManager> collisionManager_;

	// --- マネージャクラス ---
	std::unique_ptr<PlayerManager> playerManager_;
	std::unique_ptr<EnemyManager> enemyManager_;
	std::unique_ptr<BuffManager> buffManager_;

	// --- 3Dモデル群 ---
	std::unique_ptr<Model> modelSkydome_;
	std::unique_ptr<Model> modelBlock_;
	std::unique_ptr<Model> modelPlayer_;
	std::unique_ptr<Model> modelEnemy_;
	std::unique_ptr<Model> modelBullet_;
	std::unique_ptr<Model> modelObstacle1_;
	std::unique_ptr<Model> modelObstacle2_;
	std::unique_ptr<Model> modelBeam_;

	// --- ゲームオブジェクト群 ---
	std::vector<std::unique_ptr<GameObject>> gameObjects_;
	std::list<std::unique_ptr<Bullet>> bullets_;
	std::vector<std::unique_ptr<Turret>> turrets_;
	std::unique_ptr<MapChipField> mapField_;
	std::unique_ptr<Skydome> skydome_;

	// --- UI・スプライト群 ---
	std::unique_ptr<Sprite> sprite1_;
	std::unique_ptr<Sprite> skyOverlay_;
	std::unique_ptr<Sprite> swipeUI_[4];
	TimeDisplay timeDisplay_;
	ScoreDisplay scoreDisplay_;
	EmojiUI emojiUI_;
	PauseScene pauseScene_;

	//tutorial関係
	float displayTimer_ = 0.0f;
	bool isExiting_ = false;   // 消え始めたフラグ
	float exitTimer_ = 0.0f;   // 消去演出専用タイマー
	bool isSkipTriggered_ = false;
	std::unique_ptr<Sprite> skipUI_;
	std::unique_ptr<Sprite> moveUI_;
	std::unique_ptr<Sprite> moveGuide_;
	std::unique_ptr<Sprite> shootUI_;
	std::unique_ptr<Sprite> shootGuide_;
	std::unique_ptr<Sprite> scrollStartUI_;
	std::unique_ptr<Sprite> battleTrainUI_;
	std::unique_ptr<Sprite> battleTrainGuide_;
	std::unique_ptr<Sprite> swipeTrainUI_;
	std::unique_ptr<Sprite> swipeTrainGuide_;
	std::unique_ptr<Sprite> specialMoveUI_;
	std::unique_ptr<Sprite> specialMoveGuide_;
	std::unique_ptr<Sprite> epilogueUI_;

	// --- メディア系 ---
	SoundData bgmData_;
	std::unique_ptr<VideoPlayer> videoPlayer_;
	std::vector<std::string> videoList_;
	int currentVideoIndex_ = 0;

	// --- 進行・状態管理 ---
	GameTime gameTime_;
	Animation playerAnimation_;
	float animationTime_ = 0.0f;
	int videoSwitchCount_ = 0;
	int rapidFireTimer_ = 0;
	bool isPaused_ = false;
	std::vector<CloudData> clouds_;

	// --- チュートリアル関連 ---
	TutorialStep tutorialStep_ = TutorialStep::Move;
	float tutorialTimer_ = 0.0f;
	float waveStrength_ = 0.0f;
	int tutorialCount_ = 0;
	bool isTutorialActive_ = true;
	bool spawnOnce_ = true;

	// --- デバッグ・エディタ関連 ---
	bool debugScrollPaused_ = false;
	bool debugSpawnDisabled_ = false;
	bool debugEditMode_ = false;

	std::string playerObjPath_ = "Resources/player/player.obj";
	std::string playerTexPath_ = "Resources/player/player.png";
	std::string enemyObjPath_ = "Resources/boss/boss.obj";
	std::string enemyTexPath_ = "Resources/boss/boss.png";

	enum class SelectedType{ None,Player,Enemy,Turret,Camera,EnemySettings,UIElement };
	SelectedType editorSelectedType_ = SelectedType::None;
	int editorSelectedIndex_ = -1;
	std::vector<std::string> turretNames_;

	struct UIEntry{
		std::string name;
		std::unique_ptr<Sprite> sprite;
		std::string texPath;
	};
	std::vector<UIEntry> uiElements_;

	Fade fade_;
};