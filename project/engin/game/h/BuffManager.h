/**
 * @file BuffManager.h
 * @brief バフ選択 UI とバフ効果の管理を担当するクラス
 *
 * ゲーム内時間が 1 時間進むたびにバフ選択画面を表示する。
 * プレイヤーは 3 種類のバフから 1 つを選択できる。
 *
 * 選択中は IsSelecting() が true を返し、
 * GamePlayScene 側でゲームロジックの更新を止める。
 *
 * バフ種類（BuffType）:
 *   SpeedUp   … 弾速 +50%
 *   SizeUp    … 弾サイズ +50%（当たり判定も拡大）
 *   RapidFire … 連射間隔を短縮（ボタン長押しで自動連射）
 */
#pragma once
#include "Sprite.h"
#include "SpriteCommon.h"
#include "Input.h"
#include <memory>

// =====================================================
// バフの種類
// =====================================================
enum class BuffType {
    SpeedUp,   ///< 左：弾速アップ
    SizeUp,    ///< 中：弾サイズアップ
    RapidFire, ///< 右：連射（ボタン長押し自動連射、間隔を短縮）
};

// =====================================================
// 現在有効なバフ効果
// =====================================================
struct BuffEffect {
    float bulletSpeedMultiplier = 1.0f; ///< 弾速の倍率（スタック可）
    float bulletScaleMultiplier = 1.0f; ///< 弾スケールの倍率（スタック可）
    /**
     * @brief 連射間隔（フレーム数）
     *  0  : 連射無効（タップのみ）
     *  > 0: 長押し中、このフレーム数ごとに自動発射
     *  選択するたびに -2 ずつ短縮（最短 2 フレーム ≒ 30 連射/秒）
     */
    int rapidFireInterval = 0;
};

// =====================================================
// バフマネージャー本体
// =====================================================
class BuffManager {
public:
    /**
     * @brief 初期化（スプライトを生成）
     * @param spriteCommon スプライト共通設定
     */
    void Initialize(SpriteCommon* spriteCommon);

    /**
     * @brief 毎フレーム更新（選択中のみ入力を受け付ける）
     * @param input 入力クラス
     */
    void Update(Input* input);

    /**
     * @brief バフ選択 UI を描画（選択中のみ）
     * @note SpriteCommon::CommonDrawSettings() が事前に呼ばれていること
     */
    void Draw();

    /** @brief バフ選択画面を開く */
    void TriggerSelection();

    /** @brief 選択中かどうか */
    bool IsSelecting() const { return isSelecting_; }

    /** @brief 現在有効なバフ効果を取得 */
    const BuffEffect& GetEffect() const { return effect_; }

private:
    void ApplyBuff(BuffType type);
    void UpdateCardColors();

    bool isSelecting_ = false;
    int  selectedIndex_ = 1; ///< 0=左(Speed), 1=中(Size), 2=右(Triple)

    BuffEffect effect_;

    // ---- UI スプライト ----
    std::unique_ptr<Sprite> overlay_;    ///< 全画面半透明背景
    std::unique_ptr<Sprite> cards_[3];   ///< バフカード 3 枚
    std::unique_ptr<Sprite> icons_[3];   ///< カード上のアイコン画像
    std::unique_ptr<Sprite> cursor_;     ///< 選択枠（白い縁取り）
};
