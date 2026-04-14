/**
 * @file EmojiUI.h
 * @brief プレイヤーの調子に応じた絵文字3DモデルをUI表示するクラス
 *
 * 調子ごとに顔(黄色)と目口(黒)のモデルペアを持ち、
 * 毎フレームカメラ左下に追従して表示する。
 *
 * 新しい調子モデルを追加するには:
 *   1. Resources/emojiUI/ に {Name}_face.obj と {Name}_eyesMouth.obj を置く
 *   2. Initialize() 内の RegisterEmoji() 呼び出しを追加する
 */
#pragma once
#include "Camera.h"
#include "Condition.h"
#include "Model.h"
#include "ModelCommon.h"
#include "Object3d.h"
#include <map>
#include <memory>
#include <string>

class EmojiUI {
public:
    /**
     * @brief 初期化。利用可能な絵文字モデルをすべて読み込む
     * @param modelCommon モデル描画の共通設定
     */
    void Initialize(ModelCommon* modelCommon);

    /**
     * @brief 毎フレーム更新。調子に合わせた絵文字を選択し、カメラ左下に追従させる
     * @param condition 現在のプレイヤーの調子
     * @param camera    ゲームのメインカメラ
     */
    void Update(Condition::ConditionType condition, Camera* camera);

    /** @brief 絵文字モデルを描画する（ModelCommon::CommonDrawSettings() 後に呼ぶ） */
    void Draw();

private:
    struct EmojiEntry {
        std::unique_ptr<Model>    faceModel;
        std::unique_ptr<Model>    eyesMouthModel;
        std::unique_ptr<Object3d> face;
        std::unique_ptr<Object3d> eyesMouth;
    };

    /**
     * @brief 絵文字モデルを登録する（顔＋目口の2ファイル構成）
     * @param modelCommon モデル共通設定
     * @param name        ファイル名プレフィックス（例: "AbsolutelyHappy"）
     * @param condition   紐付ける調子
     */
    void RegisterEmoji(ModelCommon* modelCommon,
                       const std::string& name,
                       Condition::ConditionType condition);

    /**
     * @brief 絵文字モデルを登録する（1ファイル構成）
     * @param modelCommon モデル共通設定
     * @param filename    ファイル名（拡張子なし、例: "NormalEmojiUI"）
     * @param condition   紐付ける調子
     */
    void RegisterEmojiSingle(ModelCommon* modelCommon,
                              const std::string& filename,
                              Condition::ConditionType condition);

    /**
     * @brief 絵文字モデルを登録する（顔ファイル名・目口ファイル名を直接指定）
     * @param modelCommon     モデル共通設定
     * @param faceFilename    顔OBJのファイル名（拡張子なし、emojiUI/直下）
     * @param eyesFilename    目口OBJのファイル名（拡張子なし、emojiUI/直下）
     * @param condition       紐付ける調子
     */
    void RegisterEmojiParts(ModelCommon* modelCommon,
                             const std::string& faceFilename,
                             const std::string& eyesFilename,
                             Condition::ConditionType condition);

    /** @brief 登録済み調子から最も近い絵文字を返す（未登録調子のフォールバック） */
    EmojiEntry* GetEntry(Condition::ConditionType condition);

    std::map<Condition::ConditionType, EmojiEntry> emojis_;

    /** @brief 現フレームで表示するエントリ（Update で設定） */
    EmojiEntry* currentEntry_ = nullptr;

    // カメラからのオフセット（左下・手前）
    // fovY=0.45rad, Z=5 での可視範囲: X=±2.04, Y=±1.15
    static constexpr float kOffsetX  = -1.6f;
    static constexpr float kOffsetY  = -0.7f;
    static constexpr float kOffsetZ  =  5.0f;
    static constexpr float kScale    =  0.3f;
};
