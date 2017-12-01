//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "RandomSeed.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// 乱数生成器。
class Random
{
public:
    Random(const RandomSeed& aRandomSeed);
    int randTerm(int aTerm);                ///< [0, aTerm) の範囲で乱数を取得します。
    int randMinTerm(int aMin, int aTerm);   ///< [aMin, aTerm) の範囲で乱数を取得します。
    int randMinMax(int aMin, int aMax);     ///< [aMin, aMax] の範囲で乱数を取得します。
    uint randU32();                         ///< uint の範囲で乱数を発生させます。
private:
    RandomSeed mRandomSeed;                 ///< 乱数のシード
};

} // namespace
// EOF
