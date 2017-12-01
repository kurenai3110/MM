//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include <bitset>
#include "Stage.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// ログ記録器。
class Recorder
{
public:
    Recorder();
    void dumpJson()const;                        ///< 結果をJson形式で出力します。
    void dumpResult(bool aIsSilent)const;        ///< 結果を出力します。

    void afterInitStage(const Stage& aStage);    ///< ステージの初期化後に実行される関数。
    void afterAdvanceTurn(const Stage& aStage);  ///< ターンを進めた後に実行される関数。
    void afterFinishStage();                     ///< ステージが終了した後に実行される関数。
    void afterFinishAllStages();                 ///< 全ステージが終了した後に実行される関数。

    int totalTurn()const;                        ///< 総ターン数を取得します。
private:
    struct UFORecord
    {
        Vector2 pos;
        int type;
        int itemCount;
    };
    struct TurnRecord
    {
        UFORecord ufos[Parameter::UFOCount];
        std::bitset<Parameter::MaxHouseCount> delivered;
    };
    struct StageRecord
    {
        int turn;
        Vector2 officePos;
        Vector2 housePos[Parameter::MaxHouseCount];
        int houseCount;
        TurnRecord turnRecords[Parameter::GameTurnLimit + 1];
    };
    struct GameRecord
    {
        int totalTurn;
        StageRecord stageRecords[Parameter::GameStageCount];
    };

    void dumpJsonStage(const StageRecord& aRecord)const;                    ///< ステージのログをJson形式で出力します。
    void dumpJsonTurn(const TurnRecord& aRecord)const;                      ///< ターンのログをJson形式で出力します。
    void writeTurnRecord(int aStageNumber, int aTurn, const Stage& aStage); ///< TurnRecord に値を設定します。

    GameRecord mGameRecord;   ///< 記録用の構造体

    int mCurrentStageNumber;  ///< 現在記録しているステージの番号
    int mCurrentTurn;         ///< 現在記録しているターン数
};

} // namespace
// EOF
