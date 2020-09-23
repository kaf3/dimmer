BEGIN {
    state = 0;
}

{
    if (state == 3)
    {
        if ($1 != ".ENDS")
            print ".ENDS";
        state = 0;
    }

    out = $0;
    if ($1 == ".SUBCKT")
    {
        state = 1;
    }
    else if ($1 == ".MODEL")
    {
        state += 1;
        if (state == 3 && $(NF) != ")")
            out = out " )";
    }
    else if ($1 == ".ENDS")
    {
        state = 0;
    }

    if (out != "Zener" && out != "*SYM=HZEN")
    {
        if (out == "*        Terminals    A   K")
            out = "*       Terminals   A K";
        print out;
    }
}
