#-------------------------------------------------------------------------------
#   HAL Programming Contest 2017 makefile for GCC
#

SrcDir := src

SourceFiles := Combine.cpp

ObjectFiles := $(SourceFiles:%.cpp=%.o)
DependFiles := $(SourceFiles:%.cpp=%.d)
ExecuteFile := ./hpc2017.exe

# Atを@にしておくと、コマンドの実行結果出力を抑止できます。
# 出力が必要な場合は空白を指定します。
At := @
#At := 

# ターゲットを出力する、ユーティリティ。
# 無効にするときは空白を指定します。
#EchoTarget = @echo '\# $@'
EchoTarget = 

Compiler := g++
Linker := g++

# -Wall : 基本的な警告を全て有効に
# -Werror : 警告はエラーに
# -Wshadow : ローカルスコープの名前が、外のスコープの名前を隠している時に警告
# -Wno-error=sign-compare : 比較時の符号の有無の混在は、警告は出すがエラーにしない
# -fno-asm, -fno-exceptions : インラインアセンブラ・例外は使用不可(作品規定を参照)
CompileOption := -std=c++11 -Wall -Werror -Wshadow -Wno-error=sign-compare -fno-asm -fno-exceptions -DLOCAL -MMD -O3
LinkOption := -O3

# 処理が重いデバッグ用コードを有効にします。
# チェッカーの開発者向けです。
# CompileOption += -DHEAVY_DEBUG

#-------------------------------------------------------------------------------
.PHONY: all clean run json help

all : $(ExecuteFile)

$(ExecuteFile) : $(ObjectFiles)
	$(EchoTarget)
	$(At) $(Linker) $(LinkOption) $(ObjectFiles) -o $(ExecuteFile)

clean :
	$(EchoTarget)
	$(At) rm -fv $(ExecuteFile) $(ObjectFiles) $(DependFiles) $(ExecuteFile).stackdump

run : $(ExecuteFile)
	$(At) $(ExecuteFile)

json : $(ExecuteFile)
	$(At) $(ExecuteFile) -j

help :
	@echo '--- ターゲット一覧 ---'
	@echo '- all     : 全てをビルドし、実行ファイルを作成する。(デフォルトターゲット)'
	@echo '- clean   : 生成物を削除する。'
	@echo '- help    : このメッセージを出力する。'
	@echo '- run     : 実行する。'
	@echo '- json    : jsonを出力する。'

%.o : %.cpp Makefile
	$(At) $(EchoTarget)
	$(At) $(Compiler) $(CompileOption) -c $< -o $@

#-------------------------------------------------------------------------------
-include $(DependFiles)
