//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "UFO.hpp"

#include "Assert.hpp"
#include "Parameter.hpp"

namespace hpc {

//------------------------------------------------------------------------------
UFO::UFO()
: mType()
, mPos()
, mRadius()
, mMaxSpeed()
, mItemCount()
, mCapacity()
{
}

//------------------------------------------------------------------------------
UFO::UFO(UFOType aType, const Vector2& aPos, float aRadius, float aMaxSpeed, int aCapacity)
: mType(aType)
, mPos(aPos)
, mRadius(aRadius)
, mMaxSpeed(aMaxSpeed)
, mItemCount(0)
, mCapacity(aCapacity)
{
}

//------------------------------------------------------------------------------
void UFO::move(const Vector2& aTargetPos)
{
    Vector2 dir = aTargetPos - mPos;

    float length = dir.length();
    if (length > mMaxSpeed) {
        dir = dir / length * mMaxSpeed;
    }

    mPos += dir;
}

//------------------------------------------------------------------------------
void UFO::incItem(int aCount)
{
    HPC_MAX_ASSERT_I(mItemCount + aCount, mCapacity);
    mItemCount += aCount;
}

//------------------------------------------------------------------------------
void UFO::decItem(int aCount)
{
    HPC_MIN_ASSERT_I(mItemCount, aCount);
    mItemCount -= aCount;
}

//------------------------------------------------------------------------------
UFOType UFO::type() const
{
    return mType;
}

//------------------------------------------------------------------------------
Vector2 UFO::pos() const
{
    return mPos;
}

//------------------------------------------------------------------------------
float UFO::radius() const
{
    return mRadius;
}

//------------------------------------------------------------------------------
float UFO::maxSpeed() const
{
    return mMaxSpeed;
}

//------------------------------------------------------------------------------
int UFO::itemCount() const
{
    return mItemCount;
}

//------------------------------------------------------------------------------
int UFO::capacity() const
{
    return mCapacity;
}

} // namespace
// EOF
