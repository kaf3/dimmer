@echo off
for %%i in (*.prm) do call :t %%~ni
goto :eof

:t
gawk -f gensplit.awk -v KIND=NMOSFET -v FILENAME=%1.prm -v MODEL=%1  0<NMOSFET.txt 1>%1.asy
goto :eof
