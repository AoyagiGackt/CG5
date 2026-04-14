/**
 * @file GameTime.h
 * @brief ゲーム内時刻（18:00 → 翌 6:00）を管理するクラス
 *
 * プレイヤーの「調子」に応じてゲーム内時刻の進む速さが変わる。
 *   絶好調 : 1リアル秒 = 10ゲーム分進む
 *   好調   : 1リアル秒 =  5ゲーム分進む
 *   普通   : 1リアル秒 =  1ゲーム分進む
 *   不調   : 2リアル秒 =  1ゲーム分進む (0.5分/秒)
 *   絶不調 : 1リアル秒 = 10ゲーム秒進む (1/6分/秒)
 */
#pragma once
#include "Condition.h"

class GameTime {
public:
    // 開始時刻（18:00）を分に換算
    static constexpr int kStartMinutes = 18 * 60;   // 1080
    // 終了時刻（翌 6:00）を分に換算（24時間超え表現）
    static constexpr int kEndMinutes   = 30 * 60;   // 1800
    // ゲーム内で進める合計分数
    static constexpr float kTotalGameMinutes = float(kEndMinutes - kStartMinutes); // 720

    /** @brief 初期化（経過時間をリセット） */
    void Initialize();

    /**
     * @brief 毎フレーム呼び出す更新処理
     * @param condition プレイヤーの現在の調子
     * @note 60fps 前提で、1フレームあたり (倍率/60) ゲーム分を加算する
     */
    void Update(Condition::ConditionType condition);

    /** @brief 翌6:00 に達したら true を返す */
    bool IsCleared() const { return elapsedMinutes_ >= kTotalGameMinutes; }

    /** @brief 現在の時刻（時） 0-23 */
    int GetHour() const;

    /** @brief 現在の時刻（分） 0-59 */
    int GetMinute() const;

    /** @brief 経過ゲーム分数（0〜720） */
    float GetElapsedMinutes() const { return elapsedMinutes_; }

    /** @brief ゲーム内時刻を指定分数だけ進める（デバッグ用） */
    void SkipMinutes(float minutes);

private:
    /**
     * @brief 調子に応じたゲーム分/リアル秒の倍率を返す
     */
    static float GetMinutesPerSecond(Condition::ConditionType condition);

    // 18:00 からの経過ゲーム分数
    float elapsedMinutes_ = 0.0f;
};
