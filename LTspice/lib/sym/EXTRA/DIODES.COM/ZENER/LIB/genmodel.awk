{
    if ($1 == "XXX")
    {
        printf ("SYMATTR Value %s\n", NAME);
        printf ("SYMATTR Prefix X\n");
        printf ("SYMATTR SpiceModel ..\\sym\\EXTRA\\DIODES.COM\\ZENER\\LIB\\zener.lib\n");
        printf ("SYMATTR Value2 %s\n", MODEL);
        printf ("SYMATTR Description %s\n", DESCR);
    }
    else
        print $0;
}

