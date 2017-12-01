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
/// 小惑星。
class Asteroid {
public:
    Asteroid();
    void init(const Vector2& aPos, float aRadius); ///< 初期値を設定します。
    void getDestroyed();                           ///< 生存フラグをオフにします。
    Vector2 pos()const;                            ///< 位置を取得します。
    float radius()const;                           ///< 半径を取得します。
    bool exists()const;                            ///< 生存フラグを取得します。
private:
    Vector2 mPos;                                  ///< 位置
    float mRadius;                                 ///< 半径
    bool mExists;                                  ///< 生存フラグ
};

} // namespace
// EOF
