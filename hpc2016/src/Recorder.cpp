//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Recorder.hpp"
#include "ArrayNum.hpp"
#include "Print.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Recorder クラスのインスタンスを生成します。
Recorder::Recorder()
: mGameRecord()
, mCurrentStageNumber(0)
, mCurrentTurn(0)
{
}

//------------------------------------------------------------------------------
/// 結果をJson形式で出力します。
///
/// @note Json の形式は以下の通りです。
/// @code
///   Json           :: [合計ターン数, [ステージ0のログ, ステージ1のログ, ...]]
///   ステージのログ :: [
///                       ターン数,
///                       [小惑星0の情報, 小惑星1の情報, ...],
///                       [ターン0のログ, ターン1のログ, ...],
///                       [レーザー0の情報, レーザー1の情報, ...]
///                     ]
///   小惑星の情報     :: [中心のx座標, 中心のy座標, 半径]
///   ターンのログ   :: [宇宙船の情報, 破壊されていない小惑星のインデックスのリスト]
///   宇宙船の情報   :: [x座標, y座標, リロード時間]
///   破壊されていない小惑星のインデックスのリスト :: 例) [0, 1, 5, ...]
///   レーザーの情報 :: [発射ターン, 発射元x座標, 発射元y座標, 発射先x座標, 発射先y座標]
/// @endcode
///
/// @pre すべてのステージの記録が完了している必要があります。
void Recorder::dumpJson()const
{
    // Json の先頭
    HPC_PRINTF("["); {
        /// 合計ターン数
        HPC_PRINTF("%d,", mGameRecord.totalTurn);
        // 定数情報
        HPC_PRINTF("["); {
            HPC_PRINTF("%d,", Parameter::GameTurnLimit);
            HPC_PRINTF("%d,", Parameter::StageWidth);
            HPC_PRINTF("%d,", Parameter::StageHeight);
            HPC_PRINTF("%d", Parameter::ShipReloadTurn);
        } HPC_PRINTF("],");
        // ステージのログ
        HPC_PRINTF("["); {
            for(int i = 0; i < Parameter::GameStageCount; ++i) {
                if(i != 0) {
                    HPC_PRINTF(",");
                }
                dumpJsonStage(mGameRecord.stageRecords[i]);
            }
        } HPC_PRINTF("]");
    } HPC_PRINTF("]");

    HPC_PRINTF("\n");
}

//------------------------------------------------------------------------------
/// 結果を出力します。
///
/// @pre すべてのステージの記録が完了している必要があります。
void Recorder::dumpResult()const
{
    HPC_PRINTF("stage | turn\n");
    for(int i = 0; i < Parameter::GameStageCount; ++i) {
        HPC_PRINTF("% 5d | % 4d\n", i, mGameRecord.stageRecords[i].turn);
    }
    HPC_PRINTF("TotalTurn: %d\n", mGameRecord.totalTurn);
}

//------------------------------------------------------------------------------
/// ステージの初期化後に実行される関数。
///
/// @note AsteroidRecord 等に値を設定します。
void Recorder::afterInitStage(const Stage& aStage)
{
    StageRecord& stageRecord = mGameRecord.stageRecords[mCurrentStageNumber];

    mCurrentTurn = 0;

    for(int i = 0; i < aStage.asteroidCount(); ++i) {
        stageRecord.asteroidRecords[i].pos = aStage.asteroid(i).pos();
        stageRecord.asteroidRecords[i].radius = aStage.asteroid(i).radius();
    }
    stageRecord.asteroidCount = aStage.asteroidCount();

    writeTurnRecord(mCurrentStageNumber, 0, aStage);
}

//------------------------------------------------------------------------------
/// ターンを進めた後に実行される関数。
///
/// @note TurnRecord 等に値を設定します。
void Recorder::afterAdvanceTurn(const Stage& aStage)
{
    ++mCurrentTurn;

    writeTurnRecord(mCurrentStageNumber, mCurrentTurn, aStage);
}

//------------------------------------------------------------------------------
/// ステージが終了した後に実行される関数。
///
/// @note turn 等に値を設定します。
void Recorder::afterFinishStage()
{
    StageRecord& stageRecord = mGameRecord.stageRecords[mCurrentStageNumber];

    stageRecord.turn = mCurrentTurn;

    mGameRecord.totalTurn += mCurrentTurn;

    ++mCurrentStageNumber;
}

//------------------------------------------------------------------------------
/// 全ステージが終了した後に実行される関数。
void Recorder::afterFinishAllStages()
{
}

