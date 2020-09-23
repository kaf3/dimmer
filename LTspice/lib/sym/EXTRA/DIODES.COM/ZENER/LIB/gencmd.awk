BEGIN {
    printf ("@echo off\n\n");
}

{
    if (substr ($0, 1, 5) == "*SRC=")
    {
        title = substr ($1, 6, length ($1) - 5);
        for (i = 2; i <= NF && $(i) != "Diodes" && $(i) != "DIODES"; ++i)
            title = title " " $(i);
    }
    else if ($1 == ".SUBCKT")
    {
        model = $2;
        printf ("set DESCR=\"%s\"\n", title);
        printf ("set MODEL=%s\n", model);
        if (substr (model, 1, 3) == "DI_")
            model = substr (model, 4, length (model) - 3);
        printf ("set NAME=%s\n", model);
        printf ("call genmodel.cmd\n\n");
    }
}

