//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
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
            HPC_PRINTF("%d,", Parameter::OfficeRadius);
            HPC_PRINTF("%d,", Parameter::HouseRadius);

            HPC_PRINTF("["); {
                HPC_PRINTF("["); {
                    HPC_PRINTF("%d,", Parameter::LargeUFORadius);
                    HPC_PRINTF("%d,", Parameter::LargeUFOCapacity);
                    HPC_PRINTF("%d", Parameter::LargeUFOMaxSpeed);
                } HPC_PRINTF("],");
                HPC_PRINTF("["); {
                    HPC_PRINTF("%d,", Parameter::SmallUFORadius);
                    HPC_PRINTF("%d,", Parameter::SmallUFOCapacity);
                    HPC_PRINTF("%d", Parameter::SmallUFOMaxSpeed);
                } HPC_PRINTF("]");
            } HPC_PRINTF("]");
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
void Recorder::dumpResult(bool aIsSilent)const
{
    if (!aIsSilent) {
        HPC_PRINTF("stage | turn\n");
        for(int i = 0; i < Parameter::GameStageCount; ++i) {
            HPC_PRINTF("% 5d | % 4d\n", i, mGameRecord.stageRecords[i].turn);
        }
    }
    HPC_PRINTF("TotalTurn: %d\n", mGameRecord.totalTurn);
}

//------------------------------------------------------------------------------
/// ステージの初期化後に実行される関数。
///
/// @note StageRecord 等に値を設定します。
void Recorder::afterInitStage(const Stage& aStage)
{
    StageRecord& stageRecord = mGameRecord.stageRecords[mCurrentStageNumber];

    mCurrentTurn = 0;

    stageRecord.officePos = aStage.office().pos();
    for (int i = 0; i < aStage.houses().count(); ++i) {
        stageRecord.housePos[i] = aStage.houses()[i].pos();
    }
    stageRecord.houseCount = aStage.houses().count();

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

        // マップ情報
        HPC_PRINTF("["); {
            HPC_PRINTF("["); {
                HPC_PRINTF("%d,", int(aRecord.officePos.x));
                HPC_PRINTF("%d", int(aRecord.officePos.y));
            } HPC_PRINTF("],");
            HPC_PRINTF("["); {
                for (int i = 0; i < aRecord.houseCount; ++i) {
                    if (i != 0) {
                        HPC_PRINTF(",");
                    }
                    HPC_PRINTF("["); {
                        HPC_PRINTF("%d,", int(aRecord.housePos[i].x));
                        HPC_PRINTF("%d", int(aRecord.housePos[i].y));
                    } HPC_PRINTF("]");
                }
            } HPC_PRINTF("]");
        } HPC_PRINTF("],");

        // ターンのログ
        HPC_PRINTF("["); {
            for(int i = 0; i <= aRecord.turn; ++i) {
                if(i != 0) {
                    HPC_PRINTF(",");
                }
                dumpJsonTurn(aRecord.turnRecords[i]);
            }
        } HPC_PRINTF("]");
    } HPC_PRINTF("]");
}

//------------------------------------------------------------------------------
/// ターンのログをJson形式で出力します。
void Recorder::dumpJsonTurn(const TurnRecord& aRecord)const
{
    HPC_PRINTF("["); {
        // UFO情報
        HPC_PRINTF("["); {
            for (int i = 0; i < HPC_ARRAY_NUM(aRecord.ufos); ++i) {
                if (i != 0) {
                    HPC_PRINTF(",");
                }
                HPC_PRINTF("["); {
                    HPC_PRINTF("["); {
                        HPC_PRINTF("%d,", int(aRecord.ufos[i].pos.x));
                        HPC_PRINTF("%d", int(aRecord.ufos[i].pos.y));
                    } HPC_PRINTF("],");
                    HPC_PRINTF("%d,", aRecord.ufos[i].type);
                    HPC_PRINTF("%d", aRecord.ufos[i].itemCount);
                } HPC_PRINTF("]");
            }
        } HPC_PRINTF("],");

        // 配達情報
        HPC_PRINTF("["); {
            const int Unit = sizeof(uint) * 8;
            auto delivered = aRecord.delivered; // コピー
            for (int i = 0; i < (Parameter::MaxHouseCount + (Unit - 1)) / Unit; ++i) {
                uint elem = 0;
                for (int j = 0; j < Unit; ++j) {
                    elem |= delivered.test(0) << j;
                    delivered >>= 1;
                }

                if (i != 0) {
                    HPC_PRINTF(",");
                }
                HPC_PRINTF("%u", elem);
            }
        } HPC_PRINTF("]");
    } HPC_PRINTF("]");
}

//------------------------------------------------------------------------------
/// TurnRecord に値を設定します。
void Recorder::writeTurnRecord(int aStageNumber, int aTurn, const Stage& aStage)
{
    TurnRecord& turnRecord = mGameRecord.stageRecords[aStageNumber].turnRecords[aTurn];

    for (int i = 0; i < aStage.ufos().count(); ++i) {
        const UFO& ufo = aStage.ufos()[i];
        UFORecord& ufoRecord = turnRecord.ufos[i];
        ufoRecord.type = ufo.type();
        ufoRecord.pos = ufo.pos();
        ufoRecord.itemCount = ufo.itemCount();
    }

    for (int i = 0; i < aStage.houses().count(); ++i) {
        turnRecord.delivered.set(i, aStage.houses()[i].delivered());
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
