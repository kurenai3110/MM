//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include <array>
#include "Assert.hpp"

namespace hpc {

/// 最大要素数を指定する可変長配列。
///
/// - TElementにはデストラクタを持つクラスを指定しないでください(正しく呼び出されません)
/// - std::vectorと違い、初期化時にstd::arrayで最大数分のメモリを確保するため
///   このクラスからnew, deleteが呼ばれることはありません。
template <typename TElement, int TCapacity>
class Array
{
public:
    typedef std::array<TElement, TCapacity> ArrayType;

    typedef typename ArrayType::iterator iterator;
    typedef typename ArrayType::const_iterator const_iterator;

    /// コンストラクタ。
    /// 最大要素数分のデフォルトコンストラクタが呼び出されます(POD型の場合は0で埋められます)
    /// 要素数が多い場合はパフォーマンスに注意してください。
    inline Array();

    /// デストラクタ。
    inline ~Array();

    /// 要素を配列の末尾に追加します。
    inline void add(const TElement& aData);

    /// 全ての要素を削除します。
    inline void clear();

    /// 格納されている要素数を取得します。
    /// 戻り値の型がsize_tではないため、size()と区別した名前を付けています。
    inline int count() const;

    /// 格納可能な最大要素数を取得します。
    /// static const intでないのは、コンパイラによる挙動の違いを避けるためです。
    inline int maxCount() const;

    /// @name 要素にアクセスします。インデックスの範囲チェックを行います。
    //@{
    inline TElement& operator[](const int aIndex);
    inline const TElement& operator[](const int aIndex) const;
    //@}

    /// @name イテレータを返します。
    //@{
    inline iterator begin();
    inline iterator end();
    inline const_iterator begin() const;
    inline const_iterator end() const;
    //@}

    /// 配列が空かどうかを取得します。
    inline bool isEmpty() const;

    /// 配列が満杯かどうかを取得します。
    inline bool isFull() const;

private:
    ArrayType mContainer;
    int mCount;
};

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
Array<TElement, TCapacity>::Array()
: mContainer()
, mCount(0)
{
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
Array<TElement, TCapacity>::~Array()
{
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
void Array<TElement, TCapacity>::add(const TElement& aData)
{
    HPC_ASSERT(!isFull());

    // 追加
    mContainer[mCount] = aData;
    ++mCount;
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
void Array<TElement, TCapacity>::clear()
{
    mCount = 0;
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
int Array<TElement, TCapacity>::count() const
{
    return mCount;
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
int Array<TElement, TCapacity>::maxCount() const
{
    return TCapacity;
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
TElement& Array<TElement, TCapacity>::operator[](const int aIndex)
{
    HPC_RANGE_ASSERT_MIN_UB_I(aIndex, 0, mCount);
    return mContainer[aIndex];
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
const TElement& Array<TElement, TCapacity>::operator[](const int aIndex) const
{
    HPC_RANGE_ASSERT_MIN_UB_I(aIndex, 0, mCount);
    return mContainer[aIndex];
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
typename Array<TElement, TCapacity>::iterator Array<TElement, TCapacity>::begin()
{
    return mContainer.begin();
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
typename Array<TElement, TCapacity>::iterator Array<TElement, TCapacity>::end()
{
    return mContainer.begin() + mCount;
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
typename Array<TElement, TCapacity>::const_iterator Array<TElement, TCapacity>::begin() const
{
    return mContainer.begin();
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
typename Array<TElement, TCapacity>::const_iterator Array<TElement, TCapacity>::end() const
{
    return mContainer.begin() + mCount;
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
bool Array<TElement, TCapacity>::isEmpty() const
{
    return mCount == 0;
}

//------------------------------------------------------------------------------
template <typename TElement, int TCapacity>
bool Array<TElement, TCapacity>::isFull() const
{
    return mCount == TCapacity;
}

} // namespace
// EOF
