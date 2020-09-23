#.SUBCKT Q2006LH4  1  2  3
#*     TERMINALS: MT2 G MT1


BEGIN {
    Description="";
}

/*SRC=/{
    k=split($0,x,";");
    Description=x[k];
}


{
    if ($1==".SUBCKT")
    {
        if (Description=="")
            Description=$2;
        printf ("call :t %s \"%s\" \n", $2, Description);
        Description="";
    }
}

END {
    printf ("goto :eof\n\n");

    printf (":t\n");
    printf ("gawk -v FILENAME=%%1 -v DESCRIPTION=%%2 -f t.awk <triac >%%1.asy\n");
}
