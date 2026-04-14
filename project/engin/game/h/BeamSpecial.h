#pragma once
#include "SpecialMove.h"
#include "Object3d.h"
#include "ModelCommon.h"
#include <memory>



class BeamSpecial : public SpecialMove
{

public:

	void Initialize(Player* player) override;
	void Update() override;
	void Draw() override;
	void Activate() override;

	void InitBeam(Player* player, ModelCommon* modelCommon, Model* model);

	Vector3 GetPosition() const { return beamObject_->GetTransform().translate; }
	float GetLength() const { return beamObject_->GetTransform().scale.x;}
	float GetWidth() const { return beamObject_->GetTransform().scale.z; }
	bool IsActive() const { return isActive_; }

	AABB GetAABB() const;

private:

	std::unique_ptr<Object3d> beamObject_ = nullptr;

	int timer_ = 0;
	const int kDuration = 60; // ビーム持続フレーム

	float beamLength_ = 1.0f; // 長さ
	float beamWidth_ = 1.0f;  // 太さ

	float offsetFromPlayer_ = 0.5f;// Playerの中心からビームの開始位置までの距離
	float playerHalfWidth = 0.35f;

	bool hasHit_ = false; // 多段ヒット防止



};

