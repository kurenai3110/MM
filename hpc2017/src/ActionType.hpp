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
/// 箱の受け渡しフェーズの行動の種類。
enum ActionType
{
    ActionType_PickUp,  ///< 農場から箱を積み込む。
    ActionType_Pass,    ///< UFO同士で箱を受け渡す。
    ActionType_Deliver, ///< UFOから家に箱を配達する。

    ActionType_TERM,
};

} // namespace
// EOF
