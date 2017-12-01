//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Stage.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// ログ記録器。
class Recorder
{
public:
    Recorder();
    void dumpJson()const;                        ///< 結果をJson形式で出力します。
    void dumpResult()const;                      ///< 結果を出力します。

    void afterInitStage(const Stage& aStage);    ///< ステージの初期化後に実行される関数。
    void afterAdvanceTurn(const Stage& aStage);  ///< ターンを進めた後に実行される関数。
    void afterFinishStage();                     ///< ステージが終了した後に実行される関数。
    void afterFinishAllStages();                 ///< 全ステージが終了した後に実行される関数。

    int totalTurn()const;                        ///< 総ターン数を取得します。
private:
    struct TurnRecord
    {
        Vector2 shipPos;
        int restReloadTurn;
        Vector2 startShootPos;
        Vector2 targetShootPos;
        bool isExistingAsteroid[Parameter::AsteroidCountMax];
        bool hasShot;
    };
    struct AsteroidRecord
    {
        Vector2 pos;
        float radius;
    };
    struct StageRecord
    {
        int turn;
        AsteroidRecord asteroidRecords[Parameter::AsteroidCountMax];
        int asteroidCount;
        TurnRecord turnRecords[Parameter::GameTurnLimit + 1];
    };
    struct GameRecord
    {
        int totalTurn;
        StageRecord stageRecords[Parameter::GameStageCount];
    };

    void dumpJsonStage(const StageRecord& aRecord)const;                    ///< ステージのログをJson形式で出力します。
    void dumpJsonAsteroid(const AsteroidRecord& aRecord)const;              ///< 小惑星の情報をJson形式で出力します。
    void dumpJsonTurn(const TurnRecord& aRecord)const;                      ///< ターンのログをJson形式で出力します。
    void dumpJsonLaser(int aTurn, const TurnRecord& aRecord)const;          ///< レーザーのログをJson形式で出力します。
    void writeTurnRecord(int aStageNumber, int aTurn, const Stage& aStage); ///< TurnRecord に値を設定します。

    GameRecord mGameRecord;   ///< 記録用の構造体

    int mCurrentStageNumber;  ///< 現在記録しているステージの番号
    int mCurrentTurn;         ///< 現在記録しているターン数
};

} // namespace
// EOF
