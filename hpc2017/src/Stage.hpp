//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Action.hpp"
#include "Parameter.hpp"
#include "Random.hpp"
#include "Office.hpp"
#include "UFO.hpp"
#include "House.hpp"
#include "Array.hpp"

namespace hpc {

// 各種配列の型定義です。
using Actions = Array<Action, Parameter::MaxActionPerTurn>;
using TargetPositions = Array<Vector2, Parameter::UFOCount>;
using UFOs = Array<UFO, Parameter::UFOCount>;
using Houses = Array<House, Parameter::MaxHouseCount>;

//------------------------------------------------------------------------------
/// ステージ。
class Stage {
public:
    /// @name 内部用関数。
    //@{
    Stage(RandomSeed aSeed);
    void init();
    void moveItems(const Actions& aActions);
    void moveUFOs(const TargetPositions& aTargetPositions);
    void advanceTurn();
    bool hasFinished()const;
    //@}

    /// @name ユーザーが使用する関数。
    //@{
    int turn()const;             ///< 現在のターン数を取得します。
    const Office& office()const; ///< 農場を取得します。
    const UFOs& ufos()const;     ///< UFOの配列を取得します。
    const Houses& houses()const; ///< 家の配列を取得します。
    //@}
private:
    int mTurn;
    int mRestItemCount;
    Office mOffice;
    UFOs mUFOs;
    Houses mHouses;
    Random mRandom;
};

} // namespace
// EOF
