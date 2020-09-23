#*Symmetry IGBT Model (Version 1.0)

#* Symmetry POWER MOS Model (Version 1.0)
#* External Node Designations
#* Node 1 -> Drain
#* Node 2 -> Gate
#* Node 3 -> Source
#M1 9 7 8 8 MM L=100u W=100u
#.MODEL MM NMOS LEVEL=1 IS=1e-32

BEGIN {
    kind = "unknown";
    name = "unknown";
    type = "unknown";
    f = FILENAME;
}

{
    if ($1 == "Symmetry" || $1 == "*Symmetry")
    {
        if ($2 == "POWER" && $3 == "MOS")
            kind="MOS";
        else if ($2 == "IGBT")
            kind="IGBT";
#print 1, $2, kind, type, name;
    }
    else if ($1 == ".SUBCKT" || $1 == ".subckt")
    {
        type = "SUBCKT";
        if (name == "unknown")
            name = $2;
#print 2, $2, kind, type, name;
    }
    else if ($1 == ".MODEL" || $1 == ".model")
    {
        if (type == "unknown")
            type = "MODEL";
        if (kind == "unknown" && ($3 == "d" || $3 == "D"))
        {
            kind = "D";
            name = $2;
        }
        else if ((kind == "MOS" || kind == "unknown" || kind == "D") && ($3 == "NMOS" || $3 == "PMOS"))
        {
            kind = $3;
        }
#print 3, $2, kind, type, name;
    }
}

END {
    if (kind != "unknown" || name != "unknown" || type != "unknown")
        printf ("call :t %s %s %s %s\n", f, kind, toupper(name), type);
}

        
