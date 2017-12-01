//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "UFOType.hpp"
#include "Vector2.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// UFO。
class UFO {
public:
    /// @name 内部用関数。
    //@{
    UFO();
    UFO(UFOType aType, const Vector2& aPos, float aRadius, float aMaxSpeed, int aCapacity);
    void move(const Vector2& aTargetPos);
    void incItem(int aCount);
    void decItem(int aCount);
    //@}

    /// @name ユーザーが使用する関数。
    //@{
    UFOType type()const;   ///< UFOの種類を取得します。
    Vector2 pos()const;    ///< 座標を取得します。
    float radius()const;   ///< 半径を取得します。
    float maxSpeed()const; ///< 最大速度を取得します。
    int itemCount()const;  ///< 持っている箱の数を取得します。
    int capacity()const;   ///< 持てる箱の最大数を取得します。
    //@}
private:
    UFOType mType;
    Vector2 mPos;
    float mRadius;
    float mMaxSpeed;
    int mItemCount;
    int mCapacity;
};

} // namespace
// EOF
