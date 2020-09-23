BEGIN {
    state = 0;
    f = FILENAME;
    printf ("\n* %s.prm\n", f);
}

{
    if (state == 0)
    {
        if (toupper ($1) == ".MODEL")
        {
            x = toupper ($3);
            if (x == "PNP" || x == "PNP(" || x == "NPN" || x == "NPN(")
            {
                state = 1;
                $2 = "PH_" toupper (f);
                print $0;
            }
        }
    }
    else if (state == 1)
    {
        if (substr ($1, 1, 1) != "+")
            state = 2;
        else
            print $0;
    }
}
