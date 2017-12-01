#-------------------------------------------------------------------------------
#   HAL Programming Contest 2016 makefile for GCC
#

SrcDir := src
SourceFiles := $(wildcard $(SrcDir)/*.cpp)
ObjectFiles := $(SourceFiles:%.cpp=%.o)
DependFiles := $(SourceFiles:%.cpp=%.d)
ExecuteFile := ./hpc2016.exe

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

# -Wall : 基本的なワーニングを全て有効に
# -Werror : ワーニングはエラーに
# -Wshadow : ローカルスコープの名前が、外のスコープの名前を隠している時にワーニング
CompileOption := -std=c++11 -Wall -Werror -Wshadow -DDEBUG -MMD -O3
LinkOption := 

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
