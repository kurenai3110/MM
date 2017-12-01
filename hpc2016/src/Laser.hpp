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
/// レーザー。
class Laser
{
public:
    Laser();
    void appear(const Vector2& aStartPos, const Vector2& aTargetPos); ///< レーザーを発射します。
    void vanish();                                                    ///< レーザーを消します。
    bool isValid()const;                                              ///< レーザーが発射されている場合に true を返します。
    Vector2 startPos()const;                                          ///< レーザーの発射位置を取得します。
    Vector2 targetPos()const;                                         ///< レーザーの発射目標の位置を取得します。
    Vector2 dir()const;                                               ///< レーザーの発射方向を取得します
private:
    bool mIsValid;                                                    ///< レーザーの有効フラグ
    Vector2 mStartPos;                                                ///< レーザーの発射位置
    Vector2 mTargetPos;                                               ///< レーザーの発射目標の位置
};

} // namespace
// EOF
