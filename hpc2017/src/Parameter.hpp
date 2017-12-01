//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

namespace hpc {

//------------------------------------------------------------------------------
/// パラメータ。
class Parameter {
public:
    static const int GameStageCount = 200;   ///< ステージ数。
    static const int GameTurnLimit = 2000;   ///< 1ステージの最長ゲーム時間。最初が0ターン目、それから1999ターン目まで runTurn() します。

    static const int MaxActionPerTurn = 100; ///< 1フェーズで行動できる回数。実質無制限を意図しています。

    static const int StageWidth = 750;       ///< ステージの横幅。
    static const int StageHeight = 750;      ///< ステージの縦幅。

    static const int MaxHouseCount = 100;    ///< 家の最大数。

    static const int LargeUFOCount = 2;      ///< 大型タイプのUFOの数。
    static const int SmallUFOCount = 8;      ///< 小型タイプのUFOの数。
    static const int UFOCount = LargeUFOCount + SmallUFOCount; ///< UFOの合計数。

    static const int OfficeRadius = 20;      ///< 農場の半径。
    static const int HouseRadius = 5;        ///< 家の半径。
    static const int LargeUFORadius = 10;    ///< 大型タイプのUFOの半径。
    static const int SmallUFORadius = 2;     ///< 小型タイプのUFOの半径。

    static const int LargeUFOCapacity = 40;  ///< 大型タイプのUFOが持てる箱の最大数。
    static const int SmallUFOCapacity = 5;   ///< 小型タイプのUFOが持てる箱の最大数。

    static const int LargeUFOMaxSpeed = 5;   ///< 大型タイプのUFOの最大速度。
    static const int SmallUFOMaxSpeed = 12;  ///< 小型タイプのUFOの最大速度。
};

} // namespace
// EOF
