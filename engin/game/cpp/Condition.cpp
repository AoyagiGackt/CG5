#include "Condition.h"
#include "ImGuiManager.h"

void Condition::Initialize(){
	conditionType_ = ConditionType::Normal; // 最初は普通からスタート
}

void Condition::Update(){

#ifdef USE_IMGUI
	ImGui::Begin("Player Debug");

	// ImGuiのComboで表示するための文字列配列（enumの順番と完全に合わせる！）
	const char* conditionNames[] = {"Excellent", "Good", "Normal", "Bad", "Terrible"};

	// 現在の enum の値を、ImGuiが読める int 型に変換して変数に入れる
	int currentItem = static_cast<int>(conditionType_);

	// コンボボックス（プルダウンメニュー）を表示
	// もしユーザーがメニューから別の調子を選んだら、ifの中に入ります
	if(ImGui::Combo("Condition",&currentItem,conditionNames,IM_ARRAYSIZE(conditionNames))){
		// 4. 選ばれた int の値を、再び ConditionType (enum) に戻して適用する
		conditionType_ = static_cast<ConditionType>(currentItem);
	}

	// ちゃんと倍率が反映されているか確認できるようにテキスト表示
	ImGui::Text("Current Speed Multiplier: %.2f",GetSpeedMultiplier());

	ImGui::Text("Jump Multiplier: %.2f",GetJumpMultiplier());

	ImGui::End();
#endif 


}

// 調子を1段階上げる
void Condition::RankUp(){
	switch(conditionType_){
		case ConditionType::Terrible: conditionType_ = ConditionType::Bad;       break;
		case ConditionType::Bad:      conditionType_ = ConditionType::Normal;    break;
		case ConditionType::Normal:   conditionType_ = ConditionType::Good;      break;
		case ConditionType::Good:     conditionType_ = ConditionType::Excellent; break;
		case ConditionType::Excellent:
		// すでに絶好調なので何もしない
		break;
	}
}

// 調子を1段階下げる
void Condition::RankDown(){
	switch(conditionType_){
		case ConditionType::Excellent: conditionType_ = ConditionType::Good;     break;
		case ConditionType::Good:      conditionType_ = ConditionType::Normal;   break;
		case ConditionType::Normal:    conditionType_ = ConditionType::Bad;      break;
		case ConditionType::Bad:       conditionType_ = ConditionType::Terrible; break;
		case ConditionType::Terrible:
		// すでに絶不調なので何もしない
		break;
	}
}

float Condition::GetSpeedMultiplier() const{
	switch(conditionType_){
		case ConditionType::Excellent:
		return 1.3f; // 絶好調なら1.5倍速い！
		case ConditionType::Good:
		return 1.1f;
		case ConditionType::Normal:
		return 1.0f; // 普通なら1.0倍（そのままの速度）
		case ConditionType::Bad:
		return 0.9f;
		case ConditionType::Terrible:
		return 0.8f; // 絶不調なら0.8倍に遅くなる
		default:
		return 1.0f; // 念のためデフォルトは1.0倍
	}
}

float Condition::GetJumpMultiplier() const{
	switch(conditionType_){
		case ConditionType::Excellent:
		return 1.3f; // 絶好調なら1.8倍高く飛べる！
		case ConditionType::Good:
		return 1.1f;
		case ConditionType::Normal:
		return 1.0f; // 普通はそのまま
		case ConditionType::Bad:
		return 0.8f;
		case ConditionType::Terrible:
		return 0.6f; // 絶不調ならジャンプ力が落ちる
		default:
		return 1.0f;
	}
}

float Condition::GetScrollSpeedMultiplier() const{
	switch(conditionType_){

	case ConditionType::Excellent:
		return 1.6f; 
	case ConditionType::Good:
		return 1.2f;
	case ConditionType::Normal:
		return 1.0f; 
	case ConditionType::Bad:
		return 0.8f;
	case ConditionType::Terrible:
		return 0.6f; 
	default:
		return 1.0f;
	}
}