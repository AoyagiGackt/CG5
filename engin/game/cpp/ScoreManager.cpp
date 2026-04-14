#include "ScoreManager.h"
#include <algorithm>
#include <fstream>

ScoreManager* ScoreManager::GetInstance()
{
    static ScoreManager instance;
    return &instance;
}

void ScoreManager::ResetCurrentScore()
{
    currentScore_ = 0;
}

void ScoreManager::AddScore(Condition::ConditionType cond)
{
    int pts = 200;
    switch (cond) {
    case Condition::ConditionType::Excellent: pts = 500; break;
    case Condition::ConditionType::Good:      pts = 300; break;
    case Condition::ConditionType::Normal:    pts = 200; break;
    case Condition::ConditionType::Bad:       pts = 100; break;
    case Condition::ConditionType::Terrible:  pts =  50; break;
    }
    currentScore_ += pts;
}

void ScoreManager::SubmitAndSave()
{
    ranking_.push_back(currentScore_);
    std::sort(ranking_.begin(), ranking_.end(), std::greater<int>());
    if ((int)ranking_.size() > kMaxRank) {
        ranking_.resize(kMaxRank);
    }
    SaveScores();
}

void ScoreManager::ResetAllScores()
{
    ranking_.clear();
    SaveScores();
}

void ScoreManager::LoadScores()
{
    ranking_.clear();
    std::ifstream file(kSaveFile);
    if (!file.is_open()) { return; }
    int score;
    while (file >> score) {
        ranking_.push_back(score);
    }
    std::sort(ranking_.begin(), ranking_.end(), std::greater<int>());
    if ((int)ranking_.size() > kMaxRank) {
        ranking_.resize(kMaxRank);
    }
}

void ScoreManager::SaveScores()
{
    std::ofstream file(kSaveFile);
    for (int s : ranking_) {
        file << s << "\n";
    }
}
