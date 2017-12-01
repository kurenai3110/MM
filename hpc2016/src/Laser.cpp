//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Laser.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Laser クラスのインスタンスを生成します。
Laser::Laser()
: mIsValid(false)
, mStartPos()
, mTargetPos()
{
}

//------------------------------------------------------------------------------
/// レーザーを発射します。
///
/// @param[in] aStartPos レーザーの発射位置。
/// @param[in] aTargetPos レーザーの発射目標の位置。
void Laser::appear(const Vector2& aStartPos, const Vector2& aTargetPos)
{
    mIsValid = true;
    mStartPos = aStartPos;
    mTargetPos = aTargetPos;
}

//------------------------------------------------------------------------------
/// レーザーを消します。
void Laser::vanish()
{
    mIsValid = false;
}

//------------------------------------------------------------------------------
/// レーザーが発射されている場合に true を返します。
bool Laser::isValid()const
{
    return mIsValid;
}

//------------------------------------------------------------------------------
/// レーザーの発射位置を取得します。
Vector2 Laser::startPos()const
{
    return mStartPos;
}

//------------------------------------------------------------------------------
/// レーザーの発射目標の位置を取得します。
Vector2 Laser::targetPos()const
{
    return mTargetPos;
}

//------------------------------------------------------------------------------
/// レーザーの発射方向を取得します。
Vector2 Laser::dir()const
{
    return (targetPos() - startPos()).unit();
}

} // namespace
// EOF
