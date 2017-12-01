//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Vector2.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// 宇宙船。
class Ship
{
public:
    Ship();
    void init(const Vector2& aPos);        ///< 初期値を設定します。
    bool tryToShoot();                     ///< リロードが完了しているならばレーザーを発射します。
    void move(const Vector2& aTargetPos);  ///< 移動します。
    Vector2 pos()const;                    ///< 位置を取得します。
    bool canShoot()const;                  ///< リロードが完了しているならば true を返します。
    int restReloadTurn()const;             ///< リロード完了まで時間を取得します。
private:
    Vector2 mPos;                          ///< 位置
    int mRestReloadTurn;                   ///< リロード完了まで時間
};

} // namespace
// EOF
