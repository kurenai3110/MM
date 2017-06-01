@echo off
rem set /P SEED="seed:"

cd C:\Users\keke0\Desktop\WaterFallFishing
type nul > output.txt
for /l %%S in (1,1,5000) do (
    echo seed = %%S >> output.txt
    java -jar tester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\WaterFallFishing\Debug\WaterFallFishing.exe" -seed %%S >> output.txt
    echo DONE! %%S
)

pause