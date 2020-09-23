del /s /q *.raw *.log
del /q extra.rar
rarnt a -r -m0 extra1 \SwCADIII\lib\cmp\standard.dio
rarnt a -r -m0 extra1 \SwCADIII\lib\cmp\standard.bjt
rarnt a -r -m0 extra1 \SwCADIII\lib\cmp\standard.mos
rarnt a -r -m0 extra1 \SwCADIII\lib\cmp\standard.jft
rem rarnt a -r -m0 extra1 \SwCADIII\Balbes
rarnt a -r -m0 extra1 -xwideband*.zip -xextra*.rar \SwCADIII\lib\sym\EXTRA\*
rarnt a -m5 extra extra1.rar
del /q extra1.rar
