@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Desktop\KnightsAttacks
java -jar tester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\KnightsAttacks\Release\KnightsAttacks.exe" -seed %SEED%
pause