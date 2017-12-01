//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Vector2.hpp"
#include "Assert.hpp"
#include "Math.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// ゼロベクトルを取得します。
const Vector2 Vector2::Zero()
{
    return Vector2();
}

//------------------------------------------------------------------------------
/// Vector2 クラスのインスタンスを生成します。
Vector2::Vector2()
: x(0.0f)
, y(0.0f)
{
}

//------------------------------------------------------------------------------
/// Vector2 クラスのインスタンスを生成します。
Vector2::Vector2(float aX, float aY)
: x(aX)
, y(aY)
{
}

//------------------------------------------------------------------------------
/// ゼロベクトルと等しいかを返します。
///
/// @return x, y がそれぞれ厳密にゼロであれば @c true を返し
///         そうでなければ @c false を返します。
bool Vector2::isZeroStrict()const
{
    return x == 0.0f && y == 0.0f;
}

//------------------------------------------------------------------------------
/// 2つのベクトルが誤差の範囲内で等しいかを求めます。
///
/// @param[in] aVec 比較対象とするベクトル。
///
/// @return aVec の各要素が x, y と誤差の範囲で等しければ @c true を返し
///         そうでなければ @c false を返します。
bool Vector2::equals(const Vector2& aVec)const
{
    return Math::IsEqualLoose(x, aVec.x) && Math::IsEqualLoose(y, aVec.y);
}

//------------------------------------------------------------------------------
/// 与えた点との距離を求めます。
///
/// @param[in] aPoint 距離計算の目的地となる座標。
///
/// @return (x, y) と aPoint の距離。
float Vector2::dist(const Vector2& aPoint)const
{
    return Math::Sqrt(squareDist(aPoint));
}

//------------------------------------------------------------------------------
/// 与えた点との距離の二乗を求めます。
///
/// @param[in] aPoint 距離計算の目的地となる座標。
///
/// @return (x, y) と aPoint の距離の二乗。
float Vector2::squareDist(const Vector2& aPoint)const
{
    const float x2 = (aPoint.x - x) * (aPoint.x - x);
    const float y2 = (aPoint.y - y) * (aPoint.y - y);

    return x2 + y2;
}

//------------------------------------------------------------------------------
/// 与えたベクトルとの内積を求めます。
///
/// @param[in] aVec 内積計算の対象となるベクトル。
///
/// @return (x, y) と aVec の内積。
float Vector2::dot(const Vector2& aVec)const
{
    return x * aVec.x + y * aVec.y;
}

//------------------------------------------------------------------------------
/// 与えたベクトルとの外積を求めます。
///
/// @param[in] aVec 外積計算の対象となるベクトル。
///
/// @return (x, y) と aVec の外積。
float Vector2::cross(const Vector2& aVec)const
{
    return x * aVec.y - y * aVec.x;
}

//------------------------------------------------------------------------------
/// 与えたベクトルとの余弦を求めます。
///
/// @param[in] aVec なす角を計算するためのベクトル。
///
/// @return (x, y) と aVec とのなす角 (ラジアン) の余弦 (cos)。
float Vector2::cos(const Vector2& aVec)const
{
    const float d = dot(aVec);
    const float l = length() * aVec.length();

    HPC_LB_ASSERT_F(l, 0.0f);
    return d / l;
}

//------------------------------------------------------------------------------
/// 与えたベクトルとのなす角を求めます。
///
/// @param[in] aVec なす角を計算するためのベクトル。
///
/// @return (x, y) と aVec とのなす角の大きさ (ラジアン)。
float Vector2::angle(const Vector2& aVec)const
{
    return Math::ACos(cos(aVec));
}

//------------------------------------------------------------------------------
/// 与えたベクトルとのなす角を求めます。
///
/// @note 向きは反時計回りが正、時計回りが負を表します。
///
/// @param[in] aTarget 目標とする方向を表すベクトル。
///
/// @return 回転に必要な角度 (ラジアン) が [-π..π] の範囲で返されます。
float Vector2::rotSign(const Vector2& aTarget)const
{
    const float d = dot(aTarget);
    const float c = x * aTarget.y - y * aTarget.x;

    return Math::ATan2(c, d);
}

//------------------------------------------------------------------------------
/// ベクトルの長さを求めます。
///
/// @return ベクトルの長さ。
float Vector2::length()const
{
    return Math::Sqrt(squareLength());
}

//------------------------------------------------------------------------------
/// ベクトルの長さの二乗を求めます。
///
/// @return ベクトルの長さの二乗。
float Vector2::squareLength()const
{
    return x * x + y * y;
}

