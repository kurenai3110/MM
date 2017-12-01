//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#pragma once

#include "Stage.hpp"
#include "Action.hpp"

/// 必要であれば以下のライブラリを利用しても構いません。
#include "Math.hpp"
#include "Util.hpp"
#include "Vector2.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// ゲームの解答。
///
/// 参加者は Answer.cpp にこのクラスのメンバ関数である
/// init()、getNextAction()、finalize() を実装することで、プログラムを作成してください。
class Answer
{
public:
    Answer();
    ~Answer();
    void init(const Stage& aStage);             ///< 各ステージ開始時に呼び出されます。
    Action getNextAction(const Stage& aStage);  ///< 各ターンでの行動を返します。
    void finalize(const Stage& aStage);         ///< 各ステージ終了時に呼び出されます。
};

} // namespace
// EOF
