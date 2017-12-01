//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
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
/// シード値を変更します。
///
/// @pre run() を実行する前に設定する必要があります。
void Simulator::changeSeed(RandomSeed aSeed)
{
    mGame.changeSeed(aSeed);
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
void Simulator::printResult(bool aIsSilent)const
{
    mGame.recorder().dumpResult(aIsSilent);
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
    return mGame.recorder().totalTurn();
}

//------------------------------------------------------------------------------
/// 実行時間を秒に変換したものを取得します。
///
/// @pre 事前に run() を実行している必要があります。
double Simulator::elapsedSec()const
{
    return mGame.timer().elapsedSec();
}

} // namespace
// EOF
