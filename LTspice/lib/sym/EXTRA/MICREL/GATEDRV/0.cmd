call :t MIC4416 
call :t MIC4417 
call :t MIC4420 
call :t MIC4429 
call :t MIC4421 
call :t MIC4422 
rem call :t MIC4421A
rem call :t MIC4422A
call :t MIC4423
call :t MIC4424
call :t MIC4451
call :t MIC4452

goto :eof

:t
gawk -v FILENAME=%1 -f t.awk <1 >%1.asy
