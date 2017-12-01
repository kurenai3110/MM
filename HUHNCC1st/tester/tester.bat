@echo off
set /P GRAPH="graph:"
set /P SEED="seed:"

cd C:\Users\keke0\Dropbox\HUHNCC1st

graph_generator.out in\in_%GRAPH%_%SEED% %GRAPH% %SEED%

C:\Users\keke0\Dropbox\HUHNCC1st\Release\HUHNCC1st < in\in_%GRAPH%_%SEED% > out\out_%GRAPH%_%SEED%

score_evaluator.out in\in_%GRAPH%_%SEED% out\out_%GRAPH%_%SEED%

pause
