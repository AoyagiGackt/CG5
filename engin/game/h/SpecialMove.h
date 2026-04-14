#pragma once

// Playerクラスの実装を知る必要はないが、ポインタとして使うために前方宣言
class Player;

class SpecialMove{
public:
    // 仮想デストラクタ：これがないとメモリリークの原因になります
    virtual ~SpecialMove() = default;

    // --- 純粋仮想関数（= 0） ---
    // これを書くことで、このクラスを直接インスタンス化できなくなり、
    // 子クラスでの実装を強制させます。

    virtual void Initialize(Player* player) = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Activate() = 0;

    // --- 共通で使える便利な関数 ---
    // これらは子クラスで書き直す必要がないので、ここで実装してしまいます。

    bool IsActive() const{ return isActive_; }

protected:
    Player* player_ = nullptr; // 必殺技の持ち主
    bool isActive_ = false;    // 技が実行中（演出中）かどうか
};