//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

namespace hpc {

//------------------------------------------------------------------------------
/// パラメータ。
class Parameter {
public:
    static const int RepeatCount = 1;            ///< 繰り返し回数。コンテスト開催中は1ですが、締め切り後の再評価で4に変更されます。

    static const int GameStageCount = 200 * RepeatCount;       ///< ステージ数
    static const int GameTurnLimit = 2000;       ///< 1ステージの最長ゲーム時間。最初が0ターン目、それから1999ターン目まで runTurn() します。

    static const int StageWidth = 500;           ///< ステージの横幅
    static const int StageHeight = 500;          ///< ステージの縦幅
    static const int StageMargin = 20;           ///< 宇宙船の初期位置と小惑星の位置用のマージン

    static const int AsteroidRadiusMin = 10;      ///< 小惑星の半径の最小値
    static const int AsteroidRadiusMax = 20;      ///< 小惑星の半径の最大値
    static const int AsteroidCountMin = 20;      ///< 小惑星の数の最小値
    static const int AsteroidCountMax = 30;      ///< 小惑星の数の最大値
    static const int AsteroidMargin = 5;         ///< 小惑星間のすきまの最小値

    static const int ShipAsteroidMargin = 10;    ///< 宇宙船と小惑星の間のすきまの最小値
    static float ShipMaxSpeed();                 ///< 宇宙船の速度上限
    static const int ShipReloadTurn = 40;        ///< レーザーのリロードまでにかかる時間
};

} // namespace
// EOF
