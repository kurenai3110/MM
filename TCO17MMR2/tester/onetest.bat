@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Desktop\TCO17MMR2
type nul > %SEED%_output.txt
for /l %%S in (1,1,100) do (
    java -jar tester3.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\TCO17MMR2\Debug\TCO17MMR2.exe" -seed %SEED% -novis >> %SEED%_output.txt
    echo DONE! %%S
)

pause