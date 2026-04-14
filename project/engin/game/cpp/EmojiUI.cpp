/**
 * @file EmojiUI.cpp
 */
#include "EmojiUI.h"
#include <string>

// =====================================================
// 初期化
// =====================================================

void EmojiUI::Initialize(ModelCommon* modelCommon)
{
    // ---- 登録済み絵文字 ----
    // 新しい調子モデルができたらここに追加するだけでOK
    RegisterEmojiParts(modelCommon, "face",  "happy_eyesMouth", Condition::ConditionType::Excellent);
    RegisterEmojiParts(modelCommon, "face",  "Slump_eyesMouth", Condition::ConditionType::Terrible);
    RegisterEmoji(modelCommon, "Normal",          Condition::ConditionType::Normal);
    // face.obj = 顔球、happy_eyesMouth.obj = happy用の目口のみ、Slump_eyesMouth.obj = slump用の目口のみ
    RegisterEmoji(modelCommon, "AbsolutelyHappy", Condition::ConditionType::Good);
    RegisterEmoji(modelCommon, "AbsolutelySlump", Condition::ConditionType::Bad);
}

// =====================================================
// 更新
// =====================================================

void EmojiUI::Update(Condition::ConditionType condition, Camera* camera)
{
    currentEntry_ = GetEntry(condition);
    if (!currentEntry_) { return; }

    // カメラ位置からの相対オフセットで左下・手前に固定
    Vector3 camPos = camera->GetTranslate();
    Vector3 pos = {
        camPos.x + kOffsetX,
        camPos.y + kOffsetY,
        camPos.z + kOffsetZ,
    };

    currentEntry_->face->SetPosition(pos);
    currentEntry_->face->SetScale({ kScale, kScale, kScale });
    currentEntry_->face->Update();

    if (currentEntry_->eyesMouth) {
        currentEntry_->eyesMouth->SetPosition(pos);
        currentEntry_->eyesMouth->SetScale({ kScale, kScale, kScale });
        currentEntry_->eyesMouth->Update();
    }
}

// =====================================================
// 描画
// =====================================================

void EmojiUI::Draw()
{
    if (!currentEntry_) { return; }
    currentEntry_->face->Draw();
    if (currentEntry_->eyesMouth) {
        currentEntry_->eyesMouth->Draw();
    }
}

// =====================================================
// プライベート
// =====================================================

void EmojiUI::RegisterEmoji(ModelCommon* modelCommon,
                             const std::string& name,
                             Condition::ConditionType condition)
{
    const std::string base = "Resources/emojiUI/";
    const std::string tex  = "Resources/white.png";

    EmojiEntry e;

    e.faceModel = std::make_unique<Model>();
    e.faceModel->Initialize(modelCommon, base + name + "_face.obj", tex);

    e.eyesMouthModel = std::make_unique<Model>();
    e.eyesMouthModel->Initialize(modelCommon, base + name + "_eyesMouth.obj", tex);

    e.face = std::make_unique<Object3d>();
    e.face->Initialize(modelCommon);
    e.face->SetModel(e.faceModel.get());
    e.face->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色
    e.face->SetEnableLighting(false);

    e.eyesMouth = std::make_unique<Object3d>();
    e.eyesMouth->Initialize(modelCommon);
    e.eyesMouth->SetModel(e.eyesMouthModel.get());
    e.eyesMouth->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f }); // 黒
    e.eyesMouth->SetEnableLighting(false);

    emojis_.emplace(condition, std::move(e));
}

void EmojiUI::RegisterEmojiParts(ModelCommon* modelCommon,
                                  const std::string& faceFilename,
                                  const std::string& eyesFilename,
                                  Condition::ConditionType condition)
{
    const std::string base = "Resources/emojiUI/";
    const std::string tex  = "Resources/white.png";

    EmojiEntry e;

    e.faceModel = std::make_unique<Model>();
    e.faceModel->Initialize(modelCommon, base + faceFilename + ".obj", tex);

    e.face = std::make_unique<Object3d>();
    e.face->Initialize(modelCommon);
    e.face->SetModel(e.faceModel.get());
    e.face->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色
    e.face->SetEnableLighting(false);

    e.eyesMouthModel = std::make_unique<Model>();
    e.eyesMouthModel->Initialize(modelCommon, base + eyesFilename + ".obj", tex);

    e.eyesMouth = std::make_unique<Object3d>();
    e.eyesMouth->Initialize(modelCommon);
    e.eyesMouth->SetModel(e.eyesMouthModel.get());
    e.eyesMouth->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f }); // 黒
    e.eyesMouth->SetEnableLighting(false);

    emojis_.emplace(condition, std::move(e));
}

void EmojiUI::RegisterEmojiSingle(ModelCommon* modelCommon,
                                   const std::string& filename,
                                   Condition::ConditionType condition)
{
    const std::string base = "Resources/emojiUI/";
    const std::string tex  = "Resources/white.png";

    EmojiEntry e;

    e.faceModel = std::make_unique<Model>();
    e.faceModel->Initialize(modelCommon, base + filename + ".obj", tex);

    e.face = std::make_unique<Object3d>();
    e.face->Initialize(modelCommon);
    e.face->SetModel(e.faceModel.get());
    e.face->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // 黄色
    e.face->SetEnableLighting(false);

    // eyesMouth は使わない（1ファイル構成）
    emojis_.emplace(condition, std::move(e));
}

EmojiUI::EmojiEntry* EmojiUI::GetEntry(Condition::ConditionType condition)
{
    // 完全一致
    auto it = emojis_.find(condition);
    if (it != emojis_.end()) { return &it->second; }

    // フォールバック: 登録済みの中から近い調子を使う
    auto asInt = [](Condition::ConditionType c) {
        switch (c) {
        case Condition::ConditionType::Excellent: return 4;
        case Condition::ConditionType::Good:      return 3;
        case Condition::ConditionType::Normal:    return 2;
        case Condition::ConditionType::Bad:       return 1;
        case Condition::ConditionType::Terrible:  return 0;
        default: return 2;
        }
    };

    int target = asInt(condition);
    EmojiEntry* best = nullptr;
    int bestDist = 999;
    for (auto& [cond, entry] : emojis_) {
        int dist = asInt(cond) - target;
        if (dist < 0) { dist = -dist; }
        if (dist < bestDist) {
            bestDist = dist;
            best = &entry;
        }
    }
    return best;
}
