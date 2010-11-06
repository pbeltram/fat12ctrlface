/*
 * $Id$
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if ((argc <= 3) || (strcmp(argv[1], "-f") != 0)) {
        printf("Usage: %s -f <out_file_name> ...\n", argv[0]);
        return -1;
    }

    int f = open(argv[2], O_WRONLY|O_DIRECT|O_SYNC);
    if (f < 0) {
        printf("Open error %d - %s on file %s\n", errno,
                strerror(errno), argv[2]);
        return errno;
    }

    char buf[512] __attribute__((aligned(0x1000)));
    char * p = buf;
    for (int i = 3; i < argc; i++) {
        strcpy(p, argv[i]);
        p += strlen(p);
        if (i < (argc-1)) {
            if (size_t(512 - (p-buf)) < strlen(argv[i+1])) {
                break;
            }
            *p++ = ' ';
        }
    }

    if (write(f, buf, 512) != 512) {
        printf("Write error %d -  %s on file %s\n", errno,
                strerror(errno), argv[2]);
    }
    close(f);

    return 0;
}
