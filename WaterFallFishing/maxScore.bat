@echo off
rem set /P SEED="seed:"

cd C:\Users\keke0\Desktop\WaterFallFishing
type nul > maxScore_.txt
for /l %%S in (1,1,5000) do (
    echo seed = %%S >> maxScore_.txt
    java -jar tester3.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\WaterFallFishing\Debug\WaterFallFishing.exe" -seed %%S >> maxScore_.txt
    echo DONE! %%S
)

pause