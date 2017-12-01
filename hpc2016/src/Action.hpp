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
#include "ActionType.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// 1ターン毎の宇宙船の行動。
class Action
{
public:
    /// Move 用のインスタンスを取得します。
    static const Action Move(const Vector2& aTargetMovePos);
    /// Shoot 用のインスタンスを取得します。
    static const Action Shoot(const Vector2& aTargetShootPos);

    Action(ActionType aType, const Vector2& aTargetShootPos, const Vector2& aTargetMovePos);

    ActionType type()const;         ///< 行動の種類を取得します。
    Vector2 targetShootPos()const;  ///< レーザーの発射目標の位置を取得します。
    Vector2 targetMovePos()const;   ///< 宇宙船の移動目標の位置を取得します。
private:
    ActionType mType;               ///< 行動の種類
    Vector2 mTargetShootPos;        ///< レーザーの発射目標の位置
    Vector2 mTargetMovePos;         ///< 宇宙船の移動目標の位置
};

} // namespace
// EOF
