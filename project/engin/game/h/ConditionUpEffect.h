#pragma once
#include "MakeAffine.h" 
#include "Camera.h" // カメラクラスの定義
#include <string>
#include <vector>

class ConditionUpEffect{
public:
    // カメラの直前にエフェクトを放出する
    static void Emit(Camera* camera);
    static void CreateGroup();

private:
    // ParticleManagerで設定済みの、絵文字テクスチャを使うグループ名のリスト
    static const std::vector<std::string> kEmojiGroups;
};