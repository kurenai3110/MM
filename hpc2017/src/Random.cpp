//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Random.hpp"
#include "Assert.hpp"

namespace hpc {

//------------------------------------------------------------------------------
Random::Random(const RandomSeed& aRandomSeed)
: mRandomSeed(aRandomSeed)
{
}

//------------------------------------------------------------------------------
int Random::randTerm(int aTerm)
{
    HPC_LB_ASSERT_I(aTerm, 0);
    return int(randU32() & 0x7FFFFFFF) % aTerm;
}

//------------------------------------------------------------------------------
int Random::randMinTerm(int aMin, int aTerm)
{
    HPC_MAX_ASSERT_I(aMin, aTerm - 1);
    return aMin + randTerm(aTerm - aMin);
}

//------------------------------------------------------------------------------
int Random::randMinMax(int aMin, int aMax)
{
    HPC_MAX_ASSERT_I(aMin, aMax);
    return aMin + randTerm(1 + aMax - aMin);
}

//------------------------------------------------------------------------------
uint Random::randU32()
{
    const uint t = (mRandomSeed.x ^ (mRandomSeed.x << 11));
    mRandomSeed.x = mRandomSeed.y;
    mRandomSeed.y = mRandomSeed.z;
    mRandomSeed.z = mRandomSeed.w;
    mRandomSeed.w =
        (mRandomSeed.w ^ (mRandomSeed.w >> 19)) ^ (t ^ (t >> 8));

    return mRandomSeed.w;
}

//------------------------------------------------------------------------------
float Random::randFloatTerm(float aTerm)
{
    HPC_LB_ASSERT_F(aTerm, 0.0f);
    return randFloat() * aTerm;
}

//------------------------------------------------------------------------------
float Random::randFloat()
{
    return float(randU32() % 0x10000) / float(0x10000);
}

} // namespace
// EOF
