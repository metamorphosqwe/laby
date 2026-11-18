#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int write_all(int fd, const char *buf, size_t n)
{
    while (n > 0) {
        ssize_t w = write(fd, buf, n);
        if (w < 0)
            return -1;
        buf += w;
        n -= (size_t)w;
    }
    return 0;
}

static int parse_line(char *line, char **login, long *uid,
                      long *gid)
{
    char *f[7];
    int n = 0;
    char *p = line;
    char *end;

    f[n++] = p;
    while (*p != '\0' && n < 7) {
        if (*p == ':')
            f[n++] = p + 1;
        p++;
    }
    if (n < 4)
        return -1;
    *login = f[0];
    *uid = strtol(f[2], &end, 10);
    if (end == f[2] || *end != ':')
        return -1;
    *gid = strtol(f[3], &end, 10);
    if (end == f[3] || (*end != ':' && *end != '\0'))
        return -1;
    f[1][-1] = '\0';
    return 0;
}

static char *read_stdin(size_t *len)
{
    size_t cap = 4096, used = 0;
    char *buf = malloc(cap + 1);

    if (buf == NULL)
        return NULL;
    for (;;) {
        ssize_t r;
        if (used == cap) {
            char *t = realloc(buf, cap * 2 + 1);
            if (t == NULL) {
                free(buf);
                return NULL;
            }
            buf = t;
            cap *= 2;
        }
        r = read(0, buf + used, cap - used);
        if (r < 0) {
            free(buf);
            return NULL;
        }
        if (r == 0)
            break;
        used += (size_t)r;
    }
    buf[used] = '\0';
    *len = used;
    return buf;
}

int main(void)
{
    size_t len = 0;
    char *data, *line, *next;
    int fd_uid, fd_gid;

    (void)umask(0);
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif

    data = read_stdin(&len);
    if (data == NULL) {
        write_all(2, "Can't read stdin\n", 17);
        return 1;
    }
    fd_uid = open("uid_names.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    fd_gid = open("gid_names.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_uid < 0 || fd_gid < 0) {
        write_all(2, "Can't open file\n", 16);
        free(data);
        return 1;
    }
    for (line = data; *line != '\0'; line = next) {
        char *nl = strchr(line, '\n');
        char *login, orig[1024], rec[1100];
        long uid, gid;
        size_t ll;

        if (nl != NULL) {
            *nl = '\0';
            next = nl + 1;
        } else {
            next = line + strlen(line);
        }
        ll = strlen(line);
        if (ll == 0 || ll >= sizeof(orig) - 1)
            continue;
        memcpy(orig, line, ll);
        orig[ll] = '\n';
        if (parse_line(line, &login, &uid, &gid) < 0)
            continue;
        write_all(uid % 2 == 0 ? 1 : 2, orig, ll + 1);
        ll = (size_t)snprintf(rec, sizeof(rec), "%ld %s\n",
                              uid, login);
        write_all(fd_uid, rec, ll);
        ll = (size_t)snprintf(rec, sizeof(rec), "%ld %s\n",
                              gid, login);
        write_all(fd_gid, rec, ll);
    }
    if (close(fd_uid) < 0 || close(fd_gid) < 0)
        write_all(2, "Can't close file\n", 17);
    free(data);
    return 0;
}
