#include "GameTime.h"

void GameTime::Initialize()
{
    elapsedMinutes_ = 0.0f;
}

void GameTime::Update(Condition::ConditionType condition)
{
    if (IsCleared()) {
        return;
    }

    // 1フレーム（60fps前提）あたりに進めるゲーム分数
    float minutesPerFrame = GetMinutesPerSecond(condition) / 60.0f;
    elapsedMinutes_ += minutesPerFrame;

    // 終了時刻を超えたらクランプ
    if (elapsedMinutes_ > kTotalGameMinutes) {
        elapsedMinutes_ = kTotalGameMinutes;
    }
}

int GameTime::GetHour() const
{
    int totalMinutes = kStartMinutes + static_cast<int>(elapsedMinutes_);
    return (totalMinutes / 60) % 24;
}

int GameTime::GetMinute() const
{
    int totalMinutes = kStartMinutes + static_cast<int>(elapsedMinutes_);
    return totalMinutes % 60;
}

void GameTime::SkipMinutes(float minutes)
{
    elapsedMinutes_ += minutes;
    if (elapsedMinutes_ > kTotalGameMinutes) {
        elapsedMinutes_ = kTotalGameMinutes;
    }
}

float GameTime::GetMinutesPerSecond(Condition::ConditionType condition)
{
    switch (condition) {
    case Condition::ConditionType::Excellent:
        // 絶好調: 1リアル秒 = 10ゲーム分
        return 10.0f;

    case Condition::ConditionType::Good:
        // 好調: 1リアル秒 = 5ゲーム分
        return 5.0f;

    case Condition::ConditionType::Normal:
        // 普通: 1リアル秒 = 1ゲーム分
        return 1.0f;

    case Condition::ConditionType::Bad:
        // 不調: 2リアル秒で1ゲーム分 (= 0.5ゲーム分/秒)
        return 0.5f;

    case Condition::ConditionType::Terrible:
        // 絶不調: 1リアル秒 = 10ゲーム秒 (= 10/60 ゲーム分/秒)
        return 10.0f / 60.0f;

    default:
        return 1.0f;
    }
}
