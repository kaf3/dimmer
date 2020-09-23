BEGIN{
    irf=0
}

{
    if ($0 != "")
    {
        if ($0 == "***** IRF *****")
            irf = !irf;
        else if (!irf)
            print;
    }
}
