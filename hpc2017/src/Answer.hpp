//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
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
#include "Array.hpp"

namespace hpc {

//------------------------------------------------------------------------------
/// ゲームの解答。
///
/// 参加者は Answer.cpp にこのクラスのメンバ関数を実装することで、プログラムを作成してください。
class Answer
{
public:
    Answer();
    ~Answer();

    /// 各ステージ開始時に呼び出されます。
    void init(const Stage& aStage);

    /// 各ターンの、受け渡しフェーズの行動を指定してください。
    void moveItems(const Stage& aStage, Actions& aActions);

    /// 各ターンの、移動フェーズの行動を指定してください。
    void moveUFOs(const Stage& aStage, TargetPositions& aTargetPositions);

    /// 各ステージ終了時に呼び出されます。
    void finalize(const Stage& aStage);
};

} // namespace
// EOF
