//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Game.hpp"

#include "Assert.hpp"

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
/// シード値を変更します。
///
/// @param[in] aSeed 新しいシード値。
void Game::changeSeed(RandomSeed aSeed)
{
    mRandom = Random(aSeed);
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
		//if (i < Parameter::GameStageCount-1)continue;
        Stage stage(seed);
        stage.init();
        aAnswer.init(stage);
        mRecorder.afterInitStage(stage);
        while(!stage.hasFinished() && stage.turn() < Parameter::GameTurnLimit) {
            Actions actions;
            aAnswer.moveItems(stage, actions);
            stage.moveItems(actions);

            TargetPositions targetPositions;
            aAnswer.moveUFOs(stage, targetPositions);
            stage.moveUFOs(targetPositions);

            stage.advanceTurn();

            mRecorder.afterAdvanceTurn(stage);
        }
        mRecorder.afterFinishStage();
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

} // namespace
// EOF
