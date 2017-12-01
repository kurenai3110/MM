//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

namespace hpc {

//------------------------------------------------------------------------------
/// 2次元ベクトル。
///
/// このクラスは、 Answer.hpp からインクルードされているため、
/// 参加者が利用することができます。
class Vector2 {
public:
    static const Vector2 Zero();
    Vector2();
    Vector2(float aX, float aY);

    ///@name 等値比較
    //@{
    bool operator==(const Vector2& aRhs)const;                 ///< 2つのベクトルが等しいかを返します。
    bool isZeroStrict()const;                                  ///< ゼロベクトルと等しいかを返します。
    bool equals(const Vector2& aVec)const;                     ///< 2つのベクトルが誤差の範囲内で等しいかを求めます。
    //@}

    ///@name 四則演算
    //@{
    Vector2 operator-()const;                                  ///< 負のベクトルを返します。
    Vector2& operator+=(const Vector2& aRhs);                  ///< ベクトルを加算し結果を代入します。
    Vector2& operator-=(const Vector2& aRhs);                  ///< ベクトルを減算し結果を代入します。
    Vector2& operator*=(float aRhs);                           ///< ベクトルの実数倍を計算し結果を代入します。
    Vector2& operator/=(float aRhs);                           ///< ベクトルを実数で割った結果を代入します。
    Vector2 operator+(const Vector2& aRhs)const;               ///< 2つのベクトルの和を返します。
    Vector2 operator-(const Vector2& aRhs)const;               ///< 2つのベクトルの差を返します。
    Vector2 operator*(float aRhs)const;                        ///< ベクトルの実数倍を返します。
    Vector2 operator/(float aRhs)const;                        ///< ベクトルを実数で割った値を返します。
    friend Vector2 operator*(float aLhs, const Vector2& aRhs); ///< 実数のベクトル倍を返します。
    //@}

    float dot(const Vector2& aVec)const;                       ///< 与えたベクトルとの内積を求めます。
    float cross(const Vector2& aVec)const;                     ///< 与えたベクトルとの外積を求めます。
    float cos(const Vector2& aVec)const;                       ///< 与えたベクトルとの余弦を求めます。
    float angle(const Vector2& aVec)const;                     ///< 与えたベクトルとのなす角を求めます。
    float rotSign(const Vector2& aTarget)const;                ///< 与えたベクトルへの回転量を符号付きで求めます。
    float length()const;                                       ///< ベクトルの長さを求めます。
    float squareLength()const;                                 ///< ベクトルの長さの二乗を求めます。
    float dist(const Vector2& aPoint)const;                    ///< 与えた点との距離を求めます。
    float squareDist(const Vector2& aPoint)const;              ///< 与えた点との距離の二乗を求めます。
    void unitAssign(float aLength = 1.0f);                     ///< 一定の長さに正規化します。
    Vector2 unit(float aLength = 1.0f)const;                   ///< 一定の長さに正規化されたベクトルを返します。
    void rotateRad(float aRad);                                ///< ベクトルを一定角度回転させます。
    Vector2 getRotatedRad(float aRad)const;                    ///< 一定角度回転させたベクトルを返します。
    void project(const Vector2& aVec);                         ///< ベクトルを射影します。
    Vector2 getProjected(const Vector2& aVec)const;            ///< 射影したベクトルを返します。

    float x;    ///< 値の x 要素
    float y;    ///< 値の y 要素
};

} // namespace
// EOF
