//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "ActionType.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// 箱の受け渡しフェーズの行動。
///
/// それぞれの行動の事前条件(@pre)を満たしていない場合は、何も起こりません。
class Action
{
public:
    /// 農場から箱を積み込む行動を生成します。
    ///
    /// UFOの積載量一杯まで箱を積み込みます。
    /// 農場には無限に箱があります。
    ///
    /// @param aUFOIndex 箱を積み込むUFOのインデックス。
    /// @pre 農場とUFOが重なっている。
    static const Action PickUp(int aUFOIndex);

    /// UFO同士で箱を受け渡す行動を生成します。
    ///
    /// 持っている箱をすべて渡すか、相手の積載量一杯まで渡します。
    ///
    /// @param aSrcUFOIndex 箱を渡すUFOのインデックス。
    /// @param aDstUFOIndex 箱を受け取るUFOのインデックス。
    /// @pre UFO同士が重なっている。
    static const Action Pass(int aSrcUFOIndex, int aDstUFOIndex);

    /// UFOから家に箱を配達する行動を生成します。
    ///
    /// UFOの箱が1つ減り、家が配達済み状態になります。
    ///
    /// @param aUFOIndex 配達元のUFOのインデックス。
    /// @param aHouseIndex 配達先の家のインデックス。
    /// @pre UFOと家が重なっている。
    /// @pre UFOが箱を1つ以上持っている。
    /// @pre 家が未配達状態である。
    static const Action Deliver(int aUFOIndex, int aHouseIndex);

    Action();
    ActionType type()const;
    int ufoIndex()const;
    int srcUFOIndex()const;
    int dstUFOIndex()const;
    int houseIndex()const;
private:
    ActionType mType;
    int mUFOIndex;
    int mSrcUFOIndex;
    int mDstUFOIndex;
    int mHouseIndex;
};

} // namespace
// EOF
