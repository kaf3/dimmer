#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int ac, char **av)
{
    FILE *fd;
    int   iSize;
    char *pBuffer[2];
    char *pSrc;
    char *pDst;


    if (ac != 3)
    {
        printf ("usage: rep0d input_file output_file\n");
        return (0);
    }

    fd = fopen (av[1], "rb");
    if (fd == NULL)
    {
        printf ("error: cannot open %s\n", av[1]);
        return (0);
    }

    fseek (fd, 0, SEEK_END);
    iSize = ftell (fd);
    fseek (fd, 0, SEEK_SET);

    pBuffer[0] = malloc ((iSize + 1) * 3);
    if (pBuffer[0] == 0)
    {
        printf ("error: out of memory\n");
        return (0);
    }

    pBuffer[1] = pBuffer[0] + iSize + 1;
    if (fread (pBuffer[0], iSize, 1, fd) != 1)
    {
        printf ("cannot read from %s\n", av[1]);
        return (0);
    }

    fclose (fd);

    pBuffer[0][iSize] = 0;
    pDst = pBuffer[1];
    pSrc = pBuffer[0];

    while (*pSrc)
    {
        switch (*pDst++ = *pSrc++)
        {
        case 0x0d:
            if (*pSrc == 0x0a)
                *pDst++ = *pSrc++;
            else if (*pSrc == 0x0d && pSrc[1] == 0x0a)
                pDst -= 1;
            else if (*pSrc == '-' && pSrc[1] == 0x0d)
            {
                pSrc += 1;
                pDst -= 1;
            }
            else
                *pDst++ = 0x0a;
            break;
        }
    }

    iSize = pDst - pBuffer[1];
    fd = fopen (av[2], "wb");
    if (fd == NULL)
    {
        printf ("error: cannot create %s\n", av[2]);
        return (0);
    }

    if (fwrite (pBuffer[1], iSize, 1, fd) != 1)
    {
        printf ("write error\n");
    }

    fclose (fd);

    return (0);
}

