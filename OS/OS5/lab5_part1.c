#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    char buf[4096], out[64];
    int num = 1, at_start = 1;
    ssize_t r;

    while ((r = read(0, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < r; i++) {
            if (at_start) {
                int k = snprintf(out, sizeof(out),
                                 "%d: ", num++);
                write(1, out, (size_t)k);
                at_start = 0;
            }
            write(1, &buf[i], 1);
            if (buf[i] == '\n')
                at_start = 1;
        }
    }
    return r < 0 ? 1 : 0;
}
