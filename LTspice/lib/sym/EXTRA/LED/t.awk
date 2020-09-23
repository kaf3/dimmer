BEGIN {
    out=""
}

END {
    if (out != "")
        print out " mfg=OSRAM type=LED)";
}

{
  if ($1 == "+")
  {
    for (i = 2; i < 100 && $i != ""; ++i)
        out = out " " $i;
  }
  else
  {
    if (out != "")
        print out " mfg=OSRAM type=LED)";
    out = $0;
    for (i = 1; i < 100 && $i != ""; ++i);
    if (i != 1 && $(i-1) == "D")
      out = out "(";
  }
}
