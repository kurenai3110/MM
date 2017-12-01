//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include <ctime>

namespace hpc {

//------------------------------------------------------------------------------
/// タイマー。
class Timer
{
public:
    Timer();                    ///< タイマーのインスタンスを生成します。
    void start();               ///< タイマーの計測を開始します。
    void stop();                ///< タイマーの計測を終了します。
    double elapsedSec()const;   ///< start 関数を呼び出してから stop 関数を呼び出すまでの経過時間を取得します。
private:
    std::clock_t mTimeBegin;    ///< 開始時刻
    std::clock_t mTimeEnd;      ///< 終了時刻
};

} // namespace
// EOF
