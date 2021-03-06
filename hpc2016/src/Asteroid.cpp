﻿//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Asteroid.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Asteroid クラスのインスタンスを生成します。
Asteroid::Asteroid()
: mPos()
, mRadius(0.0f)
, mExists(false)
{
}

//------------------------------------------------------------------------------
/// 初期値を設定します。
void Asteroid::init(const Vector2& aPos, float aRadius)
{
    mPos = aPos;
    mRadius = aRadius;
    mExists = true;
}

//------------------------------------------------------------------------------
/// 生存フラグをオフにします。
void Asteroid::getDestroyed()
{
    mExists = false;
}

//------------------------------------------------------------------------------
/// 位置を取得します。
Vector2 Asteroid::pos()const
{
    return mPos;
}

//------------------------------------------------------------------------------
/// 半径を取得します。
float Asteroid::radius()const
{
    return mRadius;
}

//------------------------------------------------------------------------------
/// 生存フラグを取得します。
bool Asteroid::exists()const
{
    return mExists;
}

} // namespace
// EOF
