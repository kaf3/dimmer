for %%i in (*.lib) do gawk -f t.awk <%%i >>2.txt