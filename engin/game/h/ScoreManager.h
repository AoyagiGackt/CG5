/**
 * @file ScoreManager.h
 * @brief スコアの管理・保存・ランキングを行うクラス
 */
#pragma once
#include "Condition.h"
#include <vector>

class ScoreManager {
public:
    static ScoreManager* GetInstance();

    // --- 現セッション ---

    /** @brief 現在のスコアを0にリセット（ゲーム開始時に呼ぶ） */
    void ResetCurrentScore();

    /**
     * @brief プレイヤーの調子に応じたスコアを加算する
     * @param cond 撃破時のプレイヤー調子
     * Excellent:500 / Good:300 / Normal:200 / Bad:100 / Terrible:50
     */
    void AddScore(Condition::ConditionType cond);

    int GetCurrentScore() const { return currentScore_; }

    // --- ランキング ---

    /** @brief 現在スコアをランキングに登録してファイル保存（ゲームクリア時に呼ぶ） */
    void SubmitAndSave();

    /** @brief ランキングを全消去してファイルにも反映 */
    void ResetAllScores();

    /** @brief 保存ファイルからランキングを読み込む */
    void LoadScores();

    const std::vector<int>& GetRanking() const { return ranking_; }

private:
    ScoreManager() = default;

    int currentScore_ = 0;
    std::vector<int> ranking_;

    static constexpr int kMaxRank = 10;
    static constexpr const char* kSaveFile = "scores.txt";

    void SaveScores();
};
