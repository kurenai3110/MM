//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

namespace hpc {

//------------------------------------------------------------------------------
/// 便利関数群。
///
/// このクラスは、 Answer.hpp からインクルードされているため、
/// 参加者が利用することができます。
class Util {
public:
    /// 2つのオブジェクトが重なっているかを取得します。
    /// メンバ関数にpos(), radius()を持つ任意の型のオブジェクトを指定できます。
    ///
    /// ちょうど接している場合は重なっているとみなされます。
    template<class T, class U>
    static bool IsIntersect(const T& aLhs, const U& aRhs)
    {
        float sumRadius = aLhs.radius() + aRhs.radius();
        return aLhs.pos().squareDist(aRhs.pos()) <= sumRadius * sumRadius;
    }
};

} // namespace
// EOF
