//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2016 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください
//------------------------------------------------------------------------------

#include "Print.hpp"
#include "Simulator.hpp"

#include <cstring>

//------------------------------------------------------------------------------
namespace {
    hpc::Simulator sSim;
}

//------------------------------------------------------------------------------
/// アプリケーションのエントリポイントです。
///
/// @return プログラムが正常に終了したら 0 を返します。
int main(int argc, const char* argv[])
{
    bool willPrintJson = false;

    if(argc > 2) {
        // 2つ以上の引数は無効です。
        HPC_PRINTF("Invalid Argument.\n");
        return 0;
    } else if (argc > 1) {
        // 引数がある場合、引数を記憶します。
        if (!std::strcmp(argv[1], "-j")) {
            willPrintJson = true;
        }
    }

    sSim.run();

    if(willPrintJson) {
        // Jsonを出力します。
        sSim.printJson();
    } else {
        // 通常の出力を行います。
        sSim.printResult();
    }

    return 0;
}

// EOF
