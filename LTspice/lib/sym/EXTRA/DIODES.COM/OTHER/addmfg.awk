BEGIN {
    state = 0;
}

{
    if (substr ($0, 1, 5) == "*SRC=")
    {
        state = 1;
        Vpk   = $2;
        Iave  = $3;
        Tt    = $4;
        mfg   = "DiodesInc";
        type  = "silicon";
        if ($0 ~ /Schottky/)
            type = "Schottky";
    }
    else if (state == 1 && $(NF) == ")")
    {
        $(NF) = " Vpk=" Vpk " Iave=" Iave " Tt=" Tt " mfg=DiodesInc type=" type " )";
    }
    print $0;
}

