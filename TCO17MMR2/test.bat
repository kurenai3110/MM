@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Desktop\TCO17MMR2
java -jar tester4.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\TCO17MMR2\Debug\TCO17MMR2.exe" -seed %SEED% -delay 1 -pause

pause