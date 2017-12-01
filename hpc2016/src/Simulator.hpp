//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Game.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// ゲームのシミュレータ。
class Simulator
{
public:
    Simulator();
    void run();                   ///< ゲームを実行します。
    void printResult()const;      ///< 結果を出力します。
    void printJson()const;        ///< Jsonを出力します。
    int totalTurn()const;         ///< 総ターン数を取得します。
    double elapsedSec()const;     ///< 実行時間を秒に変換したものを取得します。
private:
    Game mGame;                   ///< ゲーム全体
};

} // namespace
// EOF
