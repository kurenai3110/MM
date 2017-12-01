//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Ship.hpp"
#include "Parameter.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// Ship クラスのインスタンスを生成します。
Ship::Ship()
: mPos()
, mRestReloadTurn(0)
{
}

//------------------------------------------------------------------------------
/// 初期値を設定します。
///
/// @param[in] aPos 初期位置。
void Ship::init(const Vector2& aPos)
{
    // 初期位置を設定する。
    mPos = aPos;
    // 最初のターンはリロードが完了していない状態で開始する。
    mRestReloadTurn = Parameter::ShipReloadTurn;
}

//------------------------------------------------------------------------------
/// リロードが完了しているならばレーザーを発射します。
///
/// @return レーザーを発射した場合は @c true を返し
///         そうでない場合は @c false を返します。
bool Ship::tryToShoot()
{
    if(canShoot()) {
        mRestReloadTurn = Parameter::ShipReloadTurn;
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
/// 移動します。
void Ship::move(const Vector2& aTargetPos)
{
    Vector2 dir = aTargetPos - mPos;

    if(dir.length() > Parameter::ShipMaxSpeed()) {
        dir = dir / dir.length() * Parameter::ShipMaxSpeed();
    }

    mPos += dir;

    if(mRestReloadTurn > 0) {
        --mRestReloadTurn;
    }
}

//------------------------------------------------------------------------------
/// 位置を取得します。
Vector2 Ship::pos()const
{
    return mPos;
}

//------------------------------------------------------------------------------
/// リロードが完了しているならば true を返します。
bool Ship::canShoot()const
{
    return mRestReloadTurn == 0;
}

//------------------------------------------------------------------------------
/// リロード完了まで時間を取得します。
int Ship::restReloadTurn()const
{
    return mRestReloadTurn;
}

} // namespace
// EOF
