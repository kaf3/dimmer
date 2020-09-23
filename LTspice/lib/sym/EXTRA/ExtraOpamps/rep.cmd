@echo off
for %%i in (*.asy) do call :t %%i
goto :eof

:t
if exist 000.tmp del /q 000.tmp
ren %1 000.tmp
gawk -f rep.awk <000.tmp >%1
del /q 000.tmp
goto :eof

