BEGIN {
    m = MODEL;
    f = FILENAME;
}

{
    gsub ("MODEL", m);
    gsub ("FILENAME", f);
    print;
}
