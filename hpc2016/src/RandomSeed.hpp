//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Types.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// 乱数のシード。
class RandomSeed {
public:
    static const RandomSeed DefaultSeed(); ///< デフォルトのシードを取得します。
    RandomSeed(uint aX, uint aY, uint aZ, uint aW);
    uint x;         ///< シード値その1。
    uint y;         ///< シード値その2。
    uint z;         ///< シード値その3。
    uint w;         ///< シード値その4。
};

} // namespace
// EOF
