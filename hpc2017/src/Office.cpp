//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Office.hpp"

#include "Assert.hpp"
#include "Parameter.hpp"

namespace hpc {

//------------------------------------------------------------------------------
Office::Office()
: mPos()
{
}

//------------------------------------------------------------------------------
Office::Office(const Vector2& aPos)
: mPos(aPos)
{
}

//------------------------------------------------------------------------------
Vector2 Office::pos() const
{
    return mPos;
}

//------------------------------------------------------------------------------
float Office::radius() const
{
    return float(Parameter::OfficeRadius);
}

} // namespace
// EOF
