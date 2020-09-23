BEGIN {
#    printf ("FILENAME='%s'\n", FILENAME);
    f = FILENAME;
    d = DESCRIPTION;
}

{
    s = $0;
    gsub ("XXX", f, s);
    gsub ("YYY", d, s);
    print s;
}
