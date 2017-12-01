@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Desktop\PoisonedWine
java -jar tester2.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\PoisonedWine\Release\PoisonedWine.exe" -seed %SEED%

pause