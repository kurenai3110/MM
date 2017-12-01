//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Answer.hpp"
#include "Simulator.hpp"
#include "Parameter.hpp"
#include "Print.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Simulator クラスのインスタンスを生成します。
Simulator::Simulator()
: mGame(RandomSeed::DefaultSeed())
{
}

//------------------------------------------------------------------------------
/// ゲームを実行します。
void Simulator::run()
{
    Answer answer;
    mGame.run(answer);
}

//------------------------------------------------------------------------------
/// 結果を出力します。
///
/// @pre 事前に run() を実行している必要があります。
void Simulator::printResult()const
{
    mGame.recorder().dumpResult();
    HPC_PRINTF("Time: %.3f sec\n", mGame.timer().elapsedSec());
}

//------------------------------------------------------------------------------
/// Jsonを出力します。
///
/// @pre 事前に run() を実行している必要があります。
void Simulator::printJson()const
{
    mGame.recorder().dumpJson();
}

//------------------------------------------------------------------------------
/// 総ターン数を取得します。
///
/// @pre 事前に run() を実行している必要があります。
int Simulator::totalTurn()const
{
    return mGame.recorder().totalTurn() / Parameter::RepeatCount;
}

//------------------------------------------------------------------------------
/// 実行時間を秒に変換したものを取得します。
///
/// @pre 事前に run() を実行している必要があります。
double Simulator::elapsedSec()const
{
    return mGame.timer().elapsedSec() / Parameter::RepeatCount;
}

} // namespace
// EOF
