//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Action.hpp"
#include "Asteroid.hpp"
#include "Laser.hpp"
#include "Parameter.hpp"
#include "Random.hpp"
#include "Ship.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// ステージ。
class Stage {
public:
    Stage(int aAsteroidCount, RandomSeed aSeed);
    void init();                                        ///< ステージを初期化します。
    void advanceTurn(const Action& aRequest);           ///< 1ターン進めます。
    bool hasFinished()const;                            ///< ステージをクリアしているならば true を返します。
    int turn()const;                                    ///< 現在のターン数を取得します。
    int asteroidCount()const;                           ///< 小惑星の数を取得します。
    int existingAsteroidCount()const;                   ///< まだ破壊されていない小惑星の数を取得します。
    const Asteroid& asteroid(int aIndex)const;          ///< 小惑星の情報を取得します。
    const Ship& ship()const;                            ///< 宇宙船の情報を取得します。
    const Laser& laser()const;                          ///< レーザーの情報を取得します。
private:
    int mTurn;                                          ///< 現在のターン数
    Asteroid mAsteroids[Parameter::AsteroidCountMax];   ///< 小惑星の情報
    int mAsteroidCount;                                 ///< 小惑星の数
    Ship mShip;                                         ///< 宇宙船
    Laser mLaser;                                       ///< レーザー
    Random mRandom;                                     ///< 乱数生成器
};

} // namespace
// EOF
