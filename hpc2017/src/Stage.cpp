//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Stage.hpp"
#include "ArrayNum.hpp"
#include "Assert.hpp"
#include "Math.hpp"
#include "Util.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Stage クラスのインスタンスを生成します。
///
/// @note 引数が同じならば、ステージも同じものが生成されます。
Stage::Stage(RandomSeed aSeed)
: mTurn(0)
, mRestItemCount(0)
, mOffice()
, mUFOs()
, mHouses()
, mRandom(aSeed)
{
}

//------------------------------------------------------------------------------
/// ステージを初期化します。
void Stage::init()
{
    mTurn = 0;

    // 農場の初期化
    mOffice = Office(
        Vector2(Parameter::StageWidth / 2.0f, Parameter::StageHeight / 2.0f)
        );

    // UFOの初期化
    // ステージ開始時はすべて農場の真上にいる
    {
        for (int i = 0; i < Parameter::LargeUFOCount; ++i) {
            UFO ufo(
                UFOType_Large,
                mOffice.pos(),
                float(Parameter::LargeUFORadius),
                float(Parameter::LargeUFOMaxSpeed),
                Parameter::LargeUFOCapacity
                );

            mUFOs.add(ufo);
        }

        for (int i = 0; i < Parameter::SmallUFOCount; ++i) {
            UFO ufo(
                UFOType_Small,
                mOffice.pos(),
                float(Parameter::SmallUFORadius),
                float(Parameter::SmallUFOMaxSpeed),
                Parameter::SmallUFOCapacity
                );

            mUFOs.add(ufo);
        }
    }

    const int MinTownCount = 2;
    const int MaxTownCount = 3;
    const int TownRadius = 100;
    const int TownHouseCount = 20;
    const int MinRandomHouseCount = 5;
    const int MaxRandomHouseCount = 40;
    const int OfficeMargin = 150;
    const int FieldMargin = 20;

    int townCount = mRandom.randMinMax(MinTownCount, MaxTownCount);
    int randomHouseCount = mRandom.randMinMax(MinRandomHouseCount, MaxRandomHouseCount);

    mRestItemCount = townCount * TownHouseCount + randomHouseCount;
    HPC_MAX_ASSERT_I(mRestItemCount, Parameter::MaxHouseCount);

    // 家の配置
    {
        // 配置済み判定用グリッド。1マスの大きさは家の半径
        // 1マスを家の直径にした方がシンプルだが、より自然な配置にするため細かいグリッドにしている
        const int GridWidth = Parameter::StageWidth / Parameter::HouseRadius + 1;
        const int GridHeight = Parameter::StageHeight / Parameter::HouseRadius + 1;
        bool placed[GridHeight][GridWidth] = {};
        Array<Vector2, MaxTownCount> townCenters;

        int checkOffsetX[] = {
            -1, -1, -1,  0,  0,  0,  1,  1,  1,
        };
        int checkOffsetY[] = {
            -1,  0,  1, -1,  0,  1, -1,  0,  1,
        };
        static_assert(HPC_ARRAY_NUM(checkOffsetX) == HPC_ARRAY_NUM(checkOffsetY), "invalid array size");

        // 街の生成
        for (int i = 0; i < townCount; ++i) {
            // 街の中心を決める
            Vector2 townCenter;
            while(true) {
                float outerMargin = float(FieldMargin + TownRadius);
                float townCenterX = outerMargin + mRandom.randFloatTerm(Parameter::StageWidth - outerMargin * 2);
                float townCenterY = outerMargin + mRandom.randFloatTerm(Parameter::StageHeight - outerMargin * 2);
                townCenter = Vector2(townCenterX, townCenterY);

                // 農場からある程度離す
                float innerMargin = float(OfficeMargin + TownRadius);
                if (townCenter.squareDist(mOffice.pos()) <= innerMargin * innerMargin) {
                    continue;
                }

                // 街同士が重ならないようにする
                bool ok = true;
                for (auto prevCenter : townCenters) {
                    float threshold = TownRadius * 2.0f;
                    if (prevCenter.squareDist(townCenter) <= threshold * threshold) {
                        ok = false;
                        break;
                    }
                }

                if (!ok) {
                    continue;
                }

                townCenters.add(townCenter);
                break;
            }

            for (int j = 0; j < TownHouseCount; ++j) {
                // 配置済みの家に重ならない位置をランダムで探す
                int gridX = -1;
                int gridY = -1;
                while (true) {
                    float r = mRandom.randFloatTerm(TownRadius - Parameter::HouseRadius);
                    float t = Math::PI * mRandom.randFloatTerm(2.0f);

                    int posX = int(townCenter.x + r * Math::Cos(t));
                    int posY = int(townCenter.y + r * Math::Sin(t));

                    gridX = posX / Parameter::HouseRadius;
                    gridY = posY / Parameter::HouseRadius;
                    HPC_RANGE_ASSERT_MIN_UB_I(gridX, 0, GridWidth);
                    HPC_RANGE_ASSERT_MIN_UB_I(gridY, 0, GridHeight);

                    if (!placed[gridY][gridX]) {
                        break;
                    }
                }

                // 決定した位置の近辺のフラグを立てて、配置禁止にする
                for (int k = 0; k < HPC_ARRAY_NUM(checkOffsetX); ++k) {
                    int checkX = gridX + checkOffsetX[k];
                    int checkY = gridY + checkOffsetY[k];
                    HPC_RANGE_ASSERT_MIN_UB_I(checkX, 0, GridWidth);
                    HPC_RANGE_ASSERT_MIN_UB_I(checkY, 0, GridHeight);

                    placed[checkY][checkX] = true;
                }

                House house(
                    Vector2(
                        float(gridX * Parameter::HouseRadius),
                        float(gridY * Parameter::HouseRadius)
                        )
                    );

                mHouses.add(house);
            }
        }

        // バラバラに家を配置
        for (int i = 0; i < randomHouseCount; ++i) {
            // 配置済みの家に重ならない位置をランダムで探す
            int gridX = -1;
            int gridY = -1;
            while (true) {
                int posX = FieldMargin + mRandom.randTerm(Parameter::StageWidth - FieldMargin * 2);
                int posY = FieldMargin + mRandom.randTerm(Parameter::StageHeight - FieldMargin * 2);

                gridX = posX / Parameter::HouseRadius;
                gridY = posY / Parameter::HouseRadius;
                HPC_RANGE_ASSERT_MIN_UB_I(gridX, 0, GridWidth);
                HPC_RANGE_ASSERT_MIN_UB_I(gridY, 0, GridHeight);

                if (placed[gridY][gridX]) {
                    continue;
                }

                if (Vector2(float(posX), float(posY)).squareDist(mOffice.pos()) <= OfficeMargin * OfficeMargin) {
                    continue;
                }

                break;
            }

            // 決定した位置の近辺のフラグを立てて、配置禁止にする
            for (int j = 0; j < HPC_ARRAY_NUM(checkOffsetX); ++j) {
                int checkX = gridX + checkOffsetX[j];
                int checkY = gridY + checkOffsetY[j];
                HPC_RANGE_ASSERT_MIN_UB_I(checkX, 0, GridWidth);
                HPC_RANGE_ASSERT_MIN_UB_I(checkY, 0, GridHeight);

                placed[checkY][checkX] = true;
            }

            House house(
                Vector2(
                    float(gridX * Parameter::HouseRadius),
                    float(gridY * Parameter::HouseRadius)
                    )
                );

            mHouses.add(house);
        }
    }

    // 家が街に属するかどうか判別できないように、配列をシャッフル
    for (int i = mHouses.count() - 1; i >= 1; --i) {
        int j = mRandom.randMinMax(0, i);
        std::swap(mHouses[i], mHouses[j]);
    }

#if HEAVY_DEBUG
    // 家の数と配送が必要なアイテムの数が等しいこと
    HPC_ASSERT(mRestItemCount == mHouses.count());

    // 家が意図通りの配置かチェック
    for (auto& house : mHouses) {
        // 農場からの距離
        float margin = OfficeMargin - Parameter::HouseRadius; // グリッドへの丸め分は許容
        HPC_ASSERT(mOffice.pos().squareDist(house.pos()) >= margin * margin);

        // マップ端からの距離
        HPC_ASSERT(FieldMargin <= house.pos().x);
        HPC_ASSERT(house.pos().x <= Parameter::StageWidth - FieldMargin);
        HPC_ASSERT(FieldMargin <= house.pos().y);
        HPC_ASSERT(house.pos().y <= Parameter::StageHeight - FieldMargin);

        // 他の家と重なっていないか
        for (auto& house2 : mHouses) {
            if (&house == &house2) {
                continue;
            }

            float minHouseDist = Parameter::HouseRadius * 2;
            HPC_ASSERT(house.pos().squareDist(house2.pos()) >= minHouseDist * minHouseDist);
        }
    }
#endif
}

