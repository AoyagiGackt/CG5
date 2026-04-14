#include "BulletHitEffect.h"
#include "ParticleManager.h"
#include <random>
#include <algorithm>

static std::mt19937 sRng{std::random_device{}()};
const std::string BulletHitEffect::kGroupName = "bulletHit";

void BulletHitEffect::CreateGroup(){
	ParticleManager::GetInstance()->CreateParticleGroup(kGroupName,"Resources/bulletHitEffect.png");
}

void BulletHitEffect::Emit(const Vector3& position){
	std::uniform_real_distribution<float> velDist(-2.0f,2.0f);
	std::uniform_real_distribution<float> lifeDist(0.05f,0.15f);
	// 色のばらつき範囲（-0.2 ～ 0.2）
	std::uniform_real_distribution<float> colorDist(-0.2f,0.2f);

	for(int i = 0; i < 30; ++i){
		Vector3 vel = {velDist(sRng), velDist(sRng), velDist(sRng)};

		Vector4 color = {
		   1.0f,                                           // R
			std::clamp(0.2f + colorDist(sRng), 0.0f, 1.0f), // G
			std::clamp(0.1f + colorDist(sRng), 0.0f, 1.0f), // B
			1.0f                                            // A
		};

		ParticleManager::GetInstance()->EmitWithColor(
			kGroupName,
			position,
			vel,
			color,
			lifeDist(sRng),
			0.2f
		);
	}
}