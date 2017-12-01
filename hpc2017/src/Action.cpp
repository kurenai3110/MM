//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Action.hpp"

namespace hpc {

//------------------------------------------------------------------------------
const Action Action::PickUp(int aUFOIndex)
{
    Action action;
    action.mType = ActionType_PickUp;
    action.mUFOIndex = aUFOIndex;
    return action;
}

//------------------------------------------------------------------------------
const Action Action::Pass(int aSrcUFOIndex, int aDstUFOIndex)
{
    Action action;
    action.mType = ActionType_Pass;
    action.mSrcUFOIndex = aSrcUFOIndex;
    action.mDstUFOIndex = aDstUFOIndex;
    return action;
}

//------------------------------------------------------------------------------
const Action Action::Deliver(int aUFOIndex, int aHouseIndex)
{
    Action action;
    action.mType = ActionType_Deliver;
    action.mUFOIndex = aUFOIndex;
    action.mHouseIndex = aHouseIndex;
    return action;
}

//------------------------------------------------------------------------------
Action::Action()
: mType()
, mUFOIndex()
, mSrcUFOIndex()
, mDstUFOIndex()
, mHouseIndex()
{
}

//------------------------------------------------------------------------------
ActionType Action::type() const
{
    return mType;
}

//------------------------------------------------------------------------------
int Action::ufoIndex() const
{
    return mUFOIndex;
}

//------------------------------------------------------------------------------
int Action::srcUFOIndex() const
{
    return mSrcUFOIndex;
}

//------------------------------------------------------------------------------
int Action::dstUFOIndex() const
{
    return mDstUFOIndex;
}

//------------------------------------------------------------------------------
int Action::houseIndex() const
{
    return mHouseIndex;
}

} // namespace
// EOF
