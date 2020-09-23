@echo off

if exist IRF-DIODE-*.log del /q IRF-DIODE-*.log
if exist diode.irf del /q diode.irf
echo>gensplit.cmd @echo off

pushd LIB
for %%i in (*.spi) do call :t %%i
popd

type>>gensplit.cmd gensplit.txt

call gensplit.cmd

goto :eof

:t
gawk -f ..\genkind.awk -v FILENAME=%1 <%1 >>..\gensplit.cmd
goto :eof
