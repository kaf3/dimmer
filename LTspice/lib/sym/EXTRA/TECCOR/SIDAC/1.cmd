call :t K1100E70 " 118V 1A " 
call :t K1050E70 " 113V 1A " 
call :t K1200E70 " 125V 1A " 
call :t K1300E70 " 138V 1A " 
call :t K1400E70 " 146V 1A " 
call :t K1500E70 " 170V 1A " 
call :t K2000F1 " 215V 1A " 
call :t K2200F1 " 230V 1A " 
call :t K2400F1 " 250V 1A " 
call :t K2401F1 " 250V 1A " 
call :t K2500F1 " 280V 1A " 
call :t K2501F1 " 280V 1A " 
call :t K3000F1 " 330V 1A " 
goto :eof

:t
gawk -v FILENAME=%1 -v DESCRIPTION=%2 -f t.awk <sidac >%1.asy
