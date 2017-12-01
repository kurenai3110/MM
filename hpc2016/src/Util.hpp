//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Vector2.hpp"

#pragma once

namespace hpc {

//------------------------------------------------------------------------------
/// 便利関数群。
///
/// このクラスは、 Answer.hpp からインクルードされているため、
/// 参加者が利用することができます。
class Util {
public:
    /// レーザーが小惑星に当たっているかを判定します。
    static bool CanShootAsteroid(Vector2 aLaserStartPos, Vector2 aLaserTargetPos, Vector2 aAsteroidPos, float aAsteroidRadius);
};

} // namespace
// EOF
