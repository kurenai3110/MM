//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "RandomSeed.hpp"
#include <time.h>
#include <cstdlib>

namespace hpc {

//------------------------------------------------------------------------------
/// デフォルトのシードを取得します。
const RandomSeed RandomSeed::DefaultSeed()
{
	
    const uint DefaultSeedX = 0xf70c0303;
    const uint DefaultSeedY = 0x7b1cf62d;
    const uint DefaultSeedZ = 0x727ff30d;
    const uint DefaultSeedW = 0x433d684d;
	
	/*
	srand((unsigned)time(NULL));
	const uint DefaultSeedX = rand();
	const uint DefaultSeedY = rand();
	const uint DefaultSeedZ = rand();
	const uint DefaultSeedW = rand();
	*/
    return RandomSeed(
        DefaultSeedX,
        DefaultSeedY,
        DefaultSeedZ,
        DefaultSeedW
        );
}

//------------------------------------------------------------------------------
/// RandomSeed クラスのインスタンスを生成します。
RandomSeed::RandomSeed(uint aX, uint aY, uint aZ, uint aW)
: x(aX)
, y(aY)
, z(aZ)
, w(aW)
{
}

} // namespace
// EOF
