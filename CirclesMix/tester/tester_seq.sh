#!/bin/sh

cd C:\\Users\\keke0\\Desktop\\CirclesMix
java -jar tester_seq.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\CirclesMix\Release\CirclesMix.exe" -seed $1 -N $2

echo "Enterキーを入力してください"
#Waitは、変数
read Wait
