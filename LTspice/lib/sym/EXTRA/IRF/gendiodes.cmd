
set DIO=diode.irf
if not exist %DIO% goto :no_dio

set DIO=standard.dio
if not exist %DIO% goto :no_dio

copy %DIO% %DIO%.bak
gawk -f irfrm.awk <%DIO% >%DIO%.new
echo>>%DIO%.new ***** IRF *****
gawk>>%DIO%.new -f dmodelup.awk <diode.irf
echo>>%DIO%.new ***** IRF *****
copy %DIO%.new %DIO%
del /s /q %DIO%.new
del /s /q diode.irf

goto :eof

:no_dio
echo cannot find file %DIO%
goto :eof
