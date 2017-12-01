@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Desktop\MM94
java -jar tester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\MM94\Release\MM94.exe" -seed %SEED% -save
pause