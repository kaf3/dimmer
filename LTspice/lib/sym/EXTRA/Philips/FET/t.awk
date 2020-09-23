BEGIN {
    state = 0;
    name = "";
}

{
    if (tolower ($1) == ".subckt")
    {
        if (state == 2)
            printf ("%s)\n", out);
        name = toupper ($2);
        out = "";
        state = 1;
    }
    else if (tolower ($1) == ".model" && (tolower ($3) == "njf" || tolower ($3) == "pjf"))
    {
        if (state == 2)
        {
            printf ("%s)\n", out);
            name = "";
            out = "";
        }
        out = toupper ($2);
        if (out == "NJF" || out == "PJF" || name != "")
            out = name;
        name = "";
        if (out == "")
            out = "???";
        out = ".model " out " " toupper ($3) "(";
        state = 2;
    }
    else if (tolower ($1) == ".ends")
    {
        if (state == 2)
            printf ("%s)\n", out);
        name = "";
        out = "";
        state = 0;
    }
    else if (state == 2)
    {
        if ($1 != "+")
        {
            printf ("%s)\n", out);
            name = "";
            state = 0;
        }
        else
        {
            for (j = 2; $j != ""; ++j)
            {
                out = out " " $j;
            }
        }
    }
}

END {
    if (state != 0)
        printf ("%s)\n", out);
}


