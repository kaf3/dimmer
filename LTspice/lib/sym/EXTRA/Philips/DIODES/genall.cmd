@echo off
set OUT=diodes.txt
if exist %OUT% del /q %OUT%
for %%i in (*.prm) do call :t %%~ni
goto :eof

:t
rem echo>>%OUT% *%1
gawk>>%OUT% -f genmodels.awk -vFILENAME=%1 <%1.PRM
goto :eof


