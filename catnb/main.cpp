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
    if (argc == 1) {
        return 0;
    }

    char buf[512] __attribute__((aligned(0x1000)));

    for (int i = 1; i<argc; i++) {
        int f = open(argv[i], O_RDONLY|O_DIRECT|O_SYNC);
        if (f < 0) {
            printf("Open error %d - %s on file %s\n", errno,
                    strerror(errno), argv[i]);
            return errno;
        }
        else {
            int ret = read(f, buf, 512);
            if (ret < -1) {
                printf("Read error %d -  %s on file %s\n", errno,
                        strerror(errno), argv[i]);
                return errno;
            }
            if (write(STDOUT_FILENO, buf, ret) != ret) {
                printf("Write error %d -  %s on file %s\n", errno,
                        strerror(errno), argv[i]);
            }
            close(f);
        }
    }

    return 0;
}
