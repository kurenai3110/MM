//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Random.hpp"
#include "Assert.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Random クラスのインスタンスを生成します。
Random::Random(const RandomSeed& aRandomSeed)
: mRandomSeed(aRandomSeed)
{
}

//------------------------------------------------------------------------------
/// [0, aTerm) の範囲で乱数を取得します。
///
/// @param[in] aTerm 乱数を発生させる範囲の上界。
///
/// @return [0, aTerm) の範囲で発生させた乱数。
int Random::randTerm(int aTerm)
{
    HPC_LB_ASSERT_I(aTerm, 0);
    return int(randU32() & 0x7FFFFFFF) % aTerm;
}

//------------------------------------------------------------------------------
/// [aMin, aTerm) の範囲で乱数を取得します。
///
/// @param[in] aMin  乱数を発生させる範囲の最小値。
/// @param[in] aTerm 乱数を発生させる範囲の上界。
///
/// @return [aMin, aTerm) の範囲で発生させた乱数。
int Random::randMinTerm(int aMin, int aTerm)
{
    HPC_MAX_ASSERT_I(aMin, aTerm - 1);
    return aMin + randTerm(aTerm - aMin);
}

//------------------------------------------------------------------------------
/// [aMin, aMax] の範囲で乱数を取得します。
///
/// @param[in] aMin  乱数を発生させる範囲の最小値。
/// @param[in] aMax  乱数を発生させる範囲の最大値。
///
/// @return [aMin, aTerm] の範囲で発生させた乱数。
int Random::randMinMax(int aMin, int aMax)
{
    HPC_MAX_ASSERT_I(aMin, aMax);
    return aMin + randTerm(1 + aMax - aMin);
}

//------------------------------------------------------------------------------
/// uint の範囲で乱数を発生させます。
///
/// @return uint の範囲で発生させた乱数。
uint Random::randU32()
{
    const uint t = (mRandomSeed.x ^ (mRandomSeed.x << 11));
    mRandomSeed.x = mRandomSeed.y;
    mRandomSeed.y = mRandomSeed.z;
    mRandomSeed.z = mRandomSeed.w;
    mRandomSeed.w =
        (mRandomSeed.w ^ (mRandomSeed.w >> 19)) ^ (t ^ (t>> 8));

    return mRandomSeed.w;
}

} // namespace
// EOF
