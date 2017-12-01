@echo off
rem set /P SEED="seed:"

cd C:\Users\keke0\Desktop\PoisonedWine
type nul > output.txt
for /l %%S in (1,1,5000) do (
    java -jar tester2_2.jar -exec "C:\Users\keke0\Documents\Visual Studio 2017\Projects\PoisonedWine\Release\PoisonedWine.exe" -seed %%S >> output.txt
    echo DONE! %%S
)

pause