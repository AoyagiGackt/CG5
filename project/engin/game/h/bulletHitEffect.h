#pragma once
#include "MakeAffine.h"
#include <string>

class BulletHitEffect{
public:
    // グループの作成
    static void CreateGroup();

    // エフェクトの発生
    static void Emit(const Vector3& position);

private:
    static const std::string kGroupName;
};