//------------------------------------------------------------------------------
/// ステージのログをJson形式で出力します。
void Recorder::dumpJsonStage(const StageRecord& aRecord)const
{
    HPC_PRINTF("["); {
        // ターン数
        HPC_PRINTF("%d,", aRecord.turn);
        // 小惑星の情報
        HPC_PRINTF("["); {
            for(int i = 0; i < aRecord.asteroidCount; ++i) {
                if(i != 0) {
                    HPC_PRINTF(",");
                }
                dumpJsonAsteroid(aRecord.asteroidRecords[i]);
            }
        } HPC_PRINTF("],");
        // ターンのログ
        HPC_PRINTF("["); {
            for(int i = 0; i <= aRecord.turn; ++i) {
                if(i != 0) {
                    HPC_PRINTF(",");
                }
                dumpJsonTurn(aRecord.turnRecords[i]);
            }
        } HPC_PRINTF("],");
        // レーザーの情報
        HPC_PRINTF("["); {
            bool isFirst = true;
            for(int i = 0; i <= aRecord.turn; ++i) {
                if(aRecord.turnRecords[i].hasShot) {
                    if(!isFirst) {
                        HPC_PRINTF(",");
                    }
                    dumpJsonLaser(i, aRecord.turnRecords[i]);
                    isFirst = false;
                }
            }
        } HPC_PRINTF("]");
    } HPC_PRINTF("]");
}

//------------------------------------------------------------------------------
/// 小惑星の情報をJson形式で出力します。
void Recorder::dumpJsonAsteroid(const AsteroidRecord& aRecord)const
{
    HPC_PRINTF("["); {
        HPC_PRINTF("%7.3f,", aRecord.pos.x);
        HPC_PRINTF("%7.3f,", aRecord.pos.y);
        HPC_PRINTF("%7.3f", aRecord.radius);
    } HPC_PRINTF("]");
}

//------------------------------------------------------------------------------
/// ターンのログをJson形式で出力します。
void Recorder::dumpJsonTurn(const TurnRecord& aRecord)const
{
    HPC_PRINTF("["); {
        // 宇宙船の情報
        HPC_PRINTF("["); {
            HPC_PRINTF("%7.3f,", aRecord.shipPos.x);
            HPC_PRINTF("%7.3f,", aRecord.shipPos.y);
            HPC_PRINTF("%d", aRecord.restReloadTurn);
        } HPC_PRINTF("],");
        // 破壊されていない小惑星のインデックスのリスト :: 例) [0, 1, 5, ...]
        HPC_PRINTF("["); {
            bool isFirst = true;
            for(int i = 0; i < HPC_ARRAY_NUM(aRecord.isExistingAsteroid); ++i) {
                if(aRecord.isExistingAsteroid[i]) {
                    if(!isFirst) {
                        HPC_PRINTF(",");
                    }
                    HPC_PRINTF("%d", i);
                    isFirst = false;
                }
            }
        } HPC_PRINTF("]");
    } HPC_PRINTF("]");
}

//------------------------------------------------------------------------------
/// レーザーのログをJson形式で出力します。
void Recorder::dumpJsonLaser(int aTurn, const TurnRecord& aRecord)const
{
    HPC_PRINTF("["); {
        // 発射ターン
        HPC_PRINTF("%d,", aTurn);
        // 発射元座標
        HPC_PRINTF("%7.3f,", aRecord.startShootPos.x);
        HPC_PRINTF("%7.3f,", aRecord.startShootPos.y);
        // 発射先座標
        HPC_PRINTF("%7.3f,", aRecord.targetShootPos.x);
        HPC_PRINTF("%7.3f", aRecord.targetShootPos.y);
    } HPC_PRINTF("]");
}

//------------------------------------------------------------------------------
/// TurnRecord に値を設定します。
void Recorder::writeTurnRecord(int aStageNumber, int aTurn, const Stage& aStage)
{
    TurnRecord& turnRecord = mGameRecord.stageRecords[aStageNumber].turnRecords[aTurn];

    // shipPos, restReloadTurn を設定します。
    turnRecord.shipPos = aStage.ship().pos();
    turnRecord.restReloadTurn = aStage.ship().restReloadTurn();

    // hasShot, startSootPos, targetShootPos を設定します。
    if(aStage.laser().isValid()) {
        turnRecord.hasShot = true;
        turnRecord.startShootPos = aStage.laser().startPos();
        turnRecord.targetShootPos = aStage.laser().targetPos();
    } else {
        turnRecord.hasShot = false;
    }

    // isExistingAsteroid を設定します。
    for(int i = 0; i < HPC_ARRAY_NUM(turnRecord.isExistingAsteroid); ++i) {
        if(i < aStage.asteroidCount()) {
            turnRecord.isExistingAsteroid[i] = aStage.asteroid(i).exists();
        } else {
            turnRecord.isExistingAsteroid[i] = false;
        }
    }
}

//------------------------------------------------------------------------------
/// 総ターン数を取得します。
///
/// @pre すべてのステージの記録が完了している必要があります。
int Recorder::totalTurn()const
{
    return mGameRecord.totalTurn;
}

} // namespace
// EOF
