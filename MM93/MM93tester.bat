@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Desktop\MM93
java -jar tester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2015\Projects\MM93\Debug\MM93.exe" -seed %SEED%

pause