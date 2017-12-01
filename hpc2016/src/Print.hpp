//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include <cstdio>

/// 文字列を画面に表示します。
///
/// @param[in] ... 書式つき文字列とデータ。 std::printf の記法に準拠します。
#define HPC_PRINTF(...) \
    do { std::printf(__VA_ARGS__); } while(false)

// EOF
