@echo off
set /P SEED="seed:"

cd C:\Users\keke0\Dropbox\HUHNCC1st
type nul > output_%SEED%.txt

setlocal enabledelayedexpansion

for /l %%S in (1,1,30) do (
    set /a GRAPH = %SEED% %% 2
    rem graph_generator.out in\in_!GRAPH!_%SEED% !GRAPH! %SEED%

    C:\Users\keke0\Dropbox\HUHNCC1st\Release\HUHNCC1st < in\in_!GRAPH!_%SEED% > out\out_!GRAPH!_%SEED%

    score_evaluator.out in\in_!GRAPH!_%SEED% out\out_!GRAPH!_%SEED% >> output_%SEED%.txt

    echo Done! %%S 
)

pause
