#!/bin/sh

cd C:\\Users\\keke0\\Desktop\\CirclesMix
cat /dev/null > output.txt
{
for i in `seq $1 $2`
do java -jar tester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\CirclesMix\Release\CirclesMix.exe" -seed $i
done
wait
}>> output.txt

#<< a
echo "Enterキーを入力してください"
#Waitは、変数
read Wait
#a