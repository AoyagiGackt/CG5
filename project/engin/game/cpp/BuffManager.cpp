#include "BuffManager.h"
#include <dinput.h>

// =====================================================
// カードレイアウト定数
// =====================================================
static const Vector2 kCardSize   = { 280.0f, 380.0f };
static const float   kCardY      = 170.0f;
static const float   kCardX[3]   = { 120.0f, 500.0f, 880.0f }; // 左・中・右
static const float   kCursorPad  = 10.0f; // カーソルのはみ出し量

// カード色（選択されていない状態 / 選択状態は alpha を上げる）
static const Vector4 kCardColors[3] = {
    { 0.15f, 0.35f, 0.90f, 0.85f }, // SpeedUp   : 青
    { 0.10f, 0.75f, 0.30f, 0.85f }, // SizeUp    : 緑
    { 0.85f, 0.20f, 0.20f, 0.85f }, // TripleShot: 赤
};
static const Vector4 kCardColorsSelected[3] = {
    { 0.35f, 0.55f, 1.00f, 1.0f },
    { 0.30f, 0.95f, 0.50f, 1.0f },
    { 1.00f, 0.40f, 0.40f, 1.0f },
};

// =====================================================
// 初期化
// =====================================================

void BuffManager::Initialize(SpriteCommon* spriteCommon)
{
    // 全画面半透明黒オーバーレイ
    overlay_ = std::make_unique<Sprite>();
    overlay_->Initialize(spriteCommon, "Resources/white.png");
    overlay_->SetPosition({ 0.0f, 0.0f });
    overlay_->SetSize({ 1280.0f, 720.0f });
    overlay_->SetColor({ 0.0f, 0.0f, 0.0f, 0.75f });
    overlay_->SetAnchorPoint({ 0.0f, 0.0f });

    // バフカード 3 枚
    for (int i = 0; i < 3; ++i) {
        cards_[i] = std::make_unique<Sprite>();
        cards_[i]->Initialize(spriteCommon, "Resources/white.png");
        cards_[i]->SetPosition({ kCardX[i], kCardY });
        cards_[i]->SetSize(kCardSize);
        cards_[i]->SetColor(kCardColors[i]);
        cards_[i]->SetAnchorPoint({ 0.0f, 0.0f });
    }

    // カードアイコン（speedUp / big / rapidFire）
    static const char* kIconPaths[3] = {
        "Resources/UI/speedUp.png",
        "Resources/UI/big.png",
        "Resources/UI/rapidFire.png",
    };
    static const Vector2 kIconSize = { 200.0f, 200.0f };
    for (int i = 0; i < 3; ++i) {
        icons_[i] = std::make_unique<Sprite>();
        icons_[i]->Initialize(spriteCommon, kIconPaths[i]);
        icons_[i]->SetSize(kIconSize);
        icons_[i]->SetAnchorPoint({ 0.5f, 0.5f }); // 中央揃え
        // カードの中央上部に配置
        icons_[i]->SetPosition({
            kCardX[i] + kCardSize.x * 0.5f,
            kCardY    + kCardSize.y * 0.35f
        });
    }

    // カーソル（白い縁取り）
    cursor_ = std::make_unique<Sprite>();
    cursor_->Initialize(spriteCommon, "Resources/white.png");
    cursor_->SetSize({ kCardSize.x + kCursorPad * 2.0f,
                       kCardSize.y + kCursorPad * 2.0f });
    cursor_->SetColor({ 1.0f, 1.0f, 1.0f, 0.9f });
    cursor_->SetAnchorPoint({ 0.0f, 0.0f });

    UpdateCardColors();
}

// =====================================================
// 毎フレーム更新
// =====================================================

void BuffManager::Update(Input* input)
{
    if (!isSelecting_) { return; }

    // ---- 左右移動 ----
    if (input->TriggerKey(DIK_LEFT) || input->TriggerKey(DIK_A) ||
        input->TriggerButton(XINPUT_GAMEPAD_DPAD_LEFT)) {
        selectedIndex_ = (selectedIndex_ + 2) % 3; // 左へ（0,1,2 サイクル）
        UpdateCardColors();
    }
    if (input->TriggerKey(DIK_RIGHT) || input->TriggerKey(DIK_D) ||
        input->TriggerButton(XINPUT_GAMEPAD_DPAD_RIGHT)) {
        selectedIndex_ = (selectedIndex_ + 1) % 3; // 右へ
        UpdateCardColors();
    }

    // ---- 決定 ----
    if (input->TriggerKey(DIK_RETURN) || input->TriggerKey(DIK_SPACE) ||
        input->TriggerButton(XINPUT_GAMEPAD_A)) {
        ApplyBuff(static_cast<BuffType>(selectedIndex_));
        isSelecting_ = false;
    }
}

// =====================================================
// 描画
// =====================================================

void BuffManager::Draw()
{
    if (!isSelecting_) { return; }

    overlay_->Update();
    overlay_->Draw();

    // カーソル（選択カードの後ろに描画）
    cursor_->SetPosition({
        kCardX[selectedIndex_] - kCursorPad,
        kCardY - kCursorPad
    });
    cursor_->Update();
    cursor_->Draw();

    // カード 3 枚 + アイコン
    for (int i = 0; i < 3; ++i) {
        cards_[i]->Update();
        cards_[i]->Draw();

        icons_[i]->Update();
        icons_[i]->Draw();
    }
}

// =====================================================
// バフ選択を開く
// =====================================================

void BuffManager::TriggerSelection()
{
    selectedIndex_ = 1; // デフォルトは中央（SizeUp）
    UpdateCardColors();
    isSelecting_ = true;
}

// =====================================================
// バフを適用
// =====================================================

void BuffManager::ApplyBuff(BuffType type)
{
    switch (type) {
    case BuffType::SpeedUp:
        effect_.bulletSpeedMultiplier += 0.5f;    // 弾速 +50%（スタック）
        break;
    case BuffType::SizeUp:
        effect_.bulletScaleMultiplier += 0.5f;    // 弾サイズ +50%（スタック）
        break;
    case BuffType::RapidFire:
        // 初回: 10 フレーム間隔（6 連射/秒）、以降 -2 ずつ短縮、最短 2 フレーム（30 連射/秒）
        if (effect_.rapidFireInterval == 0) {
            effect_.rapidFireInterval = 10;
        } else if (effect_.rapidFireInterval > 2) {
            effect_.rapidFireInterval -= 2;
        }
        break;
    }
}

// =====================================================
// カードの色を更新（選択状態を反映）
// =====================================================

void BuffManager::UpdateCardColors()
{
    for (int i = 0; i < 3; ++i) {
        if (!cards_[i]) { continue; }
        cards_[i]->SetColor(i == selectedIndex_ ? kCardColorsSelected[i] : kCardColors[i]);
    }
}
