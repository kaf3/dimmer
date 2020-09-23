@echo off
for %%i in (*.prm) do call :t %%~ni
goto :eof

:t
gawk -f gensplit.awk -v KIND=PMOSFET -v FILENAME=%1.prm -v MODEL=%1  0<PMOSFET.txt 1>%1.asy
goto :eof
