//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Util.hpp"
#include "Assert.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// レーザーが小惑星に当たっているかを判定します。
///
/// @param[in] aLaserStartPos   レーザーの発射位置。
/// @param[in] aLaserTargetPos  レーザーの発射目標の位置。
/// @param[in] aAsteroidPos     小惑星の位置。
/// @param[in] aAsteroidRaidius 小惑星の半径。
///
/// @return レーザーが小惑星に当たっている場合は @c true を返し
///         そうでない場合は @c false を返します。
bool Util::CanShootAsteroid(Vector2 aLaserStartPos, Vector2 aLaserTargetPos, Vector2 aAsteroidPos, float aAsteroidRadius)
{
    Vector2 dir = aLaserTargetPos - aLaserStartPos;
    HPC_ASSERT(!dir.isZeroStrict());

    Vector2 d = dir.unit();
    Vector2 n = Vector2(-d.y, d.x);
    Vector2 e = aAsteroidPos - aLaserStartPos;
    float r = aAsteroidRadius;

    if(e.dot(d) > 0
        && -r < e.dot(n) && e.dot(n) < r) {
        return true;
    }

    return false;
}

} // namespace
// EOF
