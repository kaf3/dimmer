pushd ..

call :t 8065

call :t 8610

call :t 8066

call :t 8099

call :t 9631AN

call :t 8512

call :t 8592


call :t 8616


call :t 744
call :t 744a
call :t 744b
call :t 744c
call :t 744j
call :t 744k
call :t 744s
call :t 744t


call :t 743
call :t 743a
call :t 743b
call :t 743j
call :t 743k
call :t 743s

call :t 746
call :t 746a
call :t 746b
call :t 746j
call :t 746s

call :t 797
call :t 797a
call :t 797b
call :t 797s


call :t 847
call :t 847a
call :t 847j
call :t 847s

call :t 846
call :t 846a
call :t 846b
call :t 846s


call :t 845
call :t 845a
call :t 845b
call :t 845j
call :t 845k
call :t 845s


call :t 844
call :t 844a
call :t 844b
call :t 844s

call :t 825
call :t 825_15V
call :t 825_5V


call :t 826
call :t 826a


call :t 827

call :t 823AN

call :t 822
call :t 822a
call :t 822b
call :t 822s

call :t 820
call :t 820a
call :t 820b

call :t 812
call :t 812a

call :t 811

call :t 810
call :t 810a
call :t 810s


call :t 815

call :t 816

call :t 817
call :t 817a

call :t 818
call :t 818a


call :o 249
call :o 249a
call :o 249e
call :o 249f
call :o 249g


call :o 275
call :o 275g


call :o 290


popd


goto :eof





:t
set NAME=AD%1
call wget http://www.analog.com//Analog_Root/static/techSupport/designTools/spiceModels/%NAME%.cir
if not exist %NAME%.cir goto :eof
gawk -f t\t.awk -vFILENAME=%NAME% <t\1 >%NAME%.asy
goto :eof

:o
set NAME=OP%1
if not exist %NAME%.cir goto :eof
call wget http://www.analog.com//Analog_Root/static/techSupport/designTools/spiceModels/%NAME%.cir
gawk -f t\t.awk -vFILENAME=%NAME% <t\1 >%NAME%.asy
goto :eof
