@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Desktop\WaterFallFishing
java -jar tester2.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\WaterFallFishing\Debug\WaterFallFishing.exe" -seed %SEED%

pause