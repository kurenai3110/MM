#!/bin/sh

cd C:\\Users\\keke0\\Desktop\\CirclesMix
java -jar tester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\CirclesMix\Release\CirclesMix.exe" -seed $1

echo "Enterキーを入力してください"
#Waitは、変数
read Wait
