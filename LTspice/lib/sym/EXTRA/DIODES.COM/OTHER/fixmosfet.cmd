for %%i in (*mosfet*.tx) do call :t %%~ni
goto :eof

:t
rep0d %1.tx 1
gawk -f addmfg.awk -- 1 >diodesinc-mosfet.lib
goto :eof

