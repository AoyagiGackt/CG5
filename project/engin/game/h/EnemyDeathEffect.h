#pragma once
#include "MakeAffine.h"

/**
 * @brief 敵撃破時の爆発パーティクルエフェクトを管理するユーティリティクラス
 */
class EnemyDeathEffect {
public:
    /** @brief パーティクルグループを登録する（GamePlayScene::Initialize で1回呼ぶ） */
    static void CreateGroup();

    /** @brief 指定座標に爆発パーティクルをバースト発生させる */
    static void Emit(const Vector3& position);
};
