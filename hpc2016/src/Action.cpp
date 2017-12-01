//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Action.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Move 用のインスタンスを取得します。
///
/// @note 宇宙船の移動を行います。
///
/// @param[in] aTargetMovePos 宇宙船の移動目標の位置。
///
/// @return Move 用のインスタンス。
const Action Action::Move(const Vector2& aTargetMovePos)
{
    return Action(
        ActionType::ActionType_Move,
        Vector2(),
        aTargetMovePos);
}

//------------------------------------------------------------------------------
/// Shoot 用のインスタンスを取得します。
///
/// @note レーザーの発射を行います。
///
/// @param[in] aTargetShootPos レーザーの発射目標の位置。
///
/// @return Shoot 用のインスタンス。
const Action Action::Shoot(const Vector2& aTargetShootPos)
{
    return Action(
        ActionType::ActionType_Shoot,
        aTargetShootPos,
        Vector2());
}

//------------------------------------------------------------------------------
/// Action クラスのインスタンスを生成します。
Action::Action(ActionType aType, const Vector2& aTargetShootPos, const Vector2& aTargetMovePos)
: mType(aType)
, mTargetShootPos(aTargetShootPos)
, mTargetMovePos(aTargetMovePos)
{
}

//------------------------------------------------------------------------------
/// 行動の種類を取得します。
ActionType Action::type()const
{
    return mType;
}

//------------------------------------------------------------------------------
/// レーザーの発射目標の位置を取得します。
Vector2 Action::targetShootPos()const
{
    return mTargetShootPos;
}

//------------------------------------------------------------------------------
/// 宇宙船の移動目標の位置を取得します。
Vector2 Action::targetMovePos()const
{
    return mTargetMovePos;
}

} // namespace
// EOF
