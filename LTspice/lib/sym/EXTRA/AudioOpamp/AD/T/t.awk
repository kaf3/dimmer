BEGIN {
    f = FILENAME;
}

{
  gsub ("XXX", f);
  print;
}

