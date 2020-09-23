@echo off

if exist bjt.lib del /q bjt.lib

for %%i in (*.prm) do call :t %%~ni
goto :eof

:t
gawk>>bjt.lib -f extract.awk -v FILENAME=%1 <%1.prm
goto :eof



