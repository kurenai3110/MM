//------------------------------------------------------------------------------
/// @file
/// @author   ハル研究所プログラミングコンテスト実行委員会
///
/// @copyright  Copyright (c) 2017 HAL Laboratory, Inc.
/// @attention  このファイルの利用は、同梱のREADMEにある
///             利用条件に従ってください。
//------------------------------------------------------------------------------

#include "Print.hpp"
#include "Simulator.hpp"

#include <cstring>
#include <cstdlib>

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
    bool silentMode = false;

    if (argc > 1) {
        for (int n = 1; n < argc; ++n) {
            // 引数がある場合、引数を記憶します。
            if (!std::strcmp(argv[n], "-j")) {
                willPrintJson = true;
            } else if (!std::strcmp(argv[n], "-r")) {
                if (n + 4 < argc) {
                    hpc::RandomSeed seed = hpc::RandomSeed::DefaultSeed();
                    seed.x = uint(std::strtoul(argv[n + 1], nullptr, 0));
                    seed.y = uint(std::strtoul(argv[n + 2], nullptr, 0));
                    seed.z = uint(std::strtoul(argv[n + 3], nullptr, 0));
                    seed.w = uint(std::strtoul(argv[n + 4], nullptr, 0));

                    if (seed.x == 0 && seed.y == 0 && seed.z == 0 && seed.w == 0) {
                        HPC_PRINTF("Invalid Argument.(-r) can't use all zero seed values.\n");
                        return 1;
                    }

                    sSim.changeSeed(seed);
                    n += 4;
                } else {
                    HPC_PRINTF("Invalid Argument.(-r) need four seed values.\n");
                    return 1;
                }
            } else if (!std::strcmp(argv[n], "-s")) {
                silentMode = true;
            } else {
                // 不明な引数
                HPC_PRINTF("Invalid Argument.(%s)\n", argv[n]);
                return 1;
            }
        }
    }

    sSim.run();

    if(willPrintJson) {
        // Jsonを出力します。
        sSim.printJson();
    } else {
        // 通常の出力を行います。
        sSim.printResult(silentMode);
    }

    return 0;
}

// EOF
