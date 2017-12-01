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
/// 数学関数群。
///
/// このクラスは、 Answer.hpp からインクルードされているため、
/// 参加者が利用することができます。
class Math {
public:
    static const float PI;                                          ///< 円周率

    ///@name 数値関係
    //@{
    static bool IsEqualLoose(float aLhs, float aRhs);               ///< 二値が等しいかを誤差の範囲で判定します。
    static float Max(float aLhs, float aRhs);                       ///< 二値のうち大きい方を返します。
    static int Max(int aLhs, int aRhs);                             ///< 二値のうち大きい方を返します。
    static float Min(float aLhs, float aRhs);                       ///< 二値のうち小さい方を返します。
    static int Min(int aLhs, int aRhs);                             ///< 二値のうち小さい方を返します。
    static float LimitMinMax(float aValue, float aMin, float aMax); ///< 値を指定の範囲 [min, max] に制限します。
    static int LimitMinMax(int aValue, int aMin, int aMax);         ///< 値を指定の範囲 [min, max] に制限します。
    static float Abs(float aValue);                                 ///< 値の絶対値を返します。
    static int Abs(int aValue);                                     ///< 値の絶対値を返します。
    static float LimitAbs(float aValue, float aLimitAbs);           ///< 値を指定の大きさに制限します。
    static float Sqrt(float aValue);                                ///< 値の平方根を求めます。
    static int Ceil(float aValue);                                  ///< 小数点の値を切り上げます。
    static bool IsValid(float aValue);                              ///< 浮動小数点の値が有効値かどうかを判定します。
    //@}

    ///@name 三角関数
    //@{
    static float Sin(float aRad);               ///< 正弦の値を求めます。
    static float Cos(float aRad);               ///< 余弦の値を求めます。
    static float Tan(float aRad);               ///< 正接の値を求めます。
    static float ACos(float aCos);              ///< 余弦の値から角度を求めます。
    static float ATan2(float aY, float aX);     ///< 正接の値から角度を求めます。
    static float RadToDeg(float aRad);          ///< 弧度法で表された角度を度数法に変換します。
    static float DegToRad(float aDeg);          ///< 度数法で表された角度を弧度法に変換します。
    //@}

private:
    Math();
};

} // namespace
// EOF
