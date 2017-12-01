//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください
//------------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <cstdio>

//------------------------------------------------------------------------------
/// メッセージ付きアサートです。
///
/// 条件を満たさない場合に任意のメッセージを表示して停止する、
/// プログラムの検証用アサートです。
///
/// @param[in] aExp  条件式。
/// @param[in] ...   書式。引数の設定方法は std::printf に準じます。
#define HPC_ASSERT_MSG(aExp, ...) \
    do { \
        if (!(aExp)) { \
            std::printf(__VA_ARGS__); \
            std::printf("\n"); \
            assert(aExp); \
        } \
    } while (false)

//------------------------------------------------------------------------------
/// 標準のアサートです。
///
/// 条件を満たさない場合に既定のメッセージを表示して停止するアサートです。
///
/// @param[in] aExp 条件式。
#define HPC_ASSERT(aExp) HPC_ASSERT_MSG(aExp, "Assertion Failed")

//------------------------------------------------------------------------------
/// int 用の最大値アサートです。
///
/// int 型の値が、指定された最大値を超えないことを表明します。
/// 表明に失敗した場合はそのことを表す文字列を出力して停止します。
///
/// @param[in] aValue   値。
/// @param[in] aMax     最大値。
#define HPC_MAX_ASSERT_I(aValue, aMax) \
    HPC_ASSERT_MSG( \
        aValue <= aMax, \
        "<%s>(%d) should be less than or equal to <%s>(%d).", \
        #aValue, aValue, #aMax, aMax)

//------------------------------------------------------------------------------
/// float 型に適用できる上限規制アサートです。
///
/// float 型の値が、指定された上限未満であることを表明します。
/// 表明に失敗した場合はそのことを表す文字列を出力して停止します。
///
/// @param[in] aValue   検査する値。
/// @param[in] aUbound  上限。
#define HPC_UB_ASSERT_F(aValue, aUbound) \
    HPC_ASSERT_MSG( \
        aValue < aUbound, \
        "<%s>(%f) should be less than <%s>(%f).", \
        #aValue, aValue, #aUbound, aUbound)

//------------------------------------------------------------------------------
/// int 型に適用できる下限規制アサートです。
///
/// int 型の値が、指定された下限より大きいことを表明します。
/// 表明に失敗した場合はそのことを表す文字列を出力して停止します。
///
/// @param[in] aValue  検査する値。
/// @param[in] aLbound 下限値。
#define HPC_LB_ASSERT_I(aValue, aLbound) \
    HPC_ASSERT_MSG( \
        aValue > aLbound, \
        "<%s>(%d) should be greater than <%s>(%d).", \
        #aValue, aValue, #aLbound, aLbound)

//------------------------------------------------------------------------------
/// float 型に適用できる下限規制アサートです。
///
/// float 型の値が、指定された下限より大きいことを表明します。
/// 表明に失敗した場合はそのことを表す文字列を出力して停止します。
///
/// @param[in] aValue  検査する値。
/// @param[in] aLbound 下限値。
#define HPC_LB_ASSERT_F(aValue, aLbound) \
    HPC_ASSERT_MSG( \
        aValue > aLbound, \
        "<%s>(%f) should be greater than <%s>(%f).", \
        #aValue, aValue, #aLbound, aLbound)

//------------------------------------------------------------------------------
/// float 型に適用できる最小値規制アサートです。
///
/// float 型の値が、指定された最小値以上であることを表明します。
/// 表明に失敗した場合はそのことを表す文字列を出力して停止します。
///
/// @param[in] aValue  検査する値。
/// @param[in] aMin    最小値。
#define HPC_MIN_ASSERT_F(aValue, aMin) \
    HPC_ASSERT_MSG( \
        aValue >= aMin, \
        "<%s>(%f) should be greater than or equal to <%s>(%f).", \
        #aValue, aValue, #aMin, aMin)

//------------------------------------------------------------------------------
/// int 型の上限最小範囲制限アサートです。
///
/// int 型の値が、指定された最小値以上かつ、上限より小さいことを表明します。
/// 表明に失敗した場合はそのことを表す文字列を出力して停止します。
///
/// @param[in] aValue    検査する符号付き整数値。
/// @param[in] aMin      最小値を表す符号付き整数。
/// @param[in] aUbound   上限値を表す符号付き整数。
#define HPC_RANGE_ASSERT_MIN_UB_I(aValue, aMin, aUbound) \
    HPC_ASSERT_MSG( \
        aMin <= aValue && aValue < aUbound, \
        "<%s>(%d) is out of range [%d, %d)", \
        #aValue, aValue, aMin, aUbound)

//------------------------------------------------------------------------------
/// 範囲制限アサートです。
///
/// int 型の値が、指定された最小値以上かつ、最大値以下であることを表明します。
/// 表明に失敗した場合はそのことを表す文字列を出力して停止します。
///
/// @param[in] aValue    検査する符号付き整数値。
/// @param[in] aMin      最小値を表す符号付き整数。(greater than or equal 比較を行います。)
/// @param[in] aMax      最大値を表す符号付き整数。(less than or equal 比較を行います。)
#define HPC_RANGE_ASSERT_MIN_MAX_I(aValue, aMin, aMax) \
    HPC_ASSERT_MSG( \
        aMin <= aValue && aValue <= aMax, \
        "<%s>(%d) is out of range [%d, %d]", \
        #aValue, aValue, aMin, aMax)

//------------------------------------------------------------------------------
/// 列挙型制限アサートです。
///
/// 列挙型の値が有効な範囲内であるかどうかを表明します。
/// 表明に失敗した場合はメッセージを表示して停止します。
///
/// @pre 列挙型の開始値が 0 であり、かつ終端値が Typename_TERM の形で宣言されている必要があります。
///
/// @param[in] aType 列挙型の型。
/// @param[in] aValue 検査する値。
#define HPC_ENUM_ASSERT(aType, aValue) \
    HPC_ASSERT_MSG( \
        0 <= aValue && aValue < aType ## _TERM, \
        "aValue %d is out of range: %s[0,%d]", \
        aValue, #aType "_TERM", aType ## _TERM)

//------------------------------------------------------------------------------
/// 到達禁止アサートです。
///
/// プログラムが現在位置に到達しないことを表明します。
/// 到達したらメッセージを表示して停止します。
#define HPC_SHOULD_NOT_REACH_HERE() HPC_ASSERT_MSG(false, "Should not reach here.")

// EOF
