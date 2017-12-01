@echo off
rem set /P SEED="seed:"

cd C:\Users\keke0\Desktop\TCO17MMR2
type nul > output2.txt
for /l %%S in (1,1,100) do (
    echo seed = %%S >> output2.txt
    java -jar testertester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\TCO17MMR2\Debug\TCO17MMR2.exe" -seed %%S -novis >> output2.txt
    echo DONE! %%S
)

pause