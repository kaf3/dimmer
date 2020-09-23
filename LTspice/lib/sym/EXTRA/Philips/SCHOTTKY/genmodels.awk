BEGIN {
    state=0;
    model="";
    f=toupper (FILENAME);
}

{
    if (state == 0)
    {
        if ($1 == "*.MODEL")
            $1 = ".MODEL";
        if ($1 == ".MODEL")
        {
            state = 1;
            type = "schottky";
            $2 = toupper ($2);
            if ($2 != f)
            {
#                printf ("*** warning: strange model %s in %s\n", $2, f);
                state = 2;
            }
            else
            {
                $2 = "PH_" $2;
                if ($3 == "D")
                    $3 = "D (";
                model = $0;
            }
        }
        else if ($1 == ".ENDS" || $1 == "*$")
        {
            printf ("%s mfg=Philips type=%s )\n", model, type);
            state = 2;
        }
    }
    else if (state == 1)
    {
        if ($1 == "+")
        {
            for (i = 2; i < 100 && $i != ""; ++i)
                model = model " " $i;
        }
#        if ($2 == "BV" && substr (f, 1, 2) != "BA" && substr (f, 1, 2) != "PM" && substr (f, 1, 2) != "1P")
#            type = "zener";
    }
}

END {
    if (state == 0)
        printf ("could not find the model for %s\n", f);
    else if (state == 1)
            printf ("%s mfg=Philips type=%s )\n", model, type);
}