//------------------------------------------------------------------------------
/// 一定の長さに正規化します。
///
/// @pre ベクトルがゼロベクトルではないことをあらかじめ確認する必要があります。
///
/// @param[in] aLength ベクトルの長さ。正の数を設定します。
void Vector2::unitAssign(float aLength)
{
    HPC_LB_ASSERT_F(aLength, 0.0f);

    *this /= length();
    *this *= aLength;
}

//------------------------------------------------------------------------------
/// 一定の長さに正規化されたベクトルを返します。
///
/// @pre ベクトルがゼロベクトルではないことをあらかじめ確認する必要があります。
///
/// @param[in] aLength ベクトルの長さ。正の数を設定します。
///
/// @return (x, y) を aLength の長さに設定したベクトル。
Vector2 Vector2::unit(float aLength)const
{
    HPC_LB_ASSERT_F(aLength, 0.0f);

    Vector2 aVec(*this);
    aVec.unitAssign(aLength);
    return aVec;
}

//------------------------------------------------------------------------------
/// ベクトルを一定角度回転させます。
///
/// @param[in] aRad 回転角 (ラジアン)。反時計回りが正の値を表します。
void Vector2::rotateRad(float aRad)
{
    const float rotatedX = Math::Cos(aRad) * x - Math::Sin(aRad) * y;
    const float rotatedY = Math::Sin(aRad) * x + Math::Cos(aRad) * y;

    x = rotatedX;
    y = rotatedY;
}

//------------------------------------------------------------------------------
/// 一定角度回転させたベクトルを返します。
///
/// @note インスタンスの値自体は変更されません。
///
/// @param[in] aRad 回転角 (ラジアン)。反時計回りが正の値を表します。
///
/// @return (x, y) を aRad だけ回転させて得られるベクトル。
Vector2 Vector2::getRotatedRad(float aRad)const
{
    Vector2 v(*this);
    v.rotateRad(aRad);

    return v;
}

//------------------------------------------------------------------------------
/// ベクトルを射影します。
///
/// @param[in] aVec 射影対象。求めたベクトルはこのベクトルと平行になります。
void Vector2::project(const Vector2& aVec)
{
    const float l = aVec.length();
    if (l == 0.0f) {
        HPC_SHOULD_NOT_REACH_HERE();
        return;
    }

    const float len = this->dot(aVec) / l;
    Vector2 result = aVec.unit() * len;
    x = result.x;
    y = result.y;
}

//------------------------------------------------------------------------------
/// 射影したベクトルを返します。
///
/// @note インスタンスの値自体は変更されません。
///
/// @param[in] aVec 射影対象。生成されるベクトルはこのベクトルと平行になります。
///
/// @return (x, y) を aVec に射影したベクトル。
Vector2 Vector2::getProjected(const Vector2& aVec)const
{
    Vector2 v(*this);
    v.project(aVec);
    return v;
}

//------------------------------------------------------------------------------
bool Vector2::operator==(const Vector2& aRhs)const
{
    return x == aRhs.x && y == aRhs.y;
}

//------------------------------------------------------------------------------
Vector2 Vector2::operator-()const
{
    return Vector2(-x, -y);
}

//------------------------------------------------------------------------------
Vector2& Vector2::operator+=(const Vector2& aRhs)
{
    x += aRhs.x;
    y += aRhs.y;

    return *this;
}

//------------------------------------------------------------------------------
Vector2& Vector2::operator-=(const Vector2& aRhs)
{
    return *this += (-aRhs);
}

//------------------------------------------------------------------------------
Vector2 Vector2::operator+(const Vector2& aRhs)const
{
    Vector2 v(*this);
    v += aRhs;

    return v;
}

//------------------------------------------------------------------------------
Vector2 Vector2::operator-(const Vector2& aRhs)const
{
    return *this + (-aRhs);
}

//------------------------------------------------------------------------------
Vector2& Vector2::operator*=(float aRhs)
{
    x *= aRhs;
    y *= aRhs;

    return *this;
}

//------------------------------------------------------------------------------
Vector2& Vector2::operator/=(float aRhs)
{
    x /= aRhs;
    y /= aRhs;

    return *this;
}

//------------------------------------------------------------------------------
Vector2 Vector2::operator*(float aRhs)const
{
    Vector2 v(*this);
    v *= aRhs;
    return v;
}

//------------------------------------------------------------------------------
Vector2 Vector2::operator/(float aRhs)const
{
    Vector2 v(*this);
    v /= aRhs;
    return v;
}

//------------------------------------------------------------------------------
Vector2 operator*(float aLhs, const Vector2& aRhs)
{
    return aRhs * aLhs;
}

} // namespace
// EOF
