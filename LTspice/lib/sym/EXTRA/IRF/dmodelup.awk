{
    if (toupper ($1) == ".MODEL")
        $2 = toupper ($2);
    print ($0);
}
