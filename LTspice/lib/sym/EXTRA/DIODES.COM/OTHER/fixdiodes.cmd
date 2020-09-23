del /q *.lib
for %%i in (*.txt) do call :t %%~ni
goto :eof

:t
rep0d %1.txt 1
gawk -f addmfg.awk -- 1 >>diodesinc.lib
goto :eof

