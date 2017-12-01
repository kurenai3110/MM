//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Vector2.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// 家。
class House {
public:
    /// @name 内部用関数。
    //@{
    House();
    House(const Vector2& aPos);
    void deliver();
    //@}

    /// @name ユーザーが使用する関数。
    //@{
    Vector2 pos()const;    ///< 座標を取得します。
    float radius()const;   ///< 半径を取得します。
    bool delivered()const; ///< 箱が配達済みかを取得します。
    //@}
private:
    Vector2 mPos;
    bool mDelivered;
};

} // namespace
// EOF
