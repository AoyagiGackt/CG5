#include "BeamSpecial.h"
#include "Player.h"
#include "EnemyManager.h"

void BeamSpecial::InitBeam(Player* player, ModelCommon* modelCommon, Model* model)
{

	player_ = player;

	beamLength_ = 3.5f;
	beamWidth_ = 1.0f;

	beamObject_ = std::make_unique<Object3d>();
	beamObject_->Initialize(modelCommon);
	beamObject_->SetModel(model);
	beamObject_->SetScale({ 0.0f, 0.0f, 0.0f });


	isActive_ = false;
	timer_ = 0;



}
void BeamSpecial::Initialize(Player* player)
{
}

void BeamSpecial::Update()
{

	if (!isActive_)
	{
		return;
	}

	timer_ --;

	if (timer_ <= 0)
	{
		isActive_ = false;
		beamObject_->SetScale({ 0.0f, 0.0f, 0.0f });
		return;
	}

	Vector3 playerPos = player_->GetPosition();
	
	Vector3 beamPos = playerPos;
	beamPos.x += playerHalfWidth + offsetFromPlayer_;

	beamObject_->SetPosition(beamPos);



    beamObject_->Update();

}

void BeamSpecial::Draw()
{

	if (isActive_)
	{

		beamObject_->Draw();

	}

}

void BeamSpecial::Activate()
{

	if (isActive_)
	{
		return;
	}

	isActive_ = true;
	timer_ = kDuration;
	hasHit_ = false;
	beamObject_->SetScale({ beamLength_, beamWidth_, beamWidth_ });

}


AABB BeamSpecial::GetAABB()const
{

	Vector3 Position = beamObject_->GetTransform().translate;
	Vector3 scale = beamObject_->GetTransform().scale;
	float length = scale.x;
	float halfWidth = scale.z * 0.5f;


	AABB beamAABB;
	beamAABB.min = {
		Position.x,
		Position.y - halfWidth,
		Position.z - halfWidth
	};

	beamAABB.max = {
		Position.x + length,
		Position.y + halfWidth,
		Position.z + halfWidth
	};

	return beamAABB;
	
}