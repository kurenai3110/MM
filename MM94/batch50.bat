@echo off
rem set /P SEED="seed:"

cd C:\Users\keke0\Desktop\MM94
type nul > output.txt
for /l %%S in (1,1,50) do (
    java -jar tester.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\MM94\Release\MM94.exe" -seed %%S >> output.txt
    echo DONE! %%S
)

pause