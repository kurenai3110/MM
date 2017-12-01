//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Stage.hpp"
#include "ArrayNum.hpp"
#include "Assert.hpp"
#include "Util.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Stage クラスのインスタンスを生成します。
///
/// @note 小惑星数・シード値が同じならば、ステージも同じものが生成されます。
Stage::Stage(int aAsteroidCount, RandomSeed aSeed)
: mTurn(0)
, mAsteroids()
, mAsteroidCount(aAsteroidCount)
, mShip()
, mLaser()
, mRandom(aSeed)
{
    HPC_RANGE_ASSERT_MIN_MAX_I(aAsteroidCount, 0, HPC_ARRAY_NUM(mAsteroids));
}

//------------------------------------------------------------------------------
/// ステージを初期化します。
void Stage::init()
{
    mTurn = 0;

    // 小惑星の初期設定をします。
    {
        for(int i = 0; i < mAsteroidCount; ++i) {
            while(true) {
                int x = mRandom.randMinMax(
                    0 + Parameter::StageMargin,
                    Parameter::StageWidth - Parameter::StageMargin);
                int y =  mRandom.randMinMax(
                    0 + Parameter::StageMargin,
                    Parameter::StageHeight - Parameter::StageMargin);
                Vector2 pos = Vector2(float(x), float(y));
                float radius = float(mRandom.randMinMax(Parameter::AsteroidRadiusMin, Parameter::AsteroidRadiusMax));

                // 他の小惑星と被ってたら、やり直します。
                bool isValid = true;
                for(int j = 0; j < i; ++j) {
                    Vector2 pos2 = mAsteroids[j].pos();
                    float radius2 = mAsteroids[j].radius();
                    if(pos.dist(pos2) < radius + radius2 + Parameter::AsteroidMargin) {
                        isValid = false;
                        break;
                    }
                }

                if(isValid) {
                    mAsteroids[i].init(pos, radius);
                    break;
                }
            }
        }
    }

    // 宇宙船の初期設定をします。
    while(true) {
        int x = mRandom.randMinMax(
            0 + Parameter::StageMargin,
            Parameter::StageWidth - Parameter::StageMargin);
        int y =  mRandom.randMinMax(
            0 + Parameter::StageMargin,
            Parameter::StageHeight - Parameter::StageMargin);
        Vector2 shipPos = Vector2(float(x), float(y));

        // 小惑星と被ってたら、やり直します。
        bool isValid = true;
        for(int i = 0; i < mAsteroidCount; ++i) {
            Vector2 pos = mAsteroids[i].pos();
            float radius = mAsteroids[i].radius();
            if(shipPos.dist(pos) < radius + Parameter::ShipAsteroidMargin) {
                isValid = false;
                break;
            }
        }

        if(isValid) {
            mShip.init(shipPos);
            break;
        }
    }
}

//------------------------------------------------------------------------------
/// 1ターン進めます。
void Stage::advanceTurn(const Action& aAction)
{
    HPC_ENUM_ASSERT(ActionType, aAction.type());

    // レーザーを発射します。
    mLaser.vanish();
    if(aAction.type() == ActionType::ActionType_Shoot) {
        Vector2 dir = aAction.targetShootPos() - mShip.pos();
        if(mShip.canShoot() && !dir.isZeroStrict()) {
            mLaser.appear(mShip.pos(), aAction.targetShootPos());
            mShip.tryToShoot();
        }
    }

    // 宇宙船を動かします。
    if(aAction.type() == ActionType::ActionType_Move) {
        mShip.move(aAction.targetMovePos());
    }

    // レーザーが小惑星に当たっていた場合は、小惑星を破壊します。
    if(mLaser.isValid()) {
        for(int i = 0; i < mAsteroidCount; ++i) {
            if(mAsteroids[i].exists()) {
                if(Util::CanShootAsteroid(
                        mLaser.startPos(), mLaser.targetPos(),
                        mAsteroids[i].pos(), mAsteroids[i].radius())) {
                    mAsteroids[i].getDestroyed();
                }
            }
        }
    }

    // 宇宙船の移動先に小惑星があったら、小惑星を破壊します。
    for(int i = 0; i < mAsteroidCount; ++i) {
        Vector2 e = mAsteroids[i].pos() - mShip.pos();
        float r = mAsteroids[i].radius();
        if(mAsteroids[i].exists()) {
            if(e.length() < r) {
                mAsteroids[i].getDestroyed();
            }
        }
    }

    ++mTurn;

    return;
}

//------------------------------------------------------------------------------
/// ステージをクリアしているならば true を返します。
bool Stage::hasFinished()const
{
    return existingAsteroidCount() == 0;
}

//------------------------------------------------------------------------------
/// 現在のターン数を取得します。
int Stage::turn()const
{
    return mTurn;
}

//------------------------------------------------------------------------------
/// 小惑星の数を取得します。
int Stage::asteroidCount()const
{
    return mAsteroidCount;
}

//------------------------------------------------------------------------------
/// まだ破壊されていない小惑星の数を取得します。
int Stage::existingAsteroidCount()const
{
    int count = mAsteroidCount;
    for(int i = 0; i < mAsteroidCount; ++i) {
        if(!mAsteroids[i].exists()) {
            --count;
        }
    }

    return count;
}

//------------------------------------------------------------------------------
/// 小惑星の情報を取得します。
const Asteroid& Stage::asteroid(int aIndex)const
{
    HPC_RANGE_ASSERT_MIN_UB_I(aIndex, 0, mAsteroidCount);

    return mAsteroids[aIndex];
}

//------------------------------------------------------------------------------
/// 宇宙船の情報を取得します。
const Ship& Stage::ship()const
{
    return mShip;
}

//------------------------------------------------------------------------------
/// レーザーの情報を取得します。
const Laser& Stage::laser()const
{
    return mLaser;
}

} // namespace
// EOF
