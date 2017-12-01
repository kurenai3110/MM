//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "House.hpp"

#include "Assert.hpp"
#include "Parameter.hpp"

namespace hpc {

//------------------------------------------------------------------------------
House::House()
: mPos()
, mDelivered()
{
}

//------------------------------------------------------------------------------
House::House(const Vector2& aPos)
: mPos(aPos)
, mDelivered(false)
{
}

//------------------------------------------------------------------------------
void House::deliver()
{
    HPC_ASSERT(!mDelivered);
    mDelivered = true;
}

//------------------------------------------------------------------------------
Vector2 House::pos() const
{
    return mPos;
}

//------------------------------------------------------------------------------
float House::radius() const
{
    return float(Parameter::HouseRadius);
}

//------------------------------------------------------------------------------
bool House::delivered() const
{
    return mDelivered;
}

} // namespace
// EOF
