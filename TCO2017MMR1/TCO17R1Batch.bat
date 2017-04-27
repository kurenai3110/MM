@echo off
rem set /P SEED="seed:"

cd C:\Users\keke0\Desktop\TCO17MMR1
type nul > output.txt
for /l %%S in (1,1,10000) do (
    java -jar tester.jar -exec "C:\Users\keke0\Desktop\TCO17MMR1\MM2017R1Batch.exe" -seed %%S >> output.txt
    echo DONE! %%S
)

pause