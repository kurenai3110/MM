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
/// 農場。
class Office {
public:
    /// @name 内部用関数。
    //@{
    Office();
    Office(const Vector2& aPos);
    //@}

    /// @name ユーザーが使用する関数。
    //@{
    Vector2 pos()const;  ///< 座標を取得します。
    float radius()const; ///< 半径を取得します。
    //@}
private:
    Vector2 mPos;
};

} // namespace
// EOF
