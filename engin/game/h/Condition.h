#pragma once

class Condition{
public:
	enum class ConditionType{
		Excellent,
		Good,
		Normal,
		Bad,
		Terrible,
	};

	void Initialize();
	void Update();

	void RankUp();
	void RankDown();

	// 外から調子を変更・確認するための関数（ゲッターとセッター）
	void SetCondition(ConditionType type){ conditionType_ = type; }
	ConditionType GetCondition() const{ return conditionType_; }

	//　調子に応じたステータス倍率（スピード）を返す関数
	float GetSpeedMultiplier() const;

	//　調子に応じたステータス倍率（スクロール）を返す関数
	float GetScrollSpeedMultiplier() const;

	//　調子に応じたステータス倍率（ジャンプ）を返す関数
	float GetJumpMultiplier() const;

private:
	// 初期値として「普通」を入れておくと安全です
	ConditionType conditionType_ = ConditionType::Normal;
};