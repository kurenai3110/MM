@echo off

cd C:\Users\keke0\Dropbox\HUHNCC1st
type nul > output.txt

setlocal enabledelayedexpansion

for /l %%S in (33,1,62) do (
    set /a GRAPH = 1
    if %%S leq 90 set /a GRAPH = 0

    graph_generator.out in\in_!GRAPH!_%%S !GRAPH! %%S

    C:\Users\keke0\Dropbox\HUHNCC1st\Release\HUHNCC1st < in\in_!GRAPH!_%%S > out\out_!GRAPH!_%%S

    score_evaluator.out in\in_!GRAPH!_%%S out\out_!GRAPH!_%%S >> output.txt

    echo Done! %%S 
)

pause