//------------------------------------------------------------------------------
void Stage::moveItems(const Actions& aActions)
{
    for (auto& action: aActions) {
        switch (action.type()) {
            case ActionType_PickUp:
            {
                HPC_RANGE_ASSERT_MIN_UB_I(action.ufoIndex(), 0, mUFOs.count());

                auto& ufo = mUFOs[action.ufoIndex()];

                if (!Util::IsIntersect(mOffice, ufo)) {
                    continue;
                }

                int pickCount = ufo.capacity() - ufo.itemCount();
                ufo.incItem(pickCount);

                break;
            }
            case ActionType_Pass:
            {
                HPC_RANGE_ASSERT_MIN_UB_I(action.srcUFOIndex(), 0, mUFOs.count());
                HPC_RANGE_ASSERT_MIN_UB_I(action.dstUFOIndex(), 0, mUFOs.count());

                auto& srcUFO = mUFOs[action.srcUFOIndex()];
                auto& dstUFO = mUFOs[action.dstUFOIndex()];

                if (!Util::IsIntersect(srcUFO, dstUFO)) {
                    continue;
                }

                int passCount = dstUFO.capacity() - dstUFO.itemCount();
                if (passCount > srcUFO.itemCount()) {
                    passCount = srcUFO.itemCount();
                }

                srcUFO.decItem(passCount);
                dstUFO.incItem(passCount);

                break;
            }
            case ActionType_Deliver:
            {
                HPC_RANGE_ASSERT_MIN_UB_I(action.ufoIndex(), 0, mUFOs.count());
                HPC_RANGE_ASSERT_MIN_UB_I(action.houseIndex(), 0, mHouses.count());

                auto& ufo = mUFOs[action.ufoIndex()];
                auto& house = mHouses[action.houseIndex()];

                if (!Util::IsIntersect(ufo, house)) {
                    continue;
                }

                if (ufo.itemCount() == 0) {
                    continue;
                }

                if (house.delivered()) {
                    continue;
                }

                ufo.decItem(1);
                house.deliver();
                mRestItemCount--;

                break;
            }
            default:
                HPC_SHOULD_NOT_REACH_HERE();
        }
    }
}

//------------------------------------------------------------------------------
void Stage::moveUFOs(const TargetPositions& aTargetPositions)
{
    HPC_ASSERT(aTargetPositions.count() == mUFOs.count());

    for (int i = 0; i < mUFOs.count(); ++i) {
        HPC_ASSERT(Math::IsValid(aTargetPositions[i].x));
        HPC_ASSERT(Math::IsValid(aTargetPositions[i].y));

        mUFOs[i].move(aTargetPositions[i]);
    }
}

//------------------------------------------------------------------------------
void Stage::advanceTurn()
{
    ++mTurn;
}

//------------------------------------------------------------------------------
bool Stage::hasFinished()const
{
    return mRestItemCount == 0;
}

//------------------------------------------------------------------------------
int Stage::turn()const
{
    return mTurn;
}

//------------------------------------------------------------------------------
const Office& Stage::office() const
{
    return mOffice;
}

//------------------------------------------------------------------------------
const UFOs& Stage::ufos() const
{
    return mUFOs;
}

//------------------------------------------------------------------------------
const Houses& Stage::houses() const
{
    return mHouses;
}

} // namespace
// EOF
