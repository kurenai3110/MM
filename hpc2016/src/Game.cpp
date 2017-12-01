//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Game.hpp"

#include "Assert.hpp"
#include <iostream>
using namespace std;
namespace hpc {

//------------------------------------------------------------------------------
/// Game クラスのインスタンスを生成します。
Game::Game(RandomSeed aSeed)
: mRandom(aSeed)
, mRecorder()
, mTimer()
{
}

//------------------------------------------------------------------------------
/// ゲームを実行します。
///
/// @param[in] aAnswer ゲームの解答。
void Game::run(Answer& aAnswer)
{
    mTimer.start();

    for(int i = 0; i < Parameter::GameStageCount; ++i) {
        uint w = mRandom.randU32();
        uint z = mRandom.randU32();
        uint y = mRandom.randU32();
        uint x = mRandom.randU32();
        RandomSeed seed(x, y, z, w);

        int asteroidCount = GetAsteroidCount(i);

        Stage stage(asteroidCount, seed);
        stage.init();
        aAnswer.init(stage);
        mRecorder.afterInitStage(stage);
        while(!stage.hasFinished() && stage.turn() < Parameter::GameTurnLimit) {
            Action action = aAnswer.getNextAction(stage);
            stage.advanceTurn(action);
            mRecorder.afterAdvanceTurn(stage);
        }
        mRecorder.afterFinishStage();
		//cout << i + 1 << "：";
        aAnswer.finalize(stage);
    }
    mRecorder.afterFinishAllStages();

    mTimer.stop();
}

//------------------------------------------------------------------------------
/// ログ記録器を取得します。
const Recorder& Game::recorder()const
{
    return mRecorder;
}

//------------------------------------------------------------------------------
/// タイマーを取得します。
const Timer& Game::timer()const
{
    return mTimer;
}

//------------------------------------------------------------------------------
/// 小惑星の数を取得します。
///
/// @note ステージレベルが上がるごとに小惑星の数が増えていきます。
///       最初のステージは AsteroidCountMin 個
///       最後のステージは AsteroidCountMax 個
///       になるように調整しています。
///       （AsteroidCountMax - AsteroidCountMin < GameStageCount を満たしている必要があります。）
///
/// @param[in] aNumber ステージの番号（0から始まります）。
///
/// @return そのステージの小惑星の数。
int Game::GetAsteroidCount(int aNumber)
{
    HPC_RANGE_ASSERT_MIN_UB_I(aNumber, 0, Parameter::GameStageCount);

    int diff = Parameter::AsteroidCountMax - Parameter::AsteroidCountMin;
    int cnt = Parameter::GameStageCount;
    int min = Parameter::AsteroidCountMin;
    int asteroidCount = (diff + 1) * aNumber / cnt + min;

    HPC_RANGE_ASSERT_MIN_MAX_I(asteroidCount, Parameter::AsteroidCountMin, Parameter::AsteroidCountMax);

    return asteroidCount;
}

} // namespace
// EOF